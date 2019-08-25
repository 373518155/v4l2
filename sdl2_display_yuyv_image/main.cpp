#define _CRT_SECURE_NO_WARNINGS

/*
��SDL����ʾ����YUYVͼƬ
*/

#include <stdio.h>
#include <SDL.h>
#include <Windows.h>

// �����ļ�
const char *yuyvFile = "in_640_480.yuyv";
// YUYVͼ��Ŀ��
const int W = 640;
// YUYVͼ��ĸ߶�
const int H = 480;

#define MB (1024 * 1024)
#define BUFFER_SIZE (1 * MB)

// ����Buffer
BYTE buffer[BUFFER_SIZE];

/*
��ȡ�ļ���С
���ʧ�ܣ�����-1
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
�����ļ����ݵ�buffer
���ʧ�ܣ�����-1
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

	// ����ͼ������������Ĭ��������������ë��
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	SDL_Window* window = SDL_CreateWindow("YUYV Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
	SDL_Renderer* render = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture* texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_YUY2, SDL_TEXTUREACCESS_STREAMING, W, H);

	while (true)
	{
		int e = SDL_UpdateTexture(texture, NULL, buffer, W * 2); // ��4��������ʾÿ�ж��ٸ��ֽڣ�����2����Ϊÿ������ռ��2���ֽ�

		//SDL_RenderClear(render);
		SDL_RenderCopy(render, texture, NULL, NULL);
		SDL_RenderPresent(render);
		Sleep(40);
	}

	return 0;
}

