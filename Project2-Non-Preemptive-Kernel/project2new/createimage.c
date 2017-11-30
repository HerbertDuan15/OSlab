#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void write_bootblock(FILE *imagefile, FILE *fp, Elf32_Ehdr **elf,Elf32_Phdr **ph,uint8_t buf[]);
void write_kernel(FILE *imagefile, FILE *fp, Elf32_Ehdr **elf,Elf32_Phdr **ph,uint8_t buf[]);
void write_process(FILE *imagefile, FILE *fp, Elf32_Ehdr **elf,Elf32_Phdr **ph,uint8_t buf[],int process_num);

int main(int argc, char *argv[]){
	
	FILE * bootblock = fopen("./bootblock","rb");
	FILE * kernel = fopen("./kernel","rb");
	FILE * image = fopen("./image","wb");
	FILE * process1 = fopen("./process1","rb");
	FILE * process2 = fopen("./process2","rb");
	FILE * process3 = fopen("./process3","rb");
	Elf32_Ehdr *bootblock_elf;
	Elf32_Phdr *bootblock_ph = NULL;
	uint8_t bootblock_buf[4096];
	Elf32_Ehdr *kernel_elf;
	Elf32_Phdr *kernel_ph = NULL;
	uint8_t kernel_buf[65536];
	Elf32_Ehdr *process1_elf;
	Elf32_Phdr *process1_ph = NULL;
	Elf32_Ehdr *process2_elf;
	Elf32_Phdr *process2_ph = NULL;
	Elf32_Ehdr *process3_elf;
	Elf32_Phdr *process3_ph = NULL;
	uint8_t process_buf[65536];
	memset(bootblock_buf,0,4096);
	memset(kernel_buf,0,65536);
	memset(process_buf,0,65536);
	write_bootblock(image,bootblock,&bootblock_elf,&bootblock_ph,bootblock_buf);
	write_kernel(image,kernel,&kernel_elf,&kernel_ph,kernel_buf);
	write_process(image,process1,&process1_elf,&process1_ph,process_buf,1);
	
	memset(process_buf,0,65536);
	write_process(image,process2,&process2_elf,&process2_ph,process_buf,2);
	
	memset(process_buf,0,65536);
	write_process(image,process3,&process3_elf,&process3_ph,process_buf,3);
	
	fclose(bootblock);
	fclose(kernel);
	fclose(image);
	fclose(process1);
	fclose(process2);
	fclose(process3);
	return 0;
}

void write_bootblock(FILE *imagefile, FILE *fp, Elf32_Ehdr **elf,Elf32_Phdr **ph,uint8_t buf[])
{	
	int i = 0;
	fread(buf,1,4096,fp);
	(*elf) = (void *)buf;

	for(i = 0, (*ph) = (void *)buf + (*elf)->e_phoff; i < (*elf)->e_phnum; i ++) {
		if ((*ph)[i].p_type == PT_LOAD) {
		uint32_t offset = (*ph)[i].p_offset;
		uint32_t filesize = (*ph)[i].p_filesz;
		uint32_t memsize = (*ph)[i].p_memsz;
		uint8_t data[filesize];

		fseek(fp,offset,SEEK_SET);
		fread(data,1,filesize,fp);
		fwrite(data,1,filesize,imagefile);
		}
	}	
}

void write_kernel(FILE *imagefile, FILE *fp, Elf32_Ehdr **elf,Elf32_Phdr **ph,uint8_t buf[])
{	

	int i = 0;
	fread(buf,1,65536,fp);
	(*elf) = (void *)buf;
	fseek(imagefile,512,SEEK_SET);

	for(i = 0, (*ph) = (void *)buf + (*elf)->e_phoff; i < (*elf)->e_phnum; i ++) {
		if ((*ph)[i].p_type == PT_LOAD) {
		uint32_t offset = (*ph)[i].p_offset;
		uint32_t filesize = (*ph)[i].p_filesz;
		uint32_t memsize = (*ph)[i].p_memsz;
		uint8_t data[filesize];

		fseek(fp,offset,SEEK_SET);
		fread(data,1,filesize,fp);
		fwrite(data,1,filesize,imagefile);
		}
	}	
}

void write_process(FILE *imagefile, FILE *fp, Elf32_Ehdr **elf,Elf32_Phdr **ph,uint8_t buf[],int process_num)
{	

	int i = 0;
	uint32_t process_offset=65536*process_num;
	fread(buf,1,65536,fp);
	(*elf) = (void *)buf;
	fseek(imagefile,process_offset,SEEK_SET);

	for(i = 0, (*ph) = (void *)buf + (*elf)->e_phoff; i < (*elf)->e_phnum; i ++) {
		if ((*ph)[i].p_type == PT_LOAD) {
		uint32_t offset = (*ph)[i].p_offset;
		uint32_t filesize = (*ph)[i].p_filesz;
		uint32_t memsize = (*ph)[i].p_memsz;
		uint8_t data[filesize];

		fseek(fp,offset,SEEK_SET);
		fread(data,1,filesize,fp);
		fwrite(data,1,filesize,imagefile);
		}
	}	
}


