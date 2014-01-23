/*===============================================*/
/* pfile.cpp                                     */
/*===============================================*/

#include "pfile.h"

#ifdef __cplusplus
extern "C"{
#endif

/*===関数定義===*/

int LoadPResource(char restype[6],char filename[8]){
	FILE *fp;
	int code_size=0,errtmp=ERR_NO_ERROR;
	fpos_t filesize;
	char tmpstr[STR_LEN_MAX];
	unsigned char *tmpptr;
	errno_t errno;

	memset(tmpstr,0x00,sizeof(tmpstr));
	sprintf_s(tmpstr,STR_LEN_MAX,"DATA/%s/%s.txt",filename,restype);
	printf("LOAD:%s\n",tmpstr);
	errno=fopen_s(&fp,tmpstr,"rb");
	if(errno!=0){
		puts("File open error");
		//TODO:ダイアログを表示
		return ERR_UNDEFINED;
	}
	fseek(fp,0,SEEK_END); 
	fgetpos(fp,&filesize);
	fseek(fp,0,SEEK_SET); 
	code_size=(int)filesize;
	tmpptr=(unsigned char *)realloc(source_ptr,sizeof(unsigned char)*(code_size+1));
	source_ptr=tmpptr;
	if((source_ptr==NULL)){
		printf("Can't malloc");
		return ERR_UNDEFINED;
	}
	fread(source_ptr,sizeof(unsigned char),code_size,fp);
	source_ptr[code_size]=0x00;
	fclose(fp);
	printf("Load OK.\n");
	//TODO:ダイアログを表示
	return ERR_NO_ERROR;
}

#ifdef __cplusplus
}
#endif