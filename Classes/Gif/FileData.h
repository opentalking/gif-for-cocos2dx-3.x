#ifndef __FILE_DATA_H__
#define __FILE_DATA_H__

#include <stdint.h> // for ssize_t on android
#include <string>   // for ssize_t on linux
#include "platform/CCStdC.h" // for ssize_t on window

class FileData
{
public:
	FileData();
	FileData( unsigned char* data, ssize_t size );
	~FileData();

	void SetData( unsigned char* buff, ssize_t sz );
	ssize_t ReadData( unsigned char* buf, ssize_t size );
	void ReleaseData( void );

private:
	unsigned char* m_Data;
	ssize_t m_Size;
	ssize_t m_CurrIndex;
};

#endif