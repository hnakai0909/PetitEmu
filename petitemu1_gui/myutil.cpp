/*===============================================*/
/* myutil.cpp                                    */
/*===============================================*/

#include "myutil.h"

/*===関数定義===*/

int mymod(int a,int b){
	return a%b+(a<0)?a:0;
}

bool inrange(int arg,int min,int max){
	return (arg>=min)&&(arg<=max);
}

int limitrange(int arg,int min,int max){
	return arg*inrange(arg,min,max)+min*(arg<min)+max*(arg>max);
}

int isdigits(st str){
	unsigned int i=0;
	if(str.s[i]==0)return false;
	for(i=0;(i<str.len);i++){
		if(!isdigit(str.s[i]))return false;
	}
	return true;
}

int isintliteral(st str){
	unsigned int i=0,flag=0;
	if(str.s[i]==0)return false;
	for(i=0;(i<str.len);i++){
		if(!(isdigit(str.s[i])||((str.s[i]=='-')&&(i==0))||((str.s[i]=='.')&&(flag==0))))return false;
		if(str.s[i]=='.')flag=1;
	}
	return true;
}

char hex2int(const char arg){
	if(arg>='A' && arg<='F')
		return arg-'A'+10;
	if(arg>='a' && arg<='f')
		return arg-'a'+10;
	if(arg>='0' && arg<='9')
		return arg-'0';
	return -1;
}

char dec2int(const char arg){
	if(arg>='0' && arg<='9')
		return arg-'0';
	return -1;
}
st str2mystr(unsigned char arg[256]){
	st str={0,""};
	unsigned int length=my_strlen(arg);
	if(length>256)return str;
	str.len=length;
	memcpy(str.s,arg,256);
	return str;
}
st str2mystr2(char arg[256]){
	st str={0,""};
	unsigned int length=strlen(arg);
	if(length>256)return str;
	str.len=length;
	memcpy(str.s,arg,256);
	return str;
}
void mystr2str2(st str,char* arg){
	memset(arg,0x00,sizeof(char)*256);
	memcpy(arg,str.s,256);
	if(str.len<256)str.s[str.len]=0;
	return;
}
bool mystrcpy2(st *str1,char arg[256]){
	unsigned int length=strlen(arg);
	if(length>256)return false;
	memcpy(str1->s,arg,256);
	str1->len=length;
	return true;
}
bool mystrcat(st *str1,st str2){
	if((str1->len+str2.len)>256)return false;
	memcpy(str1->s+str1->len,str2.s,str2.len);
	str1->len+=str2.len;
	return true;
}
int mystrcmp(st str1,st str2){
	unsigned int i=0;
	if(str1.len>str2.len)return 1;
	if(str1.len<str2.len)return -1;
	if(str1.len<256)str1.s[str1.len]=0;
	if(str2.len<256)str2.s[str2.len]=0;	
	for(;(str1.s[i]==str2.s[i])&&(i<str1.len);i++);
	return !(str2.s[i]==str1.s[i]);
}
int mystrcmp2(st str1,char arg[256]){
	unsigned int i=0;
	unsigned int arglen=strlen(arg);
	if(str1.len>arglen)return 1;
	if(str1.len<arglen)return -1;
	for(;(str1.s[i]==arg[i])&&(i<str1.len);i++);
	return !(arg[i]==str1.s[i]);
}
void mystrclear(st *str){
	memset(str,0x00,sizeof(str));
	return;
}
//NOT mystr_len BUT my_strlen
unsigned int my_strlen(unsigned char arg[256]){
	unsigned int i=0;
	for(i=0;i<256;i++){
		if(arg[i]==0)return i;
	}
	return 256;
}