#include "petitemu.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			 LPSTR lpCmdLine, int nCmdShow )
{
	char inputline[64];
	memset(inputline,0x00,sizeof(inputline));
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
			ProcessFrame();
		}
		if(ProcessMessage()==-1)break;
	}
	FreeConsole();
	DxLib_End() ;		// ＤＸライブラリ使用の終了処理
	return 0 ;		// ソフトの終了
}