#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma once
#pragma pack(1)

#define MAX 100000
#define CAT_DOC_VA_NGANG 6
#define CAT_DOC_HOAC_NGANG 4

struct BmpSignature
{
	unsigned char data[2];
};

struct BmpHeader
{
	BmpSignature signature;
	uint32_t fileSize;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t dataOffset;
};

struct BmpDib
{
	uint32_t dibSize;
	int32_t	 imageWidth;
	int32_t  imageHeight;
	uint16_t colorPlaneCount;
	uint16_t pixelSize;
	uint32_t compressMethod;
	uint32_t bitmapByteCount;
	int32_t  horizontalResolution;
	int32_t  verticalResolution;
	uint32_t colorCount;
	uint32_t importantColorCount;
};

struct Color
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

struct ColorTable
{
	Color	 *colors;
	uint32_t length;
};

struct PixelArray
{
	Color	 **pixels;
	uint32_t rowCount;
	uint32_t columnCount;
};

struct BmpFile
{
	BmpHeader	header;
	BmpDib		dib;
	ColorTable	colorTable;
	PixelArray	pixelArray;
};

bool isBmpFile(FILE *f);

void readBmpHeader(FILE *f, BmpFile &bmp);
void writeBmpHeader(FILE *f, BmpFile bmp);

void readBmpDib(FILE *f, BmpFile &bmp);
void writeBmpDib(FILE *f, BmpFile bmp);

void readBmpPixelArray(FILE *f, BmpFile &bmp);
void scanBmpPixelLine(FILE *f, Color *&line, uint32_t length);
void skipBmpPadding(FILE *f, char count);

void writeBmpPixelArray(FILE *f, BmpFile bmp);
void writeBmpPixelLine(FILE *f, Color *line, uint32_t length);
void addBmpPadding(FILE *f, char count);

char countPadding(uint32_t imageWidth, uint16_t pixelSize);
void releaseBmpPixelArray(BmpFile &bmp);

void copyBmp(BmpFile source, BmpFile &dest, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void copyBmpHeader(BmpFile source, BmpFile &dest, uint32_t width, uint32_t height);
void copyBmpDib(BmpFile source, BmpFile &dest, uint32_t width, uint32_t height);
void copyBmpPixelArray(BmpFile source, BmpFile &dest, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void readBmpFile(char *filename, BmpFile &bmp);
void writeBmpFile(char *filename, BmpFile bmp);

void joinBmpFilename(const char *destName, const char *destExt, int i, int j, char *destFile, int dem);
void splitBmpFile(char *sourceFile, char *destName, int hSplit, int vSplit);