/*===============================================*/
/* pinput.cpp                                    */
/*===============================================*/

#include "pinput.h"

/*===関数定義===*/

void ClearKeyBuffer(void){
	memset(keybuffer,0x00,sizeof(keybuffer));
	keybuffer_qhead=0;
	keybuffer_qtail=0;
}

void WriteKeyBuffer(const unsigned char arg){
	keybuffer[keybuffer_qtail]=arg;
	keybuffer_qtail++;
	if(keybuffer_qtail==KEYBUFFER_MAX)keybuffer_qtail=0;
	//リングバッファが埋まると読み書き共にインクリメントしFirstInを捨てる
	if(keybuffer_qhead==keybuffer_qtail){
		keybuffer[keybuffer_qhead]=0;
		keybuffer_qhead++;
	}
	return;
}

bool ReadKeyBuffer(unsigned char* arg){
	*arg=keybuffer[keybuffer_qhead];
	if(keybuffer_qhead==keybuffer_qtail){
		*arg=0;
		return false;
	}
	keybuffer[keybuffer_qhead]=0;
	keybuffer_qhead++;
	if(keybuffer_qhead==KEYBUFFER_MAX)keybuffer_qhead=0;
	return true;
}

void CheckPanel(void){
	int MouseX,MouseY,type,code,mouseinput,tmp,tmp2;
	unsigned int i;
	unsigned char tmpc=0;
	static int mousetime=0,type_clickstart=0,code_clickstart=0;
	mouseinput=!!(GetMouseInput()&MOUSE_INPUT_LEFT);
	keyboard_special=0;
	if(mouseinput){
		Psys_TCHST=1;
		Psys_TCHTIME++;
		if(mousetime!=-1)mousetime++;
	}else{
		Psys_TCHST=0;
		Psys_TCHTIME=0;
		mousetime=0;
	}
	if(mousetime>=(KEY_TIME_TO_RENSYA+KEY_RENSYA_INTERVAL))mousetime=KEY_TIME_TO_RENSYA;
	GetMousePoint(&MouseX,&MouseY);
	if((MouseY>=192)&&(mouseinput)){
		switch(panelmode){
		case PNLMD_OFF:
			break;
		case PNLMD_PANEL:
			if(MouseY>=484){
				tmp2=KeyboardWhichKey(MouseX,MouseY-192,&type,&code);
			}
			//未実装
			break;
		case PNLMD_KYA:case PNLMD_KYM:case PNLMD_KYK:
			tmp2=KeyboardWhichKey(MouseX,MouseY-192,&type,&code);
			if(mousetime==1){
				type_clickstart=type;
				code_clickstart=code;
			}
			if((type_clickstart!=type)||(code_clickstart!=code))mousetime=-1;
			if((mousetime==1)||(mousetime==KEY_TIME_TO_RENSYA)){
				if(tmp2){
					switch(type){
					case KT_NO:
						break;
					case KT_FUNCKEY:
						for(i=0;i<FuncKeyStr[code].len;i++){
							WriteKeyBuffer(FuncKeyStr[code].s[i]);
						}
						break;
					case KT_CHAR:
						tmpc=Keychar2Char(code,&tmp);
						if(tmpc==0)break;
						WriteKeyBuffer(tmpc);
						if(tmp==DAKU_DAKU)WriteKeyBuffer(0xDE);//濁点プッシュ
						if(tmp==DAKU_HAN)WriteKeyBuffer(0xDF);//半濁点プッシュ
						break;
					case KT_SYSTEM:
						keyboard_special=code;
						switch(code){
						case PKEY_ESC:
							break;
						case PKEY_BS:
							break;
						case PKEY_TAB:
							//本来はTABSTEP・現在の文字位置に依存する
							WriteKeyBuffer(' ');
							break;
						case PKEY_SHIFT:
							kbd_shift_flag=(kbd_shift_flag+1)%2;
							break;
						case PKEY_ENTER:
							break;
						case PKEY_CAPS:
							break;
						case PKEY_MODE_A:
							panelmode=PNLMD_KYA;
							kbd_shift_flag=0;
							//CAPS LOCK はずす
							break;
						case PKEY_MODE_G:
							panelmode=PNLMD_KYM;
							kbd_shift_flag=0;
							//CAPS LOCK はずす
							break;
						case PKEY_MODE_K:
							panelmode=PNLMD_KYK;
							kbd_shift_flag=0;
							//CAPS LOCK はずす
							break;
						case PKEY_INS:
							kbd_insert_flag=(kbd_insert_flag+1)%2;
							break;
						case PKEY_DEL:
							break;
						case PKEY_SEARCH:
							//無印(初代)時代では未実装なのでなにもしない
							break;
						case PKEY_EXIT:
							break;
						case PKEY_HELP:
							break;
						case PKEY_RUN:
							breakflag=1;
							break;
						case PKEY_EDIT:
							break;
						case PKEY_ICON_UP:
							break;
						case PKEY_ICON_DOWN:
							break;
						default:
							break;
						}
						break;
					case KT_ICON:
						break;
					default:
						break;
					}
					if((type==KT_FUNCKEY)||(type==KT_CHAR)||((type==KT_SYSTEM)&&(
						(code==PKEY_ESC)||(code==PKEY_BS)||(code==PKEY_TAB)||(code==PKEY_SHIFT)||(code==PKEY_ENTER)
						||(code==PKEY_CAPS)||(code==PKEY_MODE_A)||(code==PKEY_MODE_G)||(code==PKEY_MODE_K)
						||(code==PKEY_INS)||(code==PKEY_DEL)||(code==PKEY_SEARCH)
						))){
							if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					}
				}
			}
			break;
		case PNLMD_DIALOG:
			//MouseX MouseY
			break;
		default:
			break;
		}
	}

}

