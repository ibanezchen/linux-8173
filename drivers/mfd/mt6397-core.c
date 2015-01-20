/*
 * Copyright (c) 2014 MediaTek Inc.
 * Author: Flora Fu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/regmap.h>
#include <linux/mfd/core.h>
#include <linux/mfd/mt6397/core.h>
#include <linux/mfd/mt6397/registers.h>

static const struct mfd_cell mt6397_devs[] = {
	{
		.name = "mt6397-rtc",
		.of_compatible = "mediatek,mt6397-rtc",
	}, {
		.name = "mt6397-regulator",
		.of_compatible = "mediatek,mt6397-regulator",
	}, {
		.name = "mt6397-codec",
		.of_compatible = "mediatek,mt6397-codec",
	}, {
		.name = "mt6397-clk",
		.of_compatible = "mediatek,mt6397-clk",
	},
};

static inline u32 mt6397_irq_shift(int irq)
{
	return irq & 0xf;
}

static inline u32 mt6397_irq_reg(int irq)
{
	return irq >> 4;
}

static void mt6397_irq_lock(struct irq_data *data)
{
	struct mt6397_chip *mt6397 = irq_get_chip_data(data->irq);

	mutex_lock(&mt6397->irqlock);
}

static void mt6397_irq_sync_unlock(struct irq_data *data)
{
	int i;
	struct mt6397_chip *mt6397 = irq_get_chip_data(data->irq);

	regmap_write(mt6397->regmap, MT6397_INT_CON0, mt6397->irq_masks_cur[0]);
	regmap_write(mt6397->regmap, MT6397_INT_CON1, mt6397->irq_masks_cur[1]);

	mutex_unlock(&mt6397->irqlock);
}

static void mt6397_irq_mask(struct irq_data *data)
{
	struct mt6397_chip *mt6397 = irq_get_chip_data(data->irq);
	int shift = mt6397_irq_shift(data->hwirq);
	int reg = mt6397_irq_reg(data->hwirq);
	int reg_ofs = MT6397_INT_CON0 + reg * 2;

	mt6397->irq_masks_cur[reg] &= ~(1 << shift);
	regmap_write(mt6397->regmap, reg_ofs, mt6397->irq_masks_cur[reg]);
}

static void mt6397_irq_unmask(struct irq_data *data)
{
	struct mt6397_chip *mt6397 = irq_get_chip_data(data->irq);
	int shift = mt6397_irq_shift(data->hwirq);
	int reg = mt6397_irq_reg(data->hwirq);
	int reg_ofs = MT6397_INT_CON0 + reg * 2;

	mt6397->irq_masks_cur[reg] |= (1 << shift);
	regmap_write(mt6397->regmap, reg_ofs, mt6397->irq_masks_cur[reg]);
}

static void mt6397_irq_ack(struct irq_data *data)
{
	struct mt6397_chip *mt6397 = irq_get_chip_data(data->irq);
	int shift = mt6397_irq_shift(data->hwirq);
	int reg = mt6397_irq_reg(data->hwirq);
	int reg_ofs = MT6397_INT_STATUS0 + reg * 2;

	regmap_write(mt6397->regmap, reg_ofs, 1 << shift);
}

static struct irq_chip mt6397_irq_chip = {
	.name = "mt6397-irq",
	.irq_bus_lock = mt6397_irq_lock,
	.irq_bus_sync_unlock = mt6397_irq_sync_unlock,
	.irq_mask = mt6397_irq_mask,
	.irq_unmask = mt6397_irq_unmask,
	.irq_ack = mt6397_irq_ack,
};

static irqreturn_t mt6397_irq_thread(int irq, void *data)
{
	struct mt6397_chip *mt6397 = data;
	int irq_reg[MT6397_IRQ_GROUP_NR];
	int i, cur_irq, ret;

	for (i = 0; i < MT6397_IRQ_GROUP_NR; i++) {
		ret = regmap_read(mt6397->regmap,
			MT6397_INT_STATUS0 + i * 2, &irq_reg[i]);
		if (ret > 0) {
			dev_err(mt6397->dev,
				"failed to read interrupt status [0x%x]\n",
				MT6397_INT_STATUS0 + i * 2);
			return IRQ_NONE;
		}
	}

	for (i = 0; i < MT6397_IRQ_NR; i++) {
		int shift = mt6397_irq_shift(i);
		int reg = mt6397_irq_reg(i);
		int reg_ofs = MT6397_INT_STATUS0 + reg * 2;

		if (irq_reg[reg] & (1 << shift)) {
			cur_irq = irq_find_mapping(mt6397->irq_domain, i);
			if (cur_irq)
				handle_nested_irq(cur_irq);

			/* write 1 to status bit to clear the event.  */
			regmap_write(mt6397->regmap, reg_ofs, 1 << shift);
		}
	}

	return IRQ_HANDLED;
}

