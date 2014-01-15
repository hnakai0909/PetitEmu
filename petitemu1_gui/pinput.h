/*===============================================*/
/* pinput.h                                      */
/*===============================================*/

#ifndef PINPUT_H_INCLUDED
#define PINPUT_H_INCLUDED

#ifdef __cplusplus
#include "DxLib.h"
#endif
#include "interpreter2.h"
//#include "evalformula.h"
//#include "myutil.h"
//#include "data.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===関数プロトタイプ宣言===*/

void ClearKeyBuffer(void);
void WriteKeyBuffer(const unsigned char arg);
bool ReadKeyBuffer(unsigned char* arg);

//下画面へのタッチ等の判定
void CheckPanel(void);

//ジョイパッド押下状態検出
void CheckJoypad(void);

//コンソールから一行入力
void InputLine(st *str);

//タッチ座標からキー特定
bool KeyboardWhichKey(int x,int y,int* type,int* code);
void CheckKey(void);
unsigned char Keychar2Char(int code,int* daku_flag);

#ifdef __cplusplus
}
#endif

#endif