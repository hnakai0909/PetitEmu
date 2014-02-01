/*===============================================*/
/* pgui.cpp                                      */
/*===============================================*/

#include "pgui.h"

int GrHandle[10];

/*===関数定義===*/

extern "C" bool ProcessFrame(void){
	int tmp=0;
	static int count=GetNowCount();
	if(log_en3)ProcessDebugScreen();
	if((GetNowCount()-count)<16)return true;
	count=GetNowCount();
	UpdateSystemVariable();
	tmp=ProcessMessage();
	if(tmp)return false;
	//処理
	keyboard_special=0;
	CheckJoypad();
	switch(panelmode){
	case PNLMD_OFF:
		break;
	case PNLMD_PANEL:
		//未実装
		break;
	case PNLMD_KYA:case PNLMD_KYM:case PNLMD_KYK:
		CheckPanel();
		CheckKey();
		break;
	case PNLMD_DIALOG:
		//未実装
		break;
	default:
		break;
	}

	//描画
	ClearDrawScreen();
	//SetDrawScreen(DX_SCREEN_BACK);
	ProcessBG();
	switch(panelmode){
	case PNLMD_OFF:
		DrawBG();
		break;
	case PNLMD_PANEL:
		//未実装
		DrawBG();
		break;
	case PNLMD_KYA:case PNLMD_KYM:case PNLMD_KYK:
		DrawGraph(0,192,GrHandle[0],FALSE);
		DrawFuncKey();
		DrawKeyboard();
		DrawUnderPanel();
		break;
	case PNLMD_DIALOG:
		DrawGraph(0,192,GrHandle[1],FALSE);
		break;
	default:
		break;
	}
	DrawConsole();
	if(log_en3)DrawDebugScreen();
	//ScreenFlip();
	return true;
}

void DrawConsole(void){
	int i=0,Cr=0,tmp=0;
	char tmpstr[256];
	for(i=0;i<768;i++){
		FontTable(con_buf[i%32][i/32].chr,tmpstr);
		tmp=con_buf[i%32][i/32].color;
		Cr=GetColor(consolecharcolor[tmp][0],consolecharcolor[tmp][1],consolecharcolor[tmp][2]);
		DrawString(i%32*8,(i/32)*8,tmpstr,Cr);
	}
	return;
}

void DrawKeyboard(void){
	char tmpstr[STR_LEN_MAX];
	int i=0,tmp=0,tmp2=0;
	for(i=0;i<55;i++){
		tmp2=DAKU_NO;
		switch(panelmode){
		case PNLMD_OFF:tmp=0;break;
		case PNLMD_KYA:
			if(kbd_shift_flag){tmp=KeyMap_A_S[i];}else{tmp=KeyMap_A[i];}
			break;
		case PNLMD_KYM:
			if(kbd_shift_flag){tmp=KeyMap_G_S[i];}else{tmp=KeyMap_G[i];}
			break;
		case PNLMD_KYK:
			if(kbd_shift_flag){tmp=KeyMap_K_S[i];tmp2=KeyMap_K_S_DAKU[i];}else{tmp=KeyMap_K[i];}
			break;
		default:tmp=0;break;
		}
		FontTable(tmp,tmpstr);
		if(inrange(i,0,12))DrawString(i*16+27,51+192,tmpstr,GetColor(255,255,255));
		if(inrange(i,13,28))DrawString((i-13)*16+3,75+192,tmpstr,GetColor(255,255,255));
		if(inrange(i,29,42))DrawString((i-29)*16+27,99+192,tmpstr,GetColor(255,255,255));
		if(inrange(i,43,54))DrawString((i-43)*16+35,123+192,tmpstr,GetColor(255,255,255));
		if(tmp2==DAKU_DAKU)FontTable(0xDE,tmpstr);
		if(tmp2==DAKU_HAN)FontTable(0xDF,tmpstr);
		if(tmp2!=DAKU_NO){
			if(inrange(i,0,12))DrawString(i*16+27+5,51+192,tmpstr,GetColor(255,255,255));
			if(inrange(i,13,28))DrawString((i-13)*16+3+5,75+192,tmpstr,GetColor(255,255,255));
			if(inrange(i,29,42))DrawString((i-29)*16+27+5,99+192,tmpstr,GetColor(255,255,255));
			if(inrange(i,43,54))DrawString((i-43)*16+35+5,123+192,tmpstr,GetColor(255,255,255));
		}
		tmp2=DAKU_NO;
		if(kbd_shift_flag==0){
			switch(panelmode){
			case PNLMD_KYA:
				tmp=KeyMap_A_S[i];
				break;
			case PNLMD_KYM:
				tmp=KeyMap_G_S[i];
				break;
			case PNLMD_KYK:
				tmp=KeyMap_K_S[i];
				tmp2=KeyMap_K_S_DAKU[i];
				break;
			default:tmp=0;break;
			}
			FontTable(tmp,tmpstr);
			if(inrange(i,0,12))DrawString(i*16+29,51+8+194,tmpstr,GetColor(121,56,251));
			if(inrange(i,13,28))DrawString((i-13)*16+5,75+8+194,tmpstr,GetColor(121,56,251));
			if(inrange(i,29,42))DrawString((i-29)*16+29,99+8+194,tmpstr,GetColor(121,56,251));
			if(inrange(i,43,54))DrawString((i-43)*16+37,123+8+194,tmpstr,GetColor(121,56,251));
			if(tmp2==DAKU_DAKU)FontTable(0xDE,tmpstr);
			if(tmp2==DAKU_HAN)FontTable(0xDF,tmpstr);
			if(tmp2!=DAKU_NO){
				if(inrange(i,0,12))DrawString(i*16+27+7,51+8+194,tmpstr,GetColor(121,56,251));
				if(inrange(i,13,28))DrawString((i-13)*16+3+7,75+8+194,tmpstr,GetColor(121,56,251));
				if(inrange(i,29,42))DrawString((i-29)*16+27+7,99+8+194,tmpstr,GetColor(121,56,251));
				if(inrange(i,43,54))DrawString((i-43)*16+35+7,123+8+194,tmpstr,GetColor(121,56,251));
			}
		}
	}
	return;
}

