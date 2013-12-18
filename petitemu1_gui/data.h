/*===============================================*/
/* data.h                                        */
/*===============================================*/

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "myutil.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===定数宣言===*/
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef bool
#define bool int
#endif
#ifndef NULL
#define NULL 0
#endif
#define FONT_PETITCOM "プチコン"
#define BGM_ID_MIN 0
#define BGM_ID_MAX 29
#define BEEP_ID_MAX 0
#define BEEP_ID_MIN 69
#define DIM_MAX 32768
#define CALC_S_MAX 100
#define OP_S_MAX 128
#define FORGOSUB_S_MAX 256
#define KEYBUFFER_MAX 256
#define VAR_MAX 1024
#define STR_LEN_MAX 256
#define TYPE_INT_LIT 1
#define TYPE_STR_LIT 2
#define TYPE_INT_VAR 3
#define TYPE_STR_VAR 4
#define TYPE_INT_SYSVAR 5
#define TYPE_STR_SYSVAR 6
#define TYPE_FUNC 7
#define TYPE_SPECIAL 8
#define TYPE_SPECIAL2 9
#define TYPE_VOID 10
#define TYPE_DIM 11
#define TYPE_DIM_PTR 12
#define TYPE_INT_PTR 13
#define TYPE_STR_PTR 14

#define ATYPE_VOID	0
#define ATYPE_INT	1
#define ATYPE_STR	2
#define ATYPE_VAR	3
#define ATYPE_INT_PTR	4
#define ATYPE_STR_PTR	5

#define KEY_TIME_TO_RENSYA 40
#define KEY_RENSYA_INTERVAL 4

