#include "DxLib.h"
#include "data.h"
#include "petitemu.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			 LPSTR lpCmdLine, int nCmdShow )
{
	int tmp=0,i=0;
	char tmpstr[STR_LEN_MAX];
	char inputline[64];
	uint16_t codedata[10000];//どこまで必要？
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(inputline,0x00,sizeof(inputline));
	memset(codedata,0x0000,sizeof(codedata));
	SetWindowIconID(IDI_ICON1);
	if(!InitWindow())return -1;
	SetAlwaysRunFlag(TRUE);
	AllocConsole();
	freopen("CONOUT$","w",stdout);
	freopen("CONIN$","r",stdin);
	SystemInit();
	PutStartMessage();

	puts("READY");
	Print2Console("READY",0);
	ShowWindow(GetMainWindowHandle(),SW_SHOW);
	SetAlwaysRunFlag(FALSE);
	ShowWindow(GetMainWindowHandle(),SW_SHOW);
	while(strcmp(inputline,"EXIT")!=0){
		puts(">");
		InputLine(inputline);
		printf("%s\n",inputline);
		if(strcmp(inputline,"")!=0){
			RunInteractive(inputline);
			Draw2Console();
		}
		if(ProcessMessage()==-1)break;
	}
	FreeConsole();
	DxLib_End() ;		// ＤＸライブラリ使用の終了処理
	return 0 ;		// ソフトの終了
}