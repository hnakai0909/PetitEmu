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

int isintliteral(char* arg){
	int i=0,flag=0;
	if(arg[i]==0)return false;
	for(i=0;(i<256)&&(arg[i]!=0);i++){
		if(!(isdigit(arg[i])||((arg[i]=='-')&&(i==0))||((arg[i]=='.')&&(flag==0))))return false;
		if(arg[i]=='.')flag=1;
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