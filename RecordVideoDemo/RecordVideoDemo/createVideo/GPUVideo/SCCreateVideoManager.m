//
//  SCCreateVideoManager.m
//  SuperCoach
//
//  Created by 林 on 2018/12/28.
//  Copyright © 2018 Lin Feihong. All rights reserved.
//

#import "SCCreateVideoManager.h"
#import <AVFoundation/AVFoundation.h>
//#import "SCPhotoManager.h"
//#import "BPZipArchive.h"
//#import "CLConvertVideo.h"
//#import "CLVideoRecorder.h"
//#import "UIImage+QRCode.h"

#define SourceIdFilePrefix @"create_video_"
#define ExportAudioMaxCount 3

@implementation SCCreateVideoStuffGifItem
@end

@implementation SCCreateVideoStuffItem

+ (NSDictionary<NSString *,id> *)modelContainerPropertyGenericClass
{
    return @{@"gifList" : [SCCreateVideoStuffGifItem class]};
}

- (NSString *)realMp3FilePath
{
    NSString *randomPath = nil;
    if ([_mp3Path containsString:@"|"]) {
        NSArray *randomArr = [_mp3Path componentsSeparatedByString:@"|"];
        if ([NSArray validArray:randomArr]) {
            randomPath = [randomArr objectAtIndex:arc4random()%randomArr.count];
        }
    }

    if (randomPath.length && _rootPath.length) {
        NSString *path = [_rootPath stringByAppendingPathComponent:randomPath];
        return path;
    }

    if (_rootPath.length && _mp3Path.length) {
        NSString *path = [_rootPath stringByAppendingPathComponent:_mp3Path];
        return path;
    }
    return @"";
}

@end

@interface SCCreateVideoManager()
{
    NSInteger _exportAudioCount;
    NSString *_zipMessage;
//    CLConvertVideo *_exportSession;
    
    NSUInteger _talkRandomIndex;
    NSMutableArray *_talkRandomArr;
}
@end

@implementation SCCreateVideoManager

//- (void)checkIsReserve
//{
//    WEAK_SELF
//    [DTHTTPRequestStack async:[DTSCPhpHTTPRequest sys_app_config:@"videoredbag"] finish:^(DTDataResult *result) {
//        if (result.success) {
//            NSDictionary *dic = result.data;
//            if ([NSDictionary validDict:dic]) {
//                weakSelf.isReserve = [dic boolForKey:@"isChange"];
//            }
//        }
//    }];
//}

+ (SCCreateVideoManager *)sharedInstance
{
    static id instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if (!instance) {
            instance = [[self alloc] init];
        }
    });
    return instance;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _exportAudioCount = 0;
        _coverImageArr = [NSMutableArray array];
        NSArray *arr = [NSArray arrayWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"talk_config" ofType:@"plist"]];
        if (arr.count <= 0) {
            arr = @[@"祝大家新的一年里，财运汹涌，健步如风，事业如虹，晚辈成龙。车轮驾考通，学车特轻松！"];
        }
        _talkArr = [NSMutableArray arrayWithArray:arr];
//        NSArray *configArr = [DTOnlineConfig onlineArrayValue:@"sc_video_talk_config"];
//        if ([NSArray validArray:configArr]) {
//            [_talkArr addObjectsFromArray:arr];
//        }
//
//        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(enterForeground) name:APP_EVENT_CHELUN_ENTER_FOREGROUND object:nil];
    }
    return self;
}

- (void)enterForeground
{
    [self checkIsReserve];
}

