#define _CRT_SECURE_NO_WARNINGS

/*
在SDL中显示单张YUYV图片
*/

#include <stdio.h>
#include <SDL.h>
#include <Windows.h>

// 输入文件
const char *yuyvFile = "in_640_480.yuyv";
// YUYV图像的宽度
const int W = 640;
// YUYV图像的高度
const int H = 480;

#define MB (1024 * 1024)
#define BUFFER_SIZE (1 * MB)

// 输入Buffer
BYTE buffer[BUFFER_SIZE];

/*
获取文件大小
如果失败，返回-1
*/
long getFileSize(const char *filename) {
	printf("filename[%s]\n", filename);
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {
		return -1;
	}

	fseek(fp, 0, SEEK_END);

	long size = ftell(fp);

	fclose(fp);
	return size;
}

/*
加载文件数据到buffer
如果失败，返回-1
*/
int loadData(const char* filename) {
	long filesize = getFileSize(filename);
	if (filesize == -1) {
		return -1;
	}

	FILE* fp = fopen(filename, "rb");
	fread(buffer, filesize, 1, fp);
	fclose(fp);

	return filesize;
}

int main(int argc, char* argv[])
{
	long filesize = loadData(yuyvFile);
	printf("filesize[%ld]\n", filesize);

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	// 提升图像质量，否则默认缩放质量会有毛剌
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	SDL_Window* window = SDL_CreateWindow("YUYV Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
	SDL_Renderer* render = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture* texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_YUY2, SDL_TEXTUREACCESS_STREAMING, W, H);

	while (true)
	{
		int e = SDL_UpdateTexture(texture, NULL, buffer, W * 2); // 第4个参数表示每行多少个字节，乘以2是因为每个像素占用2个字节

		//SDL_RenderClear(render);
		SDL_RenderCopy(render, texture, NULL, NULL);
		SDL_RenderPresent(render);
		Sleep(40);
	}

	return 0;
}