void DrawFuncKey(void){
	unsigned int i=0,j=0;
	char tmpstr[STR_LEN_MAX];
	memset(tmpstr,0x00,sizeof(tmpstr));
	for(i=0;i<5;i++){
		for(j=0;(j<4)&&(j<FuncKeyStr[i].len);j++){
			FontTable(FuncKeyStr[i].s[j],tmpstr);
			DrawString(i*48+j*8+8,192,tmpstr,GetColor(255,255,255));
		}
		if(FuncKeyStr[i].len>4){
			FontTable('.',tmpstr);
			DrawString(i*48+40,192,tmpstr,GetColor(255,255,255));
		}
	}
	return;
}

void DrawUnderPanel(void){
	switch(runmode){
	case RMD_STOP:
		DrawGraph(0,357,GrHandle[2],FALSE);
		break;
	case RMD_LINE:case RMD_PRG:
		DrawGraph(0,357,GrHandle[3],FALSE);
		break;
	default:
		DrawGraph(0,357,GrHandle[4],FALSE);
		break;
	}
	return;
}

void ProcessBG(void){
	int layer=0,page=0;
	for(page=0;page<2;page++){
		for(layer=0;layer<2;layer++){
			if(bgofs_time[page][layer]>0){
				bgofs_nowx[page][layer]+=((bgofs_destx[page][layer]-bgofs_nowx[page][layer])/bgofs_time[page][layer]);
				bgofs_nowy[page][layer]+=((bgofs_desty[page][layer]-bgofs_nowy[page][layer])/bgofs_time[page][layer]);
				bgofs_time[page][layer]--;
			}
		}
	}
	return;
}

void DrawBG(void){
	int x=0,y=0,layer=0,page=0;
	int ofsx_1=0,ofsy_1=0;
	int ofsx_2=0,ofsy_2=0;
	ofsx_1=bgofs_nowx[page][layer]/8;ofsy_1=bgofs_nowy[page][layer]/8;
	ofsx_2=bgofs_nowx[page][layer]%8;ofsy_2=bgofs_nowy[page][layer]%8;
	for(page=0;page<2;page++){
		if(VisibleFlags.BG0)DrawBGChip(page,0,ofsx_1,ofsx_2,ofsy_1,ofsy_2);
		if(VisibleFlags.BG1)DrawBGChip(page,1,ofsx_1,ofsx_2,ofsy_1,ofsy_2);
	}
	return;
}

void DrawBGChip(int page,int layer,int ofsx_1,int ofsx_2,int ofsy_1,int ofsy_2){
	int character=0,palette=0,x=0,y=0;
	struct BGDATA *bgdata;
	for(x=bgclip_x_begin[page];x<=(bgclip_x_end[page]+1);x++){
		for(y=bgclip_y_begin[page];y<=(bgclip_y_end[page]+1);y++){
			bgdata=(BGData[page][layer][x+ofsx_1]+(y+ofsy_1));
			character=bgdata->character;
			palette=bgdata->palette;
			//grhandle=CreateGraphFromSoftImage(BGHandle[character]);
			//if(bgdata->h_inverse)ReverseBaseImageH(grhandle);
			//if(bgdata->v_inverse)ReverseBaseImageV(grhandle);
			//DrawGraph(x*8-ofsx_2,y*8-ofsy_2,grhandle,FALSE);
		}
	}
	return;
}