//命令
#define TOKEN_NEW 0xE000
#define TOKEN_LIST 0xE001
#define TOKEN_RUN 0xE002
#define TOKEN_CONT 0xE003
#define TOKEN_FILES 0xE004
#define TOKEN_IF 0xE005
#define TOKEN_THEN 0xE006
#define TOKEN_FOR 0xE007
#define TOKEN_TO 0xE008
#define TOKEN_STEP 0xE009
#define TOKEN_NEXT 0xE00A
#define TOKEN_STOP 0xE00B
#define TOKEN_END 0xE00C
#define TOKEN_CLEAR 0xE00D
#define TOKEN_REM 0xE00E
#define TOKEN_REM2 0xE00F
#define TOKEN_LABEL 0xE010
#define TOKEN_LABEL2 0xE011
#define TOKEN_GOTO 0xE012
#define TOKEN_GOSUB 0xE013
#define TOKEN_RETURN 0xE014
#define TOKEN_ON 0xE015
#define TOKEN_DIM 0xE016
#define TOKEN_TMREAD 0xE017
#define TOKEN_DTREAD 0xE018
#define TOKEN_DATA 0xE019
//他のライブラリに存在した同名の重複定義を回避
#ifdef TOKEN_READ
#undef TOKEN_READ
#endif
#define TOKEN_READ 0xE01A
#define TOKEN_RESTORE 0xE01B
#define TOKEN_KEY 0xE01C
#define TOKEN_VSYNC 0xE01D
#define TOKEN_CLS 0xE01E
#define TOKEN_PRINT 0xE01F
#define TOKEN_PRINT2 0xE020
#define TOKEN_LOCATE 0xE021
#define TOKEN_INPUT 0xE022
#define TOKEN_LINPUT 0xE023
#define TOKEN_COLOR 0xE024
#define TOKEN_LOAD 0xE025
#define TOKEN_SAVE 0xE026
#define TOKEN_SENDFILE 0xE028
#define TOKEN_RECVFILE 0xE029
#define TOKEN_DELETE 0xE02A
#define TOKEN_RENAME 0xE02B
#define TOKEN_VISIBLE 0xE02C
#define TOKEN_COLINIT 0xE02D
#define TOKEN_COLSET 0xE02E
#define TOKEN_COLREAD 0xE02F
#define TOKEN_CHRINIT 0xE030
#define TOKEN_CHRSET 0xE031
#define TOKEN_CHRREAD 0xE032
#define TOKEN_BGPAGE 0xE033
#define TOKEN_BGCLIP 0xE034
#define TOKEN_BGPUT 0xE035
#define TOKEN_SPPAGE 0xE036
#define TOKEN_SPSET 0xE037
#define TOKEN_SPCLR 0xE038
#define TOKEN_SPOFS 0xE039
#define TOKEN_SPSCALE 0xE03A
#define TOKEN_SPANGLE 0xE03B
#define TOKEN_GPAGE 0xE03C
#define TOKEN_GCOLOR 0xE03D
#define TOKEN_GPSET 0xE03E
#define TOKEN_GPAINT 0xE03F
#define TOKEN_GLINE 0xE040
#define TOKEN_GBOX 0xE041
#define TOKEN_GCIRCLE 0xE042
#define TOKEN_GPUTCHR 0xE043
#define TOKEN_GCLS 0xE044
#define TOKEN_PNLTYPE 0xE045
#define TOKEN_ICONSET 0xE046
#define TOKEN_ICONCLR 0xE047
#define TOKEN_PNLSTR 0xE049
#define TOKEN_BEEP 0xE04A
#define TOKEN_BGMPLAY 0xE04B
#define TOKEN_BGMSTOP 0xE04C
#define TOKEN_SPANIM 0xE04D
#define TOKEN_BGOFS 0xE04E
#define TOKEN_BGREAD 0xE04F
#define TOKEN_SPCHR 0xE052
#define TOKEN_EXEC 0xE053
#define TOKEN_GFILL 0xE054
//関数
#define TOKEN_ASC 0xE400
#define TOKEN_CHR 0xE401
#define TOKEN_VAL 0xE402
#define TOKEN_STR 0xE403
#define TOKEN_HEX 0xE404
#define TOKEN_MID 0xE405
#define TOKEN_LEN 0xE406
#define TOKEN_RND 0xE407
#define TOKEN_ABS 0xE408
#define TOKEN_SGN 0xE409
#define TOKEN_PI 0xE40A
#define TOKEN_RAD 0xE40B
#define TOKEN_SIN 0xE40C
#define TOKEN_COS 0xE40D
#define TOKEN_TAN 0xE40E
#define TOKEN_ATAN 0xE40F
#define TOKEN_SQR 0xE410
#define TOKEN_EXP 0xE411
#define TOKEN_LOG 0xE412
#define TOKEN_FLOOR 0xE413
#define TOKEN_BUTTON 0xE414
#define TOKEN_INKEY 0xE415
#define TOKEN_CHKCHR 0xE416
#define TOKEN_GSPOIT 0xE417
#define TOKEN_ICONCHK 0xE418
#define TOKEN_BGMCHK 0xE419
#define TOKEN_DEG 0xE41A
#define TOKEN_SPCHK 0xE41B
//システム変数
#define TOKEN_TRUE 0xE800
#define TOKEN_FALSE 0xE801
#define TOKEN_CANCEL 0xE802
#define TOKEN_VERSION 0xE803
#define TOKEN_TCHX 0xE804
#define TOKEN_TCHY 0xE805
#define TOKEN_TCHST 0xE806
#define TOKEN_TCHTIME 0xE807
#define TOKEN_FUNCNO 0xE808
#define TOKEN_CSRX 0xE809
#define TOKEN_CSRY 0xE80A
#define TOKEN_ERR 0xE80B
#define TOKEN_ERL 0xE80C
#define TOKEN_RESULT 0xE80D
#define TOKEN_TIME 0xE80E
#define TOKEN_DATE 0xE80F
#define TOKEN_MAINCNTL 0xE810
#define TOKEN_MAINCNTH 0xE811
#define TOKEN_ICONPUSE 0xE812
#define TOKEN_ICONPAGE 0xE813
#define TOKEN_ICONPMAX 0xE814
#define TOKEN_FREEMEM 0xE815
#define TOKEN_FREEVAR 0xE816
#define TOKEN_MEM 0xE817
#define TOKEN_TABSTEP 0xE818
#define TOKEN_SYSBEEP 0xE819
//文字演算子
#define TOKEN_NOT 0xEC00
#define TOKEN_AND 0xEC01
#define TOKEN_OR 0xEC02
#define TOKEN_XOR 0xEC03
//記号演算子1(他と記号が重複しているものなど)
#define OP_MINUSSIGN 0xED00
#define OP_SHONARI_EQUAL 0xED01
#define OP_DAINARI_EQUAL 0xED02
#define OP_EQUAL 0xED03
#define OP_NOTEQUAL 0xED04
//記号演算子2(1以外)
#define OP_MOD 0xFF05
#define OP_MULTIPLY 0xFF0A
#define OP_PLUS	0xFF0B
#define OP_MINUS 0xFF0D
#define OP_DIVIDE 0xFF0F
#define OP_SHONARI 0xFF1C
#define OP_SUBSTITUTE 0xFF1D
#define OP_DAINARI 0xFF1E

