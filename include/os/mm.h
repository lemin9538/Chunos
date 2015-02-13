#ifndef _MM_H
#define _MM_H

#include <os/list.h>
#include <os/types.h>
#include <os/bound.h>
#include <os/mutex.h>
#include <os/mirros.h>
#include <os/soc.h>
#include <os/init.h>

#define PAGE_SIZE		4096
#define PAGE_SHIFT		12

#define page_nr(size)		(align(size, PAGE_SIZE) >> PAGE_SHIFT)

#define __GFP_KERNEL		0x00000001
#define __GFP_DMA		0x00000002
#define __GFP_RES		0x00000004
#define GFP_ZONE_ID_MASK	0x0000000f

/* indicate the usage of the memory */
#define __GFP_PGT		0x00000010
#define __GFP_PAGE_HEADER	0x00000020
#define __GFP_SLAB		0x00000040
#define __GFP_USER		0x00000080

#define GFP_KERNEL		(__GFP_KERNEL)
#define GFP_USER		(__GFP_KERNEL | __GFP_USER)
#define GFP_RES			(__GFP_RES)
#define GFP_PGT			(__GFP_KERNEL | __GFP_PGT)
#define GFP_DMA			(__GFP_DMA)
#define GFP_SLAB		(__GFP_KERNEL | __GFP_SLAB)
#define __GFP_MASK		(__GFP_PGT | __GFP_USER | __GFP_SLAB)

struct user_page_attr {
	unsigned long map_address;
};

struct slab_page_attr {

};

struct mmap_page_attr {

};

/*
 * page: represent 4k in physical memory
 * phy_address: physic address this page maped to
 * flag: attribute of the page
 * count: how many continous page has
 * attr: specifical page's attribute
 */
struct page {
	unsigned long phy_address;
	unsigned long vir_address;
	unsigned long flag;
	struct list_head plist;
	u32 count : 16;
	u32 map_type : 1;

	union {
		struct user_page_attr user_page_attr;
		struct slab_page_attr slab_page_attr;
		struct mmap_page_attr mmap_page_attr;
	};
} __attribute__((packed));

void free_pages(void *addr);

void *get_free_pages(int count, int flag);

struct page *request_pages(int count, int flag);

struct page *va_to_page(unsigned long va);

unsigned long pa_to_va(unsigned long pa);

unsigned long page_to_va(struct page *page);

void copy_page_va(unsigned long target, unsigned long source);

void copy_page_pa(unsigned long target, unsigned long source);

struct page *pa_to_page(unsigned long pa);

unsigned long va_to_pa(unsigned long va);

unsigned long page_to_va(struct page *page);

unsigned long page_to_pa(struct page *page);

struct page *get_free_pages_align(int nr, u32 aligin, int flag);

struct page *get_free_page_match(unsigned long match, int flag);

void add_page_to_list_tail(struct page *page,
		struct list_head *head);

struct page *list_to_page(struct list_head *list);

void free_pages_on_list(struct list_head *head);

static inline void *get_free_page(unsigned long flag)
{
	return get_free_pages(1, flag);
}

#endif
