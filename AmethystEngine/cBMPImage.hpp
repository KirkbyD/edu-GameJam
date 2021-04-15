#pragma once
#include <string>

class cBMPImage {
public:
	cBMPImage(const std::string& filename);
	virtual ~cBMPImage(void);

	bool IsLoaded(void);
	unsigned long GetFileSize(void);
	unsigned long GetImageWidth(void);
	unsigned long GetImageHeight(void);
	unsigned long GetImageSize(void);
	char* GetData(void);

private:
	cBMPImage(void);
	bool LoadBMPFromFile(const std::string& filename);

	bool m_bIsLoaded;
	unsigned long m_FileSize;
	unsigned long m_ImageWidth;
	unsigned long m_ImageHeight;
	unsigned long m_ImageSize;
	char* m_pData;
};