//+ (UIImage *)getVideoShareImage
//{
//    UIImageView *imageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"sc_create_video_share_bg"]];
//
//    UIImage *qtImage = [UIImage QRCodeImageFromString:[DTPubUtil addParamsToUrlStr:APP_CONST_CREATE_VIDEO_SHARE_URL key:@"cid" value:[UserManager sharedInstance].coach.encrypt_cid] withSize:73];
//    UIImageView *qrImageView = [[UIImageView alloc] initWithImage:qtImage];
//    qrImageView.frame = CGRectMake(imageView.width/2-qrImageView.width/2-0.5, imageView.height-87-qrImageView.height, qrImageView.width, qrImageView.height);
//    [imageView addSubview:qrImageView];
//
//    UILabel *nameLabel = [UILabel labelWithFrame:CGRectMake(0, 106, imageView.width, 30) fontSize:28 colorString:@"F3DB8B"];
//    nameLabel.textAlignment = NSTextAlignmentCenter;
//    nameLabel.text = [UserManager sharedInstance].coach.name;
//    [imageView addSubview:nameLabel];
//
//    UIImage *retImage = [imageView captureView];
//    return retImage;
//}

- (NSString *)getRandomTalkText
{
    if (!_talkRandomArr.count) {
//        _talkRandomArr = [DTPubUtil getRandomArrFromArr:_talkArr];
        _talkRandomIndex = 0;
    }
    NSString *text = [_talkRandomArr objectAtIndex:_talkRandomIndex];
    _talkRandomIndex += 1;
    if (_talkRandomIndex > _talkRandomArr.count - 1) {
        _talkRandomIndex = 0;
    }
    _curTalk = text;
    return text;
}

- (void)createScreenHotImageArrWithVideoPath:(NSString *)videoPath
{
    if (_coverImageArr == nil) {
        _coverImageArr = [NSMutableArray array];
    }
    [_coverImageArr removeAllObjects];
    AVURLAsset *assert = [AVURLAsset assetWithURL:[NSURL fileURLWithPath:
                              videoPath]];
    if (assert) {
        //视频的总时间
        CMTime time = [assert duration];
        CGFloat second = 1.0*time.value/time.timescale;
        if (second < 0.5) {
            UIImage *cover = [self.class getScreenShotImageFromVideoPath:assert withStart:0 withTimescale:time.timescale];
            [_coverImageArr addObject:cover];
        } else {
            CGFloat coverTime = 2.f;
            while (second >= coverTime) {
                UIImage *cover = [self.class getScreenShotImageFromVideoPath:assert withStart:coverTime withTimescale:time.timescale];
                [_coverImageArr addObject:cover];
                coverTime += 1.5;
            }
        }
    }
}

// 启动就开始后台解压 封面尺寸 320 560
+ (SCCreateVideoStuffItem *)createLocalStuffItemWithSourceId:(NSString *)sourceId
{
    NSString *rootPath = [self getSCResourcePathWithSourceId:sourceId];
    NSString *configPlistPath = [rootPath stringByAppendingPathComponent:@"config.plist"];
    if (configPlistPath.length) {
        NSDictionary *dic = [NSDictionary dictionaryWithContentsOfURL:[NSURL fileURLWithPath:configPlistPath]];
        SCCreateVideoStuffItem *item = [SCCreateVideoStuffItem itemFromDict:dic];
        item.rootPath = rootPath;
        item.sourceId = sourceId;
        return item;
    }
    return nil;
}

+ (void)removeSCAllResourcePath
{
    NSString *filePath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    filePath = [filePath stringByAppendingPathComponent:@"sc_all_resource"];
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    }
}

+ (NSString *)getSCAllResourcePath
{
    NSString *filePath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    filePath = [filePath stringByAppendingPathComponent:@"sc_all_resource"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:filePath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    return filePath;
}

+ (NSString *)getSCResourcePathWithSourceId:(NSString *)sourceId {
    
    if ([sourceId containsString:@"local"]) {
        NSString *sourceIdPath = [NSString stringWithFormat:@"gif_resource/%@%@",SourceIdFilePrefix,sourceId];
        NSString *sourcePath = [[NSBundle mainBundle] pathForResource:@"config" ofType:@"plist" inDirectory:sourceIdPath];
        return [sourcePath stringByDeletingLastPathComponent];
    }
    
    NSString *filePath = [self getSCAllResourcePath];
    NSString *sourceIdPath = [NSString stringWithFormat:@"%@%@",SourceIdFilePrefix,sourceId];
    filePath = [filePath stringByAppendingPathComponent:sourceIdPath];
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        return nil;
    }
    return filePath;
}