void ConsoleClearLine(void){
	int i=0;
	for(i=0;i<32;i++){
		con_buf[i][Psys_CSRY].chr=0;
		con_buf[i][Psys_CSRY].color=0;
	}
	return;
}

int PutChar2Console(unsigned char arg,int indent_option){
	int x=0,y=0;
	if(Psys_CSRY>=24){
		ScrollConsole();
		Psys_CSRY=23;
	}
	con_buf[Psys_CSRX][Psys_CSRY].chr=arg;
	con_buf[Psys_CSRX][Psys_CSRY].color=consolecolor;
	Psys_CSRX++;
	if(Psys_CSRX>=32){
		Psys_CSRX=0;
		Psys_CSRY++;
	}
	if(indent_option!=-1){
		if(Psys_CSRY>=24){
			ScrollConsole();
			Psys_CSRY=23;
		}
	}
	return 0;
}

void ScrollConsole(void){
	int x=0,y=0;
	for(y=0;y<23;y++){
		for(x=0;x<32;x++){
			con_buf[x][y]=con_buf[x][y+1];
			con_buf[x][y+1].chr=0;
			con_buf[x][y+1].color=0;
		}
	}
	return;
}

int Print2Console(st string,int indent_option){
	unsigned int i=0;
	while(i<string.len){
		PutChar2Console(string.s[i],indent_option);
		i++;
	}
	switch(indent_option){
	case 0://改行
		if(Psys_CSRY>=23){
			ScrollConsole();
			Psys_CSRY--;
		}
		Psys_CSRX=0;
		Psys_CSRY++;
		if(Psys_CSRY>=24){
			ScrollConsole();
			Psys_CSRY=23;
			for(i=0;i<32;i++){
				con_buf[i][23].chr=0;
				con_buf[i][23].color=0;
			}
		}
		break;
	case 1://改行なし　';'
		break;
	case 2://タブ ','
		while(Psys_CSRX%4!=0)Psys_CSRX++;
		if(Psys_CSRX>=32){Psys_CSRY++;Psys_CSRX=0;}
		if(Psys_CSRY>=24){
			ScrollConsole();
			Psys_CSRY=23;
		}
		break;
	default:
		break;
	}
	return 0;
}

void PutStartMessage(void){
	Print2Console(str2mystr2("PetitcomEmulator=PetitEmu"),0);
	Print2Console(str2mystr2("Win-Edition ver0.30"),0); 
	Print2Console(str2mystr2("supports SMILEBASIC ver1.20"),0);
	Print2Console(str2mystr2("Please read Readme.txt ."),0);
	Print2Console(str2mystr2("1048576 byte free"),0);
	Print2Console(str2mystr2("developed by hnakai"),0);
	Print2Console(str2mystr2(""),0);
}

void ProcessDebugScreen(void){
	unsigned int i=0;
	char tmpstr[256];
	static int initialized=0,conslogbufp=0;
	if(runmode==RMD_STOP)return;
	if(initialized==0){
		memset(conslogbuf,0x00,sizeof(conslogbuf));
		initialized=1;
	}
	memset(tmpstr,0x00,sizeof(char)*256);
	sprintf_s(tmpstr,256,"%s/",TokenCode2Str(error_occured_token).s);
	if(strlen(tmpstr)==0){
		if(*srcpos==0x000D){
			strcpy_s(tmpstr,256,"CRLF/");
		}else{
			sprintf_s(tmpstr,256,"#%04X/",error_occured_token);
		}
	}
	for(i=0;i<strlen(tmpstr);i++){
		conslogbuf[conslogbufp]=tmpstr[i];
		conslogbufp=(conslogbufp+1)%768;
	}
	return;
}

void DrawDebugScreen(void){
	int Cr=0,tmp=0,i=0;
	char tmpstr[256];
	if(runmode==RMD_STOP)return;
	for(i=0;i<768;i++){
		FontTable(conslogbuf[i],tmpstr);
		tmp=0;
		Cr=GetColor(consolecharcolor[tmp][0],consolecharcolor[tmp][1],consolecharcolor[tmp][2]);
		DrawString(i%32*8+256,(i/32)*8,tmpstr,Cr);
	}
	return;
}