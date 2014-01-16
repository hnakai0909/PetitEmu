#include "petitemu.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow )
{
	st inputline={0,""};
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
	while(mystrcmp(inputline,str2mystr2("EXIT"))!=0){
		puts(">");
		InputLine(&inputline);
		printf("%s\n",inputline.s);
		if(mystrcmp(inputline,MYSTR_NULL)!=0){
			RunInteractive(inputline);
			ProcessFrame();
		}
		if(ProcessMessage()==-1)break;
	}
	FreeConsole();
	DxLib_End() ;		// ＤＸライブラリ使用の終了処理
	return 0 ;		// ソフトの終了
}