//エラー種別列挙型(システム変数ERRに準拠)
enum errorid{
	ERR_UNDEFINED=0,
	ERR_SYNTAX_ERROR,
	ERR_OUT_OF_RANGE,
	ERR_OUT_OF_MEMORY,
	ERR_UNDEFINED_LABEL,
	ERR_NEXT_WITHOUT_FOR,
	ERR_OUT_OF_DATA,
	ERR_ILLEGAL_FUNCTION_CALL,
	ERR_DUPLICATE_DEFINITION,
	ERR_CANNOT_CONTINUE,
	ERR_MISSING_OPERAND,
	ERR_DUPLICATE_LABEL,
	ERR_ILLEGAL_RESOURCE_TYPE,
	ERR_ILLEGAL_CHARACTER_TYPE,
	ERR_STRING_TOO_LONG,
	ERR_DIVISION_BY_ZERO,
	ERR_OVERFLOW,
	ERR_SUBSCRIPT_OUT_OF_RANGE,
	ERR_TYPE_MISMATCH,
	ERR_FORMULA_TOO_COMPLEX,
	ERR_RETURN_WITHOUT_GOSUB,
	ERR_FOR_WITHOUT_NEXT,
	ERR_NO_ERROR=100
};

//インタプリタ遷移状態用列挙型
enum State {
	ST_LINE_BEGIN,
	ST_NEW_STATEMENT,
	ST_LABEL,
	ST_SUBSTITUTION_NAME,
	ST_PRINT,
	ST_SUBSTITUTION_INSTRUCTION,
	ST_SUBSTITUTION,
	ST_INSTRUCITION,
	ST_FORMULA,
	ST_STRING_START,
	ST_STRING,
	ST_IF,
	ST_FOR,
	ST_FOR1,
	ST_FOR2,
	ST_FOR3,
	ST_DIM,
	ST_ON,
	ST_READ,
	ST_DATA,
	ST_INPUT,
	ST_LINPUT
};

//下画面パネルのキーボード用列挙型(プチコンmkIIのシステム変数KEYBOARDに準拠)
enum CharKeyCode {
	PKEY_NO=0,
	PKEY_ESC,PKEY_1,PKEY_2,PKEY_3,PKEY_4,PKEY_5,PKEY_6,PKEY_7,PKEY_8,PKEY_9,PKEY_0,PKEY_MINUS,PKEY_PLUS,PKEY_EQUAL,PKEY_BS,
	PKEY_DOLLAR,PKEY_WQUOTE,PKEY_Q,PKEY_W,PKEY_E,PKEY_R,PKEY_T,PKEY_Y,PKEY_U,PKEY_I,PKEY_O,PKEY_P,PKEY_AT,PKEY_ASTER,PKEY_LPAREN,PKEY_RPAREN,
	PKEY_TAB,PKEY_EXCLA,PKEY_A,PKEY_S,PKEY_D,PKEY_F,PKEY_G,PKEY_H,PKEY_J,PKEY_K,PKEY_L,PKEY_SCOLON,PKEY_COLON,PKEY_LT,LEY_GT,
	PKEY_SHIFT,PKEY_SQUOTE,PKEY_Z,PKEY_X,PKEY_C,PKEY_V,PKEY_B,PKEY_N,PKEY_M,PKEY_COMMA,PKEY_DOT,PKEY_SLASH,PKEY_PER,PKEY_ENTER,
	PKEY_CAPS,PKEY_MODE_A,PKEY_MODE_G,PKEY_MODE_K,PKEY_SPACE,PKEY_INS,PKEY_DEL,PKEY_SEARCH,
	PKEY_EXIT,
	PKEY_HELP,PKEY_RUN,PKEY_EDIT,PKEY_ICON_UP,PKEY_ICON_DOWN
};

