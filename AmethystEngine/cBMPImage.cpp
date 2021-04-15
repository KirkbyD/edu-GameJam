#include <fstream>
#include <iostream>

#include "cBMPImage.hpp"
#include "cResourceManager.hpp"

extern cResourceManager gResourceManager;

class cBMPFileHeader {
public:
	cBMPFileHeader(void);
	unsigned short headerField;
	unsigned long fileSize;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned long dataOffset;
	void print();
};

class cBMPInfoHeader {
public:
	cBMPInfoHeader(void);
	unsigned long sizeOfHeader;
	unsigned long imageWidth;
	unsigned long imageHeight;
	unsigned short numColorPlanes;
	unsigned short bitsPerPixel;
	unsigned long compressionMethod;
	unsigned long imageSize;
	unsigned long horizontalResolution;
	unsigned long verticalResolution;
	unsigned long numColorsInPalette;
	unsigned long numImportantColors;
	void print();
};

cBMPFileHeader::cBMPFileHeader(void) {
	this->headerField = 0;
	this->fileSize = 0;
	this->reserved1 = 0;
	this->reserved2 = 0;
	this->dataOffset = 0;
}

cBMPInfoHeader::cBMPInfoHeader(void) {
	this->sizeOfHeader = 0;
	this->imageWidth = 0;
	this->imageHeight = 0;
	this->numColorPlanes = 0;
	this->bitsPerPixel = 0;
	this->compressionMethod = 0;
	this->imageSize = 0;
	this->horizontalResolution = 0;
	this->verticalResolution = 0;
	this->numColorsInPalette = 0;
	this->numImportantColors = 0;
}

void cBMPFileHeader::print(void) {
	std::cout << "BitMap File Header\n";
	std::cout << "  Header Field: " << headerField << "\n";
	std::cout << "  File Size: " << fileSize << "\n";
	std::cout << "  Reserved 1: " << reserved1 << "\n";
	std::cout << "  Reserved 2: " << reserved2 << "\n";
	std::cout << "  Data Offset: " << dataOffset << "\n";
	std::cout << "\n";
}

void cBMPInfoHeader::print(void) {
	std::cout << "BitMap Info Header\n";
	std::cout << "  Size Of Header: " << sizeOfHeader << "\n";
	std::cout << "  Image Width: " << imageWidth << "\n";
	std::cout << "  Image Height: " << imageHeight << "\n";
	std::cout << "  Number Of Colored Planes: " << numColorPlanes << "\n";
	std::cout << "  Bits Per Pixel: " << bitsPerPixel << "\n";
	std::cout << "  Compression Method: " << compressionMethod << "\n";
	std::cout << "  Image Size: " << imageSize << "\n";
	std::cout << "  Horizontal Resolution: " << horizontalResolution << "\n";
	std::cout << "  Vertical Resolution: " << verticalResolution << "\n";
	std::cout << "  Number Of Colors In Palette: " << numColorsInPalette << "\n";
	std::cout << "  Number of Important Colors: " << numImportantColors << "\n";
	std::cout << "\n";
}

cBMPImage::cBMPImage(void)
{
	this->m_bIsLoaded = false;
	return;
}

cBMPImage::cBMPImage(const std::string& filename)
{
	this->m_bIsLoaded = false;
	this->LoadBMPFromFile(filename);
	return;
}

cBMPImage::~cBMPImage(void)
{

	return;
}

unsigned long cBMPImage::GetFileSize(void)
{
	return this->m_FileSize;
}

unsigned long cBMPImage::GetImageWidth(void)
{
	return this->m_ImageWidth;
}

unsigned long cBMPImage::GetImageHeight(void)
{
	return this->m_ImageHeight;
}

unsigned long cBMPImage::GetImageSize(void)
{
	return this->m_ImageSize;
}

char* cBMPImage::GetData(void)
{
	return this->m_pData;
}

bool cBMPImage::LoadBMPFromFile(const std::string& filename)
{

	//
	unsigned int bmpFile;
	if (!gResourceManager.OpenBinaryFile(filename, &bmpFile))
	{
		// Failed to open 
		printf("Failed to open %s\n", filename.c_str());
		return false;
	}

	// Load our BMP Header 
	cBMPFileHeader bmpFileHeader;
	// unsigned short headerField;
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.headerField);

	// Check if the file prepends with "BM"
	if (((char*)&bmpFileHeader.headerField)[0] != 'B'
		|| ((char*)&bmpFileHeader.headerField)[1] != 'M')
	{
		printf("%s is not a BitMap.\n", filename.c_str());
		gResourceManager.CloseFile(bmpFile);
		return false;
	}

	// Retrieve the rest of the file information
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.fileSize);
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.reserved1);
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.reserved2);
	gResourceManager.ReadData(bmpFile, &bmpFileHeader.dataOffset);

	// Check if info is correct
	//bmpFileHeader.print();

	// Load our image info header
	cBMPInfoHeader bmpInfoHeader;
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.sizeOfHeader);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.imageWidth);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.imageHeight);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.numColorPlanes);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.bitsPerPixel);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.compressionMethod);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.imageSize);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.horizontalResolution);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.verticalResolution);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.numColorsInPalette);
	gResourceManager.ReadData(bmpFile, &bmpInfoHeader.numImportantColors);
	//gResourceManager.ReadData(bmpFile, (char*)&bmpInfoHeader, sizeof(bmpInfoHeader));

	// Check if info is correct
	//bmpInfoHeader.print();

	// Check if valid bitmap
	if (bmpInfoHeader.numColorPlanes != 1
		|| bmpInfoHeader.bitsPerPixel != 24
		|| bmpInfoHeader.compressionMethod != 0)
	{
		printf("%s is not a raw BMP24.\n", filename.c_str());
		gResourceManager.CloseFile(bmpFile);
		return false;
	}

	// Programs not setting the image size properly
	if (bmpInfoHeader.imageSize == 0)
	{
		printf("Image size is not set.  Calculating it...\n");
		bmpInfoHeader.imageSize = bmpFileHeader.fileSize - bmpFileHeader.dataOffset;
	}

	// Load our image data
	this->m_pData = new char[bmpInfoHeader.imageSize];

	gResourceManager.Seek(bmpFile, bmpFileHeader.dataOffset);
	gResourceManager.ReadData(bmpFile, this->m_pData, bmpInfoHeader.imageSize);
	// We are done with the file, close it
	gResourceManager.CloseFile(bmpFile);

	// Save the information to the clas
	this->m_ImageHeight = bmpInfoHeader.imageHeight;
	this->m_ImageWidth = bmpInfoHeader.imageWidth;
	this->m_ImageSize = bmpInfoHeader.imageSize;

	// Successfully loaded the BMP file
	this->m_bIsLoaded = true;
	return true;
}

bool cBMPImage::IsLoaded(void)
{
	return this->m_bIsLoaded;
}

