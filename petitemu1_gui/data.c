/*===============================================*/
/* data.c                                        */
/*===============================================*/

#include "data.h"

/*===関数定義===*/

uint16_t Char2Code(unsigned char arg){
	int i;
	static uint16_t dictAry[256];
	//"PTC->TXT変換"　より引用・編集
	for(i=0x01;i<=0x20;i++)dictAry[i]=i;
	for(i=0x21;i<=0x7F;i++)dictAry[i]=0xFF00+i-0x20;
	for(i=0x80;i<=0xA0;i++)dictAry[i]=i;
	for(i=0xE0;i<=0xFF;i++)dictAry[i]=i;
	dictAry[0x22]=0x201D;dictAry[0x27]=0x2019;dictAry[0xA1]=0x3002;dictAry[0xA2]=0x300C;
	dictAry[0xA3]=0x300D;dictAry[0xA4]=0x3001;dictAry[0xA5]=0x30FB;dictAry[0xA6]=0x30F2;
	dictAry[0xA7]=0x30A1;dictAry[0xA8]=0x30A3;dictAry[0xA9]=0x30A5;dictAry[0xAA]=0x30A7;
	dictAry[0xAB]=0x30A9;dictAry[0xAC]=0x30E3;dictAry[0xAD]=0x30E5;dictAry[0xAE]=0x30E7;
	dictAry[0xAF]=0x30C3;dictAry[0xB0]=0xFF70;dictAry[0xB1]=0x30A2;dictAry[0xB2]=0x30A4;
	dictAry[0xB3]=0x30A6;dictAry[0xB4]=0x30A8;dictAry[0xB5]=0x30AA;dictAry[0xB6]=0x30AB;
	dictAry[0xB7]=0x30AD;dictAry[0xB8]=0x30AF;dictAry[0xB9]=0x30B1;dictAry[0xBA]=0x30B3;
	dictAry[0xBB]=0x30B5;dictAry[0xBC]=0x30B7;dictAry[0xBD]=0x30B9;dictAry[0xBE]=0x30BB;
	dictAry[0xBF]=0x30BD;dictAry[0xC0]=0x30BF;dictAry[0xC1]=0x30C1;dictAry[0xC2]=0x30C4;
	dictAry[0xC3]=0x30C6;dictAry[0xC4]=0x30C8;dictAry[0xC5]=0x30CA;dictAry[0xC6]=0x30CB;
	dictAry[0xC7]=0x30CC;dictAry[0xC8]=0x30CD;dictAry[0xC9]=0x30CE;dictAry[0xCA]=0x30CF;
	dictAry[0xCB]=0x30D2;dictAry[0xCC]=0x30D5;dictAry[0xCD]=0x30D8;dictAry[0xCE]=0x30DB;
	dictAry[0xCF]=0x30DE;dictAry[0xD0]=0x30DF;dictAry[0xD1]=0x30E0;dictAry[0xD2]=0x30E1;
	dictAry[0xD3]=0x30E2;dictAry[0xD4]=0x30E4;dictAry[0xD5]=0x30E6;dictAry[0xD6]=0x30E8;
	dictAry[0xD7]=0x30E9;dictAry[0xD8]=0x30EA;dictAry[0xD9]=0x30EB;dictAry[0xDA]=0x30EC;
	dictAry[0xDB]=0x30ED;dictAry[0xDC]=0x30EF;dictAry[0xDD]=0x30F3;dictAry[0xDE]=0x309B;
	dictAry[0xDF]=0x309C;dictAry[0x00]=0x0000;
	return dictAry[arg];
}

bool Str2TokenCode(st str,uint16_t* arg2){
	int i;
	for(i=0;TokenConvTable_Code[i]!=0x00;i++){
		if(strcmp(str.s,TokenConvTable_Str[i])==0){
			*arg2=TokenConvTable_Code[i];
			return true;
		}
	}
	*arg2=0x0000;
	return false;
}

