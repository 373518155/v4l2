/*
在SDL中显示YUYV视频
*/

#include <stdio.h>
#include <SDL.h>
#include <unistd.h>

// 输入文件
const char *yuyvFile = "vid_1565629469_640x480_480frames.yuyv";
// YUYV图像的宽度
const int W = 640;
// YUYV图像的高度
const int H = 480;
// 每一帧的大小
const int FRAME_SIZE = W * H * 2;
// 输入Buffer
unsigned char* buffer[FRAME_SIZE];

/*
获取文件大小
如果失败，返回-1
*/
long getFileSize(const char *filename) {
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
加载某一帧的数据到buffer中
如果失败，返回-1
 @param filename YUYV文件名
 @param frameIndex  第几帧，从0开始
*/
int loadFrameData(const char* filename, int frameIndex) {
	long filesize = getFileSize(filename);
	if (filesize == -1) {
		return -1;
	}

	FILE* fp = fopen(filename, "rb");
	fseek(fp, FRAME_SIZE * frameIndex, SEEK_SET);
	fread(buffer, FRAME_SIZE, 1, fp);
	fclose(fp);

	return FRAME_SIZE;
}

int main(int argc, char* argv[])
{
	long filesize = getFileSize(yuyvFile);
	printf("filesize[%ld]\n", filesize);

	int frameCount = filesize / FRAME_SIZE;
	printf("frameCount[%d]\n", frameCount);

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

	// 当前帧下标
	int frameIndex = 0;
	while (true)
	{
	    printf("frameIndex[%d]\n", frameIndex);
	    // 加载帧数据
	    loadFrameData(yuyvFile, frameIndex);
		int e = SDL_UpdateTexture(texture, NULL, buffer, W * 2); // 第4个参数表示每行多少个字节，乘以2是因为每个像素占用2个字节

		//SDL_RenderClear(render);
		SDL_RenderCopy(render, texture, NULL, NULL);
		SDL_RenderPresent(render);
		// 每40毫秒显示一帧
		usleep(40 * 1000);
		frameIndex = (frameIndex + 1) % frameCount;
	}

	return 0;
}

