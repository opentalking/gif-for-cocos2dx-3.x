#include "FileData.h"

FileData::FileData()
{
	m_Data = nullptr;
	m_Size = 0;
	m_CurrIndex = 0;
}

FileData::FileData(unsigned char* data, ssize_t size)
{
	SetData(data, size);
}

FileData::~FileData()
{
	ReleaseData();
}

void FileData::SetData(unsigned char* buff, ssize_t sz)
{
	m_Data = (unsigned char*)malloc(sizeof(unsigned char) * sz);
	memcpy( m_Data, buff, sz );
	m_Size = sz;
	m_CurrIndex = 0;
}

ssize_t FileData::ReadData(unsigned char* buf, ssize_t size)
{
	ssize_t sz = size;
	if (sz > m_Size - m_CurrIndex)
		sz = m_Size - m_CurrIndex;
	if (sz <= 0) return 0;

	memcpy( buf, m_Data + m_CurrIndex, sz );
	m_CurrIndex += sz;

	return sz;
}

void FileData::ReleaseData(void)
{
	if (m_Data != nullptr)
	{
		free(m_Data);
		m_Data = nullptr;
		m_Size = 0;
		m_CurrIndex = 0;
	}
}