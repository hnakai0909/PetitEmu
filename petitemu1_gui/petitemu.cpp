#include "petitemu.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow )
{
	FILE *tmpfp;
	st inputline={0,""};
	if(!InitWindow())return -1;
	SetAlwaysRunFlag(TRUE);
	AllocConsole();
	freopen_s(&tmpfp,"CONOUT$","w",stdout);freopen_s(&tmpfp,"CONIN$","r",stdin);
	//freopen_s(&tmpfp,"/dev/null","w",stdout);freopen_s(&tmpfp,"/dev/null","r",stdin);
	SystemInit();
	PutStartMessage();

	puts("READY");
	Print2Console(str2mystr2("READY"),0);
	ShowWindow(GetMainWindowHandle(),SW_SHOW);
	SetAlwaysRunFlag(FALSE);
	ShowWindow(GetMainWindowHandle(),SW_SHOW);
	while(mystrcmp2(inputline,"EXIT")!=0){
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