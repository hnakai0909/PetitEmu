/*===============================================*/
/* myutil.h                                      */
/*===============================================*/

#ifndef MYUTIL_H_INCLUDED
#define MYUTIL_H_INCLUDED

#include <ctype.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef bool
#define bool int
#endif

#define MYSTR_NULL (str2mystr2(""))

typedef struct{
	unsigned int len;
	unsigned char s[256];
} st;

/*===関数プロトタイプ宣言===*/
//min<=arg<=maxか判定
bool inrange(int arg,int min,int max);

//argをmin<=x<=maxに制限した値を返す
int limitrange(int arg,int min,int max);

int isdigits(char* arg);

//その文字列は数値リテラルか(簡易判定)
int isintliteral(char* arg);

char hex2int(const char arg);
char dec2int(const char arg);

st str2mystr(unsigned char arg[256]);
st str2mystr2(char arg[256]);
void mystr2str2(st str,char* arg);
bool mystrcpy2(st *str1,char arg[256]);
bool mystrcat(st *str1,st str2);
int mystrcmp(st str1,st str2);
int mystrcmp2(st str1,char arg[256]);
void mystrclear(st *str);
unsigned int my_strlen(unsigned char arg[256]);

#ifdef __cplusplus
}
#endif
#endif