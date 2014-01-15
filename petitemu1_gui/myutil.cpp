/*===============================================*/
/* myutil.cpp                                    */
/*===============================================*/

#include "myutil.h"

/*===関数定義===*/

bool inrange(int arg,int min,int max){
	return (arg>=min)&&(arg<=max);
}

int limitrange(int arg,int min,int max){
	return arg*inrange(arg,min,max)+min*(arg<min)+max*(arg>max);
}

int isdigits(st str){
	int i=0;
	if(str.s[i]==0)return false;
	for(i=0;(i<str.len);i++){
		if(!isdigit(str.s[i]))return false;
	}
	return true;
}

int isintliteral(st str){
	int i=0,flag=0;
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

st str2mystr(char* arg){
	st str={0,""};
	unsigned int length=strlen(arg);
	if(length>256)return str;
	str.len=length;
	strcpy(str.s,arg);	
	return str;
}
void mystrcpy(st *str1,st str2){
	memcpy(str1->s,str2.s,256);
	str1->len=str2.len;
	return;
}
bool mystrcpy2(st *str1,char arg[256]){
	unsigned int length=strlen(arg);
	if(length>256)return false;
	strcpy(str1->s,arg);
	str1->len=strlen(arg);
	return true;
}
bool mystrcat(st *str1,st str2){
	if((str1->len+str2.len)>256)return false;
	memcpy(str1->s+str1->len,str2.s,str2.len);
	str1->len+=str2.len;
	return true;
}
void mystrclear(st *str){
	memset(str,0x00,sizeof(str));
	return;
}