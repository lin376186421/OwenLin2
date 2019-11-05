//
//  SCCreateVideoManager.h
//  SuperCoach
//
//  Created by 林 on 2018/12/28.
//  Copyright © 2018 Lin Feihong. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#define VIDEO_DEFALUT_RATE (1920.f/1080.f) //视频输出比率
//#define VIDEO_DEFALUT_RATE (SCREEN_HEIGHT/SCREEN_WIDTH)

@interface SCCreateVideoStuffGifItem : DTYYModelEntity

@property (nonatomic, assign) NSUInteger count;//总数
@property (nonatomic, strong) NSString *prefixFileName;//文件名前缀以
@property (nonatomic, assign) NSInteger loopsCount;//0无限播放
@property (nonatomic, assign) CGFloat x;//
@property (nonatomic, assign) CGFloat y;//
@property (nonatomic, assign) CGFloat width;//宽
@property (nonatomic, assign) CGFloat height;//高
@property (nonatomic, strong) NSString *path;
@property (nonatomic, strong) NSString *dir;//1上 2下 3左 4又 0全屏

@property (nonatomic, assign) NSUInteger curIndex;//当前第几帧
@property (nonatomic, strong) NSMutableArray *imageArr;//帧图数组
@property (nonatomic, strong) UIImageView *imageView;//当前的图片控件
@property (nonatomic, assign) NSInteger curLoops;//当前已经循环次数

@property (nonatomic, assign) BOOL startRunning;//针对loopsCount不为0的 动画 才有触发条件 一般是开始录制就出发动画


@end

//拍摄视频的素材
@interface SCCreateVideoStuffItem : DTYYModelEntity

@property (nonatomic, strong) NSString *sourceId;//资源id
@property (nonatomic, strong) NSString *rootPath;
@property (nonatomic, strong) NSString *mp3Path;//音频路径
@property (nonatomic, strong) NSArray<SCCreateVideoStuffGifItem *> *gifList;
@property (nonatomic, strong) NSString *realMp3FilePath;//MP3全路径
@property (nonatomic, strong) NSNumber *coachInfoBottom;//教练信息水印 距离底部大小

@end

typedef void (^SCCreateVideoManagerBlock)(BOOL success,NSString *error);
typedef void (^SCEditVideoManagerBlock)(BOOL success,NSString *outPath,NSString *error);

@interface SCCreateVideoManager : NSObject

@property (nonatomic, strong) NSMutableArray *coverImageArr;
@property (nonatomic, strong) NSMutableArray *talkArr;
@property (nonatomic, strong) NSString *curTalk;
@property (nonatomic, assign) BOOL isReserve;//是否用备选方案

- (void)checkIsReserve;

- (NSString *)getRandomTalkText;
+ (SCCreateVideoManager *)sharedInstance;

+ (UIImage *)getVideoShareImage;
//导出视频中的音频
- (void)exportAudioWithVideoPath:(NSString *)path completeBlock:(SCCreateVideoManagerBlock)block isTryAgain:(BOOL)isTryAgain;

+ (void)editVideoSynthesizeVieoPath:(NSURL *)assetURL BGMPath:(NSURL *)BGMPath videoVolume:(CGFloat)videoVolume BGMVolume:(CGFloat)BGMVolume needCoachInfo:(BOOL)needCoachInfo coachInfoBottom:(NSNumber *)coachInfoBottom complition:(SCEditVideoManagerBlock)block;

+ (NSString *)getSCAlbumFilePath;
+ (void)removeSCAlbumFilePath;
+ (NSString *)getVideoOutPath;

+ (NSString *)recordPath:(NSString *)videoPath suffix:(NSString *)suffix;
+ (UIImage *)getScreenShotImageFromVideoPath:(AVURLAsset *)urlAsset
                                   withStart:(CGFloat)start
                               withTimescale:(CGFloat)timescale;


+ (void)removeSCAllResourcePath;

+ (SCCreateVideoStuffItem *)createLocalStuffItemWithSourceId:(NSString *)sourceId;
+ (NSString *)getSCResourcePathWithSourceId:(NSString *)sourceId;

- (void)checkResource:(NSString *)sourceId complete:(SCCustomCallBack)block;

+ (CGSize)getVideoNaturalSizeWithVideoPath:(NSString *)videoPath;
- (void)createScreenHotImageArrWithVideoPath:(NSString *)videoPath;

@end


