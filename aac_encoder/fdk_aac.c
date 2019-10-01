#include<stdio.h>
#include<fdk-aac/aacenc_lib.h>

typedef struct _AACContext {
    int input_ch;
	int aacenc_type;	//HE-AAC
	int sbr_enable;		//- -1: Use ELD SBR auto configurator (default). 0: Disable Spectral Band Replication. 1: Enable Spectral Band Replication.
	int sampleRate;		/*!< Audio input data sampling rate.8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000, 88200, 96000 */
	int enc_ch;			//Channel mode must match with number of input channels.
	int input_ch_order;	/*!< Input audio data channel ordering scheme:
						  - 0: MPEG channel ordering (e. g. 5.1: C, L, R, SL, SR, LFE). (default)
						  - 1: WAVE file format channel ordering (e. g. 5.1: L, R, C, LFE, SL, SR). */
	int bit_rate_mode;	/*!< Bitrate mode. Configuration can be different kind of bitrate configurations:
						  - 0: Constant bitrate, use bitrate according to ::AACENC_BITRATE. (default)
							   Within none LD/ELD ::AUDIO_OBJECT_TYPE, the CBR mode makes use of full allowed bitreservoir.
							   In contrast, at Low-Delay ::AUDIO_OBJECT_TYPE the bitreservoir is kept very small.
						  - 8: LD/ELD full bitreservoir for packet based transmission. */
	int bit_rate;		
	int aac_format;		//0-raw 2-adts
	
	HANDLE_AACENCODER 	handle;
} AACContext;

#define FRAME_LEN (2*2*1024)

int aac_encode_init(AACContext *paac_context)
{
	AACENC_InfoStruct info = { 0 };
	if(!paac_context)
		return -1;
	printf("fdk-aac encode param:\n");
	printf("input channel: %d\n", paac_context->input_ch);
	printf("encode type: %d\n", paac_context->aacenc_type);
	printf("sample Rate: %d\n", paac_context->sampleRate);
	printf("encode channel: %d\n", paac_context->enc_ch);
	printf("bit_rate: %d\n", paac_context->bit_rate);
	printf("aac_format: %d\n", paac_context->aac_format);
	
    if (aacEncOpen(&paac_context->handle, 0, paac_context->input_ch) != AACENC_OK) {
		printf("Unable to open fdkaac encoder\n");
		goto error;
	}
 
	if (aacEncoder_SetParam(paac_context->handle, AACENC_AOT, paac_context->aacenc_type) != AACENC_OK) {  //aac lc MPEG-4 AAC Low Complexity.
		printf("Unable to set the AOT\n");
		goto error;
	}
	
	if (aacEncoder_SetParam(paac_context->handle, AACENC_SBR_MODE, paac_context->sbr_enable) != AACENC_OK) { 
		printf("Unable to set the SBR MODE enable\n");
		goto error;
	}
	
	if (aacEncoder_SetParam(paac_context->handle, AACENC_SAMPLERATE, paac_context->sampleRate) != AACENC_OK) {
		printf("Unable to set the AOT\n");
		goto error;
	}
	if (aacEncoder_SetParam(paac_context->handle, AACENC_CHANNELMODE, paac_context->enc_ch) != AACENC_OK) {  // 2 channle
		printf("Unable to set the channel mode\n");
		goto error;
	}
	
	if (aacEncoder_SetParam(paac_context->handle, AACENC_CHANNELORDER, paac_context->input_ch_order) != AACENC_OK) {  // 2 channle
		printf("Unable to set the channel order\n");
		goto error;
	}
	
	if (aacEncoder_SetParam(paac_context->handle, AACENC_BITRATEMODE, paac_context->bit_rate_mode) != AACENC_OK) {  // 2 channle
		printf("Unable to set the bit_rate_mode\n");
		goto error;
	}
	
	if (aacEncoder_SetParam(paac_context->handle, AACENC_BITRATE, paac_context->bit_rate) != AACENC_OK) {
		printf("Unable to set the bitrate\n");
		goto error;
	}
	if (aacEncoder_SetParam(paac_context->handle, AACENC_TRANSMUX, paac_context->aac_format) != AACENC_OK) { //0-raw 2-adts
		printf("Unable to set the ADTS transmux\n");
		goto error;
	}
 
	if (aacEncEncode(paac_context->handle, NULL, NULL, NULL, NULL) != AACENC_OK) {
		printf("Unable to initialize the encoder\n");
		goto error;
	}
	
	if (aacEncInfo(paac_context->handle, &info) != AACENC_OK) {
		printf("Unable to get the encoder info\n");
		goto error;
	}
	
	return 0;
	
error:
	aacEncClose(&paac_context->handle);
	return -1;
}

