/*===============================================*/
/* interpreter2.h                                */
/*===============================================*/
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
#include "evalformula.h"
#include "pgui.h"
#include "pfile.h"
//#include "data.h"
#include "myutil.h"

/*===定数・マクロ宣言===*/
//指定行にシーク先を移動
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
struct Label {
	char name[LABELNAME_MAX];
	int line;
};

/*===グローバル変数宣言===*/
extern struct ForGosubStack ForGosub_s[FORGOSUB_S_MAX];
extern unsigned int ForGosub_sl;

/*===関数プロトタイプ宣言===*/

//変数名を検索し変数IDを返す
int Str2VarID(st arg);

//プログラムソースの変数名部分を特定し変数IDを返す(未定義の場合定義)
uint16_t* GetVarID(uint16_t* p,int* tmpint,int* errtmp);
void IncrementSrcPos(void);

//式読み込み終了時に演算子スタックの残留を処理
int ProcessRemainingOperator(void);

//シーク位置が' '(スペース)でなくなるまで移動
uint16_t* JumpSpace(uint16_t* p);

//FORの脱出条件をはじめから満たした場合に対応するNEXTを探す
uint16_t* ForJump(uint16_t* p,int* errtmp);

//式を読む
uint16_t* ReadFormula(uint16_t* p,int* errtmp);

//1バイト文字列から内部コード列に変換
void TranslateRaw2Code(unsigned char* input,uint16_t* output,int* outlen);

//内部コード列から1バイト文字列に変換
void TranslateCode2Raw(uint16_t* input,unsigned char* output);

//コンソールに入力された文字列を実行
void RunInteractive(st input);

//プログラムソースから実行
int RunProgram(void);

//プログラム実行前にラベルを検索し登録
int ResistLabel(uint16_t* input);

//プログラムソースのラベル名部分を特定しラベル名を返す
uint16_t* GetLabelName(uint16_t* p,char* tmpstr,int* errtmp);

//READのシーク先を行単位で指定し移動
int ReadGotoLine(uint16_t *input,unsigned int line);

//直近(直後)のDATA文までREADのシーク先を移動
int ReadSeekNext(void);

//インタプリタ本体
int Interpret(uint16_t* input,int srclen,bool interactive_flag,int* runflag);

//新変数を登録
int NewVar(st name);

#ifdef __cplusplus
}
#endif

#endif