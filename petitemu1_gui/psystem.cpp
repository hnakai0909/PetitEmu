/*===============================================*/
/* psystem.cpp                                   */
/*===============================================*/

#include "psystem.h"

enum SystemMode systemmode;

/*===関数定義===*/

bool InitWindow(void){
	SetWindowIconID(IDI_ICON1);
	SetGraphMode(512,384,32);
	SetMainWindowText("PetitEmu ver 0.30");
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

int PSysInit(void){
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
    if((source_ptr==NULL)){printf("Can't malloc(init)");return false;}
	memset(source_ptr,0x00,sizeof(source_ptr));
	memset(labellist_name,0x00,sizeof(labellist_name));
	memset(labellist_line,0x00,sizeof(labellist_line));
	read_srcpos=0;
	labelcount=0;
	keyboard_special=0;
	ClearDim();
	memset(consolecharbuf,0x00,sizeof(consolecharbuf));
	memset(consolecolorbuf,0x00,sizeof(consolecolorbuf));
	return true;
}

int SystemInit(void){
	InitPicture();
	InitSound();
	if(!PSysInit())return false;
    srand((unsigned) time(NULL));
	ChangeFont(FONT_PETITCOM);
	SetFontSize(8);
	SetFontThickness(5);
	return true;
}