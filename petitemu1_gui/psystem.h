#ifndef PSYSTEM_H_INCLUDED
#define PSYSTEM_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS

#include "DxLib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

extern "C"{

#include "evalformula.h"
#include "interpreter2.h"
#include "pfile.h"

enum SystemMode{
	SPLASH=0,
	CONSOLE,
	SOURCE,
	MANUAL,
	INTERPRETATION
};

extern enum KeyBoardMode keyboardmode;
extern unsigned char keybuffer[KEYBUFFER_MAX];
extern int keybuffer_qhead;//読む位置
extern int keybuffer_qtail;//書く位置

bool InitWindow(void);
void InitPicture(void);
void InitSound(void);
void PSysInit(void);
int SystemInit(void);

}

#endif