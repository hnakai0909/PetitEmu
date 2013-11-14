#include "interpreter2.h"

struct ForGosubStack ForGosub_s[FORGOSUB_S_MAX];
unsigned int ForGosub_sl=0;

int bracket_nest;

#ifdef __cplusplus
extern "C"{
#endif

char hex2int(const char arg){
	if(arg>='A' && arg<='F')
		return arg-'A'+10;
	if(arg>='a' && arg<='f')
		return arg-'a'+10;
	if(arg>='0' && arg<='9')
		return arg-'0';
	return -1;
}

char dec2int(const char arg){
	if(arg>='0' && arg<='9')
		return arg-'0';
	return -1;
}

int Str2VarID(const char* arg){
	int cnt=0;
	for(cnt=0;cnt<(VAR_MAX-Psys_FREEVAR);cnt++){
		if(strcmp(Variable[cnt].name,arg)==0){
			return cnt;
		}
	}
	return -1;
}

uint16_t* GetVarID(uint16_t* p,int* tmpint,int* errtmp){
	int tmpints[10],tmpstr_p=0;
	char tmpstr[STR_LEN_MAX];
	char p_char=Code2Char(*p);
	memset(tmpstr,0x00,sizeof(tmpstr));
	*errtmp=ERR_NO_ERROR;
	while(isalpha(p_char)||(p_char=='_')||(isdigit(p_char))){
		if((tmpstr_p==0)&&(isdigit(p_char))){
			*tmpint=-1;
			*errtmp=ERR_SYNTAX_ERROR;
			return p;
		}
		if(tmpstr_p>=8){
			*errtmp=ERR_STRING_TOO_LONG;
			return p;
		}
		tmpstr[tmpstr_p]=toupper(p_char);
		tmpstr_p++;
		p++;
		p_char=Code2Char(*p);
	}
	p_char=Code2Char(*p);
	if(p_char=='$'){
		tmpstr[tmpstr_p]=p_char;
		tmpstr_p++;
		p++;
	}
	p_char=Code2Char(*p);
	if(p_char=='('||p_char=='['){
		tmpstr[tmpstr_p]=p_char;
		tmpstr_p++;
		tmpints[0]=Str2VarID(tmpstr);
		*tmpint=tmpints[0];
		if(tmpints[0]==-1){
			tmpints[0]=NewVar(tmpstr);
			RegistDim(tmpints[0],10,0);
		}
		*errtmp=PushCalcStack(TYPE_DIM_PTR,tmpints[0],NULL,0);
		if(*errtmp!=ERR_NO_ERROR)return p;
		p=ReadFormula(p,errtmp);
		if(*errtmp!=ERR_NO_ERROR)return p;
		*errtmp=ProcessRemainingOperator();
		if(*errtmp!=ERR_NO_ERROR)return p;
	}else{
		p=JumpSpace(p);
		*tmpint=Str2VarID(tmpstr);
		if(*tmpint!=-1){
			//既存
			if(tmpstr[tmpstr_p-1]=='$'){
				*errtmp=PushCalcStack(TYPE_STR_VAR,*tmpint,NULL,0);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}else{
				*errtmp=PushCalcStack(TYPE_INT_VAR,*tmpint,NULL,0);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}
		}else{
			//新規登録
			*tmpint=NewVar(tmpstr);
			if(Variable[*tmpint].isStr){
				*errtmp=PushCalcStack(TYPE_STR_VAR,*tmpint,NULL,0);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}else{
				*errtmp=PushCalcStack(TYPE_INT_VAR,*tmpint,NULL,0);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}
		}
	}
	return p;
}

//演算子スタックの残留を処理
int ProcessRemainingOperator(void){
	int argcount=0,errtmp;	
	uint16_t op=0;
	while(op_sl>0)  {
		PopOpStack(&op,&argcount);
		if((op==Char2Code('('))||(op==Char2Code(')'))) {
			//括弧が対応していない
			return ERR_SYNTAX_ERROR;
		}
		errtmp=EvalFormula(op,argcount);
		if(errtmp!=ERR_NO_ERROR)return errtmp;
	}
	return ERR_NO_ERROR;
}

uint16_t* JumpSpace(uint16_t* p){
	for(;(*p==Char2Code(' '));p++){}
	return p;
}

uint16_t* ForJump(uint16_t* p,int* errtmp){
	int nest=0;
	uint16_t* srcpos;
	uint16_t* input=p;
	int i=0,f=0;
	*errtmp=ERR_NO_ERROR;
	for(i=cur_line+1;i<(srclinecount-cur_line-1);i++){
		srcpos=p;
		GOTOLINE(i);
		p=srcpos;
		while(*p!=0x000D && *p!=TOKEN_REM && *p!=TOKEN_REM2){
			srcpos=p;
			if(*p==TOKEN_FOR)nest++;
			if(*p==TOKEN_NEXT){
				if(nest==0){
					cur_line+=i;
					f=1;
					break;
				}
				nest--;
			}
			p++;
		}
		if(f==1)break;
	}
	if(f==0){
		*errtmp=ERR_FOR_WITHOUT_NEXT;
		return srcpos+1;
	}
	return srcpos+1;
}

uint16_t* ReadFormula(uint16_t* p,int *errtmp){
	char tmpstr[STR_LEN_MAX],tmpstr2[512];
	char tmpstr_p=0;
	uint16_t *pbegin=p;
	int cnt=0,tmp=0,beforetokentype=0,nest_depth=0;
	char argcount[100],brackettype[100];
	char p_char;
	int32_t tmpint=0,tmpints[100];
	int64_t tmpint2=0;
	double tmpw=0;
	*errtmp=ERR_NO_ERROR;
	memset(tmpstr, 0x00,sizeof(tmpstr));
	memset(tmpstr2,0x00,sizeof(tmpstr2));
	memset(argcount,0x00,sizeof(argcount));
	memset(brackettype,0x00,sizeof(brackettype));
	while(*p!=0){
		p_char=Code2Char(*p);
		if(isalpha(p_char)||(p_char=='_')){
			memset(tmpstr, 0x00,sizeof(tmpstr));
			tmpstr_p=0;
			cnt=0;
			while((isalpha(p_char)||(p_char=='_')||isdigit(p_char))&&(*p!=0)){
				if(tmpstr_p>=8){
					*errtmp=ERR_STRING_TOO_LONG;
					return p;
				}
				tmpstr[tmpstr_p]=toupper(p_char);
				tmpstr_p++;
				p++;
				p_char=Code2Char(*p);
				cnt++;
			}
			p_char=Code2Char(*p);
			if(p_char=='$'){ 
				tmpstr[tmpstr_p]=p_char;
				tmpstr_p++;
				p++;
				p_char=Code2Char(*p);
				cnt++;
			}
			p_char=Code2Char(*p);
			if(p_char=='(' || p_char=='['){
				tmpstr[tmpstr_p]=p_char;
				tmpstr_p++;
				tmpint=Str2VarID(tmpstr);
				if((beforetokentype==TYPE_INT_LIT)||(beforetokentype==TYPE_STR_LIT)){
					//Next statement
					//行き過ぎたのを戻す
					p-=cnt;
					return p;
				}
				if(tmpint==-1){
					tmpint=NewVar(tmpstr);
					RegistDim(tmpint,10,0);
				}
				*errtmp=PushCalcStack(TYPE_DIM,tmpint,NULL,0);
				if(*errtmp!=ERR_NO_ERROR)return p;
				beforetokentype=TYPE_FUNC;//本来はTYPE_DIMだが便宜的に。
			}else{
				tmp=Str2VarID(tmpstr);
				if(tmp!=-1){
					if((beforetokentype==TYPE_INT_LIT)||(beforetokentype==TYPE_STR_LIT)){
						//Next statement
						//行き過ぎたのを戻す
						p-=cnt;
						return p;
					}
					if(Variable[tmp].isStr){
						*errtmp=PushCalcStack(TYPE_STR_LIT,0,Variable[tmp].string,0);
						if(*errtmp!=ERR_NO_ERROR)return p;
						beforetokentype=TYPE_STR_LIT;
					}else{
						*errtmp=PushCalcStack(TYPE_INT_LIT,Variable[tmp].value,NULL,0);
						if(*errtmp!=ERR_NO_ERROR)return p;
						beforetokentype=TYPE_INT_LIT;
					}
				}else{
					if((beforetokentype==TYPE_INT_LIT)||(beforetokentype==TYPE_STR_LIT)){
						//Next statement
						//行き過ぎたのを戻す
						p-=cnt;
						return p;
					}
					tmpint=NewVar(tmpstr);
					if(Variable[tmpint].isStr){
						*errtmp=PushCalcStack(TYPE_STR_LIT,0,NULL,0);
						if(*errtmp!=ERR_NO_ERROR)return p;
						beforetokentype=TYPE_STR_LIT;
					}else{
						*errtmp=PushCalcStack(TYPE_INT_LIT,0,NULL,0);
						if(*errtmp!=ERR_NO_ERROR)return p;
						beforetokentype=TYPE_INT_LIT;
					}
				}
				p=JumpSpace(p);
			}
			p_char=Code2Char(*p);
		}else if(isdigit(p_char)){
			if(beforetokentype==TYPE_INT_LIT){
				return p;
			}
			tmpint=0;
			cnt=0;
			while(isdigit(p_char)){
				if(cnt>=7){
					*errtmp=ERR_OVERFLOW;
					return p;
				}
				tmpint=(tmpint*10)+(p_char-'0');
				p++;p_char=Code2Char(*p);cnt++;
			}
			if(tmpint>=524288){
				*errtmp=ERR_OVERFLOW;
				return p;
			}
			tmpint*=4096;
			if(p_char=='.'){
				p++;
				p_char=Code2Char(*p);
				tmpw=0;
				for(cnt=0;isdigit(Code2Char(*(p+cnt)))&&(cnt<=6);cnt++);
				cnt--;
				tmp=cnt;
				while(cnt>=0){
					tmpw=((tmpw+(double)(Code2Char(*(p+cnt))-'0'))/10.0);
					cnt--;
				}
				p+=tmp;
				p_char=Code2Char(*p);
				//切り捨てられるものに最小分解能の1/4096の1/2を足すことで
				//四捨五入する
				tmpw+=(1.0/8192.0);
				tmpw*=4096.0;
				tmpint+=(int)tmpw;
				for(;isdigit(Code2Char(*p));p++);
				p_char=Code2Char(*p);
			}
			*errtmp=PushCalcStack(TYPE_INT_LIT,tmpint,NULL,0);
			if(*errtmp!=ERR_NO_ERROR)return p;
			p=JumpSpace(p);
			p_char=Code2Char(*p);
			beforetokentype=TYPE_INT_LIT;
		}else if(p_char=='&'){
			p++;
			p_char=Code2Char(*p);
			if(toupper(p_char)=='H'){
				p++;
				p_char=Code2Char(*p);
				tmpint=0;
				cnt=0;
				while(isxdigit(p_char)){
					if(cnt>=5){
						*errtmp=ERR_SYNTAX_ERROR;
						return p;
					}
					tmpint=(tmpint<<4)|((p_char<='9')?(p_char-'0'):(toupper(p_char)-'A'+10));
					p++;p_char=Code2Char(*p);cnt++;
				}
				tmpint*=4096;
			}else if(toupper(p_char)=='B'){
				p++;
				p_char=Code2Char(*p);
				tmpint=0;
				cnt=0;
				while(isBin(p_char)){
					if(cnt>=20){
						*errtmp=ERR_SYNTAX_ERROR;
						return p;
					}
					tmpint=(tmpint<<1)|(p_char-'0');
					p++;p_char=Code2Char(*p);cnt++;
				}
				tmpint*=4096;
			}else{
				*errtmp=ERR_SYNTAX_ERROR;
				return p;
			}
			*errtmp=PushCalcStack(TYPE_INT_LIT,tmpint,NULL,0);
			if(*errtmp!=ERR_NO_ERROR)return p;
			beforetokentype=TYPE_INT_LIT;
			p=JumpSpace(p);
			p_char=Code2Char(*p);
		}else if(p_char=='"'){
			p++;
			p_char=Code2Char(*p);
			memset(tmpstr, 0x00,sizeof(tmpstr));
			tmpstr_p=0;
			while((p_char!='"')&&(p_char!=0)){
				
				//ソースは最大でも一行100文字のため、
				//ここでSTR_LEN_MAX文字を超えることはない
				tmpstr[tmpstr_p]=Code2Char(*p);
				tmpstr_p++;
				p++;	
				p_char=Code2Char(*p);
			}
			if(p_char=='"'){p++;p_char=Code2Char(*p);}
			*errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
			if(*errtmp!=ERR_NO_ERROR)return p;
			beforetokentype=TYPE_STR_LIT;
		}else if(p_char=='<'){
			if(*(p+1)==Char2Code('=')){
				p++;
				p_char=Code2Char(*p);
				*errtmp=PushCalcStack(TYPE_FUNC,OP_SHONARI_EQUAL,NULL,2);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}else{
				*errtmp=PushCalcStack(TYPE_FUNC,Char2Code('<'),NULL,2);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}
			beforetokentype=TYPE_FUNC;
			p++;
		}else if(p_char=='>'){
			if(*(p+1)==Char2Code('=')){
				p++;
				p_char=Code2Char(*p);
				*errtmp=PushCalcStack(TYPE_FUNC,OP_DAINARI_EQUAL,NULL,2);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}else{
				*errtmp=PushCalcStack(TYPE_FUNC,Char2Code('>'),NULL,2);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}
			p++;
			p_char=Code2Char(*p);
			beforetokentype=TYPE_FUNC;
		}else if(p_char=='='){
			if(*(p+1)==Char2Code('=')){
				p++;
				p_char=Code2Char(*p);
				*errtmp=PushCalcStack(TYPE_FUNC,OP_EQUAL,NULL,2);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}else{
				*errtmp=ERR_NO_ERROR;
				return p;
			}
			p++;
			p_char=Code2Char(*p);
			beforetokentype=TYPE_FUNC;
		}else if(p_char=='!'){
			if(*(p+1)==Char2Code('=')){
				p++;
				p_char=Code2Char(*p);
				*errtmp=PushCalcStack(TYPE_FUNC,OP_NOTEQUAL,NULL,2);
				if(*errtmp!=ERR_NO_ERROR)return p;
				beforetokentype=TYPE_FUNC;
			}else{
				*errtmp=ERR_SYNTAX_ERROR;
				return p;
			}
			p++;
		}else if(p_char==','){
			if(nest_depth==0){
				*errtmp=ERR_NO_ERROR;
				return p;
			}else{
				argcount[nest_depth-1]++;
				p=JumpSpace(p+1);
				p_char=Code2Char(*p);
				*errtmp=PushCalcStack(TYPE_SPECIAL,Char2Code(','),NULL,0);
				if(*errtmp!=ERR_NO_ERROR)return p;
				beforetokentype=TYPE_SPECIAL;
			}
		}else if((p_char==':')||(p_char==';')||(p_char=='\'')||(p_char=='?')){
			if(pbegin==p){
				//式なし
				*errtmp=PushCalcStack(TYPE_VOID,0,NULL,0);
				if(*errtmp!=ERR_NO_ERROR)return p;
				return p;
			}else{
				*errtmp=ERR_NO_ERROR;
				return p;
			}
		}else if(p_char=='('){
			brackettype[nest_depth]=0;
			*errtmp=PushCalcStack(TYPE_FUNC,Char2Code('('),NULL,0);
			if(*errtmp!=ERR_NO_ERROR)return p;
			p=JumpSpace(p+1);
			p_char=Code2Char(*p);
			nest_depth++;
			beforetokentype=TYPE_SPECIAL;
		}else if(p_char==')'){
			nest_depth--;
			if(brackettype[nest_depth]!=0){
				*errtmp=ERR_SYNTAX_ERROR;
				return p;
			}
			if(beforetokentype==TYPE_SPECIAL){
				*errtmp=PushCalcStack(TYPE_VOID,0,NULL,0);
				if(*errtmp!=ERR_NO_ERROR)return p;
			}else{
				argcount[nest_depth]++;
			}
			*errtmp=PushCalcStack(TYPE_FUNC,Char2Code(')'),NULL,argcount[nest_depth]);
			if(*errtmp!=ERR_NO_ERROR)return p;
			argcount[nest_depth]=0;
			p=JumpSpace(p+1);
			p_char=Code2Char(*p);
			beforetokentype=TYPE_SPECIAL2;
		}else if(p_char=='['){
			brackettype[nest_depth]=1;
			*errtmp=PushCalcStack(TYPE_FUNC,Char2Code('['),NULL,0);
			if(*errtmp!=ERR_NO_ERROR)return p;
			p=JumpSpace(p+1);
			p_char=Code2Char(*p);
			nest_depth++;
			beforetokentype=TYPE_SPECIAL;
		}else if(p_char==']'){
			nest_depth--;
			if(brackettype[nest_depth]!=1){
				*errtmp=ERR_SYNTAX_ERROR;
				return p;
			}
			argcount[nest_depth]++;
			*errtmp=PushCalcStack(TYPE_FUNC,Char2Code(']'),NULL,argcount[nest_depth]);
			if(*errtmp!=ERR_NO_ERROR)return p;
			p=JumpSpace(p+1);
			p_char=Code2Char(*p);
			beforetokentype=TYPE_SPECIAL2;
		}else if(p_char=='-'){
			if((beforetokentype==0)||(beforetokentype==TYPE_FUNC)||(beforetokentype==TYPE_SPECIAL)){
				*errtmp=PushCalcStack(TYPE_FUNC,OP_MINUSSIGN,NULL,1);
				if(*errtmp!=ERR_NO_ERROR)return p;
				p++;p_char=Code2Char(*p);
				beforetokentype=TYPE_FUNC;
			}else{
				*errtmp=PushCalcStack(TYPE_FUNC,Char2Code('-'),NULL,2);
				if(*errtmp!=ERR_NO_ERROR)return p;
				p++;p_char=Code2Char(*p);
				beforetokentype=TYPE_FUNC;
			}
		}else if(isOperator(*p)){
			*errtmp=PushCalcStack(TYPE_FUNC,*p,NULL,2);
			if(*errtmp!=ERR_NO_ERROR)return p;
			p++;p_char=Code2Char(*p);
			beforetokentype=TYPE_FUNC;
		}else if(isFunction(*p)){
			*errtmp=PushCalcStack(TYPE_FUNC,*p,NULL,0);
			if(*errtmp!=ERR_NO_ERROR)return p;
			p=JumpSpace(p+1);
			p_char=Code2Char(*p);
			beforetokentype=TYPE_FUNC;
		}else if(isSystemVariable(*p)){
			tmp=GetSystemVariableType(*p);
			switch(tmp){
				case 1:case 2:
					tmpint=*(GetSystemVariableIntPtr(*p));
					*errtmp=PushCalcStack(TYPE_INT_LIT,tmpint,NULL,0);
					if(*errtmp!=ERR_NO_ERROR)return p;
					beforetokentype=TYPE_INT_LIT;
					p++;p_char=Code2Char(*p);
					break;
				case 3:case 4:
					memset(tmpstr,0x00,sizeof(tmpstr));
					strcpy(tmpstr,GetSystemVariableStrPtr(*p));
					*errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
					if(*errtmp!=ERR_NO_ERROR)return p;
					beforetokentype=TYPE_STR_LIT;
					p++;p_char=Code2Char(*p);
					break;
				default:
					p++;
					break;
			}
		}else if(*p==0x0020){
			p++;
		}else if(*p==0x0000 || *p==0x000D){
			*errtmp=ERR_NO_ERROR;
			return p;
		}else{
			*errtmp=ERR_NO_ERROR;
			return p;
		}
	}
	return p;
}

void TranslateRaw2Code(unsigned char* input,uint16_t* output,int *outlen){
	unsigned char *inpos=input;
	uint16_t *outpos=output;
	unsigned char *srcend=input+strlen((char*)input);
	unsigned char c=0;
	char tmpstr[STR_LEN_MAX];
	unsigned char tmpstr_p=0;
	int cnt=0,i=0,j=0;
	uint16_t codetmp;
	int32_t tmpint=0;
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(output,0x0000,sizeof(output));
	memset(srcline_begin_token_pos, 0x0000,sizeof(srcline_begin_token_pos));
	memset(srcline_token_count, 0x0000,sizeof(srcline_token_count));
	srclinecount=0;
	while(*inpos!=0x00){
		c=*inpos;
		if(isalpha(c)){
			i=0;
			memset(tmpstr, 0x00,sizeof(tmpstr));
			while((i<8)&&(isalpha(c)||isdigit(c))){
				if(islower(inpos[i])){
					tmpstr[i]=toupper(inpos[i]);
				}else{
					tmpstr[i]=inpos[i];
				}
				i++;
				c=*(inpos+i);
			}
			if(c=='$'){
				tmpstr[i]=c;
				i++;
			}
			//IDに変換
			if(Str2TokenCode(tmpstr,&codetmp)){
				*outpos=codetmp;
				outpos++;
				srcline_token_count[srclinecount]++;
				inpos+=i;
			}else{
				for(j=0;j<i;j++){
					if(islower(inpos[j])){
						*outpos=Char2Code(toupper(tmpstr[j]));
					}else{
						*outpos=Char2Code(tmpstr[j]);
					}
					outpos++;
					srcline_token_count[srclinecount]++;
					inpos++;
				}
			}
		}else if(c=='"'){
			*outpos=Char2Code(c);
			outpos++;
			srcline_token_count[srclinecount]++;
			inpos++;
			c=*inpos;
			while((c!=0x00)&&(c!=0x0D)&&(c!='"')){
				*outpos=Char2Code(c);
				outpos++;
				srcline_token_count[srclinecount]++;
				inpos++;
				c=*inpos;
			}
			*outpos=Char2Code('"');
			outpos++;
			srcline_token_count[srclinecount]++;
			if(c!=0x0D)inpos++;
		}else if(c=='?'){
			*outpos=TOKEN_PRINT2;
			outpos++;
			srcline_token_count[srclinecount]++;
			inpos++;
		}else if(c=='&'){
			*outpos=Char2Code(c);
			outpos++;
			srcline_token_count[srclinecount]++;
			inpos++;
			*outpos=Char2Code(*inpos);
			outpos++;
			srcline_token_count[srclinecount]++;
			inpos++;
		}else if(c=='@'){
			*outpos=TOKEN_LABEL2;
			outpos++;
			srcline_token_count[srclinecount]++;
			inpos++;
		}else if(c==0x0D){
			inpos++;
			c=*inpos;
			if(c==0x0A)inpos++;
			*outpos=0x000D;
			outpos++;
			srcline_token_count[srclinecount]++;
			srclinecount++;
			srcline_begin_token_pos[srclinecount]=srcline_begin_token_pos[srclinecount-1]+srcline_token_count[srclinecount-1];
		}else if(c==0x0A){
			inpos++;
		}else{
			*outpos=Char2Code(*inpos);
			outpos++;
			srcline_token_count[srclinecount]++;
			inpos++;
		}
		
	};
	*outpos=0x0000;
	outpos++;
	srclinecount++;
	*outlen=outpos-output;
	if(log_en){
		for(i=0;i<*outlen;i++)printf("%X,",output[i]);
	}
	return;
}

void TranslateCode2Raw(uint16_t* input,unsigned char* output){
	uint16_t *inpos=input;
	unsigned char *outpos=output;
	char tmp;
	char tmpstr[10];
	memset(output,0x00,sizeof(output));
	while(*inpos!=0x0000){
		tmp=Code2Char(*inpos);
		if(tmp==0x00){
			strcpy(tmpstr,TokenCode2Str(*inpos));
			if(strcmp(tmpstr,"")==0){
				if(*inpos==0x000D){
					*outpos=0x0D;
					outpos++;
					*outpos=0x0A;
					outpos++;
				}
			}
			strcat((char*)outpos,tmpstr);
		}else{
			*outpos=tmp;
			outpos++;
		}
		input++;	
	}
	return;
}

void RunInteractive(char* input){
	uint16_t codedata[33];
	int codelen=0,errtmp=ERR_NO_ERROR,runflag=0;
	char tmpstr[STR_LEN_MAX];
	error_occured_token=0;
	breakflag=0;
	read_initialized=false;
	TranslateRaw2Code((unsigned char*)input,codedata,&codelen);
	runmode=RMD_LINE;
	if(Psys_CSRX!=0)Print2Console("",0);
	errtmp=Interpretation(codedata,codelen,true,&runflag);
	ClearKeyBuffer();
	runmode=RMD_STOP;
	if(runflag==1){
		runmode=RMD_PRG;
		errtmp=RunProgram();
		ClearKeyBuffer();
		if(breakflag==1){
			memset(tmpstr,0x00,sizeof(tmpstr));
			sprintf(tmpstr,"BREAK in %d",cur_line+1);
			if(Psys_CSRX!=0)Print2Console("",0);
			printf("%s\n",tmpstr);
			Print2Console(tmpstr,0);
			runmode=RMD_STOP;
			return;
		}
		runmode=RMD_STOP;
		Psys_ERR=errtmp;
		if((errtmp==ERR_NO_ERROR)&&(op_sl==0)&&(calc_sl==0)){
			if(Psys_CSRX!=0)Print2Console("",0);
			puts("OK");
			Print2Console("OK",0);
		}else{
			Psys_ERL=cur_line+1;
			printf("op_sl:%d calc_sl:%d",op_sl,calc_sl);
			memset(tmpstr,0x00,sizeof(tmpstr));
			if(Psys_CSRX!=0)Print2Console("",0);
			if(error_occured_token!=0){
				sprintf(tmpstr,"%s (%d,%s)",GetErrorMessage(errtmp),Psys_ERL,TokenCode2Str(error_occured_token));
				printf("%s\n",tmpstr);
				Print2Console(tmpstr,0);
			}else{
				printf("%s\n",GetErrorMessage(errtmp));
				Print2Console(GetErrorMessage(errtmp),0);
			}
			if(Psys_SYSBEEP)PlaySoundMem(SHandleBEEP[2],DX_PLAYTYPE_BACKBIT);
		}
		return;
	}else{
		Psys_ERR=errtmp;
		if(errtmp==ERR_NO_ERROR){
			if((op_sl==0)&&(calc_sl==0)){
				puts("OK");
				Print2Console("OK",0);
			}else{
				printf("op_sl:%d calc_sl:%d",op_sl,calc_sl);
				strcpy(tmpstr,GetErrorMessage(ERR_UNDEFINED));
				printf("%s\n",tmpstr);
				Print2Console(tmpstr,0);
			}
		}else{
			memset(tmpstr,0x00,sizeof(tmpstr));
			if(error_occured_token!=0){
				strcpy(tmpstr,TokenCode2Str(error_occured_token));
				if(strcmp(tmpstr,"")==0){
					sprintf(tmpstr,"%s",GetErrorMessage(errtmp));
				}else{
					sprintf(tmpstr,"%s (%s)",GetErrorMessage(errtmp),TokenCode2Str(error_occured_token));	
				}
				printf("%s\n",tmpstr);
				Print2Console(tmpstr,0);
			}else{
				strcpy(tmpstr,GetErrorMessage(errtmp));
				printf("%s\n",tmpstr);
				Print2Console(tmpstr,0);
			}
			if(Psys_SYSBEEP)PlaySoundMem(SHandleBEEP[2],DX_PLAYTYPE_BACKBIT);
		}
		return;
	}
}

int RunProgram(void){
	int srclen,dummy=0,errtmp=ERR_NO_ERROR;
	error_occured_token=0;
	TranslateRaw2Code(source_ptr,translated_source,&srclen);
	errtmp=ResistLabel(translated_source);
	if(errtmp!=ERR_NO_ERROR)return errtmp;
	return Interpretation(translated_source,srclen,false,&dummy);
}

int ResistLabel(uint16_t* input){
	uint16_t *srcpos=input;
	int i=0,j=0,k=0;
	char tmpstr[16],c;
	labelcount=0;
	cur_line=0;
	memset(labellist_name,0x00,sizeof(labellist_name));
	for(i=0;i<srclinecount;i++){
		GOTOLINE(i);
		srcpos=JumpSpace(srcpos);
		if(*srcpos==TOKEN_LABEL || *srcpos==TOKEN_LABEL2){
			srcpos++;
			memset(tmpstr,0x00,sizeof(tmpstr));
			for(k=0;k<8;k++){
				c=Code2Char(*srcpos);
				if((c==' ')||(*srcpos==0x000D)||(*srcpos==0x0000))break;
				if((c!='_')&&(!isalpha(c))&&(!isdigit(c))){
					Psys_ERL=i+1;
					return ERR_SYNTAX_ERROR;
				}
				tmpstr[k]=toupper(c);
				srcpos++;
			}
			strcpy(labellist_name[labelcount],tmpstr);
			labellist_line[labelcount]=i;
			labelcount++;
		}
	}
	return ERR_NO_ERROR;
}

uint16_t* GetLabelName(uint16_t* p,char* tmpstr,int* errtmp){
	int i=0;
	uint16_t c;
	memset(tmpstr,0x00,sizeof(tmpstr));
	for(i=0;i<8;i++){
		c=Code2Char(*p);
		if((c==' ')||(*p==0x000D)||(*p==0x0000)||(c==':')||(c=='\''))break;
		if((c!='_')&&(!isalpha(c))&&(!isdigit(c))){
			*errtmp=ERR_SYNTAX_ERROR;
			return p;
		}
		tmpstr[i]=toupper(c);
		p++;
	}
	if(i==0){
		*errtmp=ERR_SYNTAX_ERROR;
		return p;
	}
	*errtmp=ERR_NO_ERROR;
	return p;
}

int ReadGotoLine(uint16_t* input,unsigned int line){
	int errtmp=ERR_NO_ERROR;
	read_srcpos=input+srcline_begin_token_pos[line];
	errtmp=ReadSeekNext();
	if(errtmp!=ERR_NO_ERROR)return errtmp;
	return ERR_NO_ERROR;
}

int ReadSeekNext(void){
	while(*read_srcpos!=TOKEN_DATA && *read_srcpos!=0x0000){
		//コメントは次行まで読み飛ばし
		if(*read_srcpos==TOKEN_REM ||*read_srcpos==TOKEN_REM2){
			while(*read_srcpos!=0x000D && *read_srcpos!=0x0000)read_srcpos++;
		}
	}
	if(*read_srcpos==0x0000){
		return ERR_OUT_OF_DATA;
	}
	read_srcpos=JumpSpace(read_srcpos+1);
	read_initialized=true;
	return ERR_NO_ERROR;
}

int Interpretation(uint16_t* input,int srclen,bool interactive_flag,int* runflag){
	uint16_t *srcpos=input,*tmppos;
	const uint16_t *srcend=input+srclen;
	uint32_t tmpline=0;
	uint16_t t=0;
	char c=0,*c2,p_char;
	char tmpstr[STR_LEN_MAX],tmpstr2[STR_LEN_MAX*2],tmpstr3[STR_LEN_MAX];
	unsigned char tmpstr_p=0;
	int cnt=0,i=0,argcount=0;
	int32_t tmpint=0,tmpint3=0;
	int64_t tmpint2=0;
	int lastprintmode=0,errtmp=ERR_NO_ERROR;
	
	int state = ST_LINE_BEGIN;//状態変数

	memset(tmpstr, 0x00,sizeof(tmpstr));
	memset(tmpstr2, 0x00,sizeof(tmpstr2));
	memset(op_s,0x00,sizeof(op_s));
	memset(calc_s,0x00,sizeof(calc_s));
	calc_sl=0;
	op_sl=0;
	*runflag=0;

	while(srcpos<srcend){
		t=*srcpos;
		error_occured_token=t;
		c=Code2Char(t);
		if(c==0x00){
			//Code
			switch(t){
				case TOKEN_PRINT:case TOKEN_PRINT2:
					srcpos++;
					while(srcpos<srcend){
						srcpos=JumpSpace(srcpos);
						srcpos=ReadFormula(srcpos,&errtmp);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						errtmp=ProcessRemainingOperator();
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						if(!PopCalcStack_void()){
							memset(tmpstr,0x00,sizeof(tmpstr));
							i=PopCalcStack_int(&tmpint);
							if(i){
								if(tmpint%4096==0){
									sprintf(tmpstr,"%d",tmpint/4096);
								}else{
									sprintf(tmpstr,"%.3f",(double)(tmpint/4096.0));
									for(i=strlen(tmpstr)-1;(i!=0)&&(tmpstr[i]=='0');i--){
										tmpstr[i]=0;
									}
									if(tmpstr[i]=='.')tmpstr[i]=0;
								}
							}else{
								i=PopCalcStack_str(tmpstr);
							}
							tmpint=0;
							switch(Code2Char(*srcpos)){
								case ';':
									lastprintmode=1;
									Print2Console(tmpstr,1);
									srcpos=JumpSpace(srcpos+1);
									if(*srcpos==0x000D){
										cur_line++;
										if(cur_line>=srclinecount)return ERR_NO_ERROR;
										GOTOLINE(cur_line);
										tmpint=1;
									}
									break;
								case ',':
									lastprintmode=2;
									Print2Console(tmpstr,2);
									srcpos=JumpSpace(srcpos+1);
									if(*srcpos==0x000D){
										cur_line++;
										if(cur_line>=srclinecount)return ERR_NO_ERROR;
										GOTOLINE(cur_line);
										tmpint=1;
									}
									break;
								case '\'':
									lastprintmode=0;
									Print2Console(tmpstr,0);
									cur_line++;
									if(cur_line>=srclinecount)return ERR_NO_ERROR;
									GOTOLINE(cur_line);
									tmpint=1;
									break;
								case ':':
									lastprintmode=0;
									Print2Console(tmpstr,0);
									srcpos++;
									tmpint=1;
									break;
								default:
									lastprintmode=0;
									Print2Console(tmpstr,0);
									tmpint=1;
									break;
							}
						}else{
							Print2Console("",lastprintmode);
							lastprintmode=0;
							srcpos++;
							if(isInstruction(*srcpos))return ERR_SYNTAX_ERROR;
							break;
						}
						if(tmpint==1)break;
					}
					state=ST_NEW_STATEMENT;
					break;
				case TOKEN_DIM:
					do{
						srcpos=JumpSpace(srcpos+1);
						memset(tmpstr,0x00,sizeof(tmpstr));
						p_char=Code2Char(*srcpos);
						tmpstr_p=0;
						while(isalpha(p_char)||(p_char=='_')||(isdigit(p_char))){
							if((tmpstr_p==0)&&(isdigit(p_char))){
								return ERR_SYNTAX_ERROR;
							}
							if(tmpstr_p>=8)return ERR_STRING_TOO_LONG;
							tmpstr[tmpstr_p]=toupper(p_char);
							tmpstr_p++;
							srcpos++;
							p_char=Code2Char(*srcpos);
						}
						p_char=Code2Char(*srcpos);
						if(p_char=='$'){
							tmpstr[tmpstr_p]='$';
							tmpstr_p++;
							srcpos++;
							tmpint2=1;
						}else{
							tmpint2=0;
						}
						if(Code2Char(*srcpos)=='('){
							tmpstr[tmpstr_p]='(';
							tmpstr_p++;
						}else{
							return ERR_SYNTAX_ERROR;
						}
						tmpint=Str2VarID(tmpstr);
						if(tmpint!=-1)return ERR_DUPLICATE_DEFINITION;
						errtmp=PushCalcStack(TYPE_INT_LIT,tmpint2,NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						errtmp=PushCalcStack(TYPE_FUNC,TOKEN_DIM,NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						srcpos=ReadFormula(srcpos,&errtmp);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						ProcessRemainingOperator();
					}while(Code2Char(*srcpos)==',');
					state=ST_NEW_STATEMENT;
					break;
				case TOKEN_READ:
					if(!read_initialized){
						errtmp=ReadSeekNext();
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}
					do{
						srcpos=JumpSpace(srcpos+1);
						srcpos=GetVarID(srcpos,&tmpint,&errtmp);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						errtmp=PushCalcStack(TYPE_FUNC,Char2Code('='),NULL,2);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						//ここで右辺値をcalcstackに突っ込む
						
						//read_srcpos

						srcpos=JumpSpace(srcpos);
					}while(Code2Char(*srcpos)==',');
					state=ST_NEW_STATEMENT;
					break;
				case TOKEN_RESTORE:
					srcpos=JumpSpace(srcpos+1);
					if(*srcpos!=TOKEN_LABEL && *srcpos!=TOKEN_LABEL2)return ERR_SYNTAX_ERROR;
					srcpos=JumpSpace(srcpos+1);
					srcpos=GetLabelName(srcpos,tmpstr,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					tmpint=0;
					for(i=0;i<labelcount;i++){
						if(strcmp(labellist_name[i],tmpstr)==0){
							ReadGotoLine(input,i);
							read_initialized=false;
							tmpint=1;
							break;
						}
					}
					if(tmpint==0)return ERR_UNDEFINED_LABEL;
					break;
				case TOKEN_DATA:
					cur_line++;
					if(cur_line>=srclinecount)return ERR_NO_ERROR;
					GOTOLINE(cur_line);
				case TOKEN_IF:
					srcpos=JumpSpace(srcpos+1);
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					errtmp=PopCalcStack_int(&tmpint);
					if(!errtmp)return ERR_UNDEFINED;
					if(*srcpos==TOKEN_THEN){
						srcpos=JumpSpace(srcpos+1);
						if(*srcpos==TOKEN_LABEL || *srcpos==TOKEN_LABEL2){
							srcpos++;
							memset(tmpstr,0x00,sizeof(tmpstr));
							for(i=0;i<8;i++){
								c=Code2Char(*srcpos);
								if((c==' ')||(*srcpos==0x000D)||(*srcpos==0x0000)||(*srcpos==':')||(*srcpos=='\''))break;
								if((c!='_')&&(!isalpha(c))&&(!isdigit(c))){
									return ERR_SYNTAX_ERROR;
								}
								tmpstr[i]=toupper(c);
								srcpos++;
							}
							if(tmpint==0){
								cur_line++;
								if(cur_line>=srclinecount)return ERR_NO_ERROR;
								GOTOLINE(cur_line);
								break;
							}
							tmpint2=0;
							for(i=0;i<labelcount;i++){
								if(strcmp(labellist_name[i],tmpstr)==0){
									cur_line=labellist_line[i];
									GOTOLINE(cur_line);
									tmpint2=1;
									break;
								}
							}
							if(tmpint2==0)return ERR_UNDEFINED_LABEL;
							break;
						}
						if(tmpint!=0){
							state=ST_NEW_STATEMENT;
							break;
						}else{
							cur_line++;
							if(cur_line>=srclinecount)return ERR_NO_ERROR;
							GOTOLINE(cur_line);
						}
					}else if(*srcpos==TOKEN_GOTO){
						if(tmpint==0){
							cur_line++;
							if(cur_line>=srclinecount)return ERR_NO_ERROR;
							GOTOLINE(cur_line);
						}
						//式が成り立っている場合は、そのままの状態で次の解析をさせる
					}else{
						return ERR_SYNTAX_ERROR;
					}
					break;
				case TOKEN_FOR:
					memset(ForGosub_s+ForGosub_sl,0x00,sizeof(ForGosub_s+ForGosub_sl));
					srcpos=JumpSpace(srcpos+1);
					srcpos=GetVarID(srcpos,&tmpint,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					ForGosub_s[ForGosub_sl].VarID=tmpint;
					if(Code2Char(*srcpos)!='=')return ERR_SYNTAX_ERROR;
					srcpos=JumpSpace(srcpos+1);
					errtmp=PushCalcStack(TYPE_FUNC,Char2Code('='),NULL,2);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					//代入処理ここまで
					if(*srcpos!=TOKEN_TO)return ERR_SYNTAX_ERROR;
					srcpos=JumpSpace(srcpos+1);
					ForGosub_s[ForGosub_sl].line=cur_line;
					ForGosub_s[ForGosub_sl].col=srcpos-input;
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					if(!PopCalcStack_int(&tmpint))return ERR_SYNTAX_ERROR;
					if(*srcpos==Char2Code(':')){
						srcpos++;
						if((Variable[ForGosub_s[ForGosub_sl].VarID].value)>tmpint){
							srcpos=ForJump(srcpos,&errtmp);
							if(errtmp!=ERR_NO_ERROR)return errtmp;
						}else{
							ForGosub_s[ForGosub_sl].step=4096;
							ForGosub_sl++;
							state=ST_NEW_STATEMENT;
							break;
						}
					}else if(*srcpos==Char2Code('\'') || *srcpos==0x0000 || *srcpos==0x000D){
						if((Variable[ForGosub_s[ForGosub_sl].VarID].value)>tmpint){
							srcpos=ForJump(srcpos,&errtmp);
							if(errtmp!=ERR_NO_ERROR)return errtmp;
							break;
						}else{
							ForGosub_s[ForGosub_sl].step=4096;
							ForGosub_sl++;
							cur_line++;
							if(cur_line>=srclinecount)return ERR_NO_ERROR;
							GOTOLINE(cur_line);
							break;
						}
					}
					if(*srcpos!=TOKEN_STEP)return ERR_SYNTAX_ERROR;
					srcpos=JumpSpace(srcpos+1);
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					if(!PopCalcStack_int(&tmpint))return ERR_SYNTAX_ERROR;
					if(tmpint>0){
						if(Variable[ForGosub_s[ForGosub_sl].VarID].value>tmpint){
							srcpos=ForJump(srcpos,&errtmp);
							if(errtmp!=ERR_NO_ERROR)return errtmp;
						}
					}
					if(tmpint<0){
						if(Variable[ForGosub_s[ForGosub_sl].VarID].value<tmpint){
							srcpos=ForJump(srcpos,&errtmp);
							if(errtmp!=ERR_NO_ERROR)return errtmp;
						}
					}
					ForGosub_s[ForGosub_sl].step=tmpint;
					ForGosub_sl++;
					state=ST_NEW_STATEMENT;
					break;
				case TOKEN_INPUT:
					srcpos=JumpSpace(srcpos+1);
					memset(tmpstr,0x00,sizeof(tmpstr));
					srcpos=GetVarID(srcpos,&tmpint,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					errtmp=PushCalcStack(TYPE_FUNC,Char2Code('='),NULL,2);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					memset(tmpstr,0x00,sizeof(tmpstr));
					tmpstr_p=0;
					tmpint=0;
					do{
						if(tmpint==1)Print2Console("?REDO FROM START",0);
						//入力
						//LineInput(tmpstr);
						InputLine(tmpstr);
						tmpint=1;
					
					}while((c!='$')&&(!isdigits(tmpstr)));

					tmpint=(int32_t)(atof(tmpstr)*4096.0);
					//失敗した時？知らん。

					if(c=='$'){
						errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}else{
						errtmp=PushCalcStack(TYPE_INT_LIT,tmpint,NULL,0);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
					}
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					state=ST_NEW_STATEMENT;
					break;
				case TOKEN_NEXT:
					if(ForGosub_sl<=0)return ERR_NEXT_WITHOUT_FOR;
					ForGosub_sl--;
					memset(tmpstr,0x00,sizeof(tmpstr));
					tmpstr_p=0;
					srcpos=JumpSpace(srcpos);
					while(isalpha(Code2Char(*srcpos))||(Code2Char(*srcpos)=='_')||(isdigit(Code2Char(*srcpos)))){
						if(tmpstr_p>=8)return ERR_STRING_TOO_LONG;
						tmpstr[tmpstr_p]=toupper(Code2Char(*srcpos));
						tmpstr_p++;
						srcpos++;
					}
					if(tmpstr_p!=0){
						tmpint=Str2VarID(tmpstr);
						if(tmpint==-1)return ERR_NEXT_WITHOUT_FOR;
						if(ForGosub_s[ForGosub_sl].VarID!=tmpint)return ERR_NEXT_WITHOUT_FOR;
					}
					tmpint=Variable[ForGosub_s[ForGosub_sl].VarID].value;
					tmpint+=ForGosub_s[ForGosub_sl].step;
					Variable[ForGosub_s[ForGosub_sl].VarID].value=tmpint;
					tmppos=srcpos;
					tmpline=cur_line;
					srcpos=input+ForGosub_s[ForGosub_sl].col;
					cur_line=ForGosub_s[ForGosub_sl].line;
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					if(!PopCalcStack_int(&tmpint))return ERR_SYNTAX_ERROR;
					tmpint2=tmpint;
					if(Code2Char(*srcpos)==':'){
						if(Variable[ForGosub_s[ForGosub_sl].VarID].value>tmpint){
							cur_line=tmpline;
							GOTOLINE(cur_line);
							srcpos=tmppos+1;
						}else{
							ForGosub_s[ForGosub_sl].step=4096;
							ForGosub_sl++;
						}
						state=ST_NEW_STATEMENT;
						break;
					}else if(Code2Char(*srcpos)=='\'' || *srcpos==0x0000 || *srcpos==0x000D){
						if(Variable[ForGosub_s[ForGosub_sl].VarID].value>tmpint){
							cur_line=tmpline+1;
							GOTOLINE(cur_line);
							srcpos=tmppos;
							if(*srcpos==TOKEN_NEXT)srcpos++;
							state=ST_NEW_STATEMENT;
							break;
						}else{
							ForGosub_s[ForGosub_sl].step=4096;
							ForGosub_sl++;
							cur_line++;
							if(cur_line>=srclinecount)return ERR_NO_ERROR;
							GOTOLINE(cur_line);
							break;
						}
					}
					if(*srcpos!=TOKEN_STEP)return ERR_SYNTAX_ERROR;
					srcpos=JumpSpace(srcpos+1);
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					if(!PopCalcStack_int(&tmpint))return ERR_SYNTAX_ERROR;
					if(tmpint>0){
						if(Variable[ForGosub_s[ForGosub_sl].VarID].value>tmpint2){
							srcpos=tmppos+1;
							cur_line=tmpline;
							state=ST_NEW_STATEMENT;
							break;
						}
					}
					if(tmpint<0){
						if(Variable[ForGosub_s[ForGosub_sl].VarID].value<tmpint2){
							srcpos=tmppos+1;
							cur_line=tmpline;
							state=ST_NEW_STATEMENT;
							break;
						}
					}
					ForGosub_s[ForGosub_sl].step=tmpint;
					ForGosub_sl++;
					state=ST_NEW_STATEMENT;
					break;
				case TOKEN_LINPUT:
					memset(tmpstr,0x00,sizeof(tmpstr));
					tmpstr_p=0;
					srcpos=JumpSpace(srcpos+1);
					srcpos=GetVarID(srcpos,&tmpint,&errtmp);
					if(!Variable[tmpint].isStr)return ERR_TYPE_MISMATCH;
					errtmp=PushCalcStack(TYPE_FUNC,Char2Code('='),NULL,2);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					memset(tmpstr,0x00,sizeof(tmpstr));
					InputLine(tmpstr);
					errtmp=PushCalcStack(TYPE_STR_LIT,0,tmpstr,0);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					state=ST_NEW_STATEMENT;
					break;
				case TOKEN_LOAD:
					srcpos=JumpSpace(srcpos+1);
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					errtmp=PopCalcStack_str(tmpstr);
					if(!errtmp)return ERR_SYNTAX_ERROR;
					c2=strchr(tmpstr,':');
					if(c2==NULL){
						if(strlen(tmpstr)>8)return ERR_ILLEGAL_FUNCTION_CALL;
						for(i=0;tmpstr[i]!=0;i++){
							if((!isupper(tmpstr[i]))&&(!isdigit(tmpstr[i]))&&(tmpstr[i]!='_'))return ERR_ILLEGAL_FUNCTION_CALL;
						}
						errtmp=LoadPResource("PRG",tmpstr);
						if(errtmp!=ERR_NO_ERROR)return errtmp;						   
						if(Psys_SYSBEEP)PlaySoundMem(SHandleBEEP[46],DX_PLAYTYPE_BACK);
						break;
					}else{
						for(i=0;tmpstr[i]!=':';i++){
							if((i>6)||((!isupper(tmpstr[i]))&&(!isdigit(tmpstr[i]))))return ERR_ILLEGAL_RESOURCE_TYPE;
						}
						tmpint=i;
						for(i++;tmpstr[i]!=0;i++){
							if((i>8)||((!isupper(tmpstr[i]))&&(!isdigit(tmpstr[i]))))return ERR_ILLEGAL_FUNCTION_CALL;
						}
						memcpy(tmpstr2,tmpstr,tmpint);
						memcpy(tmpstr3,&tmpstr[tmpint+1],i-tmpint-1);
						errtmp=LoadPResource(tmpstr2,tmpstr3);
						//ダイアログ出す？
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						if(Psys_SYSBEEP)PlaySoundMem(SHandleBEEP[46],DX_PLAYTYPE_BACK);
					}
					break;
				case TOKEN_RUN:
					if(!interactive_flag)return ERR_ILLEGAL_FUNCTION_CALL;
					*runflag=1;
					return ERR_NO_ERROR;
					break;
				case TOKEN_NEW:
					if(!interactive_flag)return ERR_ILLEGAL_FUNCTION_CALL;
					memset(srcline_begin_token_pos,0x0000,sizeof(srcline_begin_token_pos));
					memset(srcline_token_count,0x0000,sizeof(srcline_token_count));
					srclinecount=0;
					memset(source_ptr,0x0000,sizeof(source_ptr));
					return ERR_NO_ERROR;
					break;
				case TOKEN_GOTO:
					srcpos=JumpSpace(srcpos+1);
					if(*srcpos!=TOKEN_LABEL && *srcpos!=TOKEN_LABEL2)return ERR_SYNTAX_ERROR;
					srcpos=JumpSpace(srcpos+1);
					srcpos=GetLabelName(srcpos,tmpstr,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					tmpint=0;
					for(i=0;i<labelcount;i++){
						if(strcmp(labellist_name[i],tmpstr)==0){
							cur_line=labellist_line[i];
							GOTOLINE(cur_line);
							tmpint=1;
							break;
						}
					}
					if(tmpint==0)return ERR_UNDEFINED_LABEL;
					break;
				case TOKEN_GOSUB:
					srcpos=JumpSpace(srcpos+1);
					if(*srcpos!=TOKEN_LABEL && *srcpos!=TOKEN_LABEL2)return ERR_SYNTAX_ERROR;
					srcpos=JumpSpace(srcpos+1);
					memset(tmpstr,0x00,sizeof(tmpstr));
					for(i=0;i<8;i++){
						c=Code2Char(*srcpos);
						if((c==' ')||(*srcpos==0x000D)||(*srcpos==0x0000)||(c==':')||(c=='\''))break;
						if((c!='_')&&(!isalpha(c))&&(!isdigit(c))){
							return ERR_SYNTAX_ERROR;
						}
						tmpstr[i]=toupper(c);
						srcpos++;
					}
					tmpint=0;
					for(i=0;i<labelcount;i++){
						if(strcmp(labellist_name[i],tmpstr)==0){
							ForGosub_s[ForGosub_sl].col=srcpos-input;
							ForGosub_s[ForGosub_sl].line=cur_line;
							cur_line=labellist_line[i];
							GOTOLINE(cur_line);
							ForGosub_sl++;
							tmpint=1;
							break;
						}
					}
					if(tmpint==0)return ERR_UNDEFINED_LABEL;
					break;
				case TOKEN_ON:
					srcpos=JumpSpace(srcpos+1);
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					if(!PopCalcStack_int(&tmpint))return ERR_SYNTAX_ERROR;
					tmpint=FloorInt(tmpint);
					if(tmpint<0){
						while((*srcpos!=0x0000)&&(*srcpos!=0x000D)&&(c!=':')&&(c!='\'')){
							srcpos++;
							c=Code2Char(*srcpos);
						}
						break;
					}
					srcpos=JumpSpace(srcpos);
					tmpint2=0;
					if(*srcpos==TOKEN_GOTO)tmpint2=1;
					if(*srcpos==TOKEN_GOSUB)tmpint2=2;
					if(tmpint2==0)return ERR_SYNTAX_ERROR;
					for(i=0;i<tmpint;i++){
						if(i!=0)srcpos++;
						while(1){
							c=Code2Char(*srcpos);
							if(c==',')break;
							if((*srcpos==0x0000)||(*srcpos==0x000D)||(c==':')||(c=='\'')){
								tmpint2=0;
								break;
							}
							srcpos++;
						}
					}
					if(tmpint2==0)break;
					srcpos=JumpSpace(srcpos+1);
					if(*srcpos!=TOKEN_LABEL && *srcpos!=TOKEN_LABEL2)return ERR_SYNTAX_ERROR;
					srcpos=JumpSpace(srcpos+1);
					memset(tmpstr,0x00,sizeof(tmpstr));
					for(i=0;i<8;i++){
						c=Code2Char(*srcpos);
						if((c==' ')||(*srcpos==0x000D)||(*srcpos==0x0000)||(c==':')||(c=='\'')||(c==','))break;
						if((c!='_')&&(!isalnum(c))){
							return ERR_SYNTAX_ERROR;
						}
						tmpstr[i]=toupper(c);
						srcpos++;
					}
					tmpint=0;
					for(i=0;i<labelcount;i++){
						if(strcmp(labellist_name[i],tmpstr)==0){
							if(tmpint2==1){//GOTO
								cur_line=labellist_line[i];
								GOTOLINE(cur_line);
								tmpint=1;
								break;
							}else if(tmpint2==2){//GOSUB
								ForGosub_s[ForGosub_sl].col=srcpos-input;
								ForGosub_s[ForGosub_sl].line=cur_line;
								cur_line=labellist_line[i];
								GOTOLINE(cur_line);
								ForGosub_sl++;
								tmpint=1;
								break;
							}
						}
					}
					if(tmpint==0)return ERR_UNDEFINED_LABEL;
					break;
				case TOKEN_RETURN:
					if(ForGosub_sl<=0)return ERR_RETURN_WITHOUT_GOSUB;
					ForGosub_sl--;
					srcpos=input+ForGosub_s[ForGosub_sl].col;
					cur_line=ForGosub_s[ForGosub_sl].line;
					srcpos=JumpSpace(srcpos);
					break;
				case TOKEN_LABEL:case TOKEN_LABEL2:
					cur_line++;
					if(cur_line>=srclinecount)return ERR_NO_ERROR;
					GOTOLINE(cur_line);
					break;
				case TOKEN_END:
					return ERR_NO_ERROR;
				case TOKEN_STOP:
					breakflag=1;
					return ERR_NO_ERROR;
				default:
					if(isInstruction(t)){
						if(isNoArgInstruction(t)){
							errtmp=PushCalcStack(TYPE_FUNC,t,NULL,0);
							if(errtmp!=ERR_NO_ERROR)return errtmp;
							errtmp=PushCalcStack(TYPE_VOID,0,NULL,0);
							if(errtmp!=ERR_NO_ERROR)return errtmp;
							ProcessRemainingOperator();
							srcpos++;
							state=ST_NEW_STATEMENT;
							break;
						}else{
							errtmp=PushCalcStack(TYPE_FUNC,t,NULL,0);
							if(!errtmp)return ERR_UNDEFINED;
							errtmp=PushCalcStack(TYPE_FUNC,Char2Code('('),NULL,0);
							if(!errtmp)return ERR_UNDEFINED;
							srcpos=JumpSpace(srcpos+1);
							if((*srcpos==0x0000)||(Code2Char(*srcpos)==':')){
								errtmp=PushCalcStack(TYPE_VOID,0,NULL,0);
								if(!errtmp)return ERR_UNDEFINED;
								errtmp=PushCalcStack(TYPE_FUNC,Char2Code(')'),NULL,0);
								if(!errtmp)return ERR_UNDEFINED;
								state=ST_NEW_STATEMENT;
								break;
							}
							srcpos=ReadFormula(srcpos,&errtmp);
							if(errtmp!=ERR_NO_ERROR)return errtmp;
							argcount=1;
							while(Code2Char(*srcpos)==','){
								errtmp=PushCalcStack(TYPE_SPECIAL,Char2Code(','),NULL,0);
								if(errtmp!=ERR_NO_ERROR)return errtmp;
								srcpos=JumpSpace(srcpos+1);
								srcpos=ReadFormula(srcpos,&errtmp);
								if(errtmp!=ERR_NO_ERROR)return errtmp;
								argcount++;
							}
							errtmp=PushCalcStack(TYPE_FUNC,Char2Code(')'),NULL,argcount);
							if(!errtmp)return ERR_UNDEFINED;
							errtmp=ProcessRemainingOperator();
							if(errtmp!=ERR_NO_ERROR)return errtmp;
							state=ST_NEW_STATEMENT;
							break;
						}
					}else if((GetSystemVariableType(*srcpos)==2)||(GetSystemVariableType(*srcpos)==4)){
						if(GetSystemVariableType(*srcpos)==2){
							errtmp=PushCalcStack(TYPE_INT_SYSVAR,*srcpos,NULL,0);
						}else{
							//MEM$
							errtmp=PushCalcStack(TYPE_STR_SYSVAR,*srcpos,NULL,0);
						}
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						srcpos=JumpSpace(srcpos+1);
						if(Code2Char(*srcpos)!='=')return ERR_SYNTAX_ERROR;
						errtmp=PushCalcStack(TYPE_FUNC,Char2Code('='),NULL,2);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						srcpos=JumpSpace(srcpos+1);
						srcpos=ReadFormula(srcpos,&errtmp);
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						errtmp=ProcessRemainingOperator();
						if(errtmp!=ERR_NO_ERROR)return errtmp;
						state=ST_NEW_STATEMENT;
						break;		
					}else if(*srcpos==0x0000){
						if(state==ST_SUBSTITUTION_NAME)return ERR_SYNTAX_ERROR;
						return ERR_NO_ERROR;
					}else{
						//error
						return ERR_SYNTAX_ERROR;
					}
					break;
			}
		}else{
			//Char
		
			switch(state){
				case ST_LINE_BEGIN:
					if(c=='\''){
						cur_line++;
						if(cur_line>=srclinecount)return ERR_NO_ERROR;
						GOTOLINE(cur_line);
						srcpos--;
					}else if(c=='@'){
						cur_line++;
						if(cur_line>=srclinecount)return ERR_NO_ERROR;
						GOTOLINE(cur_line);
						srcpos--;
					}else if(c=='?'){
						state=ST_PRINT;
					//代入
					}else if(isalpha(c)||(c=='_')){
						memset(tmpstr,0x00,sizeof(tmpstr));
						tmpstr_p=0;
						tmpstr[0]=toupper(c);//一文字目記録
						tmpstr_p++;
						state=ST_SUBSTITUTION_NAME;
					}else if((c==' ')||(c==':')){
						//NOP
					}else if(c==0){
						return ERR_NO_ERROR;
					}else{
						if(*srcpos==0x0D){
							cur_line++;
							if(cur_line>=srclinecount)return ERR_NO_ERROR;
							GOTOLINE(cur_line);
							break;
						}
						printf("srcpos=%X='%c'\n",t,c);
						return ERR_SYNTAX_ERROR;
					}
					srcpos++;
					break;
				case ST_NEW_STATEMENT:
					if(c=='\''){
						cur_line++;
						if(cur_line>=srclinecount)return ERR_NO_ERROR;
						GOTOLINE(cur_line);
						srcpos--;
					}else if(c=='@'){
						cur_line++;
						if(cur_line>=srclinecount)return ERR_NO_ERROR;
						GOTOLINE(cur_line);
						srcpos--;
					}else if(c=='?'){
						state=ST_PRINT;
					//代入
					}else if(isalpha(c)||(c=='_')){
						memset(tmpstr,0x00,sizeof(tmpstr));
						tmpstr_p=0;
						tmpstr[0]=toupper(c);//一文字目記録
						tmpstr_p++;
						state=ST_SUBSTITUTION_NAME;
					}else if((c==' ')||(c==':')){
						//NOP
					}else if(c==0){
						//正常終了
						return ERR_NO_ERROR;
					}else{
						if(*srcpos==0x0D){
							cur_line++;
							if(cur_line>=srclinecount)return ERR_NO_ERROR;
							GOTOLINE(cur_line);
							break;
						}
						printf("srcpos=%X='%c'\n",t,c);
						return ERR_SYNTAX_ERROR;
					}
					srcpos++;
					break;
				case ST_SUBSTITUTION_NAME:
					srcpos--;
					srcpos=GetVarID(srcpos,&tmpint,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos);
					if(Code2Char(*srcpos)!='=')return ERR_SYNTAX_ERROR;
					errtmp=PushCalcStack(TYPE_FUNC,Char2Code('='),NULL,2);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					srcpos=JumpSpace(srcpos+1);
					srcpos=ReadFormula(srcpos,&errtmp);
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					errtmp=ProcessRemainingOperator();
					if(errtmp!=ERR_NO_ERROR)return errtmp;
					state=ST_NEW_STATEMENT;
					break;		
				default:
					if(*srcpos==0x0D){
						cur_line++;
						if(cur_line>=srclinecount)return ERR_NO_ERROR;
						GOTOLINE(cur_line);
						break;
					}
					srcpos++;
					break;
			}
		}
		tmpint=Draw2Console();
		if(log_en2)printf("%s/",TokenCode2Str(*srcpos));
		if(!tmpint)return ERR_UNDEFINED;
		if(breakflag)return ERR_NO_ERROR;
	}
	ProcessRemainingOperator(); 
	return ERR_NO_ERROR;
}

int NewVar(char* name){
	int type=0;
	if(Psys_FREEVAR<=0)return -1;
	memset(Variable[VAR_MAX-Psys_FREEVAR].name,0x00,sizeof(Variable[VAR_MAX-Psys_FREEVAR].name));
	strcpy(Variable[VAR_MAX-Psys_FREEVAR].name,name);
	if(calc_sl>=CALC_S_MAX)return -1;
	if(name[strlen(name)-1]=='$' || name[strlen(name)-2]=='$'){
		Variable[VAR_MAX-Psys_FREEVAR].isStr=true;
		Variable[VAR_MAX-Psys_FREEVAR].isDim=0;
		type=TYPE_STR_VAR;
	}else{
		Variable[VAR_MAX-Psys_FREEVAR].isStr=false;
		Variable[VAR_MAX-Psys_FREEVAR].isDim=0;
		type=TYPE_INT_VAR;
	}
	Psys_FREEVAR--;
	return VAR_MAX-Psys_FREEVAR-1;
}



#ifdef __cplusplus
}
#endif