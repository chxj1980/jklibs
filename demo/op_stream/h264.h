#ifndef __H264_H__
#define __H264_H__

typedef struct _video_format_t
{
	unsigned int i_width;
	unsigned int i_height;
	unsigned int i_sar_num;
	unsigned int i_sar_den;
} video_format_t;

typedef struct _block_t
{
	char     *p_buffer;
	int      i_buffer;
} block_t;

video_format_t spsparser(block_t*);

#endif