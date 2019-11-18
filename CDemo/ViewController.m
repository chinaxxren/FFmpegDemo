//
//  ViewController.m
//  Demo
//
//  Created by Jiangmingz on 2016/6/30.
//  Copyright © 2016年 Jiangmingz. All rights reserved.
//

#import "ViewController.h"

#import "TableViewCell.h"

#import "metadata.h"
#import "decode_audio.h"
#import "decode_video.h"
#import "demuxing_decoding.h"

@interface ViewController ()

@end

@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];

    [self.tableView registerClass:[TableViewCell class] forCellReuseIdentifier:[TableViewCell identifier]];
}

#pragma mark - Table view data source

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 10;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:[TableViewCell identifier] forIndexPath:indexPath];
    NSInteger row = indexPath.row;
    if(row == 0) {
        cell.textLabel.text = @"metadata";
    } else if (row == 1) {
        cell.textLabel.text = @"decode audio";
    } else if (row == 2) {
        cell.textLabel.text = @"decode video";
    } else if (row == 3) {
        cell.textLabel.text = @"demuxing decoding";
    } else if (row == 4) {
        cell.textLabel.text = @"将 avframe 转成 CIImage，使用 GLKView 渲染";
    } else if (row == 5) {
        cell.textLabel.text = @"将 avframe 转成 CMSampleBufferRef，使用 AVSampleBufferDisplayLayer 渲染，60fps";
    } else if (row == 6) {
        cell.textLabel.text = @"使用 AudioUnit 渲染音频";
    } else if (row == 7) {
        cell.textLabel.text = @"使用 AudioQueue 渲染音频";
    } else if (row == 8) {
        cell.textLabel.text = @"将 FFmpeg 升级到 3.x 版本";
    } else {
        cell.textLabel.text = @"结束";
    }
    
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    NSInteger row = indexPath.row;
    if(row == 0) {
        [self metadata];
    } else if (row == 1) {
        [self decodeAudio];
    } else if (row == 2) {
        [self decodeVideo];
    } else if (row == 3) {
        [self demuxingDecoding];
    } else if (row == 4) {
        
    } else if (row == 5) {
        
    } else if (row == 6) {
        
    } else if (row == 7) {
        
    } else if (row == 8) {
        
    } else {
        
    }
    
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}

- (void)metadata {
    NSString *path = [[NSBundle mainBundle] pathForResource:@"m4" ofType:@"mp4"];
    const char *op = [path cStringUsingEncoding:NSUTF8StringEncoding];
    char  *argv[] = {"do",(char *)op};
    do_metadata(2, argv);
}

- (void)decodeAudio {

    NSString *path = [[NSBundle mainBundle] pathForResource:@"morning-mono-16k" ofType:@"mp3"];
    const char *op = [path cStringUsingEncoding:NSUTF8StringEncoding];
        
    NSString *audio_path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"decode_audio.pcm"];
    NSFileManager *manager = [NSFileManager defaultManager];
    [manager createFileAtPath:audio_path contents:nil attributes:nil];
    
    const char *np = [audio_path cStringUsingEncoding:NSUTF8StringEncoding];
    char  *argv[] = {"do",(char *)op,(char *)np};
    do_decode_audio(3, argv);
}

- (void)decodeVideo {

    NSString *path = [[NSBundle mainBundle] pathForResource:@"soccor" ofType:@"h264"];
    const char *op = [path cStringUsingEncoding:NSUTF8StringEncoding];
    
    NSString *vido_path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"decode_vidoe.yuv"];
    NSFileManager *manager = [NSFileManager defaultManager];
    [manager createFileAtPath:vido_path contents:nil attributes:nil];
    
    const char *np = [vido_path cStringUsingEncoding:NSUTF8StringEncoding];
    char  *argv[] = {"do",(char *)op,(char *)np};
    do_decode_video(3, argv);
}

- (void)demuxingDecoding {
    NSString *path = [[NSBundle mainBundle] pathForResource:@"m4" ofType:@"mp4"];
    const char *op = [path cStringUsingEncoding:NSUTF8StringEncoding];
    
     open_input_file(op);
    
    NSFileManager *manager = [NSFileManager defaultManager];
    
    NSString *vido_path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"demux_vidoe.yuv"];
    [manager createFileAtPath:vido_path contents:nil attributes:nil];
    const char *vp = [vido_path cStringUsingEncoding:NSUTF8StringEncoding];
    
    NSString *audio_path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"demux_vidoe.pcm"];
    [manager createFileAtPath:audio_path contents:nil attributes:nil];
    const char *ap = [audio_path cStringUsingEncoding:NSUTF8StringEncoding];
    
    char  *argv[] = {"do",(char *)op,(char *)vp,(char *)ap};
    do_dumuxing_decode(4, argv);
}

@end

