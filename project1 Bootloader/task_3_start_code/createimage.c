#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//void extended_opt(Elf32_Phdr *boot_phdr, Elf32_Ehdr *b_ehdr, Elf32_Phdr *kernel_phdr,Elf32_Ehdr *k_ehdr);
//int count_kernel_sectors(Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr);
void write_kernel(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr);
void write_bootblock(FILE **imagefile, FILE *boot_file, Elf32_Ehdr *boot_header, Elf32_Phdr *boot_phdr);
Elf32_Phdr * read_exec_file(FILE **execfile, char *filename, Elf32_Ehdr *ehdr); 
void record_kernel_sectors(FILE **image_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr, int num_sec);
void write_process1(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr);
void write_process2(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr);
void write_process3(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr);

int main(int argc, char *argv[]){

	FILE* bootblock;
	Elf32_Ehdr bk_ehdr;
	Elf32_Phdr* bk_phdr;
	bk_phdr=(Elf32_Phdr*)malloc(sizeof(Elf32_Phdr));
	bk_phdr=read_exec_file(&bootblock,"bootblock",&bk_ehdr);
	
	
	FILE* kernel;
	Elf32_Ehdr kl_ehdr;
	Elf32_Phdr* kl_phdr;
    kl_phdr=(Elf32_Phdr*)malloc(sizeof(Elf32_Phdr));  
	kl_phdr=read_exec_file(&kernel,"kernel",&kl_ehdr);
	
	FILE* process1;
	Elf32_Ehdr process1_ehdr;
	Elf32_Phdr* process1_phdr;
    process1_phdr=(Elf32_Phdr*)malloc(sizeof(Elf32_Phdr));  
	process1_phdr=read_exec_file(&process1,"process1",&kl_ehdr);
	
	FILE* process2;
	Elf32_Ehdr process2_ehdr;
	Elf32_Phdr* process2_phdr;
    process2_phdr=(Elf32_Phdr*)malloc(sizeof(Elf32_Phdr));  
	process2_phdr=read_exec_file(&process1,"process2",&kl_ehdr);
	
	FILE* process3;
	Elf32_Ehdr process3_ehdr;
	Elf32_Phdr* process3_phdr;
    process3_phdr=(Elf32_Phdr*)malloc(sizeof(Elf32_Phdr));  
	process3_phdr=read_exec_file(&process1,"process3",&kl_ehdr);
	
	FILE* image;
	int i;
	image=fopen("image","w+");
	if(!image) perror("error");
	write_bootblock(&image,bootblock,&bk_ehdr,bk_phdr);
    write_kernel(&image,kernel,&kl_ehdr,kl_phdr);
	write_process1(&image,process1,&process1_ehdr,process1_phdr);
	write_process2(&image,process2,&process2_ehdr,process2_phdr);
    write_process3(&image,process3,&process3_ehdr,process3_phdr);	

	
	int n = count_kernel_sectors(&kl_ehdr,kl_phdr);
//	record_kernel_sectors(&image,&kl_ehdr,kl_phdr,n);
 //   extended_opt(bk_phdr,&bk_ehdr,kl_phdr,&kl_ehdr);
}


Elf32_Phdr * read_exec_file(FILE **execfile, char *filename, Elf32_Ehdr *ehdr)
{
	Elf32_Phdr* ph;
	ph=(Elf32_Phdr*)malloc(sizeof(Elf32_Phdr));
	
    if(filename=="bootblock"){
	    if(!(*execfile=fopen("bootblock","r"))) 
			printf("error's number:%d\n",errno);
	}
	else{
		if(!(*execfile=fopen("kernel","r"))) 
			printf("error's num:%d\n",errno);	
	} 	
	fseek(*execfile, 0, SEEK_SET);
	fread(ehdr,sizeof(Elf32_Ehdr),1,*execfile);//read the elfheadder
	fseek(*execfile,ehdr->e_phoff,0);
	fread(ph,ehdr->e_phnum*ehdr->e_phentsize,1,*execfile);
	return ph; 
}