//下画面パネルのボタン種別識別用列挙型
enum KeyType {
	KT_NO,KT_FUNCKEY,KT_CHAR,KT_SYSTEM,KT_ICON	
};

// 濁点/半濁点フラグ用列挙型
enum Dakuten {
	DAKU_NO,DAKU_HAN,DAKU_DAKU
};

/*===グローバル変数宣言・定義===*/

//コンソール文字色(RGB)
static int consolecharcolor[16][3]={
	{255,255,255}	,{0,0,0}	,{191,191,191}	,{255,224,0}	,
	{0,240,31}		,{0,127,0}	,{255,203,167}	,{255,160,0}	,
	{151,94,46}		,{0,191,255},{127,63,255}	,{0,63,240}		,
	{255,95,192}	,{255,31,0}	,{63,63,63}		,{255,255,255}	
};

//トークン<-->ソース文字列テーブル
static uint16_t TokenConvTable_Code[150]={
	TOKEN_NEW,TOKEN_LIST,TOKEN_RUN,TOKEN_CONT,TOKEN_FILES,TOKEN_IF,TOKEN_THEN,TOKEN_FOR,
	TOKEN_TO,TOKEN_STEP,TOKEN_NEXT,TOKEN_STOP,TOKEN_END,TOKEN_CLEAR,TOKEN_REM,TOKEN_REM2,
	TOKEN_LABEL2,TOKEN_LABEL,TOKEN_GOTO,TOKEN_GOSUB,TOKEN_RETURN,TOKEN_ON,TOKEN_DIM,TOKEN_TMREAD,
	TOKEN_DTREAD,TOKEN_DATA,TOKEN_READ,TOKEN_RESTORE,TOKEN_KEY,TOKEN_VSYNC,TOKEN_CLS,TOKEN_PRINT,
	TOKEN_PRINT2,TOKEN_LOCATE,TOKEN_INPUT,TOKEN_LINPUT,TOKEN_COLOR,TOKEN_LOAD,TOKEN_SAVE,TOKEN_SENDFILE,
	TOKEN_RECVFILE,TOKEN_DELETE,TOKEN_RENAME,TOKEN_VISIBLE,TOKEN_COLINIT,TOKEN_COLSET,TOKEN_COLREAD,TOKEN_CHRINIT,
	TOKEN_CHRSET,TOKEN_CHRREAD,TOKEN_BGPAGE,TOKEN_BGCLIP,TOKEN_BGPUT,TOKEN_SPPAGE,TOKEN_SPSET,TOKEN_SPCLR,
	TOKEN_SPOFS,TOKEN_SPSCALE,TOKEN_SPANGLE,TOKEN_GPAGE,TOKEN_GCOLOR,TOKEN_GPSET,TOKEN_GPAINT,TOKEN_GLINE,
	TOKEN_GBOX,TOKEN_GCIRCLE,TOKEN_GPUTCHR,TOKEN_GCLS,TOKEN_PNLTYPE,TOKEN_ICONSET,TOKEN_ICONCLR,TOKEN_PNLSTR,
	TOKEN_BEEP,TOKEN_BGMPLAY,TOKEN_BGMSTOP,TOKEN_SPANIM,TOKEN_BGOFS,TOKEN_BGREAD,TOKEN_SPCHR,TOKEN_EXEC,
	TOKEN_GFILL,TOKEN_ASC,TOKEN_CHR,TOKEN_VAL,TOKEN_STR,TOKEN_HEX,TOKEN_MID,TOKEN_LEN,
	TOKEN_RND,TOKEN_ABS,TOKEN_SGN,TOKEN_PI,TOKEN_RAD,TOKEN_SIN,TOKEN_COS,TOKEN_TAN,
	TOKEN_ATAN,TOKEN_SQR,TOKEN_EXP,TOKEN_LOG,TOKEN_FLOOR,TOKEN_BUTTON,TOKEN_INKEY,TOKEN_CHKCHR,
	TOKEN_GSPOIT,TOKEN_ICONCHK,TOKEN_BGMCHK,TOKEN_DEG,TOKEN_SPCHK,TOKEN_TRUE,TOKEN_FALSE,TOKEN_CANCEL,
	TOKEN_VERSION,TOKEN_TCHX,TOKEN_TCHY,TOKEN_TCHST,TOKEN_TCHTIME,TOKEN_FUNCNO,TOKEN_CSRX,TOKEN_CSRY,
	TOKEN_ERR,TOKEN_ERL,TOKEN_RESULT,TOKEN_TIME,TOKEN_DATE,TOKEN_MAINCNTL,TOKEN_MAINCNTH,TOKEN_ICONPUSE,
	TOKEN_ICONPAGE,TOKEN_ICONPMAX,TOKEN_FREEMEM,TOKEN_FREEVAR,TOKEN_MEM,TOKEN_TABSTEP,TOKEN_SYSBEEP,TOKEN_NOT,
	TOKEN_AND,TOKEN_OR,TOKEN_XOR,NULL
};
static char TokenConvTable_Str[150][10]={
	"NEW","LIST","RUN","CONT","FILES","IF","THEN","FOR",
	"TO","STEP","NEXT","STOP","END","CLEAR","REM","'",
	"LABEL","@","GOTO","GOSUB","RETURN","ON","DIM","TMREAD",
	"DTREAD","DATA","READ","RESTORE","KEY","VSYNC","CLS","PRINT",
	"?","LOCATE","INPUT","LINPUT","COLOR","LOAD","SAVE","SENDFILE",
	"RECVFILE","DELETE","RENAME","VISIBLE","COLINIT","COLSET","COLREAD","CHRINIT",
	"CHRSET","CHRREAD","BGPAGE","BGCLIP","BGPUT","SPPAGE","SPSET","SPCLR",
	"SPOFS","SPSCALE","SPANGLE","GPAGE","GCOLOR","GPSET","GPAINT","GLINE",
	"GBOX","GCIRCLE","GPUTCHR","GCLS","PNLTYPE","ICONSET","ICONCLR","PNLSTR",
	"BEEP","BGMPLAY","BGMSTOP","SPANIM","BGOFS","BGREAD","SPCHR","EXEC",
	"GFILL","ASC","CHR$","VAL","STR$","HEX$","MID$","LEN",
	"RND","ABS","SGN","PI","RAD","SIN","COS","TAN",
	"ATAN","SQR","EXP","LOG","FLOOR","BUTTON","INKEY$","CHKCHR",
	"GSPOIT","ICONCHK","BGMCHK","DEG","SPCHK","TRUE","FALSE","CANCEL",
	"VERSION","TCHX","TCHY","TCHST","TCHTIME","FUNCNO","CSRX","CSRY",
	"ERR","ERL","RESULT","TIME$","DATE$","MAINCNTL","MAINCNTH","ICONPUSE",
	"ICONPAGE","ICONPMAX","FREEMEM","FREEVAR","MEM$","TABSTEP","SYSBEEP","NOT",
	"AND","OR","XOR",NULL
};

