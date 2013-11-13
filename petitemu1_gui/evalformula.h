#pragma once
#define _CRT_SECURE_NO_WARNINGS

#ifndef EVALFORMULA_H_INCLUDED
#define EVALFORMULA_H_INCLUDED

#ifdef __cplusplus
#include "DxLib.h"
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "data.h"
#include "pinput.h"

#define FloorInt(a) ((int)((a)/4096))

#ifdef __cplusplus
extern "C" {
#endif

struct VARIABLE {
	bool isDim;
	bool isStr;
	char name[9];
	char *address;
	int32_t value;
	char string[STR_LEN_MAX];
};

struct OP_S{
	int op;
	unsigned int argcount;
};

struct CalcStack {
	//0 int 0D
	//1 int 1Dor2D
	//2 str 0D
	//3 str 1Dor2D
	char type;
	int32_t value;
	char string[STR_LEN_MAX];
	int argcount;
};

extern bool log_en;
extern int log_en2;

extern struct VARIABLE Variable[VAR_MAX];

extern struct OP_S op_s[OP_S_MAX];//演算子スタック
extern struct CalcStack calc_s[CALC_S_MAX];//計算スタック

extern char consolecharbuf[32][24];
extern char consolecolorbuf[32][24];

//プチコンシステム変数
extern int Psys_VERSION;
extern int Psys_TRUE;
extern int Psys_FALSE0;
extern int Psys_CANCEL;
extern int Psys_CSRX;
extern int Psys_CSRY;
extern int Psys_FREEMEM;
extern int Psys_ERR;
extern int Psys_ERL;
extern int Psys_RESULT;
extern int Psys_TCHX;
extern int Psys_TCHY;
extern int Psys_TCHST;
extern int Psys_TCHTIME;
extern int Psys_MAINCNTL;
extern int Psys_MAINCNTH;
extern int Psys_TABSTEP;
extern int Psys_ICONPUSE;
extern int Psys_ICONPAGE;
extern int Psys_ICONPMAX;
extern int Psys_FUNCNO;
extern int Psys_FREEVAR;
extern bool Psys_SYSBEEP;
extern char Psys_TIME[STR_LEN_MAX];
extern char Psys_DATE[STR_LEN_MAX];
extern char Psys_MEM[STR_LEN_MAX];

extern int consolecolor;
extern int SHandleBEEP[70];
extern int SHandleBGM;

extern unsigned int calc_sl;//スタックの現在の深さ
extern unsigned int op_sl;//スタックの現在の深さ

enum PanelMode{
	PNLMD_OFF,PNLMD_PANEL,PNLMD_KYA,PNLMD_KYM,PNLMD_KYK,PNLMD_DIALOG
};

extern bool kbd_shift_flag;
extern int button_state;
extern enum PanelMode panelmode;

enum RunMode{
	RMD_STOP,RMD_LINE,RMD_PRG
};
extern enum RunMode runmode;

extern unsigned char FuncKeyStr[5][STR_LEN_MAX];

extern int error_occured_token;

struct BGDATA{
	char palette:4;
	bool v_inverse:1;
	bool h_inverse:1;
	int16_t character:10;
};

extern struct BGDATA BGData[2][2][64][64];
extern bool bgpage;
extern int bgclip_x_begin[2];
extern int bgclip_x_end[2];
extern int bgclip_y_begin[2];
extern int bgclip_y_end[2];
extern int bgofs_destx[2][2];
extern int bgofs_desty[2][2];
extern int bgofs_nowx[2][2];
extern int bgofs_nowy[2][2];
extern int bgofs_time[2][2];

extern unsigned char color_palette[3][256][3];

struct VisibleFlag{
	//コンソール,パネル,BG0,BG1,スプライト,グラフィック
	bool console:1;
	bool panel:1;
	bool BG0:1;
	bool BG1:1;
	bool sprite:1;
	bool graphic:1;
};

extern struct VisibleFlag VisibleFlags;

extern unsigned char keybuffer[KEYBUFFER_MAX];
extern int keybuffer_qhead;//読む位置
extern int keybuffer_qtail;//書く位置

//mallocする...?
extern uint16_t translated_source[10000];

extern unsigned char* source_ptr;
extern uint32_t cur_line;
extern uint32_t read_curline;
extern uint32_t read_curcol;

extern uint32_t srcline_begin_token_pos[10000];
extern uint32_t srcline_token_count[10000];
extern uint32_t srclinecount;
extern char labellist_name[10000][8];
extern int labellist_line[10000];
extern int labelcount;
extern bool breakflag;
extern int keyboard_special;

extern BYTE dim_mem[DIM_MAX*STR_LEN_MAX];
extern int dim_p;
extern int dim_count;
extern struct Dimention dim_index[VAR_MAX];

struct Dimention {
	BYTE *address;
	int VarID;
	int indexmax1;
	int	indexmax2;
	bool isStr;
};

//関数プロトタイプ宣言

int push_opstack(uint16_t op,int argcount);
int pop_opstack(uint16_t* op,int* argcount);

int push_calcstack(int type,int32_t value,char* str,int argc);
bool pop_calcstack_int(int32_t* arg);
bool pop_calcstack_str(char* arg);
bool pop_calcstack_var(int* arg);
bool pop_calcstack_intptr(int* arg);
bool pop_calcstack_strptr(int* arg);
bool pop_calcstack_void(void);

int RegistDim(int VarID,int d1,int d2);
void ClearDim(void);

int EvalFormula(const int arg,const int argcount);

int* GetSystemVariableIntPtr(uint16_t arg);
char* GetSystemVariableStrPtr(uint16_t arg);

#ifdef __cplusplus
}
#endif

#endif