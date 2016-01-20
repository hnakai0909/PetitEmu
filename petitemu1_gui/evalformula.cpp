/*===============================================*/
/* evalformula.cpp                               */
/*===============================================*/

#pragma once
#include "evalformula.h"

/*===グローバル変数定義===*/

bool log_en=false;
int log_en2=false;
int log_en3=false;

int32_t Psys_VERSION=0x1020000;//ver 1.2
int32_t Psys_TRUE=0x00001000;
int32_t Psys_FALSE=0x00000000;
int32_t Psys_CANCEL=0xFFFFF000;
int32_t Psys_CSRX;
int32_t Psys_CSRY;
int32_t Psys_FREEMEM;
int32_t Psys_ERR;
int32_t Psys_ERL;
int32_t Psys_RESULT;
int32_t Psys_TCHX;
int32_t Psys_TCHY;
int32_t Psys_TCHST;
int32_t Psys_TCHTIME;
int32_t Psys_MAINCNTL;
int32_t Psys_MAINCNTH;
int32_t Psys_TABSTEP;
int32_t Psys_ICONPUSE;
int32_t Psys_ICONPAGE;
int32_t Psys_ICONPMAX;
int32_t Psys_FUNCNO;
int32_t Psys_FREEVAR;
int32_t Psys_SYSBEEP=0x00001000;
st Psys_TIME;
st Psys_DATE;
st Psys_MEM;

struct ConsoleBuffer con_buf[32][24];
struct OP_S op_s[OP_S_MAX];//演算子スタック
struct CalcStack calc_s[CALC_S_MAX];//計算スタック
struct VARIABLE Variable[VAR_MAX];

char conslogbuf[768];
int consolecolor;
int SHandleBEEP[70];
int SHandleBGM;

unsigned int op_sl;//スタックの現在の深さ
unsigned int calc_sl;//スタックの現在の深さ
int error_occured_token;
struct BGDATA BGData[2][2][64][64];
bool bgpage;
int bgclip_x_begin[2];
int bgclip_x_end[2];
int bgclip_y_begin[2];
int bgclip_y_end[2];
int bgofs_destx[2][2];
int bgofs_desty[2][2];
int bgofs_nowx[2][2];
int bgofs_nowy[2][2];
int bgofs_ip[2][2];
struct SPDATA SPData[2][100];
struct SPDATA_ANGLESCALE SPDataAS[2][32];
bool sppage;
unsigned char color_palette[3][256][3];
struct VisibleFlag VisibleFlags={1,1,1,1,1,1};
bool kbd_shift_flag;
bool kbd_capslock_flag;
bool kbd_insert_flag;
int button_state;
enum PanelMode panelmode;
enum RunMode runmode;
st FuncKeyStr[5]={5,"FILES",5,"LOAD\"",5,"SAVE\"",4,"CONT",3,"RUN"};
unsigned char keybuffer[KEYBUFFER_MAX];
int keybuffer_qhead;//読む位置(キュー先頭)
int keybuffer_qtail;//書く位置(キュー末尾)
uint16_t translated_source[10000];
unsigned char* source_ptr;
uint16_t *srcpos;
uint32_t cur_line=0;
uint16_t *read_srcpos=NULL;
bool read_initialized=false;
uint32_t srcline_begin_token_pos[10000];
uint32_t srcline_token_count[10000];
uint32_t srclinecount;
char labellist_name[10000][9];
int labellist_line[10000];
int labelcount;
bool breakflag=0;
int keyboard_special=0;
BYTE dim_mem[DIM_MAX*STR_LEN_MAX];
int dim_p;
int dim_count;
struct Dimention dim_index[VAR_MAX];

/*===関数定義===*/

int PushOpStack(uint16_t op,int argcount){
	if(op_sl>=OP_S_MAX)return false;
	op_s[op_sl].op=op;
	op_s[op_sl].argcount=argcount;
	if(log_en)printf("=> op_s(%d) value=%d,argc=%d\n",op_sl,op,argcount);
	op_sl++;
	return true;
}

bool PopOpStack(uint16_t* op,int* argcount){
	if(op_sl<=0)return false;
	op_sl--;
	if(log_en)printf("<= op_s(%d) value=%d,argc=%d\n",op_sl,op_s[op_sl].op,op_s[op_sl].argcount);
	*op=op_s[op_sl].op;
	*argcount=op_s[op_sl].argcount;
	op_s[op_sl].op=0;
	op_s[op_sl].argcount=0;
	return true;
}

int PushCalcStack(int type,int32_t value,st str,int argc){
	int errtmp=0;
	int tmp=0,argcount=0;
	uint16_t op=0;
	if(log_en)printf("=>?cs(%d) %d %d(%.3f) %s \"%s\" %d\n",calc_sl,type,value,(float)value/4096.0,TokenCode2Str(value).s,str.s,argc);
	if(type==TYPE_FUNC){
		if(isFunction(value)){
			PushOpStack(value,argc);
		}else if(value==Char2Code('(')){
			PushOpStack(value,argc);
		}else if(value==Char2Code(')')){
			tmp=0;
			while(op_sl>0){
				PopOpStack(&op,&argcount);
				if(op==Char2Code('(')){
					tmp=1;
					break;
				}else{
					errtmp=EvalFormula(op,argcount);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
				}
			}
			if(tmp==0){
				//puts("parentheses mismatched");
				return ERR_SYNTAX_ERROR;
			}
			if(op_sl>0){
				PopOpStack(&op,&argcount);
				if(isFunction(op)||isNoArgInstruction(op)||isArgInstruction(op)||(op>=0&&op<2047)){
					errtmp=EvalFormula(op,argc);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
				}else{
					PushOpStack(op,argcount);
				}
			}
		}else if(isOperator(value)){
			while(op_sl>0){
				op=op_s[op_sl-1].op;
				if((op!=Char2Code('('))&&(op!=Char2Code(')'))&&isOperator(op) &&
                        ((isOperatorLeftAssoc(value) && (GetOperatorPriority(value) >= GetOperatorPriority(op))) ||
                           (GetOperatorPriority(value) > GetOperatorPriority(op)))){
					PopOpStack(&op,&argcount);
					errtmp=EvalFormula(op,argcount);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
				}else{
					break;
				}
			}
			PushOpStack(value,argc);
		}else if(isNoArgInstruction(value)||isArgInstruction(value)){
			PushOpStack(value,argc);
		}else{
			//Unknown token
			return ERR_SYNTAX_ERROR;
		}
	}else if(type==TYPE_DIM){
		PushOpStack(value,argc);
	}else if(type==TYPE_DIM_PTR){
		PushOpStack(value+1024,argc);
	}else if(type==TYPE_SPECIAL){
		if(value==Char2Code(',')){
			tmp=0;
			while(op_sl>0){
				op=op_s[op_sl-1].op;
				argcount=op_s[op_sl-1].argcount;
				if(op==Char2Code('(')){
					tmp=1;
					break;
				}else{
					op_sl--;
					op_s[op_sl].op=0;
					errtmp=EvalFormula(op,argcount);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
				}
			}
			if(tmp==0){
				//separator or parentheses mismatched
				return ERR_SYNTAX_ERROR;
			}
		}

	}else{
		if(log_en){
			switch(type){
			case TYPE_INT_LIT:
				printf("=>!cs(%d) INT_LIT %d(%.3f)\n",calc_sl,value,(float)value/4096.0);
				break;
			case TYPE_STR_LIT:
				printf("=>!cs(%d) STR_LIT \"%s\"[%dB]\n",calc_sl,str.s,str.len);
				break;
			case TYPE_SPECIAL2:
				printf("=>!cs(%d) SPECIAL2\n",calc_sl);
				break;
			case TYPE_VOID:
				printf("=>!cs(%d) VOID\n",calc_sl);
				break;
			default:
				printf("=>!cs(%d) %d %d(%.3f) \"%s\"[%dB] %d\n",calc_sl,type,value,(float)value/4096.0,str.s,argc);
				break;
			}
		}
		if(calc_sl>=CALC_S_MAX) return -1;
		calc_s[calc_sl].type=type;
		calc_s[calc_sl].value=value;
		if(type==TYPE_STR_LIT)calc_s[calc_sl].string=str;
		calc_s[calc_sl].argcount=argc;
		calc_sl++;
	}
	return ERR_NO_ERROR;
}

