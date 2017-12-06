#include<sys/tarfs.h>
#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/elf64.h>
#include<sys/page_table.h>
#include<sys/memory.h>
#include<sys/task.h>
#include<sys/string.h>
#include<sys/page.h>
#include<sys/filedirectory.h>
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
	
	
	Elf64_Phdr *program_header = (Elf64_Phdr *)((uint64_t)file + file->e_phoff);
	// kprintf("\nProgram Header %p Elf address %p\n", (uint64_t)program_header, (uint64_t)file);
	
	uint64_t rflags, cr3;
        __asm__ __volatile__ (
        	"movq %%cr3, %%rax;"
	         "movq %%rax, %0;"
            : "=m"(cr3)
            :
        );
        __asm__ __volatile__ (
            "pushf;"
            "pop %0;"
            : "=m"(rflags)
            :
        );
	createTask((void *)file->e_entry, rflags, cr3, file);
	changeCR3(runningTask->pml4e, runningTask->pdpe, runningTask->pde, runningTask->pte, 1);
	kprintf(" || time %p %p %p %p\n", pml4e, pdpe, pde, pte);
	int count = file->e_phnum;
	while(count > 0){
		if(program_header->p_type != 0 && program_header->p_filesz > 0){
			// kprintf("Virtual addr: %p\ntype: %d\nFile size: %d\nEntry point: %x\nPOffset: %d\n", program_header->p_vaddr, program_header->p_type, program_header->p_filesz, file->e_entry, program_header->p_offset);
			uint64_t virtual_limit_sz = program_header->p_vaddr + program_header->p_filesz;
			
			// map those virtual addresses 
			for(uint64_t i = program_header->p_vaddr; i<virtual_limit_sz; i+=4096){
				setMap(i, i, 1);
				uint64_t from_addr = (uint64_t)file + program_header->p_offset + (i - program_header->p_vaddr);
				memcpy((void *)i, (void *)from_addr,  4096);
			}
		}
		program_header = (Elf64_Phdr *)((uint64_t)program_header + file->e_phentsize); // program header entry size
		count--;
	}
	changeCR3((PML4E *)kernel_pml4e, (PDPE *)kernel_pdpe, (PDE *)kernel_pde, (PTE *)kernel_pte, 0);
	kprintf("%p %p %p %p\n", pml4e, pdpe, pde, pte);
}

void tarfs_read(){
	
	uint64_t start_addr = (uint64_t)&_binary_tarfs_start;
	struct posix_header_ustar *file = (struct posix_header_ustar *)start_addr;
	int index=0;
	filedir* fileDescriptor= (filedir *)get_free_page();
	kprintf("\n%p",fileDescriptor);
	filedir* head = (filedir*)fileDescriptor+index;
	head->parent=NULL;
	head->filename[0]= 'r'; 
	head->filename[1]='o';
	head->filename[2]='o';
	head->filename[3]='t';
	head->filename[4]= '\0';
	head->type=1;
	index++;
	while((uint64_t)file < (uint64_t)&_binary_tarfs_end){
		
		int size_of_file = octal_to_decimal(file->size, 11);
	//	if(size_of_file > 0){
			//if(file->name[4] == 'h' && file->name[5] == 'e'){
				kprintf("\n******\nFilename: %s\nMode: %p\nSize: %d\n", file->name, file->mode, octal_to_decimal(file->size, 11));
				Elf64_Ehdr *something = (Elf64_Ehdr *)(file + 1);
	index++;
	//index++;	
			
			if( strcmp(file->name,"")==0)
			create_File_Descriptor_Entry(file->name, index++ ,size_of_file , head); 
			//if(size_of_file >0)		
			//read_elf(something);
			//}
	//	}

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
	print_File_Descriptor();
	while(1);
}
