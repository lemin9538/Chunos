#include <os/elf.h>
#include <os/slab.h>
#include <os/types.h>
#include <os/mm.h>
#include <os/string.h>
#include <os/errno.h>
#include <os/printk.h>
#include <os/file.h>

int load_elf_data(struct elf_file *efile,
		unsigned long tar,
		size_t size, offset_t off)
{

}

struct elf_file *dup_elf_info(struct elf_file *src)
{
	struct elf_file *elf_file;
	int section_size;

	if (!src)
		return NULL;

	elf_file = kmalloc(sizeof(struct elf_file), GFP_KERNEL);
	if (!elf_file)
		return NULL;

	memcpy((void *)elf_file, (void *)src, sizeof(struct elf_file));
	return elf_file;
}

static struct elf_file *parse_elf_info(elf_section_header *header,
				int section_num, char *str)
{
	elf_section_header *tmp = header;
	int i;
	char *name;
	size_t size = 0;
	struct elf_file *elf_file =NULL;
	struct elf_section *section_roda;
	struct elf_section *section_bss;

	elf_file = (struct elf_file *)
		kzalloc(sizeof(struct elf_file), GFP_KERNEL);
	if (elf_file == NULL)
		return NULL;

	section_roda = &elf_file->sections[SECTION_TEXT_DATA];
	section_bss = &elf_file->section[SECTION_BSS];

	for (i = 0; i < section_num; i++) {
		/* whether this section needed allocate mem. */
		if (tmp->sh_flags & SHF_ALLOC) {
			name = &str[tmp->sh_name];
			if (strncmp(name, "bss", 3)) {
				/* can be optimized TBD */
				if (!section_roda->offset)
					section->offset = header->sh_offset;
				if (!section_roda->load_addr)
					section->load_addr = header->sh_addr;
				section->size += header->sh_size;
			} else {
				section_bss->offset = header->sh_offset;
				section_bss->load_addr = header->sh_addr;
				section->size = header->sh_size;
			}

			size += header->sh_size;
		}

		tmp++;
	}

	elf_file->elf_size = size;

	return elf_file;
}

void inline release_elf_file(struct elf_file *file)
{
	kfree(file);
}

size_t inline elf_memory_size(struct elf_file *efile)
{
	return efile->elf_size;
}

struct elf_file *get_elf_info(struct file *file)
{
	elf_header hdr;
	int ret = -1;
	elf_section_header *header;
	char *str;
	struct elf_file *elf_file;

	ret = kernel_read(file, (char *)&hdr, sizeof(elf_header));
	if (ret < 0) {
		kernel_error("read elf file error offset\n");
		return NULL;
	}
	
       /* confirm whether this file is a elf binary */
	if (strncmp(ELFMAG, hdr.e_ident, 4)) {
		kernel_error("file is not a elf file exist\n");
		return NULL;
	}

	kernel_debug("ident:          %s\n", &hdr.e_ident[1]);
	kernel_debug("type:           %d\n", hdr.e_type);
	kernel_debug("machine:        %d\n", hdr.e_machine);
	kernel_debug("version:        %d\n", hdr.e_version);
	kernel_debug("entry:          0x%x\n", hdr.e_entry);
	kernel_debug("phoff:          0x%x\n", hdr.e_phoff);
	kernel_debug("pentsize:       %d\n", hdr.e_phentsize);
	kernel_debug("phnum:          %d\n", hdr.e_phnum);
	kernel_debug("shoff:          0x%x\n", hdr.e_shoff);
	kernel_debug("shnum:          %d\n", hdr.e_shnum);
	kernel_debug("shentsize:      %d\n", hdr.e_shentsize);
	kernel_debug("shstrndx:       %d\n", hdr.e_shstrndx);


	header = (elf_section_header *)
		kzalloc(hdr.e_shnum * hdr.e_shentsize, GFP_KERNEL);
	if (header == NULL)
		return NULL;

	str = (char *)kmalloc(4096, GFP_KERNEL);
	if (str == NULL)
		goto err_str_mem;

	ret = kernel_seek(file, hdr.e_shoff, SEEK_SET);
	if (ret < 0) {
		kernel_error("seek elf file failed\n");
		return NULL;
	}
	
	ret = kernel_read(file, (char *)header, hdr.e_shnum * hdr.e_shentsize);
	if (ret < 0) {
		elf_file = NULL;
		goto go_out;
	}

	ret = kernel_seek(file, header[hdr.e_shstrndx].sh_offset, SEEK_SET);
	if (ret < 0) {
		elf_file = NULL;
		goto go_out;
	}

	ret = kernel_read(file, str, 4096);
	if (ret < 0) {
		elf_file = NULL;
		goto go_out;
	}

	elf_file = parse_elf_info(header, hdr.e_shnum, str);
	if (elf_file != NULL) {
		release_elf_file(elf_file);
		elf_file =NULL;
	}

	elf_file->entry_point_address = hdr.e_entry;

go_out:
	kfree(str);

err_str_mem:
	kfree(header);

	return elf_file;
}