bool PopCalcStack_int(int32_t* arg){
	if(calc_sl<=0)return false;
	calc_sl--;
	if(calc_s[calc_sl].type==TYPE_INT_LIT){
		*arg=calc_s[calc_sl].value;
		if(log_en)printf("<= cs(%d) INT_LIT %d(%.3f)\n",calc_sl,calc_s[calc_sl].value,(float)calc_s[calc_sl].value/4096.0);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

bool PopCalcStack_str(st* str){
	if(calc_sl<=0)return false;
	calc_sl--;
	if(calc_s[calc_sl].type==TYPE_STR_LIT){
		mystrclear(str);
		*str=calc_s[calc_sl].string;
		if(log_en)printf("<= cs(%d) STR_LIT \"%s\"[%dB]\n",calc_sl,calc_s[calc_sl].string.s,calc_s[calc_sl].string.len);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

bool PopCalcStack_var(int* arg){
	int tmpint;
	if(calc_sl<=0)return false;
	calc_sl--;
	if(
		(calc_s[calc_sl].type==TYPE_INT_VAR)||(calc_s[calc_sl].type==TYPE_STR_VAR)||
		(calc_s[calc_sl].type==TYPE_INT_SYSVAR)||(calc_s[calc_sl].type==TYPE_STR_SYSVAR)
	){
	 	memset(arg,0x00,sizeof(arg));
		*arg=calc_s[calc_sl].value;
		if(log_en){
			tmpint=GetSystemVariableType(calc_s[calc_sl].value);
			switch(tmpint){
				case 2:
					printf("<= cs(%d) SYSVAR %s\n",calc_sl,TokenCode2Str(calc_s[calc_sl].value).s);
					break;
				case 4:
					printf("<= cs(%d) SYSVAR MEM$\n",calc_sl);
					break;
				default:
					printf("<= cs(%d) VAR %s\n",calc_sl,Variable[calc_s[calc_sl].value].name.s);
			}
			
		}
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

bool PopCalcStack_intptr(int* arg){
	if(calc_sl<=0)return false;
	calc_sl--;
	if(calc_s[calc_sl].type==TYPE_INT_PTR){
	 	memset(arg,0x00,sizeof(arg));
		*arg=calc_s[calc_sl].value;
		if(log_en)printf("<= cs(%d) INTPTR %d\n",calc_sl,calc_s[calc_sl].value);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

bool PopCalcStack_strptr(int* arg){
	if(calc_sl<=0)return false;
	calc_sl--;
	if(calc_s[calc_sl].type==TYPE_STR_PTR){
	 	memset(arg,0x00,sizeof(arg));
		*arg=calc_s[calc_sl].value;
		if(log_en)printf("<= cs(%d) STRPTR %d\n",calc_sl,calc_s[calc_sl].value);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

bool PopCalcStack_void(void){
	if(calc_sl<=0)return false;
	calc_sl--;
	if(calc_s[calc_sl].type==TYPE_VOID){
		if(log_en)printf("<= cs(%d) VOID\n",calc_sl);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

int RegistDim(int VarID,int d1,int d2){
	int unit_length=0;
	if((dim_count>=VAR_MAX)||(dim_count>=DIM_MAX))return ERR_OUT_OF_MEMORY;
	if(Variable[VarID].isStr){
		unit_length=STR_LEN_MAX;
	}else{
		unit_length=4;
	}
	dim_index[dim_count].address=dim_p;
	dim_index[dim_count].VarID=VarID;
	dim_index[dim_count].indexmax1=d1;
	dim_index[dim_count].indexmax2=d2;
	dim_index[dim_count].isStr=Variable[VarID].isStr;
	Variable[VarID].isDim=true;
	Variable[VarID].value=dim_count;
	dim_count++;
	if(d2==0)d2=1;
	if((dim_p+d1*d2*unit_length)>=(DIM_MAX*STR_LEN_MAX))return ERR_OUT_OF_MEMORY;
	dim_p+=(d1*d2*unit_length);
	return ERR_NO_ERROR;
}

void ClearDim(void){
	dim_p=0;
	dim_count=0;
	memset(dim_index,0x00,sizeof(dim_index));
	memset(dim_mem,0x00,sizeof(dim_mem));
	return;
}

int EvalFormula(const int arg,const int argcount){
	int tmp=0,i=0,p=0;
	int errtmp=ERR_NO_ERROR;
	int32_t tmpint=0;
	int32_t tmpints[10];
	int64_t tmpint64=0;
	double tmpw=0;
	unsigned char tmpc=0;
	st tmpstr={0,""};	
	st tmpstrs[10];
	char tmpstr2[256];
	int argtypes[10];
	memset(tmpints,0x00,sizeof(tmpints));
	memset(argtypes,0x00,sizeof(argtypes));
	memset(tmpstr2,0x00,sizeof(tmpstr2));
	mystrclear(tmpstrs);
	if(log_en)printf("EvalFormula arg=%04X(%s) argc=%d\n",arg,TokenCode2Str(arg).s,argcount);
	if(argcount==0){
		if(!PopCalcStack_void())return ERR_SYNTAX_ERROR;
	}else{
		for(i=0;i<argcount;i++){
			if(PopCalcStack_int(&tmpint)){
				tmpints[i]=tmpint;
				argtypes[i]=ATYPE_INT;
			}else if(PopCalcStack_str(&tmpstr)){
				tmpstrs[i]=tmpstr;
				argtypes[i]=ATYPE_STR;
			}else if(PopCalcStack_var(&tmpint)){
				tmpints[i]=tmpint;
				argtypes[i]=ATYPE_VAR;
			}else if(PopCalcStack_intptr(&tmpint)){
				tmpints[i]=tmpint;
				argtypes[i]=ATYPE_INT_PTR;
			}else if(PopCalcStack_strptr(&tmpint)){
				tmpints[i]=tmpint;
				argtypes[i]=ATYPE_STR_PTR;
			}else{
				return ERR_SYNTAX_ERROR;
			}
		}
	}
	switch(arg){
		case OP_PLUS:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				tmpint64=tmpints[1]+tmpints[0];
				if((tmpint64>_I32_MAX || tmpint64<-_I32_MAX))return ERR_OVERFLOW;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)tmpint64,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if((argtypes[1]==ATYPE_STR)&&(argtypes[0]==ATYPE_STR)){
				tmpstr=tmpstrs[1];
				if(!mystrcat(&tmpstr,tmpstrs[0]))return ERR_STRING_TOO_LONG;
				errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_MULTIPLY:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				tmpint64=(int64_t)(((double)((double)tmpints[1]/4096.0)*(double)((double)tmpints[0]/4096.0))*4096);
				if((tmpint64>_I32_MAX || tmpint64<-_I32_MAX)){
					return ERR_OVERFLOW;
				}
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)tmpint64,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_DIVIDE:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if(tmpints[0]==0)return ERR_DIVISION_BY_ZERO;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				tmpint64=(int64_t)(((double)((double)tmpints[1]/4096.0)/(double)((double)tmpints[0]/4096.0))*4096);
				if((tmpint64>_I32_MAX || tmpint64<-_I32_MAX)){
					return ERR_OVERFLOW;
				}
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)tmpint64,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_MOD:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if(tmpints[0]==0)return ERR_DIVISION_BY_ZERO;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				tmpint64=(int64_t)(fmod((double)((double)tmpints[1]/4096.0),(double)((double)tmpints[0]/4096.0))*4096);
				if((tmpint64>_I32_MAX || tmpint64<-_I32_MAX)){
					return ERR_OVERFLOW;
				}
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)tmpint64,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_MINUSSIGN:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]==ATYPE_INT){
				errtmp=PushCalcStack(TYPE_INT_LIT,-tmpint,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_MINUS:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				tmpint64=tmpints[1]-tmpints[0];
				if((tmpint64>_I32_MAX || tmpint64<-_I32_MAX)){
					return ERR_OVERFLOW;
				}
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)tmpint64,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_SHONARI:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(tmpints[1]<tmpints[0])*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else if((argtypes[1]==ATYPE_STR)&&(argtypes[0]==ATYPE_STR)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(mystrcmp(tmpstrs[1],tmpstrs[0])<0)*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_DAINARI:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(tmpints[1]>tmpints[0])*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else if((argtypes[1]==ATYPE_STR)&&(argtypes[0]==ATYPE_STR)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(mystrcmp(tmpstrs[1],tmpstrs[0])>0)*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_SHONARI_EQUAL:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(tmpints[1]<=tmpints[0])*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else if((argtypes[1]==ATYPE_STR)&&(argtypes[0]==ATYPE_STR)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(mystrcmp(tmpstrs[1],tmpstrs[0])<=0)*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_DAINARI_EQUAL:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(tmpints[1]>=tmpints[0])*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else if((argtypes[1]==ATYPE_STR)&&(argtypes[0]==ATYPE_STR)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(mystrcmp(tmpstrs[1],tmpstrs[0])>=0)*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_EQUAL:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(tmpints[1]==tmpints[0])*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else if((argtypes[1]==ATYPE_STR)&&(argtypes[0]==ATYPE_STR)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(mystrcmp(tmpstrs[1],tmpstrs[0])==0)*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_NOTEQUAL:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(tmpints[1]!=tmpints[0])*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else if((argtypes[1]==ATYPE_STR)&&(argtypes[0]==ATYPE_STR)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(mystrcmp(tmpstrs[1],tmpstrs[0])!=0)*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case TOKEN_OR:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(FloorInt(tmpints[1])|FloorInt(tmpints[0]))*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case TOKEN_AND:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(FloorInt(tmpints[1])&FloorInt(tmpints[0]))*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case TOKEN_NOT:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]==ATYPE_INT){
				errtmp=PushCalcStack(TYPE_INT_LIT,(~FloorInt(tmpints[0]))*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case TOKEN_XOR:
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argcount<2)return ERR_MISSING_OPERAND;
			if((argtypes[1]==ATYPE_INT)&&(argtypes[0]==ATYPE_INT)){
				errtmp=PushCalcStack(TYPE_INT_LIT,(FloorInt(tmpints[1])^FloorInt(tmpints[0]))*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;		
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case TOKEN_ABS:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]==ATYPE_INT){
				errtmp=PushCalcStack(TYPE_INT_LIT,abs(tmpints[0]),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case TOKEN_ASC:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]==ATYPE_STR){
				//if(tmpstrs[0].len==0)return ERR_?????
				errtmp=PushCalcStack(TYPE_INT_LIT,(tmpstrs[0].s[0])*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case TOKEN_ATAN:
			if(argcount==2){
				if(argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)(atan2((double)(tmpints[1]/4096.0),(double)(tmpints[0]/4096.0))*4096.0),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)((atan((double)(tmpints[0])/4096.0))*4096.0),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_BGMCHK:
			if(argcount!=0)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_VOID)return ERR_TYPE_MISMATCH;
			tmpint=CheckSoundMem(SHandleBGM);
			if(tmpint==-1)tmpint=0;
			errtmp=PushCalcStack(TYPE_INT_LIT,tmpint*4096,MYSTR_NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_BUTTON:
			if(argcount>0)return ERR_SYNTAX_ERROR;
			errtmp=PushCalcStack(TYPE_INT_LIT,(uint32_t)button_state*4096,MYSTR_NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_COLINIT:
			//TODO:引数省略可？
			if(argcount<2){
				return ERR_MISSING_OPERAND;
			}else if(argcount>2){
				return ERR_SYNTAX_ERROR;
			}
			if(argtypes[1]!= ATYPE_STR || argtypes[1]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpints[0]=FloorInt(tmpints[0]);
			if(mystrcmp2(tmpstrs[1],"BG")){
				tmpint=0;
			}else if(mystrcmp2(tmpstrs[1],"SP")){
				tmpint=1;
			}else if(mystrcmp2(tmpstrs[1],"GRP")){
				tmpint=2;
			}else{
				//ERR_ILLEGAL_RESOURCE_TYPEかも
				return ERR_ILLEGAL_FUNCTION_CALL;
			}
			if(!inrange(tmpints[0],0,255))return ERR_ILLEGAL_FUNCTION_CALL;
			memset(color_palette[2][tmpints[0]],0x00,sizeof(unsigned char)*3);
			break;
		case TOKEN_COS:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)((cos((double)(tmpints[0])/4096.0))*4096.0),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_CHKCHR:
			if(argcount<2)return ERR_MISSING_OPERAND;
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpints[1]=FloorInt(tmpints[1]);tmpints[0]=FloorInt(tmpints[0]);
			if(tmpints[1]>=0 && tmpints[1]<=31 && tmpints[0]>=0 && tmpints[0]<=23){
				tmpint=con_buf[tmpints[1]][tmpints[0]].chr;
				if(tmpint<0)tmpint+=256;
				errtmp=PushCalcStack(TYPE_INT_LIT,tmpint*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				errtmp=PushCalcStack(TYPE_INT_LIT,(-1)*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}
			break;
		case TOKEN_DEG:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)(tmpints[0]*180/3.1415926535),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_EXP:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)((exp((double)(tmpints[0])/4096.0))*4096.0),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_FLOOR:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,tmpints[0]&0xFFFFF000,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_GSPOIT:
			if(argcount<2)return ERR_MISSING_OPERAND;
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			if(tmpints[1]>=0 && tmpints[1]<=255 && tmpints[0]>=0 && tmpints[0]<=191){
				tmpint=0;//GetPixelPalCodeSoftImage()?
				errtmp=PushCalcStack(TYPE_INT_LIT,tmpint*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				errtmp=PushCalcStack(TYPE_INT_LIT,(-1)*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}
			break;
		case TOKEN_ICONCHK:
			if(argcount!=0)return ERR_SYNTAX_ERROR;
			errtmp=PushCalcStack(TYPE_INT_LIT,/*ICON押し下げ状態　-1=X 0~3=O*/1*4096,MYSTR_NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_LEN:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_STR)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,tmpstrs[0].len*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_LOG:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)((log((double)(tmpints[0])/4096.0))*4096.0),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_PI:
			if(argcount!=0)return ERR_SYNTAX_ERROR;
			//11.0010010000111110... ≒ 11.00100100001 = 0x3243
			PushCalcStack(TYPE_INT_LIT,0x3243,MYSTR_NULL,0);
			break;
		case TOKEN_RAD:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)(tmpints[0]*3.141592/180),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_RND:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)(rand()*FloorInt(tmpints[0])/(1.0+RAND_MAX))*4096,MYSTR_NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_SGN:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,((tmpints[0]==0)?0:((tmpints[0]>0)?1:-1))*4096,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_SIN:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)((sin((double)(tmpints[0])/4096.0))*4096.0),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_SQR:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)((sqrt((double)(tmpints[0])/4096.0))*4096.0),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_TAN:
			if(argcount==1){
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)((tan((double)(tmpints[0])/4096.0))*4096.0),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(argcount==0){
				return ERR_MISSING_OPERAND;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case TOKEN_VAL:
			//要調査・改修
			if(argcount==0)return ERR_MISSING_OPERAND;
			if(argcount>1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_STR)return ERR_TYPE_MISMATCH;
			p=0;
			if(tmpstrs[0].s[p]=='-'){tmp=1;p++;}else{tmp=0;}
			if(isdigit(tmpstrs[0].s[p])){
				tmpint=0;
				i=0;
				while(isdigit(tmpstrs[0].s[p])){
					if(i>=7)return ERR_OVERFLOW;
					tmpint=(tmpint*10)+(tmpstrs[0].s[p]-'0');
					p++;i++;
				}
				if(tmpint>=524288)return ERR_OVERFLOW;
				tmpint*=4096;
				if(tmpstrs[0].s[p]=='.'){
					p++;
					tmpw=0;
					for(i=0;isdigit(tmpstrs[0].s[p+i])&&(i<=6);i++);
					i--;
					tmp=i;
					while(i>=0){
						tmpw=((tmpw+(double)(tmpstrs[0].s[p+i]-'0'))/10.0);
						i--;
					}
					p+=tmp;
					//四捨五入的な効果があることを期待
					//切り捨てられるものに
					//最小分解能である1/4096の1/2=1/8096を足す
					tmpw+=(1.0/8192.0);
					tmpw*=4096.0;
					tmpint+=(int)tmpw;
					for(;isdigit(tmpstrs[0].s[p]);p++);
				}
				errtmp=PushCalcStack(TYPE_INT_LIT,tmpint*(tmp?(-1):1),MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(tmpstrs[0].s[p]=='&'){
				p++;
				if(toupper(tmpstrs[0].s[p])=='H'){
					p++;
					tmpint=0;
					i=0;
					while(isxdigit(tmpstrs[0].s[p])){
						if(i>=5){
							return ERR_SYNTAX_ERROR;
						}
						tmpint=(tmpint<<4)|((tmpstrs[0].s[p]<='9')?(tmpstrs[0].s[p]-'0'):(toupper(tmpstrs[0].s[p])-'A'+10));
						p++;i++;
					}
					tmpint*=4096;
				}else if(toupper(tmpstrs[0].s[p])=='B'){
					p++;
					tmpint=0;
					i=0;
					while(isBin(tmpstrs[0].s[p])){
						if(i>=20){
							return ERR_SYNTAX_ERROR;
						}
						tmpint=(tmpint<<1)|(tmpstrs[0].s[p]-'0');
						p++;i++;
					}
					tmpint*=4096;
				}else{
					return ERR_SYNTAX_ERROR;
				}
				errtmp=PushCalcStack(TYPE_INT_LIT,tmpint,MYSTR_NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				//?
			}
			break;
		case TOKEN_CHR:
			if(argcount==0)return ERR_MISSING_OPERAND;
			if(argcount>1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpint=FloorInt(tmpints[0]);
			if(tmpint<0 || tmpint>255)return ERR_OUT_OF_RANGE;
			tmpstr.s[0]=tmpint;
			tmpstr.s[1]=0;
			tmpstr.len=1;
			errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_HEX:
			if(argcount==0)return ERR_MISSING_OPERAND;
			if(argcount>1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			memset(tmpstr2,0x00,sizeof(char)*256);
			sprintf_s(tmpstr2,256,"%5X",FloorInt(tmpints[0]));
			errtmp=PushCalcStack(TYPE_STR_LIT,0,str2mystr2(tmpstr2),0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_INKEY:
			//要改修
			if(argcount>0)return ERR_SYNTAX_ERROR;
			if(!ReadKeyBuffer(&tmpc)){
				//キー入力なし
				tmpstr.s[0]=0;
				tmpstr.len=0;
				errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				tmpstr.s[0]=tmpc;
				tmpstr.s[1]=0;
				tmpstr.len=1;
				errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}
			break;
		case TOKEN_MID:
			if(argcount<3)return ERR_MISSING_OPERAND;
			if(argcount>3)return ERR_SYNTAX_ERROR;
			if(argtypes[2]!=ATYPE_STR || argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpints[1]=FloorInt(tmpints[1]);
			tmpints[0]=FloorInt(tmpints[0]);
			//TODO:文字列長の調査によるメモリリーク対策
			if(tmpints[1]<0)return ERR_OUT_OF_RANGE;
			if(tmpints[0]<0)return ERR_OUT_OF_RANGE;
			memset(tmpstr.s,0x00,sizeof(char)*256);
			memcpy(tmpstr.s,&(tmpstrs[2].s[tmpints[1]]),tmpints[0]);
			tmpstr.len = tmpints[0];
			errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_STR:
			if(argcount==0)return ERR_MISSING_OPERAND;
			if(argcount>1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			memset(tmpstr2,0x00,sizeof(char)*256);
			sprintf_s(tmpstr2,256,"%.3f",(double)tmpint/4096.0);
			for(i=strlen(tmpstr2)-1;(i>0)&&(tmpstr2[i]=='0');i--){
				tmpstr2[i]=0;
			}
			if(tmpstr2[i]=='.')tmpstr2[i]=0;
			errtmp=PushCalcStack(TYPE_STR_LIT,0,str2mystr2(tmpstr2),0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_BEEP:
			//音色,音高,音量,パン
			switch(argcount){
				case 0:
					SetFrequencySoundMem(44100,SHandleBEEP[0]);
					ChangeVolumeSoundMem(127,SHandleBEEP[0]);
					SetPanSoundMem(0,SHandleBEEP[0]);
					PlaySoundMem(SHandleBEEP[0],DX_PLAYTYPE_BACK);
					break;
				case 1:
					if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
					tmpints[0]=FloorInt(tmpints[0]);
					if(tmpints[0]<0 || tmpints[0]>69)return ERR_OUT_OF_RANGE;
					SetFrequencySoundMem(44100,SHandleBEEP[tmpints[0]]);
					ChangeVolumeSoundMem(127,SHandleBEEP[tmpints[0]]);
					SetPanSoundMem(0,SHandleBEEP[tmpints[0]]);
					PlaySoundMem(SHandleBEEP[tmpints[0]],DX_PLAYTYPE_BACK);
					break;
				case 2:
					if(argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
					tmpints[1]=FloorInt(tmpints[1]);tmpints[0]=FloorInt(tmpints[0]);
					if(tmpints[1]<0 || tmpints[1]>69)return ERR_OUT_OF_RANGE;
					if(tmpints[0]<-8192 || tmpints[0]>8192)return ERR_OUT_OF_RANGE;
					SetFrequencySoundMem((int)(powf(2.0,(float)tmpints[0]/4096.0)*44100.0),SHandleBEEP[tmpints[1]]);
					ChangeVolumeSoundMem(127,SHandleBEEP[tmpints[1]]);
					SetPanSoundMem(0,SHandleBEEP[tmpints[1]]);
					PlaySoundMem(SHandleBEEP[tmpints[1]],DX_PLAYTYPE_BACK);
					break;
				case 3:
					if(argtypes[2]!=ATYPE_INT || argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
					tmpints[2]=FloorInt(tmpints[2]);tmpints[1]=FloorInt(tmpints[1]);tmpints[0]=FloorInt(tmpints[0]);
					if(tmpints[2]<0 || tmpints[2]>69)return ERR_OUT_OF_RANGE;
					if(tmpints[1]<-8192 || tmpints[1]>8192)return ERR_OUT_OF_RANGE;
					if(tmpints[0]<0 || tmpints[0]>127)return ERR_OUT_OF_RANGE;
					SetFrequencySoundMem((int)(powf(2.0,(float)tmpints[1]/4096.0)*44100.0),SHandleBEEP[tmpints[2]]);
					ChangeVolumeSoundMem(255*tmpints[0]/128,SHandleBEEP[tmpints[2]]);
					SetPanSoundMem(0,SHandleBEEP[tmpints[2]]);
					PlaySoundMem(SHandleBEEP[tmpints[2]],DX_PLAYTYPE_BACK);
					break;
				case 4:
					if(argtypes[3]!=ATYPE_INT || argtypes[2]!=ATYPE_INT || argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
					tmpints[3]=FloorInt(tmpints[3]);tmpints[2]=FloorInt(tmpints[2]);tmpints[1]=FloorInt(tmpints[1]);tmpints[0]=FloorInt(tmpints[0]);
					if(tmpints[3]<0 || tmpints[3]>69)return ERR_OUT_OF_RANGE;
					if(tmpints[2]<-8192 || tmpints[2]>8192)return ERR_OUT_OF_RANGE;
					if(tmpints[1]<0 || tmpints[1]>127)return ERR_OUT_OF_RANGE;
					if(tmpints[0]<0 || tmpints[0]>127)return ERR_OUT_OF_RANGE;
					SetFrequencySoundMem((int)(powf(2.0,(float)tmpints[2]/4096.0)*44100.0),SHandleBEEP[tmpints[3]]);
					ChangeVolumeSoundMem(255*tmpints[1]/128,SHandleBEEP[tmpints[3]]);
					SetPanSoundMem((tmpints[0]-64)*10000/64,SHandleBEEP[tmpints[3]]);
					PlaySoundMem(SHandleBEEP[tmpints[3]],DX_PLAYTYPE_BACK);
					break;
				default:
					return ERR_SYNTAX_ERROR;
					break;
			}
			break;
		case TOKEN_BGMPLAY:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpints[0]=FloorInt(tmpints[0]);
			if(tmpints[0]<0 || tmpints[0]>29)return ERR_OUT_OF_RANGE;
			memset(tmpstr2,0x00,sizeof(char)*256);
			sprintf_s(tmpstr2,256,"BGM/BGM%d.ogg",tmpints[0]);
			StopSoundMem(SHandleBGM);
			SHandleBGM=LoadSoundMem(tmpstr2);
			if(isLoop(tmpints[0])){
				SetLoopSamplePosSoundMem(GetLoopPos(tmpints[0]),SHandleBGM);
				PlaySoundMem(SHandleBGM,DX_PLAYTYPE_LOOP);
			}else{
				PlaySoundMem(SHandleBGM,DX_PLAYTYPE_BACK);
			}
			break;
		case TOKEN_BGMSTOP:
			if(argcount>0)return ERR_SYNTAX_ERROR;
			StopSoundMem(SHandleBGM);
			break;
		case TOKEN_COLOR:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpint=FloorInt(tmpints[0]);
			if(tmpint<0 || tmpint>15)return ERR_OUT_OF_RANGE;
			consolecolor=tmpint;
			break;
		case TOKEN_CLEAR:
			if(argcount>0)return ERR_SYNTAX_ERROR;
			ClearDim();
			Psys_FREEVAR=VAR_MAX*4096;
			memset(Variable,0x00,sizeof(Variable));
			break;
		case TOKEN_CLS:
			if(argcount>0)return ERR_SYNTAX_ERROR;
			consolecolor=0;
			Psys_CSRX=0;Psys_CSRY=0;
			memset(con_buf,0x00,sizeof(con_buf));
			break;
		case TOKEN_LOCATE:
			//範囲外でも受け付ける
			if(argcount<2)return ERR_MISSING_OPERAND;
			if(argcount>2)return ERR_SYNTAX_ERROR;
			if(argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpints[1]=FloorInt(tmpints[1]);tmpints[0]=FloorInt(tmpints[0]);
			if(tmpints[1]>=0 && tmpints[1]<=31)Psys_CSRX=tmpints[1];
			if(tmpints[0]>=0 && tmpints[0]<=23)Psys_CSRY=tmpints[0];
			break;
		case TOKEN_PNLTYPE:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_STR)return ERR_TYPE_MISMATCH;
			if(mystrcmp2(tmpstrs[0],"OFF")==0){
				panelmode=PNLMD_OFF;kbd_shift_flag=0;
				break;
			}
			if(mystrcmp2(tmpstrs[0],"PNL")==0){
				panelmode=PNLMD_PANEL;kbd_shift_flag=0;
				break;
			}
			if(mystrcmp2(tmpstrs[0],"KYA")==0){
				panelmode=PNLMD_KYA;kbd_shift_flag=0;
				break;
			}
			if(mystrcmp2(tmpstrs[0],"KYM")==0){
				panelmode=PNLMD_KYM;kbd_shift_flag=0;
				break;
			}
			if(mystrcmp2(tmpstrs[0],"KYK")==0){
				panelmode=PNLMD_KYK;kbd_shift_flag=0;
				break;
			}
			return ERR_ILLEGAL_FUNCTION_CALL;
		case TOKEN_VISIBLE:
			if(argcount!=6)return ERR_SYNTAX_ERROR;
			if(argtypes[5]!=ATYPE_INT || argtypes[4]!=ATYPE_INT || argtypes[3]!=ATYPE_INT || argtypes[2]!=ATYPE_INT || argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			for(i=0;i<6;i++){
				tmpints[i]=FloorInt(tmpints[i]);
				if(tmpints[i]!=0 && tmpints[i]!=1)return ERR_OUT_OF_RANGE;
			}
			VisibleFlags.console=tmpints[5];
			VisibleFlags.panel=tmpints[4];
			VisibleFlags.BG0=tmpints[3];
			VisibleFlags.BG1=tmpints[2];
			VisibleFlags.sprite=tmpints[1];
			VisibleFlags.graphic=tmpints[0];
			break;
		case TOKEN_VSYNC:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpint=FloorInt(tmpints[0]);
			if(tmpint<0)return ERR_OUT_OF_RANGE;
			for(i=0;i<tmpint;i++){
				ProcessFrame();
				WaitVSync(1);
			}
			break;
		case TOKEN_BGPAGE:
			if(argcount!=1)return ERR_SYNTAX_ERROR;
			if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpint=FloorInt(tmpints[0]);
			if((tmpint<0)||(tmpint>1))return ERR_OUT_OF_RANGE;
			bgpage=tmpint;
			break;
		case TOKEN_BGCLIP:
			if(argcount==0)return ERR_SYNTAX_ERROR;
			if(argcount!=4)return ERR_MISSING_OPERAND;
			if(argtypes[3]!=ATYPE_INT || argtypes[2]!=ATYPE_INT || argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpints[3]=FloorInt(tmpints[3]);tmpints[2]=FloorInt(tmpints[2]);tmpints[1]=FloorInt(tmpints[1]);tmpints[0]=FloorInt(tmpints[0]);
			if(tmpints[3]>32 || tmpints[3]<0)return ERR_OUT_OF_RANGE;
			if(tmpints[2]>24 || tmpints[2]<0)return ERR_OUT_OF_RANGE;
			if(tmpints[1]>32 || tmpints[1]<0)return ERR_OUT_OF_RANGE;
			if(tmpints[0]>24 || tmpints[0]<0)return ERR_OUT_OF_RANGE;
			bgclip_x_begin[bgpage]=tmpints[3];
			bgclip_y_begin[bgpage]=tmpints[2];
			bgclip_x_end[bgpage]=tmpints[1];
			bgclip_y_end[bgpage]=tmpints[0];
			break;
		case TOKEN_BGOFS:
			if(argcount==0)return ERR_SYNTAX_ERROR;
			if(argcount==3){
				if(argtypes[2]!=ATYPE_INT || argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				tmpints[2]=FloorInt(tmpints[2]);tmpints[1]=FloorInt(tmpints[1])%512;tmpints[0]=FloorInt(tmpints[0])%512;
				if(tmpints[2]>1 || tmpints[2]<0)return ERR_OUT_OF_RANGE;
				bgofs_destx[bgpage][tmpints[2]]=tmpints[1];
				bgofs_desty[bgpage][tmpints[2]]=tmpints[0];
				bgofs_nowx[bgpage][tmpints[2]]=tmpints[1];
				bgofs_nowy[bgpage][tmpints[2]]=tmpints[0];
			}else if(argcount==4){
				if(argtypes[3]!=ATYPE_INT || argtypes[2]!=ATYPE_INT || argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				tmpints[3]=FloorInt(tmpints[3]);tmpints[2]=FloorInt(tmpints[2])%512;tmpints[1]=FloorInt(tmpints[1])%512;tmpints[0]=FloorInt(tmpints[0]);
				if(tmpints[3]>1 || tmpints[3]<0)return ERR_OUT_OF_RANGE;
				if(tmpints[0]<0)return ERR_OUT_OF_RANGE;
				bgofs_destx[bgpage][tmpints[3]]=tmpints[2];
				bgofs_desty[bgpage][tmpints[3]]=tmpints[1];
				bgofs_ip[bgpage][tmpints[3]]=tmpints[0];
			}else{
				return ERR_MISSING_OPERAND;
			}
		case TOKEN_BGPUT:
			if(argcount==0)return ERR_SYNTAX_ERROR;
			if(argcount!=7)return ERR_MISSING_OPERAND;
			if(argtypes[6]!=ATYPE_INT || argtypes[5]!=ATYPE_INT || argtypes[4]!=ATYPE_INT || argtypes[3]!=ATYPE_INT || argtypes[2]!=ATYPE_INT || argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
			tmpints[6]=FloorInt(tmpints[6]);tmpints[5]=FloorInt(tmpints[5])%64;tmpints[4]=FloorInt(tmpints[4])%64;tmpints[3]=FloorInt(tmpints[3]);tmpints[2]=FloorInt(tmpints[2]);tmpints[1]=FloorInt(tmpints[1]);tmpints[0]=FloorInt(tmpints[0]);
			if(tmpints[6]>1 || tmpints[6]<0)return ERR_OUT_OF_RANGE;
			if(tmpints[3]<0 || tmpints[3]>1024)return ERR_OUT_OF_RANGE;
			if(tmpints[2]<0 || tmpints[2]>16)return ERR_OUT_OF_RANGE;
			if(tmpints[1]<0 || tmpints[1]>1)return ERR_OUT_OF_RANGE;
			if(tmpints[0]<0 || tmpints[0]>1)return ERR_OUT_OF_RANGE;
			BGData[bgpage][tmpints[6]][tmpints[5]][tmpints[4]].character=tmpints[3];
			BGData[bgpage][tmpints[6]][tmpints[5]][tmpints[4]].palette=tmpints[2];
			BGData[bgpage][tmpints[6]][tmpints[5]][tmpints[4]].h_inverse=tmpints[1];
			BGData[bgpage][tmpints[6]][tmpints[5]][tmpints[4]].v_inverse=tmpints[0];
			break;
		case TOKEN_BGREAD:
			if(argcount!=7)return ERR_MISSING_OPERAND;
			if(argtypes[6]!=ATYPE_INT || argtypes[5]!=ATYPE_INT || argtypes[4]!=ATYPE_INT || argtypes[3]!=ATYPE_VAR || argtypes[2]!=ATYPE_VAR || argtypes[1]!=ATYPE_VAR || argtypes[0]!=ATYPE_VAR)return ERR_TYPE_MISMATCH;
			tmpints[6]=FloorInt(tmpints[6]);tmpints[5]=FloorInt(tmpints[5]);tmpints[4]=FloorInt(tmpints[4]);
			if(tmpints[6]>1 || tmpints[6]<0)return ERR_OUT_OF_RANGE;
			tmpints[5]=mymod(tmpints[5],64);
			tmpints[4]=mymod(tmpints[4],64);
			memset(&tmpstr,0x00,sizeof(st));
			tmpint=BGData[bgpage][tmpints[6]][tmpints[5]][tmpints[4]].character;
			errtmp=EvalFormula_Substitution(argtypes[3],tmpints[3],ATYPE_INT,tmpint,tmpstr);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			BGData[bgpage][tmpints[6]][tmpints[5]][tmpints[4]].palette;
			errtmp=EvalFormula_Substitution(argtypes[2],tmpints[2],ATYPE_INT,tmpint,tmpstr);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			BGData[bgpage][tmpints[6]][tmpints[5]][tmpints[4]].h_inverse;
			errtmp=EvalFormula_Substitution(argtypes[1],tmpints[1],ATYPE_INT,tmpint,tmpstr);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			BGData[bgpage][tmpints[6]][tmpints[5]][tmpints[4]].v_inverse;
			errtmp=EvalFormula_Substitution(argtypes[0],tmpints[0],ATYPE_INT,tmpint,tmpstr);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case TOKEN_DIM:
			if(!PopCalcStack_str(&tmpstr))return ERR_SYNTAX_ERROR;
			if(!PopCalcStack_int(&tmpints[2]))return ERR_SYNTAX_ERROR;
			tmpints[1]=FloorInt(tmpints[1]);tmpints[0]=FloorInt(tmpints[0]);
			
			if(argcount==0){
				return ERR_SYNTAX_ERROR;
			}else if(argcount>2){
				return ERR_MISSING_OPERAND;
			}
			tmpint=NewVar(tmpstr);
			Variable[tmpint].isDim=true;
			Variable[tmpint].isStr=tmpints[2];
			if(argcount==2){
				if(argtypes[1]!=ATYPE_INT || argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=RegistDim(tmpint,tmpints[1],tmpints[0]);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
				errtmp=RegistDim(tmpint,tmpints[0],0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}
			break;
		case OP_SUBSTITUTE:
			if(argcount<2)return ERR_MISSING_OPERAND;
			if(argcount>2)return ERR_SYNTAX_ERROR;
			errtmp=EvalFormula_Substitution(argtypes[1],tmpints[1],argtypes[0],tmpints[0],tmpstrs[0]);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		default:
			//配列
			if(arg>=0 && arg<1024){
				if(argcount==1){
					tmpints[0]=FloorInt(tmpints[0]);
					if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
					tmpints[3]=Variable[arg].value;
					if(dim_index[tmpints[3]].indexmax2!=0)return ERR_SYNTAX_ERROR;
					if(tmpints[0]>=dim_index[tmpints[3]].indexmax1)return ERR_SUBSCRIPT_OUT_OF_RANGE;
					if(dim_index[tmpints[3]].isStr){
						errtmp=PushCalcStack(TYPE_STR_LIT,0,str2mystr2((char*)(dim_mem+dim_index[tmpints[3]].address+tmpints[0]*256)),0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}else{
						errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)*(int32_t*)(dim_mem+(dim_index[tmpints[3]].address+tmpints[0]*4)),MYSTR_NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}
				}else if(argcount==2){
					tmpints[0]=FloorInt(tmpints[0]);
					tmpints[1]=FloorInt(tmpints[1]);
					if(argtypes[0]!=ATYPE_INT || argtypes[1]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
					tmpints[3]=Variable[arg].value;
					if(dim_index[tmpints[3]].indexmax2==0)return ERR_SYNTAX_ERROR;
					if(tmpints[1]>=dim_index[tmpints[3]].indexmax1 || tmpints[0]>=dim_index[tmpints[3]].indexmax2)return ERR_SUBSCRIPT_OUT_OF_RANGE;
					if(dim_index[tmpints[3]].isStr){
						errtmp=PushCalcStack(TYPE_STR_LIT,0,str2mystr2((char*)(dim_mem+dim_index[tmpints[3]].address+(tmpints[1]+tmpints[0]*dim_index[tmpints[3]].indexmax1)*256)),0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}else{
						errtmp=PushCalcStack(TYPE_INT_LIT,(int32_t)*(int32_t*)(dim_mem+dim_index[tmpints[3]].address+(tmpints[1]+tmpints[0]*dim_index[tmpints[3]].indexmax1)*4),MYSTR_NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}
				}else{
					return ERR_SYNTAX_ERROR;
				}
				break;
			//配列ポインタ?
			}else if(arg>=1024 && arg<2048){
				if(argcount==1){
					tmpints[0]=FloorInt(tmpints[0]);
					if(argtypes[0]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
					tmpints[3]=Variable[arg-1024].value;
					if(dim_index[tmpints[3]].indexmax2!=0)return ERR_SYNTAX_ERROR;
					if(tmpints[0]>=dim_index[tmpints[3]].indexmax1)return ERR_SUBSCRIPT_OUT_OF_RANGE;
					if(dim_index[tmpints[3]].isStr){
						errtmp=PushCalcStack(TYPE_STR_PTR,(int32_t)(dim_index[tmpints[3]].address+tmpints[0]*256)*4096,MYSTR_NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}else{
						errtmp=PushCalcStack(TYPE_INT_PTR,(int32_t)(dim_index[tmpints[3]].address+tmpints[0]*4)*4096,MYSTR_NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}
				}else if(argcount==2){
					tmpints[0]=FloorInt(tmpints[0]);
					tmpints[1]=FloorInt(tmpints[1]);
					if(argtypes[0]!=ATYPE_INT || argtypes[1]!=ATYPE_INT)return ERR_TYPE_MISMATCH;
					tmpints[3]=Variable[arg-1024].value;
					if(dim_index[tmpints[3]].indexmax2==0)return ERR_SYNTAX_ERROR;
					if(tmpints[1]>=dim_index[tmpints[3]].indexmax1 || tmpints[0]>=dim_index[tmpints[3]].indexmax2)return ERR_SUBSCRIPT_OUT_OF_RANGE;
					if(dim_index[tmpints[3]].isStr){
						errtmp=PushCalcStack(TYPE_STR_PTR,(int32_t)(dim_index[tmpints[3]].address+(tmpints[1]+tmpints[0]*dim_index[tmpints[3]].indexmax1)*256)*4096,MYSTR_NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}else{
						errtmp=PushCalcStack(TYPE_INT_PTR,(int32_t)(dim_index[tmpints[3]].address+(tmpints[1]+tmpints[0]*dim_index[tmpints[3]].indexmax1)*4)*4096,MYSTR_NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}
				}else{
					return ERR_SYNTAX_ERROR;
				}
				break;
			}else{
				printf("!!!!!Unknown function(%d)\n",arg);
			}
			break;
	}
	return ERR_NO_ERROR;
}

int* GetSystemVariableIntPtr(uint16_t arg){
	uint16_t Table1[]={
		TOKEN_TRUE,TOKEN_FALSE,TOKEN_CANCEL,TOKEN_VERSION,TOKEN_TCHX,
		TOKEN_TCHY,TOKEN_TCHST,TOKEN_TCHTIME,TOKEN_FUNCNO,TOKEN_CSRX,
		TOKEN_CSRY,TOKEN_ERR,TOKEN_ERL,TOKEN_RESULT,TOKEN_MAINCNTL,
		TOKEN_MAINCNTH,TOKEN_FREEMEM,TOKEN_FREEVAR,
		TOKEN_TABSTEP,TOKEN_SYSBEEP,TOKEN_ICONPUSE,TOKEN_ICONPAGE,TOKEN_ICONPMAX,
		0
	};
	int* Table2[]={
		&Psys_TRUE,&Psys_FALSE,&Psys_CANCEL,&Psys_VERSION,&Psys_TCHX,
		&Psys_TCHY,&Psys_TCHST,&Psys_TCHTIME,&Psys_FUNCNO,&Psys_CSRX,
		&Psys_CSRY,&Psys_ERR,&Psys_ERL,&Psys_RESULT,&Psys_MAINCNTL,
		&Psys_MAINCNTH,&Psys_FREEMEM,&Psys_FREEVAR,
		&Psys_TABSTEP,&Psys_SYSBEEP,&Psys_ICONPUSE,&Psys_ICONPAGE,&Psys_ICONPMAX,
		NULL
	};

	int i;
	for(i=0;Table1[i]!=0;i++){
		if(Table1[i]==arg)return Table2[i];
	}
	return NULL;
}

st* GetSystemVariableStrPtr(uint16_t arg){
	uint16_t Table1[]={
		TOKEN_TIME,TOKEN_DATE,
		TOKEN_MEM,
		0
	};
	st* Table2[]={
		&Psys_TIME,&Psys_DATE,
		&Psys_MEM,
		0
	};
	int i;
	for(i=0;Table1[i]!=0;i++){
		if(Table1[i]==arg)return Table2[i];
	}
	return NULL;
}

void SystemVariableLimitValue(void){
	Psys_TABSTEP=limitrange(FloorInt(Psys_TABSTEP)*4096,0,16*4096);
	Psys_ICONPUSE=limitrange(FloorInt(Psys_ICONPUSE)*4096,0,4096);
	Psys_ICONPAGE=limitrange(FloorInt(Psys_ICONPAGE)*4096,0,FloorInt(Psys_ICONPAGE)*4096);
	Psys_ICONPMAX=limitrange(FloorInt(Psys_ICONPMAX)*4096,0,FloorInt(Psys_ICONPMAX)*4096);
	Psys_SYSBEEP=limitrange(FloorInt(Psys_SYSBEEP)*4096,0,4096);
	return;
}

void UpdateSystemVariable(void){
	char tmpstr[STR_LEN_MAX];
	time_t nowtime;
	struct tm *nowtime2;
	static int maincount_start=GetNowCount();
	int maincount;
	nowtime=time(NULL);
	nowtime2=localtime(&nowtime);
	maincount=(GetNowCount()-maincount_start)*3/50;
	memset(tmpstr,0x00,sizeof(tmpstr));
	sprintf_s(tmpstr,256,"%02d:%02d:%02d",nowtime2->tm_hour,nowtime2->tm_min,nowtime2->tm_sec);
	mystrcpy2(&Psys_TIME,tmpstr);
	memset(tmpstr,0x00,sizeof(tmpstr));
	sprintf_s(tmpstr,256,"%04d/%02d/%02d",nowtime2->tm_year+1900,nowtime2->tm_mon+1,nowtime2->tm_mday);
	mystrcpy2(&Psys_DATE,tmpstr);
	//TODO:Psys_FREEMEM
	Psys_MAINCNTL=(maincount%524288)*4096;
	Psys_MAINCNTH=(maincount/524288)*4096;
	if(runmode!=RMD_PRG)Psys_ICONPAGE=0x00000000;
	return;
}

void InitSystemVariable(void){
	Psys_VERSION=0x1020000;//ver 1.2
	Psys_TRUE=0x00001000;
	Psys_FALSE=0x00000000;
	Psys_CANCEL=0xFFFFF000;
	Psys_CSRX=0x00000000;
	Psys_CSRY=0x00000000;
	Psys_FREEMEM=0x00400000;
	Psys_ERR=0x00000000;
	Psys_ERL=0x00000000;
	Psys_RESULT=0x00000000;
	Psys_TCHX=0x00000000;
	Psys_TCHY=0x00000000;
	Psys_TCHST=0x00000000;
	Psys_TCHTIME=0x00000000;
	Psys_MAINCNTL=0x00000000;
	Psys_MAINCNTH=0x00000000;
	Psys_TABSTEP=0x00000000;
	Psys_ICONPUSE=0x00000000;
	Psys_ICONPAGE=0x00000000;
	Psys_ICONPMAX=0x00000000;
	Psys_FUNCNO=0x00000000;
	Psys_FREEVAR=VAR_MAX*4096;
	Psys_SYSBEEP=0x00001000;
	mystrclear(&Psys_TIME);
	mystrclear(&Psys_DATE);
	mystrclear(&Psys_MEM);
}

int EvalFormula_Substitution(int type1,int value1,int type0,int value0,st str0){
	int tmpint;
	st str={0,""};
	char tmpstr[257];
	if(type1!=ATYPE_VAR && type1!=ATYPE_INT_PTR && type1!=ATYPE_STR_PTR)return ERR_SYNTAX_ERROR;
	if(type1==ATYPE_VAR){
		tmpint=GetSystemVariableType(value1);
		if(tmpint!=0){
			switch(tmpint){
				case 2:
					*GetSystemVariableIntPtr(value1)=FloorInt(value0)*4096;
					break;
				case 4:
					Psys_MEM=str0;
					break;
				case 1: case 3: default:
					return ERR_SYNTAX_ERROR;
			}
			SystemVariableLimitValue();
		}else if(type0==ATYPE_INT){
			if(Variable[value1].isStr)return ERR_TYPE_MISMATCH;
			Variable[value1].value=value0;
		}else if(type0==ATYPE_STR){
			if(!(Variable[value1].isStr))return ERR_TYPE_MISMATCH;
			Variable[value1].string=str0;
		}else{
			return ERR_SYNTAX_ERROR;
		}
	}else{
		//PTR
		value1=FloorInt(value1);
		if(type0==ATYPE_INT){
			if(type1==ATYPE_STR_PTR)return ERR_TYPE_MISMATCH;
			*(int32_t*)(dim_mem+value1)=value0;
		}else if(type0==ATYPE_STR){
			if(type1==ATYPE_INT_PTR)return ERR_TYPE_MISMATCH;
			memset((char*)(dim_mem+value1),0x00,sizeof(BYTE)*256);
			memset(tmpstr,0x00,257);
			mystr2str2(str0,tmpstr);
			strcpy_s((char*)(dim_mem+value1),256,tmpstr);
		}else{
			return ERR_SYNTAX_ERROR;
		}
	}
	return ERR_NO_ERROR;
}