unsigned char Code2Char(uint16_t arg){
	int i;
	static uint16_t dictAry[256];
	//"PTC->TXT変換"　より引用・編集
	for(i=0x01;i<=0x20;i++)dictAry[i]=i;
	for(i=0x21;i<=0x7F;i++)dictAry[i]=0xFF00+i-0x20;
	for(i=0x80;i<=0xA0;i++)dictAry[i]=i;
	for(i=0xE0;i<=0xFF;i++)dictAry[i]=i;
	dictAry[0x22]=0x201D;dictAry[0x27]=0x2019;dictAry[0xA1]=0x3002;dictAry[0xA2]=0x300C;
	dictAry[0xA3]=0x300D;dictAry[0xA4]=0x3001;dictAry[0xA5]=0x30FB;dictAry[0xA6]=0x30F2;
	dictAry[0xA7]=0x30A1;dictAry[0xA8]=0x30A3;dictAry[0xA9]=0x30A5;dictAry[0xAA]=0x30A7;
	dictAry[0xAB]=0x30A9;dictAry[0xAC]=0x30E3;dictAry[0xAD]=0x30E5;dictAry[0xAE]=0x30E7;
	dictAry[0xAF]=0x30C3;dictAry[0xB0]=0xFF70;dictAry[0xB1]=0x30A2;dictAry[0xB2]=0x30A4;
	dictAry[0xB3]=0x30A6;dictAry[0xB4]=0x30A8;dictAry[0xB5]=0x30AA;dictAry[0xB6]=0x30AB;
	dictAry[0xB7]=0x30AD;dictAry[0xB8]=0x30AF;dictAry[0xB9]=0x30B1;dictAry[0xBA]=0x30B3;
	dictAry[0xBB]=0x30B5;dictAry[0xBC]=0x30B7;dictAry[0xBD]=0x30B9;dictAry[0xBE]=0x30BB;
	dictAry[0xBF]=0x30BD;dictAry[0xC0]=0x30BF;dictAry[0xC1]=0x30C1;dictAry[0xC2]=0x30C4;
	dictAry[0xC3]=0x30C6;dictAry[0xC4]=0x30C8;dictAry[0xC5]=0x30CA;dictAry[0xC6]=0x30CB;
	dictAry[0xC7]=0x30CC;dictAry[0xC8]=0x30CD;dictAry[0xC9]=0x30CE;dictAry[0xCA]=0x30CF;
	dictAry[0xCB]=0x30D2;dictAry[0xCC]=0x30D5;dictAry[0xCD]=0x30D8;dictAry[0xCE]=0x30DB;
	dictAry[0xCF]=0x30DE;dictAry[0xD0]=0x30DF;dictAry[0xD1]=0x30E0;dictAry[0xD2]=0x30E1;
	dictAry[0xD3]=0x30E2;dictAry[0xD4]=0x30E4;dictAry[0xD5]=0x30E6;dictAry[0xD6]=0x30E8;
	dictAry[0xD7]=0x30E9;dictAry[0xD8]=0x30EA;dictAry[0xD9]=0x30EB;dictAry[0xDA]=0x30EC;
	dictAry[0xDB]=0x30ED;dictAry[0xDC]=0x30EF;dictAry[0xDD]=0x30F3;dictAry[0xDE]=0x309B;
	dictAry[0xDF]=0x309C;dictAry[0x00]=0x0000;
	for(i=0;i<256;i++){
		if(dictAry[i]==arg)return i;
	}
	return 0x00;
}

st TokenCode2Str(uint16_t arg){
	int i;
	st str;
	str.len=0;
	memset(str.s,0x00,sizeof(str.s));
	for(i=0;TokenConvTable_Code[i]!=0x00;i++){
		if(arg==TokenConvTable_Code[i]){
			mystrcpy2(&str,TokenConvTable_Str[i]);
			return str;
		}
	}
	return str;
};

