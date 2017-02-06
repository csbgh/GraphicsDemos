#ifndef _DEMO_DEFINITIONS_H
#define _DEMO_DEFINITIONS_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <vector>

#include "DemoTypes.h"
#include "utility/Log.h"
#include "utility/Time.h"
#include "utility/Bit.h"
#include "utility/EnumFlags.h"

#define DS_ASSERT(_condition) assert(_condition)

static int32 FileLength(FILE *file)
{
	DS_ASSERT(file);

	fseek(file, 0, SEEK_END);
	int32 fileSize = ftell(file);
	rewind(file);

	return fileSize;
}

static char *FileReadAll(const std::string &fileLoc, int32 &dataSize)
{
	// attempt to open the file
	FILE *pFile = fopen(fileLoc.c_str(), "rb");
	if (pFile == nullptr)
	{
		LOG_ERROR("unable to open file [%s]", fileLoc.c_str());
		return nullptr;
	}

	// read all the data from the file and close it
	int32 fileSize = FileLength(pFile);
	dataSize = fileSize;

	char *buffer = nullptr;

	try
	{
		buffer = new char[fileSize];
	}
	catch (std::bad_alloc &e)
	{
		LOG_ERROR("error allocating memory for file [%s]", fileLoc.c_str());
		LOG_ERROR(std::string(e.what()).c_str());
		return nullptr;
	}

	int32 bytesRead = fread(buffer, 1, fileSize, pFile);

	fclose(pFile);

	if (bytesRead != fileSize)
	{
		LOG_ERROR("file read %l of %l bytes", bytesRead, fileSize);
		return nullptr;
	}

	return buffer;
}

#endif // _DEMO_DEFINITIONS_H