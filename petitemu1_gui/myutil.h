//#ifndef MYUTIL_H_INCLUDED
//#define MYUTIL_H_INCLUDED

#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

int inrange(int arg,int min,int max);

int isdigits(char* arg);

#ifdef __cplusplus
}
#endif
//#endif