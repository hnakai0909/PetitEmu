/*===============================================*/
/* pgui.h                                        */
/*===============================================*/

#ifndef PGUI_H_INCLUDED
#define PGUI_H_INCLUDED

#ifdef __cplusplus
#include "DxLib.h"
#endif
#include "data.h"
#include "evalformula.h"
#include "pinput.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===グローバル変数宣言===*/
extern int GrHandle[10];

/*===関数プロトタイプ宣言===*/

//1フレームごとの処理(描画等)
bool ProcessFrame(void);

//コンソール画面の描画
void DrawConsole(void);

//キーボード画面の描画
void DrawKeyboard(void);

//ファンクションキーの描画
void DrawFuncKey(void);

//下画面下部パネルの描画
void DrawUnderPanel(void);

//BG処理
void ProcessBG(void);

//BG描画(未実装)
void DrawBG(void);

//BGチップ単位描画(未実装)
void DrawBGChip(int page,int layer,int ofsx_1,int ofsx_2,int ofsy_1,int ofsy_2);

//コンソールの[CSRY]行目消去
void ConsoleClearLine(void);

//コンソールバッファに1文字追加
int PutChar2Console(uint16_t arg,int indent_option);

//コンソールバッファに文字列追加
int Print2Console(char *string,int indent_option);

//コンソールスクロール(バッファを操作)
void ScrollConsole(void);

//起動時の文章をコンソールバッファに追加
void PutStartMessage(void);

//デバッグ画面を描画
void DrawDebugScreen(void);

#ifdef __cplusplus
}
#endif

#endif