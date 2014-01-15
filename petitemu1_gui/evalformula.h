/*===============================================*/
/* evalformula.h                                 */
/*===============================================*/

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

/*===定数・マクロ宣言===*/
#define FloorInt(a) ((int)((a)/4096))

#ifdef __cplusplus
extern "C" {
#endif

enum PanelMode{
	PNLMD_OFF,PNLMD_PANEL,PNLMD_KYA,PNLMD_KYM,PNLMD_KYK,PNLMD_DIALOG
};
enum RunMode{
	RMD_STOP,RMD_LINE,RMD_PRG
};

/*===構造体宣言・定義===*/
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
struct BGDATA{
	char palette:4;
	bool v_inverse:1;
	bool h_inverse:1;
	int16_t character:10;
};
struct VisibleFlag{
	//コンソール,パネル,BG0,BG1,スプライト,グラフィック
	bool console:1;
	bool panel:1;
	bool BG0:1;
	bool BG1:1;
	bool sprite:1;
	bool graphic:1;
};
struct Dimention {
	BYTE *address;
	int VarID;
	int indexmax1; 
	int	indexmax2;
	bool isStr;
};
struct ConsoleBuffer {
	char chr;
	uint8_t color;
};
/*===グローバル変数宣言===*/
extern bool log_en;
extern int log_en2;
extern int log_en3;
extern struct VARIABLE Variable[VAR_MAX];
extern struct OP_S op_s[OP_S_MAX];//演算子スタック
extern struct CalcStack calc_s[CALC_S_MAX];//計算スタック
extern struct ConsoleBuffer con_buf[32][24];//コンソールバッファ

//プチコンシステム変数
extern int32_t Psys_VERSION;
extern int32_t Psys_TRUE;
extern int32_t Psys_FALSE;
extern int32_t Psys_CANCEL;
extern int32_t Psys_CSRX;
extern int32_t Psys_CSRY;
extern int32_t Psys_FREEMEM;
extern int32_t Psys_ERR;
extern int32_t Psys_ERL;
extern int32_t Psys_RESULT;
extern int32_t Psys_TCHX;
extern int32_t Psys_TCHY;
extern int32_t Psys_TCHST;
extern int32_t Psys_TCHTIME;
extern int32_t Psys_MAINCNTL;
extern int32_t Psys_MAINCNTH;
extern int32_t Psys_TABSTEP;
extern int32_t Psys_ICONPUSE;
extern int32_t Psys_ICONPAGE;
extern int32_t Psys_ICONPMAX;
extern int32_t Psys_FUNCNO;
extern int32_t Psys_FREEVAR;
extern int32_t Psys_SYSBEEP;
extern st Psys_TIME;
extern st Psys_DATE;
extern st Psys_MEM;

extern char conslogbuf[768];//デバッグ用ログバッファ
extern int consolecolor;//コンソール文字色
extern int SHandleBEEP[70];//BEEP音声データハンドル
extern int SHandleBGM;//BGM音声データハンドル
extern unsigned int calc_sl;//計算スタックの現在の深さ
extern unsigned int op_sl;//演算子スタックの現在の深さ
extern bool kbd_shift_flag;//キーボードシフトフラグ
extern bool kbd_capslock_flag;//キーボードCapslockフラグ
extern bool kbd_insert_flag;//キーボード挿入(true)/上書き(false)モードフラグ
extern int button_state;//ボタン状態(各ビットに格納)
extern enum PanelMode panelmode;//下画面の表示
extern enum RunMode runmode;//実行モード
extern st FuncKeyStr[5];//ファンクションキー文字列
extern int error_occured_token;//エラー発生トークン
//BG用各種状態変数(未使用)
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
extern unsigned char color_palette[3][256][3];//BG,スプライト,GRPのパレットデータ(未使用)
extern struct VisibleFlag VisibleFlags;//VISIBLEで指定された描画/非描画情報
extern unsigned char keybuffer[KEYBUFFER_MAX];//キー入力バッファ(リングバッファ)
extern int keybuffer_qhead;//読む位置
extern int keybuffer_qtail;//書く位置
extern uint16_t translated_source[10000];//内部コードに変換したソースデータ
extern unsigned char* source_ptr;
extern uint32_t cur_line;//インタプリタが現在シークしている行
extern uint16_t *srcpos;//インタプリタのシーク位置
extern uint16_t *read_srcpos;//READのシーク位置
extern bool read_initialized;//READ準備完了フラグ
extern uint32_t srcline_begin_token_pos[10000];//ソースの各行の開始位置
extern uint32_t srcline_token_count[10000];//ソースの各行の内部コード長
extern uint32_t srclinecount;//ソースの行数
extern char labellist_name[10000][9];//ラベル名リスト
extern int labellist_line[10000];//ラベルのある行リスト
extern int labelcount;//ラベルの数
extern bool breakflag;//実行中断フラグ
extern int keyboard_special;
extern BYTE dim_mem[DIM_MAX*STR_LEN_MAX];//配列変数用メモリ空間
extern int dim_p;//配列変数メモリ先頭ポインタ
extern int dim_count;//配列変数メモリの空きの先頭
extern struct Dimention dim_index[VAR_MAX];//配列変数情報

/*===関数プロトタイプ宣言===*/

//演算子スタック操作関数
int PushOpStack(uint16_t op,int argcount);
int PopOpStack(uint16_t* op,int* argcount);

//計算スタック操作関数
int PushCalcStack(int type,int32_t value,char* str,int argc);
bool PopCalcStack_int(int32_t* arg);
bool PopCalcStack_str(char* arg);
bool PopCalcStack_var(int* arg);
bool PopCalcStack_intptr(int* arg);
bool PopCalcStack_strptr(int* arg);
bool PopCalcStack_void(void);

//配列変数を新規登録/領域確保
int RegistDim(int VarID,int d1,int d2);

//配列変数データ全消去
void ClearDim(void);

//内部コードに対応した関数/命令/代入/演算子等を実行
int EvalFormula(const int arg,const int argcount);

//システム変数のポインタを渡す(数値型/文字列型)
int* GetSystemVariableIntPtr(uint16_t arg);
char* GetSystemVariableStrPtr(uint16_t arg);

//システム変数の値の範囲を制限
void SystemVariableLimitValue(void);

//システム状態変数を更新
void UpdateSystemVariable(void);

//システム変数初期化
void InitSystemVariable(void);

#ifdef __cplusplus
}
#endif

#endif