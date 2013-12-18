#include "pfile.h"


//Illegalなため封印
/*
bool LoadFile_Program(void){
	//参考http://d.hatena.ne.jp/gust_notch/20120316/p1
	FILE *fp;
    int i,tmp,tmp2[8],data_size=0,code_size=0,version_num=0;
	char filebuf[STR_LEN_MAX],tmpstr[STR_LEN_MAX],tmpdata[STR_LEN_MAX],filename[8],tmpc;
	unsigned char *tmpptr;
	memset(tmpstr,0x00,sizeof(tmpstr));
	GetCurrentDirectory(STR_LEN_MAX,tmpstr);
	printf("%s\n",tmpstr);
	memset(tmpstr,0x00,sizeof(tmpstr));
	fp=fopen("RPRG000.PTC", "rb");
	if(fp==NULL){puts("File open error");return false;}
	fseek(fp,0,SEEK_SET);
	fread(tmpstr,sizeof(char),4,fp);
	if(strcmp(tmpstr,"PX01")!=0){puts("Illegal header");return false;}
	for(i=0;i<4;i++)data_size+=fgetc(fp)*(int)pow(STR_LEN_MAX.0,(double)(i));
	fseek(fp,4,SEEK_CUR);
	memset(filename,0x00,sizeof(filename));
    fread(filename,sizeof(char),8,fp);
	//MD5
	fread(tmpdata,sizeof(char),16,fp);
	memset(tmpstr,0x00,sizeof(tmpstr));
    fread(tmpstr,sizeof(char),4,fp);
	if(strcmp(tmpstr,"PETC")!=0){puts("Illegal DATA header");return false;}
	for(i=0;i<4;i++){
		tmpc=fgetc(fp);
		if(!isdigit(tmpc)){puts("Illegal DATA header Version String");return false;}
		version_num+=(tmpc-'0')*(int)pow(10.0,(double)(3-i));
	}
	//初代
	if(version_num==100){
		puts("Version:1.0 - 1.2");
		//中間コードらしい
	//mkII
	}else if(version_num==300){
		puts("Version:2.0 - 2.3");
				
	}else{
		puts("Version:Unknown");
		printf("%d",version_num);
		return false;
	}
	memset(tmpstr,0x00,sizeof(tmpstr));
    fread(tmpstr,sizeof(char),4,fp);
	if(strcmp(tmpstr,"RPRG")!=0){puts("Illegal DATA header");return false;}
	fseek(fp,8,SEEK_CUR);
	fread(&code_size,sizeof(uint16_t),1,fp);
	printf("%d",code_size);
	tmpptr=(unsigned char *)realloc(source_ptr,sizeof(unsigned char)*(code_size+1));
	source_ptr=tmpptr;
	if((source_ptr==NULL)){printf("Can't malloc");return false;}
	fread(source_ptr,sizeof(unsigned char),code_size,fp);

    fclose(fp);
	//MessageBox(GetMainWindowHandle(),TEXT("Kitty on your lap"),TEXT("メッセージボックス"),MB_OK);
	return true;
}
*/



#ifdef __cplusplus
extern "C"{
#endif

int LoadPResource(char restype[6],char filename[8]){
	FILE *fp;
    int code_size=0,errtmp=ERR_NO_ERROR;
	fpos_t filesize;
	char tmpstr[STR_LEN_MAX];
	unsigned char *tmpptr;

	memset(tmpstr,0x00,sizeof(tmpstr));
	sprintf(tmpstr,"DATA/%s/%s.txt",filename,restype);
	printf("LOAD:%s\n",tmpstr);
	fp=fopen(tmpstr,"rb");
	if(fp==NULL){
		puts("File open error");
		//ダイアログ出す
		return ERR_UNDEFINED;
	}
	fseek(fp,0,SEEK_END); 
	fgetpos(fp,&filesize);
	fseek(fp,0,SEEK_SET); 
	code_size=filesize;
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
	return ERR_NO_ERROR;
}

#ifdef __cplusplus
}
#endif