int aac_encode_frame(FILE * outstream, void *pData, unsigned int insize, AACContext *paac_context)
{
	if(!outstream||!pData||!paac_context)
		return -1;
	
	unsigned char m_aacOutbuf[FRAME_LEN];
	
	AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
	AACENC_InArgs in_args = { 0 };
	AACENC_OutArgs out_args = { 0 };
	
	int in_identifier = IN_AUDIO_DATA;
	int in_elem_size = 2;  
	in_args.numInSamples = insize/2;  				//size为pcm字节数
	in_buf.numBufs = 1;
	in_buf.bufs = &pData;  							//pData为pcm数据指针
	in_buf.bufferIdentifiers = &in_identifier;
	in_buf.bufSizes = &insize;
	in_buf.bufElSizes = &in_elem_size;
 
	int out_identifier = OUT_BITSTREAM_DATA;
	void *out_ptr = m_aacOutbuf;
	unsigned int out_size = sizeof(m_aacOutbuf);
	int out_elem_size = 1;
	out_buf.numBufs = 1;
	out_buf.bufs = &out_ptr;
	out_buf.bufferIdentifiers = &out_identifier;
	out_buf.bufSizes = &out_size;
	out_buf.bufElSizes = &out_elem_size;
 
	if ((aacEncEncode(paac_context->handle, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK) {
		printf("Encoding aac failed\n");
		return -2;
	}
	if (out_args.numOutBytes == 0)
	{
		printf("Encoding aac out size is 0\n");
		return -3;
	}
	fwrite(m_aacOutbuf, 1, out_args.numOutBytes, outstream);	//编码后的aac数据存在outbuf中，大小为out_args.numOutBytes

	return 0;
}

void main(int argc, char *argv[])
{
	FILE * out_stream, *in_stream;
	unsigned char m_aacInbuf[FRAME_LEN];
	unsigned int m_aacInsize;
	AACContext aac_context;
	//unsigned int i = 0;
	
	//init encoder param
	aac_context.input_ch 		= 2;
	aac_context.sbr_enable  	= 0;
	aac_context.aacenc_type 	= 2;
	aac_context.sampleRate 		= 44100;
	aac_context.enc_ch 			= MODE_2;
	aac_context.input_ch_order	= 1;
	aac_context.bit_rate_mode	= 0;
	aac_context.bit_rate 		= 128*1024;	
	aac_context.aac_format 		= 2;	
	
	out_stream = fopen("./out.aac","w");
	in_stream  = fopen("./little_signed_16bit_2channel_44100hz.pcm","r");
	
	if((!out_stream)||(!in_stream))
		return;
	
	if(aac_encode_init(&aac_context)<0)
		goto closefile;
	
	while(1)
	{
		m_aacInsize = fread(m_aacInbuf,1, FRAME_LEN, in_stream);
		if(m_aacInsize == 0)
			break;
		aac_encode_frame(out_stream, (void *)m_aacInbuf, m_aacInsize, &aac_context);
		//printf("frame%d  size:%d\t", i++, m_aacInsize);
	}
	
closefile:
	
	fclose(in_stream);
	fclose(out_stream);
}