void CheckJoypad(void){
	int padinput,padx,pady;
	bool i_UP,i_DOWN,i_LEFT,i_RIGHT,i_A,i_B,i_X,i_Y,i_L,i_R,i_START,i_SELECT;
	GetJoypadAnalogInput(&padx,&pady,DX_INPUT_KEY_PAD1);
	padinput=GetJoypadInputState(DX_INPUT_PAD1);
	i_UP=(pady<-500);
	i_DOWN=(pady>500);
	i_LEFT=(padx<-500);
	i_RIGHT=(padx>500);
	i_A=!!(padinput&PAD_INPUT_1);
	i_B=!!(padinput&PAD_INPUT_2);
	i_X=!!(padinput&PAD_INPUT_3);
	i_Y=!!(padinput&PAD_INPUT_4);
	i_L=!!(padinput&PAD_INPUT_5);
	i_R=!!(padinput&PAD_INPUT_6);
	i_START=!!(padinput&PAD_INPUT_8);
	i_SELECT=!!(padinput&PAD_INPUT_7);
	button_state=i_UP+i_DOWN*2+i_LEFT*4+i_RIGHT*8+i_A*16+i_B*32+i_X*64+i_Y*128+i_L*256+i_R*512+i_START*1024+i_SELECT*2048;
	if(i_SELECT)breakflag=1;
	return;
}

bool KeyboardWhichKey(int x,int y,int* type,int* code){
	*type=KT_NO;
	*code=PKEY_NO;
	if(inrange(y,0,15)){
		if(inrange(x,0,239)){
			*type=KT_FUNCKEY;
			*code=x/48;
			return true;
		}else if(inrange(x,240,255)){
			*type=KT_SYSTEM;
			*code=PKEY_EXIT;
			return true;
		}
	}else if(inrange(y,48,143)){
		switch(y/24){
		case 2:
			if(inrange(x,0,23)){
				*type=KT_SYSTEM;
				*code=PKEY_ESC;
				return true;
			}else if(inrange(x,232,255)){
				*type=KT_SYSTEM;
				*code=PKEY_BS;
				return true;
			}else if(inrange((x-24)/16,0,12)){
				*type=KT_CHAR;
				*code=(x-24)/16+2;
				return true;
			}
			break;
		case 3:
			if(inrange(x/16,0,15)){
				*type=KT_CHAR;
				*code=x/16+16;
				return true;
			}
			break;
		case 4:
			if(inrange(x,0,23)){
				*type=KT_SYSTEM;
				*code=PKEY_TAB;
				return true;
			}else if(inrange((x-24)/16,0,13)){
				*type=KT_CHAR;
				*code=(x-24)/16+33;
				return true;
			}
			break;
		case 5:
			if(inrange(x,0,31)){
				*type=KT_SYSTEM;
				*code=PKEY_SHIFT;
				return true;
			}else if(inrange(x,224,255)){
				*type=KT_SYSTEM;
				*code=PKEY_ENTER;
				return true;
			}else if(inrange(x/16,2,13)){
				*type=KT_CHAR;
				*code=x/16+46;
				return true;
			}
			break;
		default:
			return 0;
			break;
		}
	}else if(inrange(y,144,159)){
		if(inrange(x,0,15)){
			*type=KT_SYSTEM;
			*code=PKEY_CAPS;
			return true;
		}else if(inrange(x,24,39)){
			*type=KT_SYSTEM;
			*code=PKEY_MODE_A;
			return true;
		}else if(inrange(x,40,55)){
			*type=KT_SYSTEM;
			*code=PKEY_MODE_G;
			return true;
		}else if(inrange(x,56,71)){
			*type=KT_SYSTEM;
			*code=PKEY_MODE_K;
			return true;
		}else if(inrange(x,80,191)){
			*type=KT_CHAR;
			*code=PKEY_SPACE;
			return true;
		}else if(inrange(x,200,215)){
			*type=KT_SYSTEM;
			*code=PKEY_INS;
			return true;
		}else if(inrange(x,216,231)){
			*type=KT_SYSTEM;
			*code=PKEY_DEL;
			return true;
		}
	}else if(inrange(y,168,191)){
		if(inrange(x,0,31)){
			*type=KT_SYSTEM;
			*code=PKEY_HELP;
			return true;
		}else if(inrange(x,40,87)){
			*type=KT_SYSTEM;
			*code=PKEY_RUN;
			return true;
		}else if(inrange(x,88,135)){
			*type=KT_SYSTEM;
			*code=PKEY_EDIT;
			return true;
		}else if(inrange(x,144,159)){
			*type=KT_SYSTEM;
			if(y<180){
				*code=PKEY_ICON_UP;
			}else{
				*code=PKEY_ICON_DOWN;
			}
			return true;
		}else if(inrange(x,160,255)&&inrange((x-160)/24,0,3)){
			*type=KT_ICON;
			*code=(x-160)/24+1;
			return true;
		}
		return false;
	}
	return false;
}

