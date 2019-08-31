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
} PCMWAVEFORMAT;

// WAVEFORMAT结构定义如下：
typedef struct
{
    WORD wFormatag;//编码格式，包括WAVE_FORMAT_PCM，WAVEFORMAT_ADPCM等
    WORD nChannls;//声道数，单声道为1，双声道为2;
    DWORD nSamplesPerSec;//采样频率；
    DWORD nAvgBytesperSec;//每秒的数据量；
    WORD nBlockAlign;//块对齐；
} WAVEFORMAT;


// WAV文件头信息由44个字节组成，所以只需要在PCM文件头部添加44个字节的WAV文件头，就可以生成WAV格式文件。
typedef struct {
    BYTE ChunkID[4]; // offset: 00H, 大小为4个字节数据，内容为“RIFF”，表示资源交换文件标识
    DWORD ChunkSize; // offset: 04H, 大小为4个字节数据，内容为一个整数，表示从下个地址开始到文件尾的总字节数(由于从后面的开始算，所以文件大小为本数值加上8)
    BYTE Format[4]; // offset: 08H, 大小为4个字节数据，内容为“WAVE”，表示WAV文件标识
    BYTE Subchunk1_ID[4]; // offset: 0CH, 大小为4个字节数据，内容为“fmt ”，表示波形格式标识（fmt ），最后一位空格。
    DWORD Subchunk1_Size; // offset: 10H, 大小为4个字节数据，内容为一个整数，表示PCMWAVEFORMAT的长度。
    WORD AudioFormat; // offset: 14H, 大小为2个字节数据，内容为一个短整数，表示格式种类（值为1时，表示数据为线性PCM编码）
    WORD NumChannels; // offset: 16H, 大小为2个字节数据，内容为一个短整数，表示通道数，单声道为1，双声道为2
    DWORD SampleRate; // offset: 18H, 大小为4个字节数据，内容为一个整数，表示采样率，比如44100
    DWORD ByteRate; // offset: 1CH, 大小为4个字节数据，内容为一个整数，表示波形数据传输速率（每秒平均字节数），大小为 采样率 * 通道数 * 采样位数 / 8
    WORD BlockAlign; // offset: 20H, 大小为2字节数据，内容为一个短整数，表示DATA数据块长度，大小为 通道数 * 采样位数 / 8
    WORD BitsPerSample; // offset: 22H, 大小为2个字节数据，内容为一个短整数，表示采样位数，即PCM位宽，通常为8位或16bit
    BYTE Subchunk2_ID[4];  // offset: 24H, 大小为4个字节数据，内容为“data”，表示数据标记符
    DWORD Subchunk2_Size;  // offset: 28H, 大小为4个字节数据，内容为一个整数，表示接下来声音数据的总大小，需要减去头部的44个字节。
    // BYTE Data[n];  //  offset: 2CH, 实际的声音数据
} WAVE_HEADER;

#pragma pack() // 取消1字节对齐，恢复为默认对齐