int FontTable(const unsigned char arg,char* arg2){
	const char* table1[]={ "　","А","Б","В","Г","Д","Е","Ё","Ж","З","И","Й","К","Л","М","Н",
		"♪","О","н","П","Р","С","Т","У","Ф","Х","◎","Ц","→","←","↑","↓",
		"　","！","”","＃","＄","％","＆","′","（","）","＊","＋","，","－","．","／",
		"０","１","２","３","４","５","６","７","８","９","：","；","＜","＝","＞","？",
		"＠","Ａ","Ｂ","Ｃ","Ｄ","Ｅ","Ｆ","Ｇ","Ｈ","Ｉ","Ｊ","Ｋ","Ｌ","Ｍ","Ｎ","Ｏ",
		"Ｐ","Ｑ","Ｒ","Ｓ","Ｔ","Ｕ","Ｖ","Ｗ","Ｘ","Ｙ","Ｚ","［","￥","］","＾","＿",
		"｀","ａ","ｂ","ｃ","ｄ","ｅ","ｆ","ｇ","ｈ","ｉ","ｊ","ｋ","ｌ","ｍ","ｎ","ｏ",
		"ｐ","ｑ","ｒ","ｓ","ｔ","ｕ","ｖ","ｗ","ｘ","ｙ","ｚ","｛","￤","｝","～","＼",
		"◇","━","┃","┏","┓","┛","┗","┣","┳","┫","┻","╋","┠","┯","┨","┷",
		"┴","┬","├","┼","┤","─","│","┿","┌","┐","└","┘","┝","┰","┥","┸",
		"―","。","「","」","、","・","ヲ","ァ","ィ","ゥ","ェ","ォ","ャ","ュ","ョ","ッ",
		"ー","ア","イ","ウ","エ","オ","カ","キ","ク","ケ","コ","サ","シ","ス","セ","ソ",
		"タ","チ","ツ","テ","ト","ナ","ニ","ヌ","ネ","ノ","ハ","ヒ","フ","ヘ","ホ","マ",
		"ミ","ム","メ","モ","ヤ","ユ","ヨ","ラ","リ","ル","レ","ロ","ワ","ン","゛","゜",
		"■","●","▲","▼","□","○","△","▽","Ч","Ш","Щ","Ъ","Ы","Ь","Э","Ю",
		"Я","а","б","в","г","д","е","ё","ж","з","и","й","к","л","м","╂"};
	memset(arg2,0x00,sizeof(char)*256);
	strcpy_s(arg2,256,table1[arg]);
	return 0;
}

bool isBin(const char arg){
	return (arg=='0')||(arg=='1');
}

unsigned int GetOperatorArgCount(const uint16_t arg){
	if(arg==TOKEN_NOT || arg==OP_MINUSSIGN)return 1;
	return 2;
}

st GetErrorMessage(const char arg){
	st str={0,""};
	static char *errmesstr[]={
		"Undefined Error",
		"Syntax error",
		"Out of range",
		"Out of memory",
		"Undefined label",
		"NEXT without FOR",
		"Out of DATA",
		"Illegal function call",
		"Duplicate definition",
		"Can't continue",
		"Missing operand",
		"Duplicate label",
		"Illegal resource type",
		"Illegal character type",
		"String too long",
		"Division by zero",
		"Overflow",
		"Subscript out of range",
		"Type mismatch",
		"Fomula too complex",
		"RETURN without GOSUB",
		"FOR without NEXT",
		NULL
	};
	mystrcpy2(&str,errmesstr[arg]);
	return str;
}