void write_bootblock(FILE **imagefile, FILE *boot_file, Elf32_Ehdr *boot_header, Elf32_Phdr *boot_phdr)
{
	uint8_t buf[512];
    memset(buf, 0, 512);
    fseek(boot_file,boot_phdr->p_offset,0);
    fread(buf,boot_phdr->p_filesz,1,boot_file);
	fwrite(buf,1,512,*imagefile); 
}

void write_kernel(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr)
{   
    int seg_num = 127;
	uint8_t buf[512*seg_num];
    memset(buf, 0, 512 * seg_num);
    fseek(kernel_file,kernel_phdr->p_offset,0);               
    fread(buf,kernel_phdr->p_filesz,1,kernel_file);
    fseek(*image_file,512,0);
    fwrite(buf, 512*seg_num, 1 ,*image_file);
}


void write_process1(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr)
{   
    int seg_num = 128;
	uint8_t buf[512*seg_num];
    memset(buf, 0, 512 * seg_num);
    fseek(kernel_file,kernel_phdr->p_offset,0);               
    fread(buf,kernel_phdr->p_filesz,1,kernel_file);
    fseek(*image_file,66535,0);
    fwrite(buf, 512*seg_num, 1 ,*image_file);
}

void write_process2(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr)
{   
    int seg_num = 128;
	uint8_t buf[512*seg_num];
    memset(buf, 0, 512 * seg_num);
    fseek(kernel_file,kernel_phdr->p_offset,0);               
    fread(buf,kernel_phdr->p_filesz,1,kernel_file);
    fseek(*image_file,131072,0);
    fwrite(buf, 512*seg_num, 1 ,*image_file);
}
	
void write_process3(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr)
{   
    int seg_num = 128;
	uint8_t buf[512*seg_num];
    memset(buf, 0, 512 * seg_num);
    fseek(kernel_file,kernel_phdr->p_offset,0);               
    fread(buf,kernel_phdr->p_filesz,1,kernel_file);
    fseek(*image_file,196608,0);
    fwrite(buf, 512*seg_num, 1 ,*image_file);
}

int count_kernel_sectors(Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){
	int size = kernel_phdr->p_filesz;
	int seg_num;
	seg_num = size / 512;
	if(seg_num * 512 != size) seg_num++;
 	return seg_num;
}

void extended_opt(Elf32_Phdr *boot_phdr, Elf32_Ehdr *b_ehdr, Elf32_Phdr *kernel_phdr,Elf32_Ehdr *k_ehdr){	
	/*文件的大小和扇区个数  
	segment在文件中的偏移 
	在内存中，segment的偏移
	文件的segment的大小 
	在内存中segment的大小 
	文件制作的imge的对齐字节数*/
	printf("\n Bootblock Information:");
	
	printf("\n 程序头表条目数目: %hx",b_ehdr->e_phnum);
	printf("\n tne number of blocks: %hx",count_kernel_sectors(b_ehdr,boot_phdr));
	printf("\n file size: %hx",boot_phdr->p_filesz);
	printf("\n memory size: %hx",boot_phdr->p_memsz);
	printf("\n segment offset in file: %hx",boot_phdr->p_offset);
	printf("\n segment offset in memory: %hx",boot_phdr->p_vaddr);
	printf("\n imge align: %hx",boot_phdr->p_align);
	
	
	printf("\n\nKernel Information:");
	printf("\n 程序头表条目数目: %hx",k_ehdr->e_phnum);
	printf("\n tne number of blocks: %hx",count_kernel_sectors(k_ehdr,kernel_phdr));
	printf("\n file size: %hx",kernel_phdr->p_filesz);
	printf("\n memory size: %hx",kernel_phdr->p_memsz);
	printf("\n segment offset in file: %hx",kernel_phdr->p_offset);
	printf("\n segment offset in memory: %hx",kernel_phdr->p_vaddr);
	printf("\n imge align: %hx",kernel_phdr->p_align);
	
}

void record_kernel_sectors(FILE **image_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr, int num_sec){
      
	  fseek(*image_file, 0x0a0, SEEK_SET);
	  int *num = &num_sec;
	  fwrite(num,4,1,*image_file);
}


