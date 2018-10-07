#include "bmp.h"

bool isBmpFile(FILE *f)
{
	if (f == NULL)
		return false;

	BmpSignature signature;
	fseek(f, 0, SEEK_SET);
	fread(&signature, sizeof(BmpSignature), 1, f);

	return signature.data[0] == 'B' && signature.data[1] == 'M';
}

void readBmpHeader(FILE *f, BmpFile &bmp)
{
	if (f == NULL)
		return;

	fseek(f, 0, SEEK_SET);
	fread(&bmp.header, sizeof(BmpHeader), 1, f);
}
void writeBmpHeader(FILE *f, BmpFile bmp)
{
	if (f == NULL)
		return;

	fseek(f, 0, SEEK_SET);
	fwrite(&bmp.header, sizeof(BmpHeader), 1, f);
}

void readBmpDib(FILE *f, BmpFile &bmp)
{
	if (f == NULL)
		return;

	fseek(f, sizeof(BmpHeader), SEEK_SET);
	fread(&bmp.dib, sizeof(BmpDib), 1, f);
}
void writeBmpDib(FILE *f, BmpFile bmp)
{
	if (f == NULL)
		return;

	fseek(f, sizeof(BmpHeader), SEEK_SET);
	fwrite(&bmp.dib, sizeof(BmpDib), 1, f);
}

void readBmpPixelArray(FILE *f, BmpFile &bmp)
{
	if (f == NULL)
		return;

	bmp.pixelArray.rowCount = bmp.dib.imageHeight;
	bmp.pixelArray.columnCount = bmp.dib.imageWidth;
	bmp.pixelArray.pixels = new Color *[bmp.pixelArray.rowCount];

	char paddingCount = countPadding(bmp.dib.imageWidth, bmp.dib.pixelSize);

	fseek(f, bmp.header.dataOffset, SEEK_SET);

	for (int i = 0; i < bmp.pixelArray.rowCount; i++)
	{
		scanBmpPixelLine(f, bmp.pixelArray.pixels[bmp.pixelArray.rowCount - i - 1], bmp.dib.imageWidth);
		skipBmpPadding(f, paddingCount);
	}
}
void scanBmpPixelLine(FILE *f, Color *&line, uint32_t length)
{
	if (f == NULL)
		return;

	line = new Color[length];
	fread(line, sizeof(Color), length, f);
}

void skipBmpPadding(FILE *f, char count)
{
	if (f == NULL)
		return;

	if (count == 0)
		return;

	char padding[3];
	fread(padding, count, 1, f);
}

void writeBmpPixelArray(FILE *f, BmpFile bmp)
{
	if (f == NULL)
		return;

	char paddingCount = countPadding(bmp.dib.imageWidth, bmp.dib.pixelSize);

	fseek(f, bmp.header.dataOffset, SEEK_SET);

	for (int i = 0; i < bmp.pixelArray.rowCount; i++)
	{
		writeBmpPixelLine(f, bmp.pixelArray.pixels[bmp.pixelArray.rowCount - i - 1], bmp.dib.imageWidth);
		addBmpPadding(f, paddingCount);
	}
}
void writeBmpPixelLine(FILE *f, Color *line, uint32_t length)
{
	if (f == NULL)
		return;

	fwrite(line, sizeof(Color), length, f);
}

void addBmpPadding(FILE *f, char count)
{
	if (f == NULL)
		return;

	if (count == 0)
		return;

	char padding[3];
	fwrite(padding, count, 1, f);
}
char countPadding(uint32_t imageWidth, uint16_t pixelSize)
{
	return (4 - (imageWidth * (pixelSize / 8) % 4)) % 4;
}
void releaseBmpPixelArray(BmpFile &bmp)
{
	for (int i = 0; i < bmp.pixelArray.rowCount; i++)
		delete[]bmp.pixelArray.pixels[i];

	delete[]bmp.pixelArray.pixels;
}

void copyBmp(BmpFile source, BmpFile &dest, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	copyBmpHeader(source, dest, width, height);
	copyBmpDib(source, dest, width, height);
	copyBmpPixelArray(source, dest, x, y, width, height);
}
void copyBmpHeader(BmpFile source, BmpFile &dest, uint32_t width, uint32_t height)
{
	dest.header.signature.data[0] = 'B';
	dest.header.signature.data[1] = 'M';
	dest.header.fileSize = (width * source.dib.pixelSize / 8 + countPadding(width, source.dib.pixelSize)) * height + source.header.dataOffset;
	dest.header.dataOffset = source.header.dataOffset;
}
void copyBmpDib(BmpFile source, BmpFile &dest, uint32_t width, uint32_t height)
{
	dest.dib = source.dib;
	dest.dib.imageWidth = width;
	dest.dib.imageHeight = height;
	dest.dib.bitmapByteCount = (width * source.dib.pixelSize / 8 + countPadding(width, source.dib.pixelSize)) * height;
}
void copyBmpPixelArray(BmpFile source, BmpFile &dest, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	PixelArray *sArray = &source.pixelArray;
	PixelArray *dArray = &dest.pixelArray;

	dArray->rowCount = height;
	dArray->columnCount = width;
	dArray->pixels = new Color *[dArray->rowCount];

	for (int i = 0; i < dArray->rowCount; i++)
	{
		dArray->pixels[i] = new Color[dArray->columnCount];
		for (int j = 0; j < dArray->columnCount; j++)
			dArray->pixels[i][j] = sArray->pixels[i + y][j + x];
	}
}

void readBmpFile(char *filename, BmpFile &bmp)
{
	FILE *f = fopen(filename, "rb");

	if (f == NULL || !isBmpFile(f))
	{
		printf("Error or Invalid Bmp File.\n");
		return;
	}

	readBmpHeader(f, bmp);
	readBmpDib(f, bmp);
	readBmpPixelArray(f, bmp);

	fclose(f);
}
void writeBmpFile(char *filename, BmpFile bmp)
{
	FILE *f = fopen(filename, "wb");

	if (f == NULL)
	{
		printf("Error.\n");
		return;
	}

	writeBmpHeader(f, bmp);
	writeBmpDib(f, bmp);
	writeBmpPixelArray(f, bmp);

	fclose(f);
}

void joinBmpFilename(const char *destName, const char *destExt, int i, int j, char *destFile, int dem)
{
	//char temp[4] = { i + 49, 'x', j + 49, '\0' };
	char temp[MAX];
	itoa(dem, temp, 10);
	strcpy(destFile, destName);
	strcat(destFile, temp);
	strcat(destFile, destExt);
}
void splitBmpFile(char *sourceFile, char *destName, int hSplit, int vSplit)
{
	const char *BMP_EXT = ".bmp";

	BmpFile bmpSource;
	readBmpFile(sourceFile, bmpSource);

	int partWidth = bmpSource.dib.imageWidth / vSplit;
	int partHeight = bmpSource.dib.imageHeight / hSplit;

	char *destFile = new char[strlen(destName) + strlen(BMP_EXT) + 1];

	BmpFile bmpDest;
	int y = 0;
	int dem = 0;
	for (int i = 0; i < hSplit; i++)
	{
		int x = 0;
		for (int j = 0; j < vSplit; j++)
		{
			dem++;
			copyBmp(bmpSource, bmpDest, x, y, partWidth, partHeight);
			joinBmpFilename(destName, BMP_EXT, i, j, destFile,dem);
			writeBmpFile(destFile, bmpDest);

			releaseBmpPixelArray(bmpDest);
			x += partWidth;
		}
		y += partHeight;
	}
	releaseBmpPixelArray(bmpSource);
}