//#pragma once

# //ifndef PGUI_H_INCLUDED
# //define PGUI_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS

#ifdef __cplusplus
#include "DxLib.h"
#endif
#include "data.h"
#include "evalformula.h"
#include "pinput.h"
#ifdef __cplusplus
extern "C" {
#endif
extern int GrHandle[10]; 
bool ProcessFrame(void);
void LineInput(char* arg);
void DrawConsole(void);
void DrawKeyboard(void);
void DrawFuncKey(void);
void DrawUnderPanel(void);
void ProcessBG(void);
void DrawBG(void);
void DrawBGChip(int page,int layer,int ofsx_1,int ofsx_2,int ofsy_1,int ofsy_2);
void ConsoleClearLine(void);
int PutChar2Console(uint16_t arg,int indent_option);
void DisplayConsole(void);
int Print2Console(char *string,int indent_option);
void ScrollConsole(void);
void PutStartMessage(void);
void DrawDebugScreen(void);
#ifdef __cplusplus
}
#endif
# //endif