- (void)checkResource:(NSString *)sourceId complete:(SCCustomCallBack)block
{
    NSString *sourcePath = [SCCreateVideoManager getSCResourcePathWithSourceId:sourceId];
    if (sourcePath.length) {
        //资源存在
        if (block) {
            block(@YES);
        }
    } else {
//        [DTPubUtil showLoadingWithText:@"正在加载资源"];
//        [DTPubUtil addBlockOnGlobalThread:^{
//            _zipMessage = nil;
//            NSString *sourcePath = [[NSBundle mainBundle] pathForResource:[NSString stringWithFormat:@"%@%@",SourceIdFilePrefix,sourceId] ofType:@"zip" inDirectory:@"gif_resource"];
//            NSString *unzipFilePath = [self.class getSCAllResourcePath];
//            if ([[NSFileManager defaultManager] fileExistsAtPath:sourcePath] && [[NSFileManager defaultManager] fileExistsAtPath:unzipFilePath]) {
//                BPZipArchive *zipArchive = [[BPZipArchive alloc] init];
//                zipArchive.delegate = self;
//                [zipArchive UnzipOpenFile:sourcePath];
//                [zipArchive UnzipFileTo:unzipFilePath overWrite:YES];
//                [zipArchive UnzipCloseFile];
//                [DTPubUtil addBlockOnMainThread:^{
//                    BOOL success = NO;
//                    if (_zipMessage.length) {
//                        [DTPubUtil showHUDErrorHintInWindow:_zipMessage];
//                    } else {
//                        NSString *sourcePath = [SCCreateVideoManager getSCResourcePathWithSourceId:sourceId];
//                        if (sourcePath.length) {
//                            [DTPubUtil stopLoading];
//                            success = YES;
//                        } else {
//                            [DTPubUtil showHUDErrorHintInWindow:@"资源出错，请尝试其他资源"];
//                        }
//                    }
//                    if (block) {
//                        block(@(success));
//                    }
//                }];
//            } else {
//                [DTPubUtil addBlockOnMainThread:^{
//                    [DTPubUtil showHUDErrorHintInWindow:@"资源出错，请尝试其他资源"];
//                    if (block) {
//                        block(@(NO));
//                    }
//                }];
//            }
//        }];
    }
}

//#para mark BPZipArchiveDelegate

-(void) ErrorMessage:(NSString*) msg
{
    _zipMessage = msg;
}