void CheckKey(void){
	char KeyCheckSTBuf[256];
	static char KeyCheckTimeBuf[256];
	bool ShiftFlag=0;
	unsigned int i=0,j=0,type=0,code=0;
	int Table1_1[256]={
		KEY_INPUT_1,KEY_INPUT_2,KEY_INPUT_3,KEY_INPUT_4,KEY_INPUT_5,KEY_INPUT_6,KEY_INPUT_7,KEY_INPUT_8,KEY_INPUT_9,KEY_INPUT_0,
		KEY_INPUT_MINUS,KEY_INPUT_PREVTRACK,KEY_INPUT_YEN,
		KEY_INPUT_Q,KEY_INPUT_W,KEY_INPUT_E,KEY_INPUT_R,KEY_INPUT_T,KEY_INPUT_Y,KEY_INPUT_U,KEY_INPUT_I,
		KEY_INPUT_O,KEY_INPUT_P,KEY_INPUT_AT,KEY_INPUT_LBRACKET,
		KEY_INPUT_A,KEY_INPUT_S,KEY_INPUT_D,KEY_INPUT_F,KEY_INPUT_G,KEY_INPUT_H,KEY_INPUT_J,KEY_INPUT_K,KEY_INPUT_L,
		KEY_INPUT_SEMICOLON,KEY_INPUT_COLON,KEY_INPUT_RBRACKET,
		KEY_INPUT_Z,KEY_INPUT_X,KEY_INPUT_C,KEY_INPUT_V,KEY_INPUT_B,KEY_INPUT_N,KEY_INPUT_M,
		KEY_INPUT_COMMA,KEY_INPUT_PERIOD,KEY_INPUT_SLASH,KEY_INPUT_BACKSLASH,
		KEY_INPUT_SPACE,0
	};
	int Table1_2[256]={
		KEY_INPUT_ESCAPE,KEY_INPUT_F1,KEY_INPUT_F2,KEY_INPUT_F3,KEY_INPUT_F4,KEY_INPUT_F5,
		KEY_INPUT_INSERT,KEY_INPUT_DELETE,
		KEY_INPUT_BACK,KEY_INPUT_TAB,KEY_INPUT_RETURN,KEY_INPUT_CAPSLOCK,
		KEY_INPUT_PGUP,KEY_INPUT_PGDN,KEY_INPUT_HOME,KEY_INPUT_END,
		KEY_INPUT_UP,KEY_INPUT_LEFT,KEY_INPUT_RIGHT,KEY_INPUT_DOWN,0
	};
	unsigned char Table2[256]={
		'1','2','3','4','5','6','7','8','9','0','-','^','\\',
		'Q','W','E','R','T','Y','U','I','O','P','@','[',
		'A','S','D','F','G','H','J','K','L',';',':',']',
		'Z','X','C','V','B','N','M',',','.','/',0x7F,
		' ',0x00
	};
	unsigned char Table2_S[256]={
		'!','"','#','$','%','&','\'','(',')',0x00,'=','~','|',
		'q','w','e','r','t','y','u','i','o','p','`','{',
		'a','s','d','f','g','h','j','k','l','+','*','}',
		'z','x','c','v','b','n','m','<','>','?','_',
		' '
	};
	GetHitKeyStateAll(KeyCheckSTBuf);
	ShiftFlag=KeyCheckSTBuf[KEY_INPUT_LSHIFT]^KeyCheckSTBuf[KEY_INPUT_RSHIFT];
	for(i=0;Table1_1[i]!=0;i++){
		if(KeyCheckSTBuf[Table1_1[i]]==1){
			if((KeyCheckTimeBuf[Table1_1[i]]==0)||(KeyCheckTimeBuf[Table1_1[i]]==KEY_TIME_TO_RENSYA)){
				if(ShiftFlag){
					if(Table2_S[i]!=0)WriteKeyBuffer(Table2_S[i]);
				}else{
					WriteKeyBuffer(Table2[i]);
				}
				if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
			}
			KeyCheckTimeBuf[Table1_1[i]]++;
			if(KeyCheckTimeBuf[Table1_1[i]]>=(KEY_TIME_TO_RENSYA+KEY_RENSYA_INTERVAL))KeyCheckTimeBuf[Table1_1[i]]=KEY_TIME_TO_RENSYA;
		}else{
			KeyCheckTimeBuf[Table1_1[i]]=0;
		}
	}
	for(i=0;Table1_2[i]!=0;i++){
		if(KeyCheckSTBuf[Table1_2[i]]==1){
			if((KeyCheckTimeBuf[Table1_2[i]]==0)||(KeyCheckTimeBuf[Table1_2[i]]==KEY_TIME_TO_RENSYA)){
				switch(Table1_2[i]){
					/*
					TODO:KEY_INPUT_PGUP,KEY_INPUT_PGDN,KEY_INPUT_HOME,KEY_INPUT_END
					*/
				case KEY_INPUT_ESCAPE:
					breakflag=1;
					keyboard_special=PKEY_ESC;
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_INSERT:
					kbd_insert_flag=(kbd_insert_flag+1)%2;
					keyboard_special=PKEY_INS;
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_DELETE:
					keyboard_special=PKEY_DEL;
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_BACK:
					keyboard_special=PKEY_BS;
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_TAB:
					keyboard_special=PKEY_TAB;
					break;
				case KEY_INPUT_CAPSLOCK:
					keyboard_special=PKEY_CAPS;
					kbd_capslock_flag=(kbd_capslock_flag+1)%2;
					break;
				case KEY_INPUT_UP:
					button_state|=1;
					break;
				case KEY_INPUT_DOWN:
					button_state|=2;
					break;
				case KEY_INPUT_LEFT:
					button_state|=4;
					break;
				case KEY_INPUT_RIGHT:
					button_state|=8;
					break;
				case KEY_INPUT_F1:
					for(j=0;j<FuncKeyStr[0].len;j++)WriteKeyBuffer(FuncKeyStr[0].s[j]);
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_F2:
					for(j=0;j<FuncKeyStr[1].len;j++)WriteKeyBuffer(FuncKeyStr[1].s[j]);
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_F3:
					for(j=0;j<FuncKeyStr[2].len;j++)WriteKeyBuffer(FuncKeyStr[2].s[j]);
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_F4:
					for(j=0;j<FuncKeyStr[3].len;j++)WriteKeyBuffer(FuncKeyStr[3].s[j]);
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_F5:
					for(j=0;j<FuncKeyStr[4].len;j++)WriteKeyBuffer(FuncKeyStr[4].s[j]);
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				case KEY_INPUT_RETURN:
					keyboard_special=PKEY_ENTER;
					if(Psys_SYSBEEP==0x00001000)PlaySoundMem(SHandleBEEP[9],DX_PLAYTYPE_BACK);
					break;
				default:
					break;
				}
			}
			KeyCheckTimeBuf[Table1_2[i]]++;
			if(KeyCheckTimeBuf[Table1_2[i]]>=(KEY_TIME_TO_RENSYA+KEY_RENSYA_INTERVAL))KeyCheckTimeBuf[Table1_2[i]]=KEY_TIME_TO_RENSYA;
		}else{
			KeyCheckTimeBuf[Table1_2[i]]=0;
		}
	}
	return;
}

