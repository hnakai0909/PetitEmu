#include "evalformula.h"

struct OP_S op_s[OP_S_MAX];//演算子スタック

struct CalcStack calc_s[CALC_S_MAX];//計算スタック

struct VARIABLE Variable[1024];

//プチコンシステム変数(文字列)
char Psys_DATE_D[10];	//DATE$
char Psys_TIME_D[10];	//TIME$
char Psys_MEM_D[256];	//MEM$

char consolecharbuf[32][24];
char consolecolorbuf[32][24];
const int Psys_VERSION=0x1020;//ver 1.2
const int Psys_TRUE=1;
const int Psys_FALSE=0;
const int Psys_CANCEL=-1;
int Psys_CSRX;
int Psys_CSRY;
int Psys_FREEMEM;
int Psys_ERR;
int Psys_ERL;
int Psys_RESULT;
int Psys_TCHX;
int Psys_TCHY;
int Psys_TCHST;
int Psys_TCHTIME;
int Psys_MAINCNTL;
int Psys_MAINCNTH;
int Psys_TABSTEP;
int Psys_ICONPUSE;
int Psys_ICONPAGE;
int Psys_ICONPMAX;
int Psys_FUNCNO;
int Psys_FREEVAR;
bool Psys_SYSBEEP;

int consolecolor;

int push_opstack(int op,int argcount){
	if(op_sl>=OP_S_MAX)return false;
	(op_s+op_sl)->op=op;
	(op_s+op_sl)->argcount=argcount;
	printf("push op_s(%d) value=%s,argc=%d\n",op_sl,TokenID2Str(op),argcount);
	op_sl++;
	return true;
}

bool pop_opstack(int* op){
	if(op_sl<=0)return false;
	op_sl--;
	*op=(op_s+op_sl)->op;
	(op_s+op_sl)->op=0;
	(op_s+op_sl)->argcount=0;
	return true;
}

int push_calcstack(int type,int32_t value,char* str,int argc){
	int errtmp=0;
	int tmp=0,tmp2=0;
	printf("push?cs(%d) %d %d(%s) %s %d\n",calc_sl,type,value,TokenID2Str(value),str,argc);
	if(type==TYPE_FUNC){
		if(isFunction(value)){
			push_opstack(value,0);
		}else if(value==OP_BRACKET_L){
			push_opstack(value,0);
		}else if(value==OP_BRACKET_R){
			tmp=0;
			while(op_sl>0){
				tmp2=(op_s+op_sl-1)->op;
				if(tmp2==OP_BRACKET_L){
					tmp=1;
					break;
				}else{
					pop_opstack(&tmp2);
					errtmp=EvalFormula(tmp2);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
				}
			}
			if(tmp==0){
				//puts("parentheses mismatched");
				return ERR_SYNTAX_ERROR;
			}
			(op_s+op_sl)->op=0;
			op_sl--;
			if(op_sl>0){
				tmp2=(op_s+op_sl-1)->op;
				if(isFunction(tmp2)){
					(op_s+op_sl)->op=0;
					op_sl--;
					errtmp=EvalFormula(tmp2);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
				}
			}
		}else if(isOperator(value)){
			while(op_sl>0){
				tmp2=(op_s+op_sl-1)->op;
				if((tmp2!=OP_BRACKET_L)&&(tmp2!=OP_BRACKET_R)&&isOperator(tmp2) &&
                        ((isOperatorLeftAssoc(value) && (GetOperatorPriority(value) >= GetOperatorPriority(tmp2))) ||
                           (GetOperatorPriority(value) > GetOperatorPriority(tmp2)))){
					pop_opstack(&tmp2);
					errtmp=EvalFormula(tmp2);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
				}else{
					break;
				}
			}
			push_opstack(value,0);
		}else if(isNoArgInstruction(value)||isArgInstruction(value)){
			push_opstack(value,0);
		}else{
			//Unknown token
			return ERR_SYNTAX_ERROR;
		}
	}else if(type==TYPE_SPECIAL){
		if(value==CODE_COMMA){
			tmp=0;
			while(op_sl>0){
				tmp2=(op_s+op_sl-1)->op;
				if(tmp2==OP_BRACKET_L){
					tmp=1;
					break;
				}else{
					op_sl--;
					(op_s+op_sl)->op=0;
					errtmp=EvalFormula(tmp2);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
				}
			}
			if(tmp==0){
				//separator or parentheses mismatched
				return ERR_SYNTAX_ERROR;
			}
		}

	}else{
		printf("push!cs(%d) %d %d(%.3f) %s %d\n",calc_sl,type,value,(float)value/4096.0,str,argc);
		if(calc_sl>=CALC_S_MAX) return -1;
		(calc_s+calc_sl)->type=type;
		(calc_s+calc_sl)->value=value;
		if(type==TYPE_STR_LIT)strcpy((calc_s+calc_sl)->string,str);
		(calc_s+calc_sl)->argcount=argc;
		calc_sl++;
	}
	return ERR_NO_ERROR;
}

