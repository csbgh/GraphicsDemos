#ifndef _LOH_H
#define _LOG_H

#pragma once

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>

#define _LOG_FILE "Log.txt"
#define _LOG_BUFFER_SIZE 2048

#define _ENABLE_CONSOLE_LOG

#ifdef _ENABLE_CONSOLE_LOG
	#define _LOG_CONSOLE(_value) std::cout<<_value // TODO : change to printf?
#else
	#define _LOG_CONSOLE(_value)
#endif

enum class LogType
{
	LOG_INFO,    // basic log message
	LOG_ERROR,   // log message for some serious error which has occurred
	LOG_WARNING, // log message is a warning that some problem has occurred
	LOG_CONSOLE  // basic log message that is only written to the console
};

class Log
{
public:

	static void Write(const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		_write(format, LogType::LOG_INFO, args);
		va_end(args);
	}

	static void WriteError(const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		_write(format, LogType::LOG_ERROR, args);
		va_end(args);
	}

	static void WriteWarning(const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		_write(format, LogType::LOG_WARNING, args);
		va_end(args);
	}

private:

	static void _write(const char *format, LogType type, va_list args)
	{
		static char buffer[_LOG_BUFFER_SIZE];
		FILE *log_file = getLogFile();

		// construct log message string
		int length = vsnprintf(buffer, _LOG_BUFFER_SIZE, format, args);
		if (length < 0)
			return;

		// write to file
		if (type == LogType::LOG_ERROR)
		{
			fprintf(log_file, "ERROR : ");
			_LOG_CONSOLE("ERROR : ");
		}
		else if (type == LogType::LOG_WARNING)
		{
			fprintf(log_file, "WARNING : ");
			_LOG_CONSOLE("WARNING : ");
		}

		// write log message
		fputs(buffer, log_file);
		_LOG_CONSOLE(buffer);

		fprintf(log_file, "\n");
		_LOG_CONSOLE("\n");

		fflush(log_file);
	}

	static FILE *getLogFile()
	{
		static FILE *log_file = fopen(_LOG_FILE, "w");
		return log_file;
	}
};

#define LOG(_format, ...) Log::Write(_format, __VA_ARGS__)

#define LOG_ERROR(_format, ...) Log::WriteError(_format, __VA_ARGS__)

#define LOG_WARNING(_format, ...) Log::WriteWarning(_format, __VA_ARGS__)

#endif // _LOG_H