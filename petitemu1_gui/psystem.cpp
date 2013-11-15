#include "psystem.h"

enum SystemMode systemmode;

bool InitWindow(void){
	SetGraphMode(256,384,32);
	SetMainWindowText("PetitEmu ver 0.20");
	ChangeWindowMode(TRUE);
	if(DxLib_Init()==-1)return false;
	return true;
}

void InitPicture(void){
	GrHandle[0]=LoadGraph("test1.bmp");
	GrHandle[1]=LoadGraph("dialog_LOAD.bmp");
	GrHandle[2]=LoadGraph("panel1.bmp");
	GrHandle[3]=LoadGraph("panel2.bmp");
	GrHandle[4]=LoadGraph("panel3.bmp");
	return;
}

void InitSound(void){
	int i=0;
	char str[30];
	memset(str,0x00,sizeof(str));
	for(i=0;i<70;i++){
		sprintf(str,"BEEP/BEEP%d.ogg",i);
		SHandleBEEP[i]=LoadSoundMem(str);
	}
	return;
}

void PSysInit(void){
	InitPicture();
	InitSound();

	Psys_CSRX=0;
	Psys_CSRY=0;
	Psys_FREEMEM=1024;
	Psys_ERR=0;
	Psys_ERL=0;
	Psys_RESULT=0;
	Psys_TCHX=0;
	Psys_TCHY=0;
	Psys_TCHST=0;
	Psys_TCHTIME=0;
	Psys_MAINCNTL=0;
	Psys_MAINCNTH=0;
	Psys_TABSTEP=0;
	Psys_ICONPUSE=0;
	Psys_ICONPAGE=0;
	Psys_ICONPMAX=0;
	Psys_FUNCNO=0;
	Psys_FREEVAR=VAR_MAX;
	Psys_SYSBEEP=1;
	memset(Psys_TIME,0x00,sizeof(Psys_TIME));
	memset(Psys_DATE,0x00,sizeof(Psys_DATE));
	memset(Psys_MEM,0x00,sizeof(Psys_MEM));

	consolecolor=0;
	panelmode=PNLMD_KYA;
	runmode=RMD_STOP;
	memset(keybuffer,0x00,sizeof(keybuffer));
	kbd_shift_flag=0;
	keybuffer_qhead=0;
	keybuffer_qtail=0;
	memset(BGData,0x00,sizeof(BGData));
	bgpage=0;
	bgclip_x_begin[0]=0;bgclip_x_begin[1]=0;
	bgclip_x_end[0]=31;bgclip_x_end[1]=31;
	bgclip_y_begin[0]=0;bgclip_y_begin[1]=0;
	bgclip_y_end[0]=23;bgclip_y_end[1]=23;
	memset(bgofs_nowx,0x00,sizeof(bgofs_nowx));
	memset(bgofs_nowy,0x00,sizeof(bgofs_nowy));
	memset(bgofs_destx,0x00,sizeof(bgofs_destx));
	memset(bgofs_desty,0x00,sizeof(bgofs_desty));
	memset(bgofs_time,0x00,sizeof(bgofs_time));
	memset(ForGosub_s,0x00,sizeof(ForGosub_s));
	ForGosub_sl=0;
	error_occured_token=0;
	memset(srcline_begin_token_pos,0x0000,sizeof(srcline_begin_token_pos));
	memset(srcline_token_count,0x0000,sizeof(srcline_token_count));
	srclinecount=0;
	source_ptr=(unsigned char *)malloc(sizeof(unsigned char)*100);//とりあえずmalloc
    if((source_ptr==NULL)){printf("Can't malloc(init)");return ;}
	memset(source_ptr,0x00,sizeof(source_ptr));
	memset(labellist_name,0x00,sizeof(labellist_name));
	memset(labellist_line,0x00,sizeof(labellist_line));
	read_srcpos=0;
	labelcount=0;
	keyboard_special=0;
	ClearDim();
	return;
}

int SystemInit(void){
	PSysInit();
	memset(consolecharbuf,0x00,sizeof(consolecharbuf));
	memset(consolecolorbuf,0x00,sizeof(consolecolorbuf));
    srand((unsigned) time(NULL));
	// 描画する文字列の文字セットを変更します
	ChangeFont(FONT_PETITCOM);
	SetFontSize(8);
	SetFontThickness(5);
	return 0;
}