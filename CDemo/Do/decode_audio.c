//
//  decode_audio.c
//  CDemo
//
//  Created by 赵江明 on 2019/11/15.
//  Copyright © 2019 赵江明. All rights reserved.
//

#include "decode_audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILE_SIZE 4096

static int get_format_from_sample_fmt(const char **fmt, enum AVSampleFormat sample_fmt) {
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt;
        const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
            {AV_SAMPLE_FMT_U8, "u8", "u8"}, ///< unsigned 8 bits
            {AV_SAMPLE_FMT_S16, "s16be", "s16le"},///< signed 16 bits
            {AV_SAMPLE_FMT_S32, "s32be", "s32le"},///< signed 32 bits
            {AV_SAMPLE_FMT_FLT, "f32be", "f32le"},///< float
            {AV_SAMPLE_FMT_DBL, "f64be", "f64le"},///< double
    };

    /*
     讲一下AV_SAMPLE_FMT_S16和AV_SAMPLE_FMT_S16P格式，AV_SAMPLE_FMT_S16保存一个样本采用有符号16bit交叉存储的方式，
     AV_SAMPLE_FMT_S16P保存一个样本采用有符号16bit平面存储的方式。举例有两个通道，通道1数据流 c1 c1 c1c1... ,
     通道2数据流 c2 c2 c2 c2...
     */
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); ++i) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    return -1;
}
/*
static void decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, FILE *outfile) {
    int i,ch;
    int ret , data_size;
    
    // 将AVPacket压缩数据给解码器
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error submiting the packet to the decoder\n");
        exit(1);
    }
    
    while (ret >= 0) {
        // 获取到解码后的AVFrame数据
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        
        //  获取每个sample中的字节数
        data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        if (data_size < 0) {
            fprintf(stderr, "Failed to caculate data size\n");
            exit(1);
        }
        
        for (i = 0; i < frame->nb_samples; i++) {
            for (ch = 0; ch < dec_ctx->channels; ch++) {
                fwrite(frame->data[ch] + data_size * 1, 1, data_size, outfile);
            }
        }
    }
}
*/

static void decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame,FILE *outfile) {
    int i, ch;
    int ret, data_size;
    int got_frame_ptr = 0;
    
    ret = avcodec_decode_audio4(dec_ctx, frame, &got_frame_ptr, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error avcodec_decode_audio4 \n");
        return;
    }

    if (ret == AVERROR(EAGAIN)) {
        return;
    }
    
    if(ret == AVERROR_EOF) {
        fprintf(stdout, "file is eof\n");
        return;
    }
    
    data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
    if (data_size < 0) {
        /* This should not occur, checking just for paranoia */
        fprintf(stderr, "Failed to calculate data size\n");
        exit(1);
    }
    
    if (got_frame_ptr > 0)
    {
        //data[0] = 左声道L data[1] = 右声道R
        //nb_samples：音频的一个AVFrame中可能包含多个音频帧，在此标记包含了几个
        
        for (i = 0; i < frame->nb_samples; i++)
            for (ch = 0; ch < dec_ctx->channels; ch++)
                fwrite(frame->data[ch] + data_size*i, 1, data_size, outfile);
    }
}

int do_decode_audio(int argc, char **argv) {
    const char *outfilename, *filename;
    const AVCodec *codec;
    AVCodecContext *c = NULL;
    
    // 用于解析输入的数据流并把它分成一帧一帧的压缩编码数据。比较形象的说法就是把长长的一段连续的数据“切割”成一段段的数据
    AVCodecParserContext *parser = NULL;
    int len, ret;
    FILE *infile, *outfile;
    uint8_t intbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_MIN_SIZE];
    uint8_t *data;
    size_t data_size;
    AVPacket *pkt;
    AVFrame *decoded_frame = NULL;
    enum AVSampleFormat sfmt;
    int n_channels = 0;
    const char *fmt;

    if (argc < 2) {
        fprintf(stderr, "%s", argv[0]);
        exit(0);
    }

    filename = argv[1];
    outfilename = argv[2];

    pkt = av_packet_alloc();
    
    // 根据指定的AVCodecID查找注册的解码器
    codec = avcodec_find_decoder(AV_CODEC_ID_MP3);
    if (!codec) {
        fprintf(stderr, "Codec not find\n");
        exit(1);
    }
    
    // 初始化AVCodecParserContext
    parser = av_parser_init(codec->id);
    if (!parser) {
        fprintf(stderr, "Parser not found\n");
        exit(1);
    }
    
    // 为AVCodecContext分配内存
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Counld not allocate audio codec context\n");
        exit(1);
    }
    
    // 打开解码器。
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Counld not open codec\n");
        exit(1);
    }
    
    infile = fopen(filename, "rb");
    if (!infile) {
        fprintf(stderr, "Counld not open %s",filename);
        exit(1);
    }
    
    outfile = fopen(outfilename, "wb");
    if (!outfile) {
        av_free(c);
        exit(1);
    }
    
    data = intbuf;
    data_size = fread(intbuf, 1, AUDIO_INBUF_SIZE, infile);
    
    while (data_size > 0) {
        if(!decoded_frame) {
            if(!(decoded_frame = av_frame_alloc())) {
                fprintf(stderr, "Counld not allocate frame\n");
            }
        }
        
        // 解析获得一个Packet
        ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
        if (ret < 0) {
            fprintf(stderr, "Error while parsing\n");
            exit(1);
        }
        
        data += ret;
        data_size -= ret;
        
        if (pkt->size) {
            decode(c,pkt, decoded_frame, outfile);
        }
        
        if (data_size < AUDIO_REFILE_SIZE) {
            memmove(intbuf, data, data_size);
            data = intbuf;
            len = fread(data + data_size, 1, AUDIO_INBUF_SIZE - data_size, infile);
            if (len > 0) {
                data_size += len;
            }
        }
    }
    
    pkt->data = NULL;
    pkt->size = 0;
    decode(c, pkt, decoded_frame, outfile);
    
    sfmt = c->sample_fmt;
    
    if (av_sample_fmt_is_planar(sfmt)) {
        const char *packed = av_get_sample_fmt_name(sfmt);
        sfmt = av_get_packed_sample_fmt(sfmt);
    }
    
    n_channels = c->channels;
    if((ret = get_format_from_sample_fmt(&fmt, sfmt))<0) {
        goto end;
    }
    
end:
    fclose(outfile);
    fclose(infile);
    
    avcodec_free_context(&c);
    av_parser_close(parser);
    av_frame_free(&decoded_frame);
    av_packet_free(&pkt);
    
    return 0;
}
