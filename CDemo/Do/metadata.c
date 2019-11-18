//
//  metadata.c
//  CDemo
//
//  Created by 赵江明 on 2019/11/18.
//  Copyright © 2019 赵江明. All rights reserved.
//

#include "metadata.h"

#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavutil/dict.h>

int do_metadata(int argc, char **argv) {
    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *tag = NULL;
    int ret;

    if (argc != 2) {
        printf("usage: %s <input_file>\n"
               "example program to demonstrate the use of the libavformat metadata API.\n"
               "\n", argv[0]);
        return 1;
    }

    if ((ret = avformat_open_input(&fmt_ctx, argv[1], NULL, NULL)))
        return ret;

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
        printf("%s=%s\n", tag->key, tag->value);

    avformat_close_input(&fmt_ctx);
    
    return 0;
}


int open_input_file(const char* filename) {
    AVFormatContext *ifmt_ctx = NULL;
    int ret = 0;
    
    //open the input
    if ((ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0) {
        printf("can not open input");
        return ret;
    }
    
    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL))) {
        printf("can not find input stream info");
        return ret;
    }

    //open the decoder
    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVCodecParameters *codec_param = ifmt_ctx->streams[i]->codecpar;
        if (codec_param->codec_type == AVMEDIA_TYPE_AUDIO) {
            return codec_param->codec_id;
        }
    }

    return 0;
}
