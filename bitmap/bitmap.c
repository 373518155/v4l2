/**
 * YUYV转RGB24参考
 *
 * YUYV码流中提取单帧并转为RGB图片
 * https://blog.csdn.net/willib/article/details/51865514
 */

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


/**
 * 单点yuv转rgb
 * @param y
 * @param u
 * @param v
 * @return
 */
int convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}

/**
 * YUV图像转RGB图像
 * @param yuv
 * @param rgb
 * @param width
 * @param height
 * @return RGB数据占用的字节数
 */
int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) { // 每次循环处理2个像素点(即一个宏像素)，每个像素点占用2字节，所以in变量递增4
        pixel_16 =
                yuv[in + 3] << 24 |
                yuv[in + 2] << 16 |
                yuv[in + 1] <<  8 |
                yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);   // y0和y1共用一对uv
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);   // y0和y1共用一对uv
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return out;
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


