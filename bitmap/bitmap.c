#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;


#define BYTES_PER_PIXEL 3

#pragma pack(1) // 让编译器做1字节对齐
/**
 * 文件头,大小:14字节
 * https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
 */
typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;


/**
 * 信息头，大小:40字节
 * https://docs.microsoft.com/en-us/previous-versions//dd183376(v=vs.85)
 */
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

#pragma pack() // 取消1字节对齐，恢复为默认对齐


/*
 *  * append [len] bytes [data] to [filename] at the end of file, if the spcified file is not exist, this function will create it
 *  * return [len] on success, otherwise return a negative integer
 *  * */

int appenddata(const char *filename, const void *data, int len)
{
    if (len <= 0)
    {
        return len;
    }


    /* open for writing data */
    FILE *fp = fopen(filename, "ab");
    if (!fp)
    {
        return -1;
    }


    if (1 != fwrite(data, len, 1, fp))
    {
        fclose(fp);
        return -2;
    }

    fclose(fp);
    return len;

}


int create_bitmap24(const char *filename, int width, int height, unsigned char *buffer) {
    BITMAPFILEHEADER fileHeader;
    memset(&fileHeader, 0x00, sizeof(BITMAPFILEHEADER));

    char *pch = (char *) &fileHeader.bfType;
    pch[0] = 'B';
    pch[1] = 'M';

    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;

    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + BYTES_PER_PIXEL * width * height;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER infoHeader;
    memset(&infoHeader, 0x00, sizeof(BITMAPINFOHEADER));
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;  // RGB24
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = BYTES_PER_PIXEL * width * height;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrUsed = 0;

    // 先删除旧文件
    unlink(filename);

    int filesize = 0;
    filesize += appenddata(filename, &fileHeader, sizeof(fileHeader));
    filesize += appenddata(filename, &infoHeader, sizeof(infoHeader));
    filesize += appenddata(filename, buffer, BYTES_PER_PIXEL * width * height);
    return filesize;
}


int main(int argc, const char **argv) {
    printf("sizeof(BITMAPFILEHEADER) = %d\n", sizeof(BITMAPFILEHEADER));
    printf("sizeof(BITMAPINFOHEADER) = %d\n", sizeof(BITMAPINFOHEADER));

    int width = 64;
    int height = 32;
    int pixel_count = width * height;
    unsigned char *buffer = (unsigned char *) malloc(BYTES_PER_PIXEL * pixel_count);

    unsigned char r = 0x20, g = 0x40, b = 0x80;

    for (int i = 0; i < pixel_count; i++) {
        buffer[BYTES_PER_PIXEL * i] = b;
        buffer[BYTES_PER_PIXEL * i + 1] = g;
        buffer[BYTES_PER_PIXEL * i + 2] = r;
    }

    int filesize = create_bitmap24("./sample.bmp", width, height, buffer);
    printf("filesize[%d]\n", filesize);
    free(buffer);
    return 0;
}