+ (NSString *)getSCAlbumFilePath {
    NSString *filePath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    filePath = [filePath stringByAppendingPathComponent:@"sc_album"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:filePath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    return filePath;
}

+ (void)removeSCAlbumFilePath
{
    NSString *filePath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    filePath = [filePath stringByAppendingPathComponent:@"sc_album"];
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        NSArray *contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:filePath error:NULL];
        if (contents.count) {
            for (NSString *fileName in contents) {
                NSString *path = [filePath stringByAppendingPathComponent:fileName];
                [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
            }
        }
//        [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    }
}

+ (NSString *)getVideoOutPath {
    NSString *videoPath = [self getSCAlbumFilePath];
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyyMMddHHmmss"];
    NSString *fileName = [NSString stringWithFormat:@"%@-%@",[formatter stringFromDate:[NSDate date]], @"video.mp4"];
    videoPath = [videoPath stringByAppendingPathComponent:fileName];
    
    return videoPath;
}

+ (NSString *)recordPath:(NSString *)videoPath suffix:(NSString *)suffix
{
    if (videoPath.length) {
        NSString *recordPath = [videoPath stringByReplacingOccurrencesOfString:@".mp4" withString:suffix];
        return recordPath;
    }
    return nil;
}

//owen mark 会不小失败的概率
- (void)exportAudioWithVideoPath:(NSString *)path completeBlock:(SCCreateVideoManagerBlock)block isTryAgain:(BOOL)isTryAgain
{
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) {
        if (block) {
            block(NO,@"视频文件丢失");
        }
        return;
    }
    
    if (isTryAgain) {
        _exportAudioCount += 1;
    } else {
        _exportAudioCount = 0;
    }
    NSLog(@"导出音频次数：%zd  %@",_exportAudioCount,path);
    
    NSDictionary* options = @{AVURLAssetPreferPreciseDurationAndTimingKey:@YES};
    AVAsset *asset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:path] options:options];
    
    
    AVMutableComposition *composition = [AVMutableComposition composition];
    //音频
    AVMutableCompositionTrack *audioTrack = [composition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
    //获取asset中的音频
    NSArray *audioArray = [asset tracksWithMediaType:AVMediaTypeAudio];
    AVAssetTrack *assetAudio = [audioArray objectAtIndex:0];
    //向audioTrack中加入音频
    NSError *audioError = nil;
    //开始合成
    CMTime totalDuration = kCMTimeZero;
    [audioTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, asset.duration)
                        ofTrack:assetAudio
                         atTime:totalDuration
                          error:&audioError];
    
//    //视频导出处理
    AVAssetExportSession *export = [AVAssetExportSession exportSessionWithAsset:composition
                                                                     presetName:AVAssetExportPresetPassthrough];
    
    NSLog(@"导出音频支持的格式:%@",export.supportedFileTypes);
    export.outputURL = [NSURL fileURLWithPath:[SCCreateVideoManager recordPath:path suffix:@".wav"]];
    export.outputFileType = AVFileTypeWAVE;
//    export.shouldOptimizeForNetworkUse = YES;
    WEAK_SELF
    [export exportAsynchronouslyWithCompletionHandler:^{
        [DTPubUtil addBlockOnMainThread:^{
            if (export.error) {
                NSLog(@"导出音频失败:%@ 状态%zd",export.error,export.status);
                if (_exportAudioCount < ExportAudioMaxCount) {
                    [weakSelf exportAudioWithVideoPath:path completeBlock:block isTryAgain:YES];
                } else {
                    if (block) {
                        block(NO,@"导出音频失败");
                    }
                }
            } else {
                block(YES,@"导出音频成功");
            }
        }];
    }];
}