//キーボードマッピングデータ
//Alphabet
static const unsigned char KeyMap_A[256]={
	'1','2','3','4','5','6','7','8','9','0',
	'-','+','=',
	'$','"','Q','W','E','R','T','Y','U','I',
	'O','P','@','*','(',')',
	'!','A','S','D','F','G','H','J','K','L',';',':','<','>',
	'\'','Z','X','C','V','B','N','M',',','.','/','%',
	' '
};
//Alphabet + Shift
static const unsigned char KeyMap_A_S[256]={
	0,0,'#',0,0,'&',0,'^','\\','~',
	0,0x7F,'|',
	0,0,'q','w','e','r','t','y','u','i',
	'o','p','`',0,'[',']',
	0,'a','s','d','f','g','h','j','k','l',0,0,'{','}',
	0,'z','x','c','v','b','n','m',0,0,'?','_',
	' '
};
//Graph
static const unsigned char KeyMap_G[256]={
	0xF1,0xF2,0xF0,0xF4,0x0A,0xE5,0x1A,0xE4,0xE6,0xE7,
	0x10,0x07,0x14,
	0x03,0x96,0x98,0x91,0x99,0xEC,0xEE,0xEF,0xED,0x0E,
	0x0F,0x05,0x06,0x04,0x0B,0x1B,
	0x95,0x92,0x93,0x94,0x15,0x07,0x06,0xF4,0xF6,0xF7,0xF5,0xEB,0xEA,0xE8,
	0xFF,0x9A,0x90,0x9B,0xFC,0xFD,0xFE,0x97,0x9C,0x9D,0x9E,0x9F,
	' '
};
//Graph + Shift
static const unsigned char KeyMap_G_S[256]={
	0,0x80,0,0,0,0xE1,0,0xE0,0xE2,0xE3,
	0x11,0x08,0,
	0,0,0xF8,0x1E,0xFA,0x01,0x02,0x18,0x19,0x0C,
	0x12,0x13,0,0,0xE9,0,
	0,0x1D,0,0x1C,0,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0,0,
	0,0xF9,0x1F,0xFB,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
	' '
};
//Kana
static const unsigned char KeyMap_K[256]={
	0xB1,0xB2,0xB3,0xB4,0xB5,0xC5,0xC6,0xC7,0xC8,0xC9,
	'-','+','=',
	0xA0,0xA5,0xB6,0xB7,0xB8,0xB9,0xBA,0xCA,0xCB,0xCC,
	0xCD,0xCE,0xA2,0xA3,0xDD,0xA1,
	0xBB,0xBC,0xBD,0xBE,0xBF,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xA4,
	0xC0,0xC1,0xC2,0xC3,0xC4,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xA6,
	' '
};
//Kana + Shift
static const unsigned char KeyMap_K_S[256]={
	0xA7,0xA8,0xA9,0xAA,0xAB,0,0,0,0,0,
	0xCA,0xCB,0xCC,
	0xAF,0xB3,0xB6,0xB7,0xB8,0xB9,0xBA,0xCA,0xCB,0xCC,
	0xCD,0xCE,0xCD,0xCE,0,0,
	0xBB,0xBC,0xBD,0xBE,0xBF,'1','2','3','4','5',0xAC,0xAD,0xAE,0,
	0xC0,0xC1,0xC2,0xC3,0xC4,'6','7','8','9','0',0,0,
	' '
};
//Kana + Shift時の濁点・半濁点フラグ
static const unsigned char KeyMap_K_S_DAKU[256]={
	DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,
	DAKU_HAN,DAKU_HAN,DAKU_HAN,
	DAKU_NO,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,
	DAKU_DAKU,DAKU_DAKU,DAKU_HAN,DAKU_HAN,DAKU_NO,DAKU_NO,
	DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,
	DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_DAKU,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,DAKU_NO,
	DAKU_NO
};