unsigned char Keychar2Char(int code,int* daku_flag){
	int i=0;
	int Table1[256]={
		PKEY_1		,PKEY_2		,PKEY_3		,PKEY_4	,PKEY_5	,PKEY_6	,PKEY_7	,PKEY_8	,PKEY_9	,PKEY_0	,
		PKEY_MINUS	,PKEY_PLUS	,PKEY_EQUAL	,
		PKEY_DOLLAR	,PKEY_WQUOTE,PKEY_Q		,PKEY_W	,PKEY_E	,PKEY_R	,PKEY_T	,PKEY_Y	,PKEY_U	,PKEY_I	,
		PKEY_O	,PKEY_P	,PKEY_AT,PKEY_ASTER	,PKEY_LPAREN	,PKEY_RPAREN,
		PKEY_EXCLA,PKEY_A,PKEY_S,PKEY_D,PKEY_F,PKEY_G,PKEY_H,PKEY_J,PKEY_K,PKEY_L,PKEY_SCOLON,PKEY_COLON,PKEY_LT,LEY_GT,
		PKEY_SQUOTE,PKEY_Z,PKEY_X,PKEY_C,PKEY_V,PKEY_B,PKEY_N,PKEY_M,PKEY_COMMA,PKEY_DOT,PKEY_SLASH,PKEY_PER,
		PKEY_SPACE
	};
	*daku_flag=DAKU_NO;
	for(i=0;i<56;i++){
		if(code==Table1[i]){
			switch(panelmode){
			case PNLMD_KYA:
				if(kbd_shift_flag){
					return KeyMap_A_S[i];
				}else{
					return KeyMap_A[i];
				}
			case PNLMD_KYM:
				if(kbd_shift_flag){
					return KeyMap_G_S[i];
				}else{
					return KeyMap_G[i];
				}
			case PNLMD_KYK:
				if(kbd_shift_flag){
					*daku_flag=KeyMap_K_S_DAKU[i];
					return KeyMap_K_S[i];
				}else{
					return KeyMap_K[i];
				}
			default:
				return 0;
			}
		}
	}
	return 0;
}

