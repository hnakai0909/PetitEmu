#pragma once
//#ifndef PINPUT_H_INCLUDED
//#define PINPUT_H_INCLUDED

#ifdef __cplusplus
#include "DxLib.h"
#endif
#include "data.h"
#include "evalformula.h"
#include "interpreter2.h"
#include "myutil.h"
#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char keybuffer[KEYBUFFER_MAX];
extern int keybuffer_qhead;//読む位置
extern int keybuffer_qtail;//書く位置

extern unsigned char KeyMap_A[256];
extern unsigned char KeyMap_A_S[256];
extern unsigned char KeyMap_G[256];
extern unsigned char KeyMap_G_S[256];
extern unsigned char KeyMap_K[256];
extern unsigned char KeyMap_K_S[256];
//濁点・半濁点フラグ
extern unsigned char KeyMap_K_S_DAKU[256];

void ClearKeyBuffer(void);
void WriteKeyBuffer(const unsigned char arg);
bool ReadKeyBuffer(unsigned char* arg);
void CheckPanel(void);
void CheckJoypad(void);
void InputLine(char* arg);
bool keyboard_whichkey(int x,int y,int* type,int* code);
void CheckKey(void);
unsigned char keychar2char(int code,int* daku_flag);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#endif
//#endif