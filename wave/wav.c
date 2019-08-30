/*
 * 音频格式简介和PCM转换成WAV
 * https://blog.csdn.net/u010126792/article/details/86493494
 * PCM格式文件中不包含头部信息，播放器无法知道采样率，声道数，采样位数，音频数据大小等信息，导致无法播放。
 * WAV格式全称为WAVE，只需要在PCM文件的前面添加WAV文件头，就可以生成WAV格式文件。
 * WAV符合 RIFF Resource Interchange File Format规范，RIFF文件结构可以看作是树状结构,其基本构成是称为"块"（Chunk）的单元，WAVE文件是由
 * 若干个Chunk组成的。WAV文件本身由三个“块”信息组成：将文件标识为WAV文件的RIFF块，识别采样率等参数的FORMAT块和包含实际数据（样本）的DATA块。
 * 所有的WAV都有一个文件头，这个文件头记录着音频流的编码参数。数据块的记录方式是little-endian字节顺序。
 */



#include <stdio.h>

#pragma pack(1) // 让编译器做1字节对齐
typedef struct
{
    WAVEFORMAT wf;//波形格式；
    WORD wBitsPerSample;//WAVE文件的采样大小；
}PCMWAVEFORMAT;

// WAVEFORMAT结构定义如下：
typedef struct
{
    WORD wFormatag;//编码格式，包括WAVE_FORMAT_PCM，WAVEFORMAT_ADPCM等
    WORD nChannls;//声道数，单声道为1，双声道为2;
    DWORD nSamplesPerSec;//采样频率；
    DWORD nAvgBytesperSec;//每秒的数据量；
    WORD nBlockAlign;//块对齐；
}WAVEFORMAT;

#pragma pack() // 取消1字节对齐，恢复为默认对齐

