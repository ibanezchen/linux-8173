/*
 * Copyright (c) 2014 MediaTek Inc.
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

#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset-controller.h>

struct mtk_infracfg {
	struct regmap *regmap;
	struct reset_controller_dev rcdev;
};

static int mtk_infracfg_reset_assert(struct reset_controller_dev *rcdev,
			      unsigned long id)
{
	struct mtk_infracfg *data = container_of(rcdev, struct mtk_infracfg, rcdev);

	return regmap_update_bits(data->regmap, 0x30 + ((id / 32) << 2), BIT(id % 32), ~0u);
}

static int mtk_infracfg_reset_deassert(struct reset_controller_dev *rcdev,
				unsigned long id)
{
	struct mtk_infracfg *data = container_of(rcdev, struct mtk_infracfg, rcdev);

	return regmap_update_bits(data->regmap, 0x30 + ((id / 32) << 2), BIT(id % 32), 0);
}

static int mtk_infracfg_reset(struct reset_controller_dev *rcdev,
			      unsigned long id)
{
	int ret;

	ret = mtk_infracfg_reset_assert(rcdev, id);
	if (ret)
		return ret;

	return mtk_infracfg_reset_deassert(rcdev, id);
}

static struct reset_control_ops mtk_infracfg_reset_ops = {
	.assert = mtk_infracfg_reset_assert,
	.deassert = mtk_infracfg_reset_deassert,
	.reset = mtk_infracfg_reset,
};

static struct regmap_config mtk_infracfg_regmap_config = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
};

static int mtk_infracfg_probe(struct platform_device *pdev)
{
	struct mtk_infracfg *data;
	struct resource *res;
	void __iomem *base;

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(base))
		return PTR_ERR(base);

	data->regmap = devm_regmap_init_mmio(&pdev->dev, base,
                                        &mtk_infracfg_regmap_config);
	if (IS_ERR(data->regmap))
		return PTR_ERR(data->regmap);

	data->rcdev.owner = THIS_MODULE;
	data->rcdev.nr_resets = 64;
	data->rcdev.ops = &mtk_infracfg_reset_ops;
	data->rcdev.of_node = pdev->dev.of_node;

	return reset_controller_register(&data->rcdev);
}

static int mtk_infracfg_remove(struct platform_device *pdev)
{
	struct mtk_infracfg *data = platform_get_drvdata(pdev);

	reset_controller_unregister(&data->rcdev);

	return 0;
}

static const struct of_device_id mtk_infracfg_dt_ids[] = {
	{
		.compatible = "mediatek,mt8173-infracfg",
	}, {
		.compatible = "mediatek,mt8135-infracfg",
	}, {
		/* sentinel */
	},
};
MODULE_DEVICE_TABLE(of, mtk_infracfg_dt_ids);

static struct platform_driver mtk_infracfg_driver = {
	.probe = mtk_infracfg_probe,
	.remove = mtk_infracfg_remove,
	.driver = {
		.name = "mtk-infracfg",
		.of_match_table = mtk_infracfg_dt_ids,
	},
};

module_platform_driver(mtk_infracfg_driver);

MODULE_AUTHOR("Sascha Hauer <s.hauer@pengutronix.de>");
MODULE_DESCRIPTION("MediaTek infracfg controller");
MODULE_LICENSE("GPL");
