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
#include<sys/mm_struct.h>
#include<sys/vm_area_struct.h>

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

void load_vmas(Elf64_Ehdr *file, struct mm_struct* task_mm_struct){
    
    uint16_t num_pheaders = (uint64_t)file->e_phnum;
    //kprintf("Number of header: %d\n", num_pheaders);
    //El64_Phdr pg_headers[num_pheaders];
    //struct mm_struct* task_mm_struct = get_mm_struct();
    
    for(uint16_t ph_num = 0; ph_num < num_pheaders; ph_num++){
        Elf64_Phdr *pg_header = (Elf64_Phdr*)((uint64_t)file + (ph_num*file->e_phoff));
        //if(pg_header->p_type == 0x01){ //Loadable segment
            uint64_t offset = pg_header->p_offset;
            uint64_t v_addr = pg_header->p_vaddr;
            uint64_t flags  = pg_header->p_flags;
            uint64_t size   = pg_header->p_memsz;
            uint64_t prot   = 0; //What do I set??
            uint64_t start  = pg_header->p_vaddr;
            uint64_t end    = start+ size;
            struct vm_area_struct new_vma;
            new_vma.vm_start = start;
            new_vma.vm_end   = end;
            new_vma.vm_flags = flags;
            new_vma.max_size = size;
            new_vma.vm_page_prot = prot;
            new_vma.next = NULL;
            new_vma.prev = NULL;
            kprintf("\nAdd VMA called\n");
            print_vma_content(&new_vma);
            addVMAtoTask(runningTask, &new_vma );
            print_all_vmas(task_mm_struct);
            while(1);
        //} 
    }   
    kprintf("\nLoad VMAs");
    print_all_vmas(task_mm_struct);
    while(1);
   
}



void read_elf(Elf64_Ehdr *file ,int index){
		
	
	Elf64_Phdr *program_header = (Elf64_Phdr *)((uint64_t)file + file->e_phoff);
	// kprintf("\nProgram Header %p Elf address %p\n", (uint64_t)program_header, (uint64_t)file)
	
//	vm_area_stack =init_vm_area_struct(   STACK_REGION_END,STACK_REGION_START,  );	
	

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
	createTask((void *)file->e_entry, rflags, cr3);
	//load_vmas(file, runningTask->mm);
	changeCR3(runningTask->pml4e, runningTask->pdpe, runningTask->pde, runningTask->pte, 1);
	kprintf(" || time %p %p %p %p\n", pml4e, pdpe, pde, pte);
	int count = file->e_phnum;
	while(count > 0){
		if(program_header->p_type != 0 && program_header->p_filesz > 0){
			kprintf("Virtual addr: %p\ntype: %d\nFile size: %d\nEntry point: %x\nPOffset: %d\n", program_header->p_vaddr, program_header->p_type, program_header->p_filesz, file->e_entry, program_header->p_offset);
			uint64_t virtual_limit_sz = program_header->p_vaddr + program_header->p_filesz;
//		vm_area_struct new_vma =  init_vm_area_struct(program_header->p_vaddr , program_header->p_memsz+ program_header->p_vaddr,MAP ,NULL,NULL );
			// map those virtual addresses 
			for(uint64_t i = program_header->p_vaddr; i<virtual_limit_sz; i+=4096){
				setMap(i, i+index*0x10000, 1);
				uint64_t from_addr = (uint64_t)file + program_header->p_offset + (i - program_header->p_vaddr);
				memcpy((void *)i, (void *)from_addr,  4096);
			}
		}
		program_header = (Elf64_Phdr *)((uint64_t)program_header + file->e_phentsize); // program header entry size
		count--;
	}
	setMap(runningTask->regs.user_rsp, runningTask->regs.user_rsp, 1);  // mapping the stacks as well
	changeCR3((PML4E *)kernel_pml4e, (PDPE *)kernel_pdpe, (PDE *)kernel_pde, (PTE *)kernel_pte, 0);
	kprintf("%p %p %p %p\n", pml4e, pdpe, pde, pte);

}

void tarfs_read(){
	
	uint64_t start_addr = (uint64_t)&_binary_tarfs_start;
	struct posix_header_ustar *file = (struct posix_header_ustar *)start_addr;
	int index=0;
	fileDescriptor = (filedir *)get_free_pages(3);
	currentPath= (char*)get_free_page();
	currentfile= (filedir*)get_free_page();
	kprintf("\n%p",fileDescriptor);
	filedir* head = create_file_entry("root/", index, 0, 0);
	
	

	while((uint64_t)file <= (uint64_t)&_binary_tarfs_end ){
		
		int size_of_file = octal_to_decimal(file->size, 11);
		//kprintf("\n******Filename: %s Mode: %p Size: %d\n", file->name, file->mode, octal_to_decimal(file->size, 11));
		Elf64_Ehdr *something = (Elf64_Ehdr *)(file + 1);
		index++;
		if(strcmp(file->name, "bin/cat")==1){
		kprintf("Adding %s: %d %p < %p\n", file->name, index, file, (uint64_t)&_binary_tarfs_end);
			sbush_elf = something;
		}
		if(strcmp(file->name, "bin/init")==1){
			idle_elf = something;
		}	
		if(strlen(file->name)!=0){
			
			create_File_Descriptor_Entry(file->name, index ,size_of_file , head, (uint64_t)file+512); 
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
	setCurrentPath("root/");	
}

Elf64_Ehdr *findElfByName(char *filename){

	uint64_t start_addr = (uint64_t)&_binary_tarfs_start;
	struct posix_header_ustar *file = (struct posix_header_ustar *)start_addr;
	Elf64_Ehdr *desired_elf;
	while((uint64_t)file < (uint64_t)&_binary_tarfs_end){
		
		int size_of_file = octal_to_decimal(file->size, 11);
		//kprintf("\n******Filename: %s Mode: %p Size: %d\n", file->name, file->mode, octal_to_decimal(file->size, 11));
		Elf64_Ehdr *the_elf = (Elf64_Ehdr *)(file + 1);
		if(strcmp(file->name, filename) == 1){
			desired_elf = the_elf;
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
	return desired_elf;
}


void load_binary(Elf64_Ehdr *file, int index){
		
	Elf64_Phdr *program_header = (Elf64_Phdr *)((uint64_t)file + file->e_phoff);
	
	runningTask->regs.rip = (uint64_t)file->e_entry;
	int count = file->e_phnum;
	while(count > 0){
		if(program_header->p_type != 0 && program_header->p_filesz > 0){
			uint64_t virtual_limit_sz = program_header->p_vaddr + program_header->p_filesz;
			// map those virtual addresses 
			for(uint64_t i = program_header->p_vaddr; i<virtual_limit_sz; i+=4096){
				setMap(i, i+index*0x10000, 1);
				uint64_t from_addr = (uint64_t)file + program_header->p_offset + (i - program_header->p_vaddr);
				memcpy((void *)i, (void *)from_addr,  4096);
			}
		}
		program_header = (Elf64_Phdr *)((uint64_t)program_header + file->e_phentsize); // program header entry size
		count--;
	}
	kprintf("\n Binary Loaded \n");
}
