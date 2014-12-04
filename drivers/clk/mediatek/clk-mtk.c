/*
 * Copyright (c) 2014 MediaTek Inc.
 * Author: James Liao <jamesjj.liao@mediatek.com>
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

#include <linux/of.h>
#include <linux/of_address.h>

#include <linux/io.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/clkdev.h>

#include "clk-mtk.h"
#include "clk-gate.h"

void mtk_init_factors(struct mtk_fixed_factor *clks, int num,
		struct clk_onecell_data *clk_data)
{
	int i;
	struct clk *clk;

	for (i = 0; i < num; i++) {
		struct mtk_fixed_factor *ff = &clks[i];

		clk = clk_register_fixed_factor(NULL, ff->name, ff->parent_name,
				CLK_SET_RATE_PARENT, ff->mult, ff->div);

		if (IS_ERR(clk)) {
			pr_err("Failed to register clk %s: %ld\n",
					ff->name, PTR_ERR(clk));
			continue;
		}

		if (clk_data)
			clk_data->clks[ff->id] = clk;
	}
}

void mtk_init_clk_gates(void __iomem *reg_base,
		struct mtk_gate *clks, int num,
		struct clk_onecell_data *clk_data, spinlock_t *lock)
{
	int i;
	struct clk *clk;

	for (i = 0; i < num; i++) {
		struct mtk_gate *gate = &clks[i];

		clk = mtk_clk_register_gate(gate->name, gate->parent_name,
				reg_base + gate->regs->set_ofs,
				reg_base + gate->regs->clr_ofs,
				reg_base + gate->regs->sta_ofs,
				gate->shift, gate->ops, lock);

		if (IS_ERR(clk)) {
			pr_err("Failed to register clk %s: %ld\n",
					gate->name, PTR_ERR(clk));
			continue;
		}

		if (clk_data)
			clk_data->clks[gate->id] = clk;
	}
}

struct clk_onecell_data *mtk_alloc_clk_data(unsigned int clk_num)
{
	int i;
	struct clk_onecell_data *clk_data;

	clk_data = kzalloc(sizeof(clk_data), GFP_KERNEL);
	if (!clk_data)
		return NULL;

	clk_data->clks = kcalloc(clk_num, sizeof(struct clk *), GFP_KERNEL);
	if (!clk_data->clks) {
		kfree(clk_data);
		return NULL;
	}

	clk_data->clk_num = clk_num;

	for (i = 0; i < clk_num; ++i)
		clk_data->clks[i] = ERR_PTR(-ENOENT);

	return clk_data;
}

struct clk *mtk_clk_register_mux(
		const char *name,
		const char **parent_names,
		u8 num_parents,
		void __iomem *base_addr,
		u8 shift,
		u8 width,
		u8 gate_bit,
		spinlock_t *lock)
{
	struct clk *clk;
	struct clk_mux *mux;
	struct clk_gate *gate = NULL;
	struct clk_hw *gate_hw = NULL;
	const struct clk_ops *gate_ops = NULL;
	u32 mask = BIT(width) - 1;

	mux = kzalloc(sizeof(struct clk_mux), GFP_KERNEL);
	if (!mux)
		return ERR_PTR(-ENOMEM);

	mux->reg = base_addr;
	mux->mask = mask;
	mux->shift = shift;
	mux->flags = 0;
	mux->lock = lock;

	if (gate_bit <= MAX_MUX_GATE_BIT) {
		gate = kzalloc(sizeof(struct clk_gate), GFP_KERNEL);
		if (!gate) {
			kfree(mux);
			return ERR_PTR(-ENOMEM);
		}

		gate->reg = base_addr;
		gate->bit_idx = gate_bit;
		gate->flags = CLK_GATE_SET_TO_DISABLE;
		gate->lock = lock;

		gate_hw = &gate->hw;
		gate_ops = &clk_gate_ops;
	}

	clk = clk_register_composite(NULL, name, parent_names, num_parents,
		&mux->hw, &clk_mux_ops,
		NULL, NULL,
		gate_hw, gate_ops,
		CLK_SET_RATE_PARENT);

	if (IS_ERR(clk)) {
		kfree(gate);
		kfree(mux);
	}

	return clk;
}
