## CL_FFMpeg

pod file 添加 cl_ffmpeg 

然后项目类中 添加头文件 #import <CL_FFmpeg_Framework/CL_FFmpeg_Framework.h>

就可以使用了如下：

    NSString *inputUrl = [[NSBundle mainBundle] pathForResource:@"sc_bg_video_music_1" ofType:@"mp3"];
   
    NSString *filePath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
   
   
    NSString *outPath = [NSString stringWithFormat:@"%@/out.wav",filePath];
    
    
    NSString *commandStr = [NSString stringWithFormat:@"ffmpeg -i %@ %@",inputUrl,outPath];
    
    
    [[FFmpegManager sharedManager] converWithCommandStr:commandStr processBlock:^(float process) {
        NSLog(@"导出音频wav进度：%f",process);
        _logLabel.text = [NSString stringWithFormat:@"导出音频wav进度：%f",process];
    } completionBlock:^(NSError *error) {
        if (error == nil) {
            _logLabel.text = [NSString stringWithFormat:@"转换完成:%@",outPath];
        } else {
            NSLog(@"%@",error);
            _logLabel.text = [NSString stringWithFormat:@"转换出错:%@",error];
            
        }
    }];