int isOperator(const uint16_t arg){
	char tmp=Code2Char(arg);
	if(
		(tmp=='(')||(tmp==')')||(tmp=='[')||(tmp==']')||(tmp=='=')||(tmp=='+')||
		(tmp=='-')||(tmp=='*')||(tmp=='/')||(tmp=='%')||(tmp=='<')||(tmp=='>')||
		(arg==TOKEN_NOT)||(arg==TOKEN_AND)||(arg==TOKEN_OR)||(arg==TOKEN_XOR)||
		(arg==OP_MINUSSIGN)||(arg==OP_SHONARI_EQUAL)||(arg==OP_DAINARI_EQUAL)||(arg==OP_EQUAL)||
		(arg==OP_NOTEQUAL)
		)return true;
	return false;
}

int GetSystemVariableType(const int arg){
	if(
		inrange(arg,TOKEN_TRUE,TOKEN_RESULT)||(arg==TOKEN_MAINCNTL)||(arg==TOKEN_MAINCNTH)||
		(arg==TOKEN_FREEMEM)||(arg==TOKEN_FREEVAR)
		)return 1;//int,R
	if(
		inrange(arg,TOKEN_ICONPUSE,TOKEN_ICONPMAX)||(arg==TOKEN_TABSTEP)||(arg==TOKEN_SYSBEEP)
		)return 2;//int,R/W
	if(
		(arg==TOKEN_DATE)||(arg==TOKEN_TIME)
		)return 3;//str,R
	if(arg==TOKEN_MEM)return 4;//str,R/W
	return 0;
}

int GetFunctionType(const int arg){
	if(!isFunction(arg))return 0;
	if(
		(arg==TOKEN_ASC)||(arg==TOKEN_VAL)||(arg==TOKEN_LEN)||
		inrange(arg,TOKEN_RND,TOKEN_BUTTON)||inrange(arg,TOKEN_CHKCHR,TOKEN_SPCHK)
		){
			return 1;//int
	}else {
		return 2;//str
	}
	return 0;
}

bool isSystemVariable(const int arg){
	if((arg&0xFF00)==0xE800)return true;
	return false;
}

bool isFunction(const int arg){
	if((arg&0xFF00)==0xE400)return true;
	return false;
}

bool isInstruction(const int arg){
	if(inrange(arg,TOKEN_GOTO,TOKEN_GFILL)||(arg==TOKEN_CLEAR))return true;
	return false;
}

bool isNoArgInstruction(const int arg){
	if(
		(arg==TOKEN_CLEAR)||(arg==TOKEN_CLS)||(arg==TOKEN_BGMSTOP)
		)return true;
	return false;
}

bool isArgInstruction(const int arg){
	if(isInstruction(arg)&&(!(isNoArgInstruction(arg))))return true;
	return false;
} 

int GetLoopPos(const char arg){
	static int data[]={
		72140,0,28152,577536,0,0,0,0,0,0,
		0,8480,22016,302560,0,0,0,192480,0,38016,
		37888,192480,0,0,0,0,0,0,0,0
	};
	return data[arg];
}

bool isLoop(const char arg){
	if((arg>=4)&&(arg<=6))return false;
	return true;
}

int GetOperatorPriority(const uint16_t arg){
	switch(Code2Char(arg)){
	case '(':case ')':case '[':case ']':
		return 1;
	case '*':case '/':case '%':
		return 4;
	case '+':case '-':
		return 5;
	case '<':case '>':
		return 6;
	case '=':
		return 10;
	default:
		switch(arg){
		case TOKEN_NOT:case OP_MINUSSIGN:
			return 2;
		case OP_SHONARI_EQUAL:case OP_DAINARI_EQUAL:
			return 6;
		case OP_EQUAL:case OP_NOTEQUAL:
			return 7;
		case TOKEN_AND:case TOKEN_OR:case TOKEN_XOR:
			return 8;
		default:
			return -1;
		}

	}
	return -1;
}

bool isOperatorLeftAssoc(const uint16_t arg){
	if((Code2Char(arg)=='=')||(arg==OP_MINUSSIGN)||(arg==TOKEN_NOT))return false;
	return true;
}