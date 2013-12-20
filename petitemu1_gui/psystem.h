/*===============================================*/
/* psystem.h                                     */
/*===============================================*/

#ifndef PSYSTEM_H_INCLUDED
#define PSYSTEM_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS

#include "DxLib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

extern "C"{

#include "interpreter2.h"
#include "pfile.h"
#include "evalformula.h"
#include "resource.h"

enum SystemMode{
	SPLASH=0,
	CONSOLE,
	SOURCE,
	MANUAL,
	INTERPRETATION
};

/*===関数プロトタイプ宣言===*/

//ウィンドウ初期化処理
bool InitWindow(void);

//ボタン等用画像初期化処理
void InitPicture(void);

//音声初期化処理
void InitSound(void);

//グローバル変数初期化等処理
int PSysInit(void);

//システム初期化処理
int SystemInit(void);

}

#endif