static int mt6397_irq_domain_map(struct irq_domain *d, unsigned int irq,
					irq_hw_number_t hw)
{
	struct mt6397_chip *mt6397 = d->host_data;

	irq_set_chip_data(irq, mt6397);
	irq_set_chip_and_handler(irq, &mt6397_irq_chip, handle_level_irq);
	irq_set_nested_thread(irq, 1);
	set_irq_flags(irq, IRQF_VALID);

	return 0;
}

static struct irq_domain_ops mt6397_irq_domain_ops = {
	.map = mt6397_irq_domain_map,
};

static int mt6397_irq_init(struct mt6397_chip *mt6397)
{
	int ret;

	mutex_init(&mt6397->irqlock);

	/* Mask all interrupt sources */
	regmap_write(mt6397->regmap, MT6397_INT_CON0, 0x0);
	regmap_write(mt6397->regmap, MT6397_INT_CON1, 0x0);

	mt6397->irq_domain = irq_domain_add_linear(mt6397->dev->of_node,
		MT6397_IRQ_NR, &mt6397_irq_domain_ops, mt6397);
	if (!mt6397->irq_domain) {
		dev_err(mt6397->dev, "could not create irq domain\n");
		return -ENOMEM;
	}

	ret = devm_request_threaded_irq(mt6397->dev, mt6397->irq, NULL,
		mt6397_irq_thread, IRQF_ONESHOT, "mt6397-pmic", mt6397);
	if (ret) {
		dev_err(mt6397->dev, "failed to register irq=%d; err: %d\n",
			mt6397->irq, ret);
		return ret;
	}

	return 0;
}

static int mt6397_probe(struct platform_device *pdev)
{
	int ret;
	struct mt6397_chip *mt6397;

	mt6397 = devm_kzalloc(&pdev->dev, sizeof(*mt6397), GFP_KERNEL);
	if (!mt6397)
		return -ENOMEM;

	mt6397->dev = &pdev->dev;
	/*
	 * mt6397 MFD is child device of soc pmic wrapper.
	 * Regmap is set from its parent.
	 */
	mt6397->regmap = dev_get_regmap(pdev->dev.parent, NULL);
	if (!mt6397->regmap)
		return -ENODEV;

	platform_set_drvdata(pdev, mt6397);

	mt6397->irq = platform_get_irq(pdev, 0);
	if (mt6397->irq > 0) {
		ret = mt6397_irq_init(mt6397);
		if (ret)
			return ret;
	}

	ret = mfd_add_devices(&pdev->dev, -1, mt6397_devs,
			ARRAY_SIZE(mt6397_devs), NULL, 0, NULL);
	if (ret)
		dev_err(&pdev->dev, "failed to add child devices: %d\n", ret);

	return ret;
}

static int mt6397_remove(struct platform_device *pdev)
{
	mfd_remove_devices(&pdev->dev);

	return 0;
}

static const struct of_device_id mt6397_of_match[] = {
	{ .compatible = "mediatek,mt6397" },
	{ }
};
MODULE_DEVICE_TABLE(of, mt6397_of_match);

static struct platform_driver mt6397_driver = {
	.probe = mt6397_probe,
	.remove = mt6397_remove,
	.driver = {
		.name = "mt6397",
		.of_match_table = of_match_ptr(mt6397_of_match),
	},
};

module_platform_driver(mt6397_driver);

MODULE_AUTHOR("Flora Fu");
MODULE_DESCRIPTION("Driver for MediaTek MT6397 PMIC");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:mt6397");