+ (void)editVideoSynthesizeVieoPath:(NSURL *)assetURL BGMPath:(NSURL *)BGMPath videoVolume:(CGFloat)videoVolume BGMVolume:(CGFloat)BGMVolume needCoachInfo:(BOOL)needCoachInfo coachInfoBottom:(NSNumber *)coachInfoBottom complition:(SCEditVideoManagerBlock)block
{
    //    素材
//    AVAsset *asset = [AVAsset assetWithURL:assetURL];
    NSDictionary* options = @{AVURLAssetPreferPreciseDurationAndTimingKey:@YES};
    AVAsset *asset = [AVURLAsset URLAssetWithURL:assetURL options:options];
    AVAsset *audioAsset = nil;
    if (BGMPath) {
        audioAsset = [AVAsset assetWithURL:BGMPath];
    }
    
    CMTime duration = asset.duration;
    CMTimeRange video_timeRange = CMTimeRangeMake(kCMTimeZero, duration);
    
    //    分离素材
    AVAssetTrack *videoAssetTrack = [[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];//视频素材
    AVAssetTrack *audioAssetTrack = nil;
    if (audioAsset) {
        audioAssetTrack = [[audioAsset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];// 背景音乐音频素材
    }
    //    创建编辑环境
    AVMutableComposition *composition = [[AVMutableComposition alloc]init];
    
    //    视频素材加入视频轨道
    AVMutableCompositionTrack *videoCompositionTrack = [composition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
    [videoCompositionTrack insertTimeRange:video_timeRange ofTrack:videoAssetTrack atTime:kCMTimeZero error:nil];
    
    AVMutableCompositionTrack *audioCompositionTrack = nil;
    if (audioAssetTrack) {
        //    音频素材加入音频轨道
        audioCompositionTrack = [composition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
        [audioCompositionTrack insertTimeRange:video_timeRange ofTrack:audioAssetTrack atTime:kCMTimeZero error:nil];
    }
 
    //    audioCompositionTrack.preferredVolume = 0;
    
    //    加入视频原声
    AVMutableCompositionTrack *originalAudioCompositionTrack = nil;
    AVAssetTrack *originalAudioAssetTrack = [[asset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];
    originalAudioCompositionTrack = [composition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
    [originalAudioCompositionTrack insertTimeRange:video_timeRange ofTrack:originalAudioAssetTrack atTime:kCMTimeZero error:nil];
    
    //是否需要合成教练信息
    AVMutableVideoComposition* videoComp = nil;
//    if ([DTOnlineConfig onlineBoolValue:@"sc_hide_coach_info"]) {
//        needCoachInfo = NO;
//    }
//    if (needCoachInfo) {
//        CGSize renderSize = [videoAssetTrack naturalSize];
//        NSLog(@"renderSize:%@",NSStringFromCGSize(renderSize));
//        if (renderSize.width > 0 && renderSize.height > 0) {
//            videoComp = [AVMutableVideoComposition videoComposition];
//            videoComp.renderSize = renderSize;
//            videoComp.frameDuration = CMTimeMake(1, 30);
//            CALayer *contentLayer = [[CALayer alloc] init];
//
//            SCCoach *coach = [UserManager sharedInstance].coach;
//
//            UIImage *image = [UIImage imageNamed:@"sc_creat_video_coach_info_bg"];
//            UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
//
//            UILabel *nameLabel = [UILabel labelWithFrame:CGRectMake(54, 23, 150, 18) fontSize:17 color:[UIColor whiteColor]];
//            nameLabel.text = coach.name;
//            [imageView addSubview:nameLabel];
//
//            UILabel *bottomLabel = [UILabel labelWithFrame:CGRectMake(nameLabel.left, nameLabel.bottom + 5, nameLabel.width, 18) fontSize:17 color:[UIColor whiteColor]];
//            bottomLabel.text = @"车轮驾考通代言人";
//            [imageView addSubview:bottomLabel];
//
//            UIImage *retImage = [imageView capturedImage];
//
//            contentLayer.contents = (id)retImage.CGImage;
//            CGFloat width = image.size.width*SCREEN_WIDTH/375;
//            CGFloat height = width*image.size.height/image.size.width;
//
//            contentLayer.frame = CGRectMake(renderSize.width/2-width/2 , renderSize.height-(coachInfoBottom?coachInfoBottom.floatValue:50)-height ,width , height);
//
//
//            CALayer *parentLayer = [CALayer layer];
//            CALayer *videoLayer = [CALayer layer];
//            parentLayer.frame = CGRectMake(0, 0, renderSize.width, renderSize.height);
//            videoLayer.frame = CGRectMake(0, 0, renderSize.width, renderSize.height);
//            [parentLayer addSublayer:videoLayer];
//            [parentLayer addSublayer:contentLayer];
//            parentLayer.geometryFlipped = true;//坐标回到左上角
//
//            videoComp.animationTool = [AVVideoCompositionCoreAnimationTool videoCompositionCoreAnimationToolWithPostProcessingAsVideoLayer:videoLayer inLayer:parentLayer];
//
//            //处理视频方向
//            AVMutableVideoCompositionInstruction* instruction = [AVMutableVideoCompositionInstruction videoCompositionInstruction];
//
//            instruction.timeRange = CMTimeRangeMake(kCMTimeZero, duration);
//            AVMutableVideoCompositionLayerInstruction* layerInstruction = [AVMutableVideoCompositionLayerInstruction videoCompositionLayerInstructionWithAssetTrack:videoAssetTrack];
//
//            instruction.layerInstructions = [NSArray arrayWithObjects:layerInstruction, nil];
//            videoComp.instructions = [NSArray arrayWithObject: instruction];
//        }
//    }
    
    //    创建导出素材类
    AVAssetExportSession *exporter = [[AVAssetExportSession alloc]initWithAsset:composition presetName:AVAssetExportPresetHighestQuality];
    //    设置输出路径
    NSURL *outputPath = [self exporterPath];
    exporter.outputURL = outputPath;
    exporter.outputFileType = AVFileTypeMPEG4;//指定输出格式
    exporter.shouldOptimizeForNetworkUse= YES;
    //    音量控制
    if (audioCompositionTrack) {
        exporter.audioMix = [self buildAudioMixWithVideoTrack:originalAudioCompositionTrack VideoVolume:videoVolume BGMTrack:audioCompositionTrack BGMVolume:BGMVolume atTime:kCMTimeZero];
    }
    if (videoComp) {
        exporter.videoComposition = videoComp;
    }
    [exporter exportAsynchronouslyWithCompletionHandler:^{
        [DTPubUtil addBlockOnMainThread:^{
            if (block) {
                if (exporter.error) {
                    block(NO,nil,@"合成背景音乐失败");
                } else {
                    block(YES,outputPath.path,@"成功合成背景音乐");
                }
            }
        }];
    }];
}

//+ (void)editVideoSynthesizeComplete:(NSString *)newFilePath handler:(SCEditVideoManagerBlock)handler
//{
//    [[SCCreateVideoManager sharedInstance] exportAsynchronouslyWithCompletionHandler:^(BOOL success, NSString *outPath, NSString *error) {
//        if (handler) {
//            handler(success,outPath,error);
//        }
//    } withVideoPath:newFilePath];
//}

//压缩视频
//- (void)exportAsynchronouslyWithCompletionHandler:(SCEditVideoManagerBlock)handler withVideoPath:(NSString *)videoPath
//{
//    NSDictionary* options = @{AVURLAssetPreferPreciseDurationAndTimingKey:@YES};
//    AVAsset *videoAsset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:videoPath] options:options];
////    AVAsset *videoAsset = [AVAsset assetWithURL:[NSURL fileURLWithPath:videoPath]];
//
//    _exportSession = [CLConvertVideo exportSessionWithAsset:videoAsset];
//    //截取时长
//    _exportSession.timeRange = CMTimeRangeMake(kCMTimeZero, videoAsset.duration); //根据时长截取压缩转码
//    _exportSession.outputFileType = AVFileTypeMPEG4;
//    NSURL *outFileUrl = [self.class exporterCoverPath];
//    _exportSession.outputURL = outFileUrl;
//
////    AVAssetTrack *videoTrack = [[videoAsset tracksWithMediaType:AVMediaTypeVideo] safeObjectAtIndex:0];
//    CGSize videoSize = [SCCreateVideoManager getVideoNaturalSizeWithVideoPath:videoPath];
//
//    _exportSession.videoCompressionSettings = [[self class] videoCompressionSettings:videoSize.width height:videoSize.height];
//    _exportSession.audioCompressionSettings = [CLVideoRecorder audioCompressionSettings];
//
//    [_exportSession exportAsynchronouslyWithCompletionHandler:^{
//        [DTPubUtil addBlockOnMainThread:^{
//            if (handler) {
//                BOOL success = NO;
//                NSString *error = nil;
//                if (_exportSession.status == AVAssetExportSessionStatusCompleted) {
//                    success = YES;
//                } else {
//                    error = @"压缩视频出错";
//                }
//                handler(success,outFileUrl.path,error);
//            }
//        }];
//    }];
//}

+ (NSDictionary *)videoCompressionSettings:(CGFloat)width height:(CGFloat)height
{
    NSInteger frameKey = 0;
    if (frameKey <= 0) {
        frameKey = 15;
    }
    
    NSInteger bitRateKey = 0;
    if (bitRateKey <= 0) {
        bitRateKey = 10;
    }
    return [NSDictionary dictionaryWithObjectsAndKeys:
            AVVideoCodecH264, AVVideoCodecKey,
            [NSNumber numberWithInteger:width], AVVideoWidthKey,
            [NSNumber numberWithInteger:height], AVVideoHeightKey,
            AVVideoScalingModeResizeAspectFill, AVVideoScalingModeKey,
            [NSDictionary dictionaryWithObjectsAndKeys:
             [NSNumber numberWithInteger:width * height * bitRateKey], AVVideoAverageBitRateKey,
             [NSNumber numberWithInteger:frameKey], AVVideoMaxKeyFrameIntervalKey,
             nil], AVVideoCompressionPropertiesKey,
            nil];
}


+ (UIImage *)getScreenShotImageFromVideoPath:(AVURLAsset *)urlAsset
                                   withStart:(CGFloat)start
                               withTimescale:(CGFloat)timescale  {
    //    //视频的总时间
    //    CMTime time = [self.kj_urlAsset duration];
    //    CGFloat second = 1.0*time.value/time.timescale;
    //    if (second < 0.5) {0.5
    //        UIImage *cover = [KJUtility kj_getScreenShotImageFromVideoPath:self.kj_urlAsset withStart:0 withTimescale:time.timescale];
    //        NSDictionary *dict = @{@"time":@"0",@"image":cover};
    //        [self.kj_coverArray addObject:dict];
    //    } else {
    //        CGFloat coverTime = 0.5;
    //        while (second >= coverTime) {
    //            NSDictionary *dict = @{@"time":[NSNumber numberWithFloat:coverTime]};
    //            [self.kj_coverArray addObject:dict];
    //            coverTime += 0.5;
    //        }
    //    }
    if (timescale == 0) {
        timescale = [urlAsset duration].timescale;
    }
    UIImage *shotImage;
    AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:urlAsset];
    gen.appliesPreferredTrackTransform = YES;
    CMTime time = CMTimeMakeWithSeconds(start, timescale);
    NSError *error = nil;
    CMTime actualTime;
    CGImageRef image = [gen copyCGImageAtTime:time actualTime:&actualTime error:&error];
    shotImage = [[UIImage alloc] initWithCGImage:image];
    CGImageRelease(image);
    return shotImage;
    
}

#pragma mark - 调节合成的音量
+ (AVAudioMix *)buildAudioMixWithVideoTrack:(AVCompositionTrack *)videoTrack VideoVolume:(float)videoVolume BGMTrack:(AVCompositionTrack *)BGMTrack BGMVolume:(float)BGMVolume atTime:(CMTime)volumeRange {
    
    //    创建音频混合类
    AVMutableAudioMix *audioMix = [AVMutableAudioMix audioMix];
    
    //    拿到视频声音轨道设置音量
    AVMutableAudioMixInputParameters *Videoparameters = [AVMutableAudioMixInputParameters audioMixInputParametersWithTrack:videoTrack];
    [Videoparameters setVolume:videoVolume atTime:volumeRange];
    
    //    设置背景音乐音量
    AVMutableAudioMixInputParameters *BGMparameters = [AVMutableAudioMixInputParameters audioMixInputParametersWithTrack:BGMTrack];
    [BGMparameters setVolume:BGMVolume atTime:volumeRange];
    
    //    加入混合数组
    audioMix.inputParameters = @[Videoparameters,BGMparameters];
    
    return audioMix;
}

#pragma mark - 输出路径
+ (NSURL *)exporterPath {
    NSInteger nowInter = (long)[[NSDate date] timeIntervalSince1970];
    NSString *fileName = [NSString stringWithFormat:@"output%ld.mp4",(long)nowInter];
    NSString *documentsDirectory = [self getSCAlbumFilePath];
    NSString *outputFilePath =[documentsDirectory stringByAppendingPathComponent:fileName];
    if([[NSFileManager defaultManager]fileExistsAtPath:outputFilePath]){
        [[NSFileManager defaultManager]removeItemAtPath:outputFilePath error:nil];
    }
    return [NSURL fileURLWithPath:outputFilePath];
}

+ (NSURL *)exporterCoverPath {
    NSInteger nowInter = (long)[[NSDate date] timeIntervalSince1970];
    NSString *fileName = [NSString stringWithFormat:@"cover_output%ld.mp4",(long)nowInter];
    NSString *documentsDirectory = [self getSCAlbumFilePath];
    NSString *outputFilePath =[documentsDirectory stringByAppendingPathComponent:fileName];
    if([[NSFileManager defaultManager]fileExistsAtPath:outputFilePath]){
        [[NSFileManager defaultManager]removeItemAtPath:outputFilePath error:nil];
    }
    return [NSURL fileURLWithPath:outputFilePath];
}

#pragma mark - 音视频剪辑,如果是视频把下面的类型换为AVFileTypeAppleM4V
+ (void)cutAudioVideoResourcePath:(NSURL *)assetURL startTime:(CGFloat)startTime endTime:(CGFloat)endTime complition:(void (^)(NSURL *outputPath,BOOL isSucceed)) completionHandle{
    //    素材
    AVAsset *asset = [AVAsset assetWithURL:assetURL];
    
    //    导出素材
    AVAssetExportSession *exporter = [[AVAssetExportSession alloc]initWithAsset:asset presetName:AVAssetExportPresetAppleM4A];
    
    //剪辑(设置导出的时间段)
    CMTime start = CMTimeMakeWithSeconds(startTime, asset.duration.timescale);
    CMTime duration = CMTimeMakeWithSeconds(endTime - startTime,asset.duration.timescale);
    exporter.timeRange = CMTimeRangeMake(start, duration);
    
    //    输出路径
    NSURL *outputPath = [self exporterPath];
    exporter.outputURL = [self exporterPath];
    
    //    输出格式
    exporter.outputFileType = AVFileTypeAppleM4A;
    
    exporter.shouldOptimizeForNetworkUse= YES;
    
    //    合成后的回调
    [exporter exportAsynchronouslyWithCompletionHandler:^{
        [DTPubUtil addBlockOnMainThread:^{
            switch ([exporter status]) {
                case AVAssetExportSessionStatusFailed: {
                    NSLog(@"合成失败：%@",[[exporter error] description]);
                    completionHandle(outputPath,NO);
                } break;
                case AVAssetExportSessionStatusCancelled: {
                    completionHandle(outputPath,NO);
                } break;
                case AVAssetExportSessionStatusCompleted: {
                    completionHandle(outputPath,YES);
                } break;
                default: {
                    completionHandle(outputPath,NO);
                } break;
            }
        }];
    }];
}

+ (CGSize)getVideoNaturalSizeWithVideoPath:(NSString *)videoPath
{
    NSDictionary* options = @{AVURLAssetPreferPreciseDurationAndTimingKey:@YES};
    AVAsset *videoAsset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:videoPath] options:options];
    AVAssetTrack *videoTrack = [[videoAsset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
    CGSize videoSize = videoTrack.naturalSize;
    NSInteger width = videoSize.width;
    NSInteger height = videoSize.height;
    if (width <= 0 || height <= 0) {
        width = SCREEN_WIDTH;
        height = SCREEN_WIDTH * VIDEO_DEFALUT_RATE;
    }
    return CGSizeMake(width, height);
}

@end
