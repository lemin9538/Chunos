/*
 * include/os/pgt.h
 *
 * Created by Le Min at 2.14/12/01
 *
 */

#ifndef _PGT_H_
#define _PGT_H_

#include <os/mm.h>
#include <os/kernel.h>
#include <os/mmu.h>
#include <os/task_mm.h>
#include <os/spin_lock.h>

/*
 * lvl1_pgt_base: the level 1 pgt base address
 * lvl2_pgt_list: the memory for lvl2 page table
 * lvl2_alloc_size: alloc size of lvl2 page table
 * lvl2_free_size: free size of lvl2 pgt current
 * lvl2_free_base: base address of current lvl2 pgt
 */
struct task_page_table {
	unsigned long pde_base;

	struct list_head pte_list;
	size_t pte_alloc_size;
	size_t pte_free_size;
	size_t pte_current_free;
	struct list_head *pte_current_page;
	unsigned long pte_free_base;

	unsigned long temp_buffer_base;
	int temp_buffer_nr;
};

void free_task_page_table(struct task_page_table *pgt);

int init_task_page_table(struct task_page_table *table);

int pgt_map_task_memory(struct task_page_table *table,
		struct list_head *mem_list,
		unsigned long map_base, int type);

struct list_head *pgt_map_temp_memory(struct list_head *head,
		int *count, int *nr, int type);

int pgt_map_task_page(struct task_page_table *table,
		struct page *page, unsigned long user_addr);

#endif
