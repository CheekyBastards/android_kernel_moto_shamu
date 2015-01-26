/*
 * Copyright (C) 2012 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/list.h>
#include <linux/memblock.h>
#include <linux/persistent_ram.h>
#include <linux/rslib.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

struct persistent_ram_buffer {
	uint32_t    sig;
	atomic_t    start;
	atomic_t    size;
	uint8_t     data[0];
};

#define PERSISTENT_RAM_SIG (0x43474244) /* DBGC */

static __initdata LIST_HEAD(persistent_ram_list);

static inline size_t buffer_size(struct persistent_ram_zone *prz)
{
	return atomic_read(&prz->buffer->size);
};

static inline size_t buffer_start(struct persistent_ram_zone *prz)
{
	return atomic_read(&prz->buffer->start);
};



int __init persistent_ram_early_init(struct persistent_ram *ram)
{
	int ret;

	ret = memblock_reserve(ram->start, ram->size);
	if (ret) {
		pr_err("Failed to reserve persistent memory from %08lx-%08lx\n",
			(long)ram->start, (long)(ram->start + ram->size - 1));
		return ret;
	}

	list_add_tail(&ram->node, &persistent_ram_list);

	pr_info("Initialized persistent memory from %08lx-%08lx\n",
		(long)ram->start, (long)(ram->start + ram->size - 1));

	return 0;
}
