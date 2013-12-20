/*===============================================*/
/* myutil.h                                      */
/*===============================================*/

#ifndef MYUTIL_H_INCLUDED
#define MYUTIL_H_INCLUDED

#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef bool
#define bool int
#endif

/*===関数プロトタイプ宣言===*/
//min<=arg<=maxか判定
bool inrange(int arg,int min,int max);

//argをmin<=x<=maxに制限した値を返す
int limitrange(int arg,int min,int max);

//その文字列が0-9のみで構成されているか
int isdigits(char* arg);

#ifdef __cplusplus
}
#endif
#endif