bool pop_calcstack_int(int32_t* arg){
	calc_sl--;
	if((calc_s+calc_sl)->type==TYPE_INT_LIT){
		*arg=(calc_s+calc_sl)->value;
		printf("pop cs(%d) INT %d\n",calc_sl,(calc_s+calc_sl)->value);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

//整数
bool pop_calcstack_flooredint(int32_t* arg){
	calc_sl--;
	if((calc_s+calc_sl)->type==TYPE_INT_LIT){
		*arg=(calc_s+calc_sl)->value;	
		*arg&=0xfffff000;
		//-0バグ再現?
		printf("pop cs(%d) fINT %d(%d)\n",calc_sl,*arg,*arg/4096);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

bool pop_calcstack_str(char* arg){
	calc_sl--;
	if((calc_s+calc_sl)->type==TYPE_STR_LIT){
		memset(arg,0x00,sizeof(arg));
		strcpy(arg,(calc_s+calc_sl)->string);
		printf("pop cs(%d) STR %s\n",calc_sl,(calc_s+calc_sl)->string);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

bool pop_calcstack_var(int* arg){
	calc_sl--;
	if(((calc_s+calc_sl)->type==TYPE_INT_VAR)||((calc_s+calc_sl)->type==TYPE_STR_VAR)){
		memset(arg,0x00,sizeof(arg));
		*arg=(calc_s+calc_sl)->value;
		printf("pop cs(%d) VAR %s\n",calc_sl,(Variable+((calc_s+calc_sl)->value))->name);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

bool pop_calcstack_void(void){
	calc_sl--;
	if((calc_s+calc_sl)->type==TYPE_VOID){
		printf("pop cs(%d) VOID\n",calc_sl);
		memset(calc_s+calc_sl,0x00,sizeof(calc_s+calc_sl));
		return true;
	}else{
		calc_sl++;
	}
	return false;
}

int EvalFormula(const int arg){
	int tmp=0,i=0;
	int errtmp=ERR_NO_ERROR;
	int32_t tmpint=0;
	int32_t tmpints[10];
	int64_t tmpint64=0;
	char tmpstr[512];	
	char tmpstrs[10][256];
	memset(tmpints,0x00,sizeof(tmpints));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(tmpstrs,0x00,sizeof(tmpstrs));
	printf("EvalFormula arg=%d(%s)\n",arg,TokenID2Str(arg));
	switch(arg){
		case OP_PLUS:
			if(pop_calcstack_int(&tmpint)){
				tmpints[0]=tmpint;
				if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
				tmpint64=tmpint+tmpints[0];
				if((tmpint64>2147483647 || tmpint64<-2147483647))return ERR_OVERFLOW;
				errtmp=push_calcstack(TYPE_INT_LIT,(int32_t)tmpint64,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(pop_calcstack_str(tmpstr)){
				strcpy(tmpstrs[0],tmpstr);
				if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
				if((strlen(tmpstrs[0])+strlen(tmpstr))>256){
					return ERR_STRING_TOO_LONG;
				}else{
					strcpy(tmpstr,tmpstrs[1]);
					strcat(tmpstr,tmpstrs[0]);
					errtmp=push_calcstack(TYPE_STR_LIT,0,tmpstr,0);
					if(errtmp==ERR_NO_ERROR)return errtmp;
				}
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case OP_MULTIPLY:
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;//mkII:文字列対応
			tmpints[0]=tmpint;
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			tmpint64=(int64_t)(((double)((double)tmpint/4096.0)*(double)((double)tmpints[0]/4096.0))*4096);
			if((tmpint64>2147483647 || tmpint64<-2147483647)){
				return ERR_OVERFLOW;
			}
			errtmp=push_calcstack(TYPE_INT_LIT,(int32_t)tmpint64,NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case OP_DIVIDE:
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			if(tmpint==0)return ERR_DIVISION_BY_ZERO;
			tmpints[0]=tmpint;
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			tmpint64=(int64_t)(((double)((double)tmpint/4096.0)/(double)((double)tmpints[0]/4096.0))*4096);
			if((tmpint64>2147483647 || tmpint64<-2147483647)){
				return ERR_OVERFLOW;
			}
			errtmp=push_calcstack(TYPE_INT_LIT,(int32_t)tmpint64,NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case OP_MOD:
			/*
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			if(tmpint==0)return ERR_DIVISION_BY_ZERO;
			tmpints[0]=tmpint;
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[1]=tmpint;
			tmpint64=(int64_t)(((double)((double)tmpints[1]/4096.0)/(double)((double)tmpints[0]/4096.0))*4096);
			if((tmpint64>2147483647 || tmpint64<-2147483647)){
				return ERR_OVERFLOW;
			}
			errtmp=push_calcstack(TYPE_INT_LIT,(int32_t)tmpint64,NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			*/
			break;
		case OP_MINUSSIGN:
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			errtmp=push_calcstack(TYPE_INT_LIT,-tmpint,NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case OP_MINUS:
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[0]=tmpint;
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			tmpint64=tmpint-tmpints[0];
			if((tmpint64>2147483647 || tmpint64<-2147483647)){
				return ERR_OVERFLOW;
			}
			errtmp=push_calcstack(TYPE_INT_LIT,(int32_t)tmpint64,NULL,0);
			if(errtmp!=ERR_NO_ERROR)return errtmp;
			break;
		case OP_SHONARI:
			if(pop_calcstack_int(&tmpint)){
				tmpints[0]=tmpint;
				if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(tmpint<tmpints[0])*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(pop_calcstack_str(tmpstr)){
				strcpy(tmpstrs[0],tmpstr);
				if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(strcmp(tmpstr,tmpstrs[0])<0)*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_DAINARI:
			if(pop_calcstack_int(&tmpint)){
				tmpints[0]=tmpint;
				if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(tmpint>tmpints[0])*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(pop_calcstack_str(tmpstr)){
				strcpy(tmpstrs[0],tmpstr);
				if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(strcmp(tmpstr,tmpstrs[0])>0)*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_SHONARI_EQUAL:
			if(pop_calcstack_int(&tmpint)){
				tmpints[0]=tmpint;
				if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(tmpint<=tmpints[0])*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(pop_calcstack_str(tmpstr)){
				strcpy(tmpstrs[0],tmpstr);
				if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(strcmp(tmpstr,tmpstrs[0])<=0)*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_DAINARI_EQUAL:
			if(pop_calcstack_int(&tmpint)){
				tmpints[0]=tmpint;
				if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(tmpint>=tmpints[0])*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(pop_calcstack_str(tmpstr)){
				strcpy(tmpstrs[0],tmpstr);
				if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(strcmp(tmpstr,tmpstrs[0])>=0)*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{	
				return ERR_TYPE_MISMATCH;
			}
			break;
		case OP_EQUAL:
			if(pop_calcstack_int(&tmpint)){
				tmpints[0]=tmpint;
				if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(tmpint==tmpints[0])*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(pop_calcstack_str(tmpstr)){
				strcpy(tmpstrs[0],tmpstr);
				if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(strcmp(tmpstr,tmpstrs[1])==0)*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case OP_NOTEQUAL:
			if(pop_calcstack_int(&tmpint)){
				tmpints[0]=tmpint;
				if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(tmpint!=tmpints[0])*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else if(pop_calcstack_str(tmpstr)){
				strcpy(tmpstrs[0],tmpstr);
				if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
				errtmp=push_calcstack(TYPE_INT_LIT,(strcmp(tmpstrs[0],tmpstr)!=0)*4096,NULL,0);
				if(errtmp!=ERR_NO_ERROR)return errtmp;
			}else{
				return ERR_SYNTAX_ERROR;
			}
			break;
		case OP_OR:
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[1]=tmpint/4096;
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[0]=tmpint/4096;
			push_calcstack(TYPE_INT_LIT,((tmpints[0])|(tmpints[1]))*4096,NULL,0);
			break;
		case OP_AND:
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[1]=tmpint/4096;
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[0]=tmpint/4096;
			push_calcstack(TYPE_INT_LIT,((tmpints[0])&(tmpints[1]))*4096,NULL,0);
			break;
		case OP_NOT:
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			push_calcstack(TYPE_INT_LIT,(~(tmpint/4096))*4096,NULL,0);
			break;
		case OP_XOR:
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[1]=tmpint/4096;
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[0]=tmpint/4096;
			push_calcstack(TYPE_INT_LIT,((tmpints[0])^(tmpints[1]))*4096,NULL,0);
			break;
		case FUNC_ABS:
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			push_calcstack(TYPE_INT_LIT,abs(tmpint),NULL,0);
			break;
		case FUNC_ASC:
			if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
			push_calcstack(TYPE_INT_LIT,tmpstr[0]*4096,NULL,0);
			break;
		case FUNC_ATAN:
			//引数2個なら
			if(/*args==*/2){
				//返り値小数なんだけど訴訟
				//(calc_s+calc_sl-1)->value=atan2((calc_s+calc_sl-1)->value,(calc_s+calc_sl)->value);
			//引数1個なら
			}else if(/*args==*/1){
//				(calc_s+calc_sl-1)->value=(int32_t)((atan((double)((calc_s+calc_sl)->value)/4096.0))*4096.0);
			}else{
				//???
			}
			break;
		case FUNC_BGMCHK:
			if(!pop_calcstack_void())return ERR_SYNTAX_ERROR;
			push_calcstack(TYPE_INT_LIT,/*   BGMCHK した結果   */0,NULL,0);
			//引数なし
			break;
		case FUNC_BUTTON:
			//引数省略可
			break;
		case FUNC_COS:
			//返り値小数なんだけど訴訟
			break;
		case FUNC_CHKCHR:
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[0]=tmpint/4096;
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[1]=tmpint/4096;
			if(tmpints[1]>=0 && tmpints[1]<=31 && tmpints[0]>=0 && tmpints[0]<=23){
				tmpint=consolecharbuf[tmpints[1]][tmpints[0]];
				push_calcstack(TYPE_INT_LIT,tmpint*4096,NULL,0);
			}else{
				return ERR_OUT_OF_RANGE;
			}
			break;
		case FUNC_DEG:
			if(!pop_calcstack_int(&tmpint))return ERR_TYPE_MISMATCH;
			push_calcstack(TYPE_INT_LIT,tmpint*180/3.141592,NULL,0);
			break;
		case FUNC_EXP:
			break;
		case FUNC_FLOOR:
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			push_calcstack(TYPE_INT_LIT,tmpint,NULL,0);
			break;
		case FUNC_GSPOIT:
			break;
		case FUNC_ICONCHK:
			break;
		case FUNC_LEN:
			tmp=pop_calcstack_str(tmpstr);
			if(tmp){
				push_calcstack(TYPE_INT_LIT,strlen(tmpstr),NULL,0);
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case FUNC_LOG:
			break;
		case FUNC_PI:
			//11.0010010000111110... ≒ 11.00100100001 = 0x3243
			tmp=pop_calcstack_void();
			push_calcstack(TYPE_INT_LIT,0x00003243,NULL,0);
			break;
		case FUNC_RAD:
			tmp=pop_calcstack_int(&tmpint);
			if(!tmp)return ERR_TYPE_MISMATCH;
			push_calcstack(TYPE_INT_LIT,tmpint*3.141592/180,NULL,0);
			break;
		case FUNC_RND:
			tmp=pop_calcstack_flooredint(&tmpint);
			tmpint/=4096;
			if(!tmp)return ERR_TYPE_MISMATCH;
			push_calcstack(TYPE_INT_LIT,(int32_t)(rand()*(tmpint)/(1.0+RAND_MAX))*4096,NULL,0);
			break;
		case FUNC_SGN:
			tmp=pop_calcstack_int(&tmpint);
			if(tmp){
				push_calcstack(TYPE_INT_LIT,((tmpint==0)?0:((tmpint>0)?1:-1))*4096,NULL,0);
			}else{
				return ERR_TYPE_MISMATCH;
			}
			break;
		case FUNC_SIN:
			break;
		case FUNC_SQR:
			break;
		case FUNC_TAN:
			break;
		case FUNC_VAL:
			break;
		case SFUNC_CHR:
			memset(tmpstr,0x00,sizeof(tmpstr));
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpint/=4096;
			if(tmpint>=0 && tmpint<=255){
				tmpstr[0]=tmpint;
				push_calcstack(TYPE_STR_LIT,0,tmpstr,0);
			}else{
				return ERR_OUT_OF_RANGE;
			}
			break;
		case SFUNC_HEX:
			memset(tmpstr,0x00,sizeof(tmpstr));
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpint/=4096;
			sprintf(tmpstr,"%05X",tmpint);
			push_calcstack(TYPE_STR_LIT,0,tmpstr,0);
			break;
		case SFUNC_INKEY:
			break;
		case SFUNC_MID:
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[0]=tmpint/4096;
			if(!pop_calcstack_flooredint(&tmpint))return ERR_TYPE_MISMATCH;
			tmpints[1]=tmpint/4096;
			if(!pop_calcstack_str(tmpstr))return ERR_TYPE_MISMATCH;
			memset(tmpstrs[0],0x00,sizeof(tmpstrs[0]));
			//failsafeしてません　#しろ
			memcpy(tmpstrs[0],&tmpstr[tmpints[1]],tmpints[0]);
			push_calcstack(TYPE_STR_LIT,0,tmpstrs[0],0);
			break;
		case SFUNC_STR:
			pop_calcstack_int(&tmpint);
			//if(!tmp)return ERR_TYPE_MISMACH;
			memset(tmpstr,0x00,sizeof(tmpstr));
			sprintf(tmpstr,"%.3f",tmpint);
			push_calcstack(TYPE_STR_LIT,0,tmpstr,0);
			break;
		case INST_COLOR:
			pop_calcstack_flooredint(&tmpint);
			tmpint/=4096;
			if(tmpint<0 || tmpint>=16)return ERR_OUT_OF_RANGE;
			consolecolor=tmpint;
			break;
		case INST_CLEAR:
			Psys_FREEVAR=1024;
			memset(Variable,0x00,sizeof(Variable));
			break;
		case INST_CLS:
			consolecolor=0;
			Psys_CSRX=0;Psys_CSRY=0;
			memset(consolecharbuf,0x00,sizeof(consolecharbuf));
			memset(consolecolorbuf,0x00,sizeof(consolecolorbuf));
			break;
		case INST_LOCATE:
			pop_calcstack_flooredint(&tmpint);
			Psys_CSRY=tmpint/4096;
			pop_calcstack_flooredint(&tmpint);
			Psys_CSRX=tmpint/4096;
			break;
		case OP_SUBSTITUTE:
			tmp=pop_calcstack_int(&tmpint);
			if(tmp){
				tmpints[0]=tmpint;
				tmp=pop_calcstack_var(&tmpint);
				if(tmp){
					if(!((Variable+tmpint)->isStr)){
						(Variable+tmpint)->value=tmpints[0];
					}else{
						return ERR_TYPE_MISMATCH;
					}
				}else{
					return ERR_SYNTAX_ERROR;
				}
			}else{
				tmp=pop_calcstack_str(tmpstr);
				if(tmp){
					tmp=pop_calcstack_var(&tmpint);
					if(tmp){
						if((Variable+tmpint)->isStr){
							memset((Variable+tmpint)->string,0x00,sizeof((Variable+tmpint)->string));
							strcpy((Variable+tmpint)->string,tmpstr);
						}else{
							return ERR_TYPE_MISMATCH;
						}
					}else{
					}
				}else{
				}
			}
			break;
		default:
			printf("!!!!!(%d)\n",arg);
		break;
	}
	return ERR_NO_ERROR;
}