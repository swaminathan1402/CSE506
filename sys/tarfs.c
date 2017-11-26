#include<sys/tarfs.h>
#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/elf64.h>

int octal_to_decimal(char *str, int size){
	int n = 0;
	char *c = str;
	while(size-- > 0){
		
		n *= 8;
		n += *c - '0';
		c++;
	}
	return n;
}

void read_elf(Elf64_Ehdr *file){
	
	kprintf("Type(Reloc/Exec/Shared/Core) %d\nInstruction Set: %d\nELF Version: %d\nPh Offset: %d\n", file->e_type, file->e_machine, file->e_version, file->e_phoff);
	
	Elf64_Phdr *program_header = (Elf64_Phdr *)((uint64_t)file + file->e_phoff);
	kprintf("Program Header\n");
	kprintf("Virtual addr: %p\nPhysical addr: %p type: %d\nFile size: %d\n", program_header->p_vaddr, program_header->p_paddr, program_header->p_type, program_header->p_filesz);
}

void tarfs_read(){
	
	uint64_t start_addr = (uint64_t)&_binary_tarfs_start;
	struct posix_header_ustar *file = (struct posix_header_ustar *)start_addr;
	
	while((uint64_t)file < (uint64_t)&_binary_tarfs_end){
		
		// kprintf("Filename: %s\nMode: %p\nSize: %d\n", file->name, file->mode, octal_to_decimal(file->size, 11));
		int size_of_file = octal_to_decimal(file->size, 11);
		if(size_of_file > 0){
			kprintf("Filename: %s\nMode: %p\nSize: %d\n", file->name, file->mode, octal_to_decimal(file->size, 11));
			Elf64_Ehdr *something = (Elf64_Ehdr *)(file + 1);

			read_elf(something);
			break;
		}

		if(size_of_file == 0) {
			file+=1;
		}
		else {
			file += (1 + size_of_file/512);
			if(size_of_file  % 512 != 0){
	                	file += 1;
                        }

		}
	}
	kprintf("finished traversing\n");
}
