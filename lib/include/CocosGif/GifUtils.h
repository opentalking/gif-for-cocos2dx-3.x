#ifndef GIFUTILS_H
#define GIFUTILS_H

#include <stdio.h>

class GifUtils
{
public:
	static FILE* openFile(const char* fileName)
	{
		FILE* file = NULL;
#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (CC_TARGET_PLATFORM != CC_PLATFORM_MARMALADE)
		errno_t err = fopen_s(&file, fileName, "rb" );

		if(err && file)
		{
			fclose(file);
			file = NULL;
		}
#else
		file = fopen(fileName, "rb" );
#endif
		return file;

	}

	static void closeFile(FILE* file)
	{
		if(file)
		{
			fclose(file);
		}
	}

	static bool isGif(const char* fileName)
	{
		FILE* file = openFile(fileName);
		bool res = isGifFile(file);
		closeFile(file);

		return res;
	}

	static bool isGifFile(FILE* file)
	{
		if(file == NULL)
		{
			return false;
		}
		long index = ftell(file);
		bool res = false;
		char buf[GIF_STAMP_LEN];
		int read = fread(buf,1,GIF_STAMP_LEN,file);
		if (read == GIF_STAMP_LEN) {
			if (memcmp(GIF_STAMP,   buf, GIF_STAMP_LEN) == 0 ||
				memcmp(GIF87_STAMP, buf, GIF_STAMP_LEN) == 0 ||
				memcmp(GIF89_STAMP, buf, GIF_STAMP_LEN) == 0) {
					res =  true;
			}
		}
		fseek(file,index,SEEK_SET);
		return res;
	}
};

#endif//GIFUTILS_H