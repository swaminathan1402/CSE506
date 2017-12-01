#include<sys/tarfs.h>
#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/elf64.h>
#include<sys/page_table.h>
#include<sys/memory.h>

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
	
	// kprintf("Type(Reloc/Exec/Shared/Core) %d\nInstruction Set: %d\nELF Version: %d\nPh Offset: %d\n", file->e_type, file->e_machine, file->e_version, file->e_phoff);
	
	Elf64_Phdr *program_header = (Elf64_Phdr *)((uint64_t)file + file->e_phoff);
	kprintf("\nProgram Header %p Elf address %p\n", (uint64_t)program_header, (uint64_t)file);
	
	int count = file->e_phnum;
	
	while(count > 0){
		if(program_header->p_type != 0 && program_header->p_filesz > 0){
			kprintf("Virtual addr: %p\ntype: %d\nFile size: %d\nEntry point: %x\nPOffset: %d\n", program_header->p_vaddr, program_header->p_type, program_header->p_filesz, file->e_entry, program_header->p_offset);
			uint64_t virtual_limit_sz = program_header->p_vaddr + program_header->p_filesz;
			
			// map those virtual addresses 
			for(uint64_t i = program_header->p_vaddr; i<virtual_limit_sz; i+=4096){
				setMap(i, i, 1);
				// setMap(program_header + program_header->p_offset + (i-program_header), i, 1);
				uint64_t from_addr = (uint64_t)file + program_header->p_offset + (i - program_header->p_vaddr);
				memcpy((void *)i, (void *)from_addr,  4096);
			}
		}
		program_header = (Elf64_Phdr *)((uint64_t)program_header + file->e_phentsize); // program header entry size
		count--;
	}
}

void tarfs_read(){
	
	uint64_t start_addr = (uint64_t)&_binary_tarfs_start;
	struct posix_header_ustar *file = (struct posix_header_ustar *)start_addr;
	
	while((uint64_t)file < (uint64_t)&_binary_tarfs_end){
		
		int size_of_file = octal_to_decimal(file->size, 11);
		if(size_of_file > 0){
			if(file->name[4] == 's' && file->name[5] == 'b'){
				kprintf("\n******\nFilename: %s\nMode: %p\nSize: %d\n", file->name, file->mode, octal_to_decimal(file->size, 11));
				Elf64_Ehdr *something = (Elf64_Ehdr *)(file + 1);
				read_elf(something);
			}
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
	//kprintf("finished traversing\n");
}
