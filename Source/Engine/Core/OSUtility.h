#pragma once

namespace Reflex
{
#define FOREGROUND_YELLOW FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY

	struct SetConsoleTextAttributes
	{
		SetConsoleTextAttributes(const WORD attributes)
		{
			CONSOLE_SCREEN_BUFFER_INFO Info;
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hStdout, &Info);
			savedAttributes = Info.wAttributes;
			SetConsoleTextAttribute(hStdout, attributes);
		}

		~SetConsoleTextAttributes()
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), savedAttributes);
		}

	private:
		WORD savedAttributes = 0;
	};
}