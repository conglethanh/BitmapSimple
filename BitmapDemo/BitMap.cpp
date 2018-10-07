#include "BitMap.h"

void main()
{
	BmpHeader header;
	BmpDib dib;
	PixelArray data;
	int R1, G1, B1, R2, G2, B2;
	
	//cau e
	FILE* fi = fopen("Bitmap.in", "r");
	FILE* fo = fopen("Bitmap.out", "w");
	if (fi == NULL)
	{
		printf("File Bitmap.in can not be opened!\n");
		return;
	}
	char*filePathStart = new char[MAX];
	char*filePathEnd = new char[MAX];
	fscanf(fi, "%s\n", filePathStart);
	fscanf(fi, "%s\n", filePathEnd);
	fscanf(fi, "%d %d %d\n", &R1, &G1, &B1);
	fscanf(fi, "%d %d %d\n", &R2, &G2, &B2);
	FILE* bmpIn = fopen(filePathStart, "rb");
	FILE* bmpOut = fopen(filePathEnd, "wb");
	if (bmpIn == NULL)
	{
		printf("File BMP source can not be opened!\n");
		delete[]filePathStart;
		delete[]filePathEnd;
		return;
	}

	//cau a
	if (isBmpFile(bmpIn) == true)
		fprintf(fo, "%d\n", LA_BMP);
	else
		fprintf(fo, "%d\n", KO_PHAI_BMP);

	//cau b
	readBmpHeader(bmpIn, header);
	readBmpDib(bmpIn, dib);
	fprintf(fo, "%d %d %d %d", header.fileSize, dib.imageWidth, dib.imageHeight, dib.pixelSize);

	//cau c
	readBmpPixelArray(bmpIn, header, dib, data);
	for (int i = 0; i < dib.imageHeight; i++)
	for (int j = 0; j < dib.imageWidth; j++)
	{
		Color* p = &data.pixels[i][j];
		if (p->blue == B1&&p->green == G1&&p->red == R1)
		{
			p->blue = B2;
			p->green = G2;
			p->red = R2;
		}
	}
	//luu file vao bmp khac
	fseek(bmpOut, 0, 0L);
	fwrite(&header, sizeof(BmpHeader), 1, bmpOut);
	fwrite(&dib, sizeof(BmpDib), 1, bmpOut);
	for (int i = 0; i < dib.imageHeight; i++)
	for (int j = 0; j < dib.imageWidth; j++)
	{
		Color* p = &data.pixels[dib.imageHeight-i-1][j];
		fwrite(p, sizeof(Color), 1, bmpOut);
	}

	//cau d
	printBmpHeader(header);
	printBmpDib(dib);
	drawBmp(dib, data);

	fcloseall();
	delete[]filePathStart;
	delete[]filePathEnd;
	releaseBmpPixelArray(data);
}