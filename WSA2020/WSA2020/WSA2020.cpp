﻿#include "includelib.h"
#include "Log.h"
#include "In.h"
#include "Lex.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "ru");
	Log::LOG log = Log::INITLOG;
	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);
		log = Log::getlog(parm.log);
		Log::WriteLine(log, "Логический анализатор: ", "");
		Log::WriteLog(log);
		Log::WriteParm(log, parm);
		In::IN in = In::getin(parm.in);
		In::WriteInputTextInConsole(in);
		LEX::LEX lex;
		LEX::LexType lextype ;
		for (int i = 0; i < in.text.size(); ++i)
		{
			lextype = LEX::LAnaliz(in.text[i]);
		}
		
		Log::WriteIn(log, in);
		std::cout << std::endl;
	}
	catch (Error::ERROR e)
	{
		Log::WriteError(log, e);
	}
    return 0;
}