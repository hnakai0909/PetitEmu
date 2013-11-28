//interpreter2.h
#pragma once

#ifndef INTERPRETER2_H_INCLUDED
#define INTERPRETER2_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS

#ifdef __cplusplus
#include "DxLib.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include "data.h"
#include "evalformula.h"
#include "myutil.h"
#include "pgui.h"
#include "pfile.h"

#define GOTOLINE(line) srcpos=translated_source+srcline_begin_token_pos[line];Psys_ERL=line;

#ifdef __cplusplus
extern "C" {
#endif

struct ForGosubStack{
	int line;
	int col;
	int VarID;
	int32_t step;
};

extern struct ForGosubStack ForGosub_s[FORGOSUB_S_MAX];
extern unsigned int ForGosub_sl;

struct Label {
	char name[8];
	int line;
};

char hex2int(const char arg);
char dec2int(const char arg);
int Str2VarID(const char* arg);
uint16_t* GetVarID(uint16_t* p,int* tmpint,int* errtmp);
void IncrementSrcPos(void);
int ProcessRemainingOperator(void);
uint16_t* JumpSpace(uint16_t* p);
uint16_t* ForJump(uint16_t* p,int* errtmp);
uint16_t* ReadFormula(uint16_t* p,int* errtmp);
void TranslateRaw2Code(unsigned char* input,uint16_t* output,int* outlen);
void TranslateCode2Raw(uint16_t* input,unsigned char* output);
void RunInteractive(char* input);
int RunProgram(void);
int ResistLabel(uint16_t* input);
uint16_t* GetLabelName(uint16_t* p,char* tmpstr,int* errtmp);
int ReadGotoLine(uint16_t *input,unsigned int line);
int ReadSeekNext(void);
int Interpretation(uint16_t* input,int srclen,bool interactive_flag,int* runflag);

int NewVar(char* name);

#ifdef __cplusplus
}
#endif

#endif