void InputLine(st* str){
	int i=0,j=0,cursol=0;
	int cursol_blink_timer=0;
	unsigned char tmpc=0;
	mystrclear(str);
	while(ProcessFrame()){
		if(breakflag==1 && runmode==RMD_PRG)break;
		if(cursol_blink_timer<30){
			DrawBox(cursol*8,(kbd_insert_flag)?(Psys_CSRY*8+5):(Psys_CSRY*8),cursol*8+7,Psys_CSRY*8+7,GetColor(255,255,255),TRUE);
		}
		cursol_blink_timer=(cursol_blink_timer+1)%60;
		WaitVSync(1);
		tmpc=0;
		if(keyboard_special==PKEY_ENTER)break;
		if(keyboard_special==PKEY_BS){
			if(cursol>0){
				for(i=cursol-1;i<31;i++){
					con_buf[i][Psys_CSRY]=con_buf[i+1][Psys_CSRY];
				}
				con_buf[31][Psys_CSRY].chr=0;
				con_buf[31][Psys_CSRY].color=0;
				cursol--;
			}
		}
		if(keyboard_special==PKEY_DEL){
			for(i=cursol;i<31;i++){
				con_buf[i][Psys_CSRY]=con_buf[i+1][Psys_CSRY];
			}
			con_buf[31][Psys_CSRY].chr=0;
			con_buf[31][Psys_CSRY].color=0;
		}
		if(ReadKeyBuffer(&tmpc)){
			if((kbd_insert_flag)&&(con_buf[31][Psys_CSRY].chr==0)){
				for(i=31;i>cursol;i--){
					con_buf[i][Psys_CSRY]=con_buf[i-1][Psys_CSRY];
				}
			}
			con_buf[cursol][Psys_CSRY].chr=tmpc;
			con_buf[cursol][Psys_CSRY].color=consolecolor;
			cursol++;
		}
		if(button_state&0x0004)cursol--;
		if(button_state&0x0008)cursol++;
		cursol=limitrange(cursol,0,31);
	}
	if(breakflag==1 && runmode==RMD_PRG)return;
	for(i=0;i<32;i++){
		if(con_buf[i][Psys_CSRY].chr!=0)break;
	}
	for(j=0;(j+i)<32;j++){
		str->s[j]=con_buf[j+i][Psys_CSRY].chr;
		if(con_buf[j+i][Psys_CSRY].chr==0)break;
	}
	str->len=j;
	Print2Console(MYSTR_NULL,0);
	return;
}