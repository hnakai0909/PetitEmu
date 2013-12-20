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

int isdigits(char* arg){
	int i=0;
	for(i=0;(i<256)&&(arg[i]!=0);i++){
		if(!isdigit(arg[i]))return false;
	}
	return true;
}