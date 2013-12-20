/*===============================================*/
/* pfile.h                                       */
/*===============================================*/

#ifndef PFILE_H_INCLUDED
#define PFILE_H_INCLUDED

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "evalformula.h"

#ifdef __cplusplus
#include "DxLib.h"
#endif

#ifdef __cplusplus
extern "C"{
#endif

/*===関数プロトタイプ宣言===*/
//リソースをロード(現在PRGのみ対応)
int LoadPResource(char restype[6],char filename[8]);

#ifdef __cplusplus
}
#endif

#endif