/*===関数プロトタイプ宣言===*/
//テキストデータ(1文字)を内部コードに変換
uint16_t Char2Code(unsigned char arg);

//テキストデータ(1トークン)を内部コードに変換
bool Str2TokenCode(char* arg,uint16_t* arg2);

//内部コードからテキストデータ(1文字)に変換
unsigned char Code2Char(uint16_t arg);

//内部コードからテキストデータ(1トークン)に変換
char* TokenCode2Str(uint16_t arg);

//1バイト文字からプチコンフォント用文字へ
int FontTable(const unsigned char arg,char* arg2);

//2進法数値リテラルの文字か
bool isBin(const char arg);

//演算子でとる引数の数
unsigned int GetOperatorArgCount(const uint16_t arg);

//エラーメッセージ一覧
char* GetErrorMessage(const char arg);

//演算子トークンか
int isOperator(const uint16_t arg);

//そのシステム変数は数値型か/文字列型か/代入可能か
int GetSystemVariableType(const int arg);

//その関数は数値を返すか/文字列を返すか
int GetFunctionType(const int arg);

//システム変数か
bool isSystemVariable(const int arg);

//関数か
bool isFunction(const int arg);

//命令か
bool isInstruction(const int arg);

//その命令は引数なしか
bool isNoArgInstruction(const int arg);

//その命令は引数ありか
bool isArgInstruction(const int arg);

//BGMのABループ位置(サンプル数)
int GetLoopPos(const char arg);

//そのBGMは単発かループか
bool isLoop(const char arg);

//演算子の優先順位
int GetOperatorPriority(const int arg);

//左結合性か
bool isOperatorLeftAssoc(const int arg);

#ifdef __cplusplus
}
#endif

#endif