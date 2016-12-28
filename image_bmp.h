//https://en.wikipedia.org/wiki/BMP_file_format

#if !defined(IMAGE_BMP_H_7E0_C_1B_INCLUDED)
#define IMAGE_BMP_H_7E0_C_1B_INCLUDED
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"image_types.h"
#define IMAGE_BMP_H_VER		20161228
#if defined(_MSC_VER)
#include<Windows.h>
#elif defined(__GNUC__)	//support gcc(Windows, linux) version is at least 4.0.0
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int       DWORD;
#pragma pack(push,1)
typedef struct tagBITMAPFILEHEADER { 
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
}  BITMAPFILEHEADER, *PBITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{
	DWORD  biSize;
	DWORD   biWidth;
	DWORD   biHeight;
	WORD   biPlanes;
	WORD   biBitCount;
	DWORD  biCompression;
	DWORD  biSizeImage;
	DWORD   biXPelsPerMeter;
	DWORD   biYPelsPerMeter;
	DWORD  biClrUsed;
	DWORD  biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct tagRGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;
#pragma pack(pop)
#endif
//Function define area
/*
*	@ReadBMP : Load BMP(DIB) file from disk. 
		support 1,2,4,8,24 bpp Bitmap image
*	@param1 : File path.
*	@param2 : image
*	@param3 : Color Palette. if bpp of BMP is greater than 8, then value is NULL.
*	@return : If Success, The return value is 1, If falis, the return value is 0
*/
int ReadBMP(char* file,IMAGE_TYPE* img,RGBQUAD** paletteArr) {
	FILE* fp = NULL;
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	//Data defined
	if (file == NULL)return IMAGE_FAILURE;
	if (img == NULL)return IMAGE_FAILURE;
	fp = fopen(file, "rb");
	if (fp == NULL)return IMAGE_FAILURE;
	fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
	if (bfh.bfType != 0x4D42)return IMAGE_FAILURE;
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
	if (ftell(fp) != bih.biSize + sizeof(BITMAPFILEHEADER)){
		//만일 DIB 헤더의 크기가 40보다 크면 뒷부분은 무시한다.
		int remainder = ftell(fp) - (bih.biSize + sizeof(BITMAPFILEHEADER));
		while (remainder--)fgetc(fp);
	}
	img->width = bih.biWidth;
	img->height = bih.biHeight;
	img->data = (IMAGE_PIXEL**)Malloc2D(img->width, img->height, sizeof(IMAGE_PIXEL));
	if (img->data == NULL)return IMAGE_FAILURE;
	if (bih.biBitCount <= 8){
		int numOfPalette = 1 << bih.biBitCount;
		RGBQUAD* palette = (RGBQUAD*)calloc(numOfPalette, sizeof(RGBQUAD));
		for (int i = 0; i < numOfPalette; i++)
			fread(palette + i, sizeof(RGBQUAD), 1, fp);
		int byteInPixel = 8 / bih.biBitCount;
		// bit단위를 byte단위로 표기하게 패딩을 붙인다.
		int byteInWidth = (img->width + byteInPixel - 1)&~(byteInPixel - 1);
		//가로줄의 크기는 4bytes 의 배수여야 한다.
		int padding = ((byteInWidth / byteInPixel + 3)&~3) - (byteInWidth / byteInPixel);
		for (int y = img->height-1; y >=0; y--){
			for (int x = 0; x < img->width; x += byteInPixel){
				unsigned char buffer = fgetc(fp);
				for (int k = 0; k < byteInPixel && x + k<img->width; k++){
					int index = (buffer >> ((byteInPixel - k - 1)*bih.biBitCount)) & ((2 << (bih.biBitCount - 1)) - 1);
					img->data[y][x + k].r = palette[index].rgbRed;
					img->data[y][x + k].g = palette[index].rgbGreen;
					img->data[y][x + k].b = palette[index].rgbBlue;
				}
			}
			for (int i = 0; i < padding; i++)
				fgetc(fp);
		}
		if (paletteArr != NULL)
			*paletteArr = palette; 
		else 
			free(palette);
	}
	else if(bih.biBitCount==24){
		int padding = ((img->width + 3)&~3) - img->width;
		for (int y = img->height - 1; y >= 0; y--){
			for (int x = 0; x < img->width; x++){
				img->data[y][x].b = fgetc(fp);
				img->data[y][x].g = fgetc(fp);
				img->data[y][x].r = fgetc(fp);
			}
			for (int i = 0; i < padding; i++)fgetc(fp);
		}
	}
	else{
		fclose(fp);
		return IMAGE_FAILURE;
	}
	fclose(fp);
	return IMAGE_SUCCESS;
}
/*
*	@ReadBMP : Save BMP(DIB) file to disk.
		support 24 bpp Bitmap image only
*	@param1 : File path.
*	@param2 : image
*	@param3 : Bit per pixel
*	@param4 : must be NULL.(Not used now)
*	@return : If Success, The return value is 1, If falis, the return value is 0
*/
//[24bit <> 16bit](http://blog.errorstory.net/entry/Bitmap-16bit-rgb565-to-24bit-RGB888)
//(http://berry2.tistory.com/entry/%EB%B9%84%ED%8A%B8%EB%A7%B5-81624-%EB%B9%84%ED%8A%B8-%EC%83%81%ED%98%B8%EB%B3%80%ED%99%98)

int WriteBMP(char* file, IMAGE_TYPE* img,WORD bpp,IMAGE_PIXEL* palette) {
	FILE* fp = NULL;
	BITMAPFILEHEADER bfh = { 0 };
	BITMAPINFOHEADER bih = { 0 };
	//==
	//bpp가 16이하인 2의 멱수이거나 24이여야만 한다.
	if (file == NULL || img == NULL)return IMAGE_FAILURE;
	if (!(((bpp&(bpp - 1) != 0 && bpp <= 16)) || bpp == 24))return IMAGE_FAILURE;
	fp = fopen(file, "wb");
	if (fp == NULL)return IMAGE_FAILURE;
	bfh.bfType = 0x4D42;
	bfh.bfOffBits = 54;
	bih.biSize = 40;
	bih.biWidth = img->width;
	bih.biHeight = img->height;
	bih.biPlanes = 1;
	bih.biBitCount = bpp;
	bih.biSizeImage = (DWORD)(img->width*img->height*((float)bpp / 8));
	bih.biXPelsPerMeter = 0xec4;
	bih.biYPelsPerMeter = 0xec4;
	if (bpp == 24){
		bfh.bfSize = (((img->width * 3 + 3)&~3)*img->height) + 54;
		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
		int padding = ((img->width + 3)&~3) - img->width;
		for (int y = img->height-1; y >=0; y--){
			for (int x = 0; x < img->width; x++){
				fputc(img->data[y][x].b, fp);
				fputc(img->data[y][x].g, fp);
				fputc(img->data[y][x].r, fp);
			}
			for (int i = 0; i < padding; i++)
				fputc('\0',fp);
		}
		fclose(fp);
	}
	else{
		fclose(fp);
		return IMAGE_FAILURE;
	}
	return IMAGE_SUCCESS;
}

#endif