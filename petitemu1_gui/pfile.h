//#ifndef PFILE_H_INCLUDED
//#define PFILE_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS

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
	
//bool LoadFile_Program(void);
int LoadPResource(char restype[6],char filename[8]);

#ifdef __cplusplus
}
#endif

//#endif