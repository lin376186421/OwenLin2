//
//  SCGPUCreatVideoController.m
//  SuperCoach
//
//  Created by 林 on 2018/12/20.
//  Copyright © 2018 Lin Feihong. All rights reserved.
//

//#define RECORD_MAX_TIME 10.0           //最长录制时间
#define TIMER_INTERVAL 0.05         //计时器刷新频率

#import "SCGPUCreatVideoController.h"
#import "SCCreateVideoProgressView.h"
//#import "SCAuthorizationManager.h"
#import "GPUImage.h"
#import "FSKGPUImageBeautyFilter.h"
#import "SCVideoPlayController.h"
#import "SCFileManager.h"
#import "SCCreateVideoManager.h"
#import "FFmpegManager.h"
//#import "SCCreateVideoChooseTalkAlertView.h"


typedef void(^PropertyChangeBlock)(AVCaptureDevice *captureDevice);


@interface SCGPUCreatVideoController ()<GPUImageVideoCameraDelegate>
{
    GPUImageMovieWriter *movieWriter;
    BOOL _isFirstLoad;
    NSInteger RECORD_MAX_TIME;
//    NSUInteger _captureCount;
    UIImageView *_recordTipImageView;
    
}

@property (nonatomic, strong) UIButton *cancelBtn;
@property (nonatomic, strong) UIView *timeView;
@property (nonatomic, strong) UILabel *timelabel;
@property (nonatomic, strong) UIButton *turnCamera;
@property (nonatomic, strong) UIButton *flashBtn;
@property (nonatomic, strong) UIView *topTipView;
@property (nonatomic, strong) UILabel *topTipLabel;
@property (nonatomic, strong) SCCreateVideoRecordView *progressView;
//@property (nonatomic, strong) SCCreateVideoRecordView *recordView;
@property (nonatomic, assign) CGFloat recordTime;

@property (strong, nonatomic) NSMutableArray *videoArray;//支持断点录制

@property (nonatomic, strong) GPUImageVideoCamera *videoCamera;
@property (nonatomic, strong) GPUImageView *filterView;
@property (nonatomic, strong) FSKGPUImageBeautyFilter *beautifyFilter;
//裁剪1:1
@property (strong, nonatomic) GPUImageCropFilter *cropFilter;
/// 水印
@property (strong, nonatomic) GPUImageUIElement *dissolveElement;
/// 与水印合并的界面滤镜 主要用来显示
@property (strong, nonatomic) GPUImageAlphaBlendFilter *dissolveFilter;
/// 水印界面
@property (strong, nonatomic) UIView *gifContentView;

@property (assign, nonatomic) SCGPUCreateVideoRecordState recordState;
//计时器
@property (strong, nonatomic) NSTimer *timer;
//已录制时间
@property (assign, nonatomic) CGFloat currentTime;
//最终的视频路径（合成后）
@property (copy, nonatomic) NSString *outPath;

@property (assign, nonatomic)  BOOL cameraAllowAccess;
@property (assign, nonatomic)  BOOL recordAllowAccess;
@property (assign, nonatomic)  BOOL photeAllowAccess;

//相机layer
@property (strong, nonatomic) UIImageView *imgView;
@property (nonatomic) CGPoint startPoint;
@property (assign, nonatomic) CGFloat focalLength;


@property (strong, nonatomic) SCCreateVideoStuffItem *stuffItem;
@property (strong, nonatomic) NSMutableArray<SCCreateVideoStuffGifItem *> *holdGitItemArr;

@end

@implementation SCGPUCreatVideoController


- (void)dealloc {
    [self.videoCamera stopCameraCapture];
    [self.videoCamera removeInputsAndOutputs];
    [self.videoCamera removeAllTargets];
    [self.beautifyFilter removeAllTargets];
    [self.dissolveFilter removeAllTargets];
    [self.cropFilter removeAllTargets];
    self.videoCamera = nil;
    self.filterView = nil;
    self.beautifyFilter = nil;
    self.cropFilter = nil;
    self.dissolveFilter = nil;
    [DTPubUtil stopLoading];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    if (!_isFirstLoad) {
        _isFirstLoad = YES;
        [self customSystemSession];
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self.navigationController setNavigationBarHidden:YES animated:animated];
    [[UIApplication sharedApplication] setStatusBarHidden:YES animated:animated];
    if (_recordState == SCGPUCreateVideoRecordStateFinish) {
        self.recordState = SCGPUCreateVideoRecordStateInit;
    }
    [_videoCamera startCameraCapture];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    [_videoCamera stopCameraCapture];
    [[UIApplication sharedApplication] setStatusBarHidden:NO animated:animated];
}

- (void)dismissVC
{
    [self.navigationController dismissViewControllerAnimated:YES completion:nil];
}

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        [SCCreateVideoManager removeSCAlbumFilePath];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    RECORD_MAX_TIME = 15;
//    NSInteger newValue = [DTOnlineConfig onlineIntValue:@"RECORD_MAX_TIME"];
//    if (newValue > 0) {
//        RECORD_MAX_TIME = newValue;
//    }
//    self.disableBackGesture = YES;
    self.view.backgroundColor = [UIColor blackColor];
    self.videoArray = [NSMutableArray arrayWithCapacity:0];
    
    _cameraAllowAccess = YES;
    _recordAllowAccess = YES;
    _photeAllowAccess = YES;
    [self checkAllReady];
//    //相机授权
//    WEAK_SELF
//    [SCAuthorizationManager cameraAuthorizationStatus:self completeBlock:^(BOOL allowAccess) {
//        weakSelf.cameraAllowAccess = allowAccess;
//        [weakSelf checkAllReady];
//    }];
//    //麦克风
//    [SCAuthorizationManager requestRecordPermission:self completeBlock:^(BOOL allowAccess) {
//        weakSelf.recordAllowAccess = allowAccess;
//        [weakSelf checkAllReady];
//    }];
//
//    if ([SCCreateVideoManager sharedInstance].isReserve) {
//        [SCAuthorizationManager requestSavePhotoPermission:self completeBlock:^(BOOL allowAccess) {
//            weakSelf.photeAllowAccess = allowAccess;
//            [weakSelf checkAllReady];
//        }];
//    } else {
//        weakSelf.photeAllowAccess = YES;
//        [weakSelf checkAllReady];
//    }
    
    [DTPubUtil showLoadingWithText:@"正在加载相机"];
    [SCCreateVideoManager getSCAlbumFilePath];
}

- (void)checkAllReady
{
    if (_cameraAllowAccess && _recordAllowAccess && _photeAllowAccess && _progressView) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self showTipAlertView];
            [self showRecordTip];
        });
        
    }
}

//焦距
- (void)onPanAction:(UIPanGestureRecognizer *)sender {
    if (sender.state == UIGestureRecognizerStateBegan) {
        self.startPoint = [sender locationInView:self.view];
    } else {
        CGPoint stopLocation = [sender locationInView:self.view];
        CGFloat dy = stopLocation.y - self.startPoint.y;
        if (sender.state == UIGestureRecognizerStateEnded) {
            self.focalLength -= (dy/100.0);
            if (self.focalLength > 3) {
                self.focalLength = 3;
            }
            if (self.focalLength < 1) {
                self.focalLength = 1;
            }
        } else {
            CGFloat focal = self.focalLength-dy/100.0;
            if (focal > 3) {
                focal = 3;
            }
            if (focal < 1) {
                focal = 1;
            }
            [CATransaction begin];
            [CATransaction setAnimationDuration:.025];
            NSError *error;
            if([self.videoCamera.inputCamera lockForConfiguration:&error]){
                [self.videoCamera.inputCamera setVideoZoomFactor:focal];
                [self.videoCamera.inputCamera unlockForConfiguration];
            }
            else {
                NSLog(@"ERROR = %@", error);
            }
            
            [CATransaction commit];
        }
        NSLog(@"Distance: %f", dy);
    }
}

- (void)customSystemSession
{
    self.stuffItem = [SCCreateVideoManager createLocalStuffItemWithSourceId:_sourceId];
    
    self.imgView = [UIImageView new];
    self.imgView.clipsToBounds = YES;
    [self.view addSubview:self.imgView];
    self.imgView.frame =  self.view.bounds;
    self.imgView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
    
//    if ([DTOnlineConfig onlineBoolValue:@"show_create_video_panges"]) {
//        self.imgView.userInteractionEnabled = YES;
//        UIPanGestureRecognizer *pinch = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onPanAction:)];
//        [self.imgView addGestureRecognizer:pinch];
//    }
    
    NSString *defalutSessionPreset = AVCaptureSessionPresetHigh;
    GPUImageVideoCamera *camera = [[GPUImageVideoCamera alloc] initWithSessionPreset:defalutSessionPreset cameraPosition:AVCaptureDevicePositionFront];
    if (camera == nil) {
        camera = [[GPUImageVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPreset1280x720 cameraPosition:AVCaptureDevicePositionFront];
    }
    if (camera == nil) {
        [self dismissVC];
        [DTPubUtil showHUDErrorHintInWindow:@"相机不可用，请尝试更换手机"];
        return;
    }
    self.videoCamera = camera;
    self.videoCamera.delegate = self;
    _videoCamera.outputImageOrientation = UIInterfaceOrientationPortrait;
    _videoCamera.horizontallyMirrorFrontFacingCamera = YES;
    
    self.filterView = [[GPUImageView alloc] initWithFrame:CGRectMake(self.imgView.width/2-SCREEN_WIDTH/2, 0, SCREEN_WIDTH, SCREEN_HEIGHT)];
    _filterView.fillMode = kGPUImageFillModePreserveAspectRatioAndFill;
    [self.imgView addSubview:_filterView];
    
    //default 0.9 0.7 0.9
    
    //    /** 美颜程度 */
    CGFloat beautyLevel = 0;
    if (beautyLevel <= 0.f) {
        beautyLevel = 0.9;
    }
    //    /** 美白程度 */
    CGFloat brightLevel = 0;
    if (brightLevel <= 0.f) {
        brightLevel = 0.5;
    }
    //    /** 色调强度 */
    CGFloat toneLevel = 0;
    if (toneLevel <= 0.f) {
        toneLevel = 0.8;
    }
    
    self.beautifyFilter = [[FSKGPUImageBeautyFilter alloc] init];
    _beautifyFilter.beautyLevel = beautyLevel;
    _beautifyFilter.brightLevel = brightLevel;
    _beautifyFilter.toneLevel = toneLevel;
    
    UIView *contentView = [[UIView alloc] initWithFrame:self.filterView.bounds];
    self.gifContentView = contentView;
    
    for (int i = 0; i < _stuffItem.gifList.count; i++) {
        SCCreateVideoStuffGifItem *gifItem = [_stuffItem.gifList objectAtIndex:i];
        gifItem.imageArr = [NSMutableArray array];
        gifItem.curIndex = 0.f;
        gifItem.curLoops = 0.f;
        for (int index = 1; index <= gifItem.count; index++) {
            NSString *imagePath = [NSString stringWithFormat:@"%@/%@/%@%zd.png",_stuffItem.rootPath,gifItem.path,gifItem.prefixFileName,index];
            NSData *imageData = [NSData dataWithContentsOfFile:imagePath];
            UIImage *image = [UIImage imageWithData:imageData];
            if (image) {
                 [gifItem.imageArr addObject:image];
            }
        }
        if (gifItem.imageArr.count) {
            CGFloat width = SCREEN_WIDTH/375*gifItem.width;
            CGFloat height = width/gifItem.width*gifItem.height;
            
            CGFloat x = 0,y = 0;
            NSString *dir = gifItem.dir;
            if (dir.length) {
                if ([dir isEqualToString:@"0"]) {
                    width = SCREEN_WIDTH;
                    height = SCREEN_HEIGHT;
                    x=y=0.f;
                }
                else if ([dir isEqualToString:@"1"]) {
                    x=y=0.f;
                } else if ([dir isEqualToString:@"2"]) {
                    x = 0;
                    y = self.gifContentView.height - height;
                } else if ([dir isEqualToString:@"3"]) {
                    x = 0;
                    y = 0;
                } else if ([dir isEqualToString:@"4"]) {
                    x = self.gifContentView.width - width;
                    y = 0;
                }
            } else {
                x = gifItem.x * (SCREEN_WIDTH/375.f);
                y = gifItem.y * (SCREEN_WIDTH/375.f);
            }
            UIImageView *imv = [[UIImageView alloc] initWithFrame:CGRectMake(x, y, width,height)];
            gifItem.imageView = imv;
            imv.image = [gifItem.imageArr objectAtIndex:0];
            [contentView addSubview:imv];
            
            if (gifItem.loopsCount > 0) {
                if (_holdGitItemArr == nil) {
                    _holdGitItemArr = [NSMutableArray array];
                }
                imv.hidden = YES;
                [_holdGitItemArr addObject:gifItem];
            }
        }
        
    }
    
    GPUImageUIElement *uiElement = [[GPUImageUIElement alloc] initWithView:contentView];
    self.dissolveElement = uiElement;
    self.dissolveFilter = [[GPUImageAlphaBlendFilter alloc] init];
    self.dissolveFilter.mix = 1.f;
    
    [self.videoCamera addAudioInputsAndOutputs];
    
//    self.cropFilter = nil;
    //滤镜裁剪 屏幕比例大于(1920.f/1080.f) = 1.7777777 需要做裁剪处理
    CGSize realSize = SCREEN_SIZE;
    if (SCREEN_HEIGHT/SCREEN_WIDTH > VIDEO_DEFALUT_RATE) {
        CGFloat width = SCREEN_HEIGHT/VIDEO_DEFALUT_RATE;
        realSize = CGSizeMake(ceil(width), SCREEN_HEIGHT);
        CGFloat offWidth = realSize.width/2 - SCREEN_WIDTH/2;
        self.cropFilter = [[GPUImageCropFilter alloc] initWithCropRegion:CGRectMake(0,0, (realSize.width - 2*offWidth)/realSize.width , 1)];
    } else {
        self.cropFilter = [[GPUImageCropFilter alloc] initWithCropRegion:CGRectMake(0, 0, 1, 1)];
    }
    
    [self.videoCamera addTarget:self.cropFilter];
    [self.cropFilter addTarget:self.beautifyFilter];
    [self.beautifyFilter addTarget:self.dissolveFilter];
    [self.dissolveElement addTarget:self.dissolveFilter];
    [self.dissolveFilter addTarget:self.filterView];
    
    __unsafe_unretained GPUImageUIElement *weakOverlay = self.dissolveElement;
    BOOL needGif = self.stuffItem.gifList.count;
    WEAK_SELF
    [self.beautifyFilter setFrameProcessingCompletionBlock:^(GPUImageOutput *output, CMTime time) {
        if (needGif) {
            dispatch_async(dispatch_get_main_queue(), ^{
                for (int i = 0; i < weakSelf.stuffItem.gifList.count; i++) {
                    SCCreateVideoStuffGifItem *gifItem = [weakSelf.stuffItem.gifList objectAtIndex:i];
                    if (gifItem.loopsCount > 0 && gifItem.startRunning == NO) {
                        gifItem.imageView.hidden = YES;
                    } else {
                        gifItem.curIndex += 1;
                        if (gifItem.curIndex >= gifItem.count) {
                            gifItem.curIndex = 0;
                            gifItem.curLoops += 1;
                        }
                        if (gifItem.loopsCount == 0 || gifItem.curLoops < gifItem.loopsCount) {
                            gifItem.imageView.hidden = NO;
                            gifItem.imageView.image = [gifItem.imageArr objectAtIndex:gifItem.curIndex];
                        } else {
                            gifItem.imageView.hidden = YES;
                        }
                    }
                }
            });
        }
        [weakOverlay update];
    }];
    [self.videoCamera startCameraCapture];
}

- (void)addUI
{
    CGFloat statusOff = 20.f;
    
    self.timeView = [[UIView alloc] init];
    self.timeView.hidden = YES;
    self.timeView.frame = CGRectMake((SCREEN_WIDTH - 100)/2, 16+statusOff, 100, 34);
    self.timeView.backgroundColor = [UIColor colorWithString:@"242424" alpha:0.7];
    self.timeView.layer.cornerRadius = 4;
    self.timeView.layer.masksToBounds = YES;
    [self.view addSubview:self.timeView];
    
    UIView *redPoint = [[UIView alloc] init];
    redPoint.frame = CGRectMake(0, 0, 6, 6);
    redPoint.layer.cornerRadius = 3;
    redPoint.layer.masksToBounds = YES;
    redPoint.center = CGPointMake(25, 17);
    redPoint.backgroundColor = [UIColor redColor];
    [self.timeView addSubview:redPoint];
    
    self.timelabel =[[UILabel alloc] init];
    self.timelabel.font = [UIFont systemFontOfSize:13];
    self.timelabel.textColor = [UIColor whiteColor];
    self.timelabel.frame = CGRectMake(40, 8, 40, 28);
    [self.timeView addSubview:self.timelabel];
    
    self.cancelBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.cancelBtn.frame = CGRectMake(8, 8, 32, 32);
    [self.cancelBtn setImage:[UIImage imageNamed:@"sc_creat_video_cancel"] forState:UIControlStateNormal];
    [self.cancelBtn addTarget:self action:@selector(dismissVC) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.cancelBtn];
    
    CGFloat recordR = 108.f;
    self.progressView = [[SCCreateVideoRecordView alloc] initWithFrame:CGRectMake((SCREEN_WIDTH - recordR)/2, SCREEN_HEIGHT - 32 - recordR, recordR, recordR)];
    [self.progressView addTarget:self singleTapAction:@selector(startRecord)];
    [self.view addSubview:self.progressView];
    
    [self.progressView resetProgress];
    
    self.turnCamera = [UIButton buttonWithType:UIButtonTypeCustom];
    self.turnCamera.frame = CGRectMake(0, 0, 32, 27);
    [self.turnCamera setImage:[UIImage imageNamed:@"sc_creat_video_listing_camera_lens"] forState:UIControlStateNormal];
    [self.turnCamera addTarget:self action:@selector(turnCameraAction) forControlEvents:UIControlEventTouchUpInside];
    [self.turnCamera sizeToFit];
    [self.view addSubview:self.turnCamera];
    
    //暂不支持闪光
//    self.flashBtn = [UIButton buttonWithType:UIButtonTypeCustom];
//    self.flashBtn.frame = CGRectMake(0, 0, 28, 34);
//    [self.flashBtn setImage:[UIImage imageNamed:@"sc_creat_video_listing_flash_off"] forState:UIControlStateNormal];
//    [self.flashBtn addTarget:self action:@selector(flashAction) forControlEvents:UIControlEventTouchUpInside];
//    [self.flashBtn sizeToFit];
//    [self.view addSubview:self.flashBtn];
//    self.flashBtn.centerY = self.progressView.centerY;
//    self.flashBtn.centerX = self.view.width * 0.25;
    
    self.cancelBtn.centerY = self.progressView.centerY;
    self.cancelBtn.centerX = self.view.width * 0.25;
    
    self.turnCamera.centerY  = self.progressView.centerY;
    self.turnCamera.centerX = self.view.width * 0.75;
    
    [self checkAllReady];
    self.recordState = SCGPUCreateVideoRecordStateInit;
}

- (void)turnCameraAction
{
    AVCaptureDevice *captureDevice = self.videoCamera.inputCamera;
    if ([captureDevice position] == AVCaptureDevicePositionBack) {//只有后置摄像头才有闪光灯
        [self setFlashMode:AVCaptureFlashModeOff];
        [self.flashBtn setImage:[UIImage imageNamed:@"sc_creat_video_listing_flash_off"] forState:UIControlStateNormal];
    }
    [self.videoCamera rotateCamera];
    if ([captureDevice position] == AVCaptureDevicePositionFront) {//从后置摄像头转换到前置
        [self setFlashMode:AVCaptureFlashModeOff];
        [self.flashBtn setImage:[UIImage imageNamed:@"sc_creat_video_listing_flash_on"] forState:UIControlStateNormal];
    }
}

- (void)flashAction
{
    AVCaptureDevice *captureDevice = self.videoCamera.inputCamera;
    if ([captureDevice position] == AVCaptureDevicePositionBack) {//只有后置摄像头才有闪光灯
        AVCaptureFlashMode flashMode=captureDevice.flashMode;
        switch (flashMode) {
            case AVCaptureFlashModeAuto:
                [self setFlashMode:AVCaptureFlashModeOn];
                [self.flashBtn setImage:[UIImage imageNamed:@"sc_creat_video_listing_flash_on"] forState:UIControlStateNormal];
                break;
            case AVCaptureFlashModeOn:
                [self setFlashMode:AVCaptureFlashModeOff];
                [self.flashBtn setImage:[UIImage imageNamed:@"sc_creat_video_listing_flash_off"] forState:UIControlStateNormal];
                break;
            case AVCaptureFlashModeOff:
                [self setFlashMode:AVCaptureFlashModeAuto];
                [self.flashBtn setImage:[UIImage imageNamed:@"sc_creat_video_listing_flash_auto"] forState:UIControlStateNormal];
                break;
            default:
                break;
        }
    } else {
        [DTPubUtil showHUDErrorHintInWindow:@"自拍模式不支持闪光哦"];
    }
}

- (void)showRecordTip
{
//    BOOL hasShow = [[BPAppPreference sharedInstance] objectForKey:@"sc_showRecordTip"] != nil;
//    if (hasShow) {
//        return;
//    }
//    [[BPAppPreference sharedInstance] setObject:@1 forKey:@"sc_showRecordTip"];

    UIImageView *imageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"sc_creat_video_start_record_tip"]];
    imageView.left = _progressView.left + 20;
    imageView.bottom = _progressView.top + 10;
    [self.view addSubview:imageView];
    _recordTipImageView = imageView;
    
    CAKeyframeAnimation *ani = [CAKeyframeAnimation animationWithKeyPath:@"transform.translation.y"];
    ani.values = [NSArray arrayWithObjects:
                  [NSNumber numberWithFloat:0],
                  [NSNumber numberWithFloat:0],
                  [NSNumber numberWithFloat:+5],
                  [NSNumber numberWithFloat:0],
                  nil];
    ani.cumulative = YES;
    ani.duration = 1.0f;
    ani.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionLinear];
    ani.repeatCount = HUGE_VAL;
    ani.autoreverses = YES;
    ani.removedOnCompletion = NO;
    [_recordTipImageView.layer addAnimation:ani forKey:@"ani_forever"];
}

- (void)showTipAlertView
{
//    WEAK_SELF
//    __block SCCreateVideoChooseTalkAlertView * alertView = [SCCreateVideoChooseTalkAlertView showSelfWithChangeBlock:^(id data) {
//        [alertView setTipText:[SCCreateVideoManager sharedInstance].getRandomTalkText];
//    } withSureBlock:^(id data) {
//        [weakSelf showTopViewWithText:[SCCreateVideoManager sharedInstance].curTalk];
//    }];
//    [alertView setTipText:[SCCreateVideoManager sharedInstance].getRandomTalkText];
}

- (void)showTopViewWithText:(NSString *)text
{
    if (_topTipView == nil) {
        _topTipView = [[UIView alloc] initWithFrame:CGRectMake(12, 40, self.view.width-24, 68)];
        _topTipView.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.5];
        _topTipView.layer.cornerRadius = _topTipView.height/2;
        _topTipLabel.layer.masksToBounds = YES;
        [self.view addSubview:_topTipView];
        
        UIButton *changeBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        changeBtn.backgroundColor = [UIColor whiteColor];
        changeBtn.frame = CGRectMake(_topTipView.width-12-52, _topTipView.height/2 - 30/2, 52, 30);
        changeBtn.layer.cornerRadius = changeBtn.height/2;
        changeBtn.layer.masksToBounds = YES;
        [changeBtn setTitle:@"换"];
        [changeBtn setTitleFont:FONT_B(15)];
        [changeBtn setTitleColor:[UIColor blackColor]];
        [changeBtn addTarget:self action:@selector(changeTipAction)];
        [_topTipView addSubview:changeBtn];
        
        _topTipLabel = [UILabel labelWithFrame:CGRectMake(26, 2, changeBtn.left - 12-26, _topTipView.height-4) fontSize:15 color:[UIColor whiteColor]];
        _topTipLabel.numberOfLines = 0;
        _topTipLabel.minimumScaleFactor = 0.5;
        _topTipLabel.adjustsFontSizeToFitWidth = YES;
        _topTipLabel.textAlignment = NSTextAlignmentLeft;
        [_topTipView addSubview:_topTipLabel];
        
        UIView *maskView = [[UIView alloc]initWithFrame:CGRectOffset(_topTipView.bounds, -_topTipView.width, 0)];
        maskView.backgroundColor = [UIColor blackColor];
        _topTipView.layer.mask = maskView.layer;
        
//        [DTPubUtil setAnchorPoint:CGPointMake(0, 0.5) forView:_topTipView];
//        _topTipView.transform = CGAffineTransformMakeScale(0, 1);
        WEAK_SELF
        [UIView animateWithDuration:1.f animations:^{
//            weakSelf.topTipView.transform = CGAffineTransformIdentity;
            maskView.left = 0;
        } completion:^(BOOL finished) {
            weakSelf.topTipView.layer.mask = nil;
        }];
    }
    
    _topTipLabel.hidden = NO;
    _topTipLabel.text = text;
}

- (void)changeTipAction
{
    [self showTopViewWithText:[SCCreateVideoManager sharedInstance].getRandomTalkText];
}

+ (NSString *)recordPath:(NSString *)videoPath
{
    if (videoPath.length) {
        NSString *recordPath = [videoPath stringByReplacingOccurrencesOfString:@".mp4" withString:@".wav"];
        return recordPath;
    }
    return nil;
}

- (void)holdGifStuffItemStartRunning:(BOOL)isStartRunning
{
    for (SCCreateVideoStuffGifItem *item in _holdGitItemArr) {
        item.startRunning = isStartRunning;
        if (isStartRunning == NO) {
            item.curLoops = 0;
            item.curIndex = 0;
            item.imageView.image = [item.imageArr objectAtIndex:0];
        }
    }
}

- (void)setRecordState:(SCGPUCreateVideoRecordState)recordState
{
    _recordState = recordState;
    if (recordState == SCGPUCreateVideoRecordStateInit) {
        [self updateViewWithStop];
        [self.progressView resetProgress];
        [self clearAllVideo];
        if (self.timer) {
            [self.timer invalidate];
            self.timer = nil;
        }
        [self holdGifStuffItemStartRunning:NO];
    } else if (recordState == SCGPUCreateVideoRecordStateRecording) {
        if (_recordTipImageView) {
            [_recordTipImageView removeFromSuperview];
            _recordTipImageView = nil;
        }
        _currentTime = 0;
//        _captureCount = 0;
        [[SCCreateVideoManager sharedInstance].coverImageArr removeAllObjects];
        [self clearAllVideo];//暂不支持多段录制
        [self updateViewWithRecording];
        [self holdGifStuffItemStartRunning:YES];
        NSString *outPath = [SCCreateVideoManager getVideoOutPath];
        NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:@{@"path":outPath}];
        [self.videoArray addObject:dict];
        NSURL *videoURL = [NSURL fileURLWithPath:outPath];
        CGSize realSize = self.filterView.size;
        int width = realSize.width;
        width = width%2==0?width:width+1;
        int height = realSize.height;
        height = height%2==0?height:height+1;
        movieWriter = [[GPUImageMovieWriter alloc] initWithMovieURL:videoURL size:CGSizeMake(width, height)];
        movieWriter.encodingLiveVideo = YES;
        movieWriter.shouldPassthroughAudio = YES;
        movieWriter.assetWriter.movieFragmentInterval = kCMTimeInvalid;
        self.videoCamera.audioEncodingTarget = movieWriter;//音轨s
        [self.dissolveFilter addTarget:movieWriter];
        [movieWriter startRecording];
        self.timer = [NSTimer scheduledTimerWithTimeInterval:TIMER_INTERVAL
                                                      target:self
                                                    selector:@selector(updateTime)
                                                    userInfo:nil
                                                     repeats:YES];
    } else if (recordState == SCGPUCreateVideoRecordStateFinish) {
        //停止拍摄
        WEAK_SELF
        [self updateViewWithStop];
        [self holdGifStuffItemStartRunning:NO];
        self.videoCamera.audioEncodingTarget = nil;
        [self.dissolveFilter removeTarget:movieWriter];
        if (self.timer) {
            [self.timer invalidate];
            self.timer = nil;
        }
        NSMutableDictionary *dict = self.videoArray.lastObject;
        if (dict) {
            [dict setObject:[NSNumber numberWithFloat:self.currentTime] forKey:@"time"];
        }
        [movieWriter finishRecordingWithCompletionHandler:^{
            [DTPubUtil addBlockOnMainThread:^{
                [weakSelf onCompleteAction];
            }];
        }];
        
    }
}

//确定(完成)
- (void)onCompleteAction {
    if (self.videoArray.count > 0) {
//        [DTPubUtil showLoadingWithText:@"视频处理中..."];
        //只有一段视频
        NSDictionary *dict = self.videoArray.firstObject;
        self.outPath = [dict objectForKey:@"path"];
        [self exportAudioForWav];
    }
}

- (void)exportAudioForWav
{
    [DTPubUtil showLoadingWithText:@"请稍后"];
    NSString *commandStr = [NSString stringWithFormat:@"ffmpeg -i %@ -f wav -ar 16000 %@",self.outPath,[SCCreateVideoManager recordPath:self.outPath suffix:@".wav"]];
    
    WEAK_SELF
    [[FFmpegManager sharedManager] converWithCommandStr:commandStr processBlock:^(float process) {
        NSLog(@"导出音频wav进度：%f",process);
    } completionBlock:^(NSError *error) {
        if (error == nil) {
//            [weakSelf exportAudioForPcm];
//            [DTPubUtil stopLoading];
            [weakSelf editVideoSynthesize];
        } else {
            [DTPubUtil showHUDErrorHintInWindow:@"导出音频wav失败"];
            NSLog(@"%@",error);
            [weakSelf dismissVC];
        }
    }];
}

- (void)exportAudioForPcm
{
    NSString *commandStr = [NSString stringWithFormat:@"ffmpeg -i %@ -f s16le -ar 44100 -acodec pcm_s16le %@",[SCCreateVideoManager recordPath:self.outPath suffix:@".wav"],[SCCreateVideoManager recordPath:self.outPath suffix:@".pcm"]];
    WEAK_SELF
    [[FFmpegManager sharedManager] converWithCommandStr:commandStr processBlock:^(float process) {
        NSLog(@"导出音频进度：%f",process);
    } completionBlock:^(NSError *error) {
        if (error == nil) {
            [DTPubUtil stopLoading];
            [weakSelf editVideoSynthesize];
        } else {
            [DTPubUtil showHUDErrorHintInWindow:@"导出音频pcm失败"];
            NSLog(@"%@",error);
            [weakSelf dismissVC];
        }
    }];
}

- (void)editVideoSynthesize
{
//    NSString *path = _stuffItem.realMp3FilePath;
    NSUInteger musicCount = 3;
    NSString *path = [[NSBundle mainBundle] pathForResource:[NSString stringWithFormat:@"sc_bg_video_music_%zd",arc4random()%musicCount+1] ofType:@"mp3"];
    NSURL *musicPath = nil;
    if ([[NSFileManager defaultManager] fileExistsAtPath:path]) {
        musicPath = [NSURL fileURLWithPath:path];
    }
    CGFloat videoVolume = 0.f;
    if (videoVolume <= 0.f) {
        videoVolume = 1.f;
    }
    CGFloat BGMVolume = 0.f;
    if (BGMVolume <= 0.f) {
        BGMVolume = 0.06;
    }

//    [DTPubUtil showLoadingWithText:@"请稍后"];
    WEAK_SELF
    [SCCreateVideoManager editVideoSynthesizeVieoPath:[NSURL fileURLWithPath:self.outPath] BGMPath:musicPath videoVolume:videoVolume BGMVolume:BGMVolume needCoachInfo:YES coachInfoBottom:_stuffItem.coachInfoBottom complition:^(BOOL success, NSString *outPath, NSString *error) {
        if (success) {
            [DTPubUtil stopLoading];
            [weakSelf editVideoSynthesizeCompleteWithOutPath:outPath];
        } else {
            [DTPubUtil showHUDErrorHintInWindow:error];
            [weakSelf dismissVC];
        }
    }];

}

- (void)editVideoSynthesizeCompleteWithOutPath:(NSString *)outPath
{
    [SCFileManager removeItemAtPath:self.outPath];
    [SCFileManager moveItemAtPath:outPath toPath:self.outPath overwrite:YES];
    
    BOOL hasFile = [[NSFileManager defaultManager] fileExistsAtPath:self.outPath];
    if (hasFile) {
        if (1) {
            NSError *error = nil;
            NSMutableString *logStr = [NSMutableString string];
            NSString *size = [SCFileManager sizeFormattedOfFileAtPath:self.outPath error:&error];
            if (!error) {
                NSLog(@"原视频大小:%@",size);
                [logStr appendFormat:@"原视频大小:%@",size];
            } else {
                NSLog(@"获取原视频大小出错:%@",error);
                [logStr appendFormat:@"获取原视频大小出错:%@",error];
            }
            
            error = nil;
            size = [SCFileManager sizeFormattedOfFileAtPath:[SCCreateVideoManager recordPath:self.outPath suffix:@".wav"] error:&error];
            if (!error) {
                NSLog(@"音频大小:%@",size);
                [logStr appendFormat:@"\n音频大小:%@",size];
            } else {
                NSLog(@"获取音频大小出错:%@",error);
                [logStr appendFormat:@"\n获取音频大小出错:%@",error];
            }
            [DTPubUtil showToastWithText:logStr delay:2.f];
        }
        [[SCCreateVideoManager sharedInstance] createScreenHotImageArrWithVideoPath:self.outPath];//取帧图
        SCVideoPlayController *playVC = [[SCVideoPlayController alloc] init];
        playVC.videoUrl = [NSURL fileURLWithPath:self.outPath];
        [self.navigationController pushViewController:playVC animated:YES];

    } else {
        [DTPubUtil showHUDErrorHintInWindow:@"生成视频出错"];
        [self dismissVC];
    }
}

- (void)updateViewWithRecording
{
//    self.timeView.hidden = NO;
    self.timeView.hidden = YES;
    self.cancelBtn.hidden = self.turnCamera.hidden = self.flashBtn.hidden = YES;
    
    [self changeToRecordStyle];
}

- (void)updateViewWithStop
{
    self.timeView.hidden = YES;
    self.cancelBtn.hidden = self.turnCamera.hidden = self.flashBtn.hidden = NO;
    [self changeToStopStyle];
}

- (void)changeToRecordStyle
{
    [_progressView startAni];
}

- (void)changeToStopStyle
{
    [_progressView stopAni];
}

- (void)updateRecordingProgress:(CGFloat)progress
{
    [self.progressView updateProgressWithValue:progress];
    self.timelabel.text = [self changeToVideotime:progress * RECORD_MAX_TIME];
    [self.timelabel sizeToFit];
}

- (NSString *)changeToVideotime:(CGFloat)videocurrent {
    
    return [NSString stringWithFormat:@"%02li:%02li",lround(floor(videocurrent/60.f)),lround(floor(videocurrent/1.f))%60];
    
}

- (void)startRecord
{
    if (_recordState != SCGPUCreateVideoRecordStateRecording) {
        self.recordState = SCGPUCreateVideoRecordStateRecording;
    } else if (_recordState == SCGPUCreateVideoRecordStateRecording) {
        CGFloat minTime = 3.f;
        if (self.currentTime < minTime) {
            [DTPubUtil showHUDErrorHintInWindow:[NSString stringWithFormat:@"视频不能少于%.0f秒",minTime]];
            return;
        }
        self.recordState = SCGPUCreateVideoRecordStateFinish;
    }
}

//显示拍摄进度
- (void)updateTime {
    self.currentTime += TIMER_INTERVAL;
//    _captureCount += 1;
//    if (_captureCount % 2 == 1) {
//        UIImage *image = [self.view captureView];
//        if (image) {
//            [[SCCreateVideoManager sharedInstance].coverImageArr addObject:image];
//        }
//    }
    [self updateRecordingProgress:_currentTime/RECORD_MAX_TIME];
    if (self.currentTime >= RECORD_MAX_TIME) {
        self.recordState = SCGPUCreateVideoRecordStateFinish;
    }
}

//清除录制的视频
- (void)clearAllVideo {
    if (self.videoArray.count > 0) {
        for (NSDictionary *dict in self.videoArray) {
            if ([[NSFileManager defaultManager] fileExistsAtPath:dict[@"path"]]) {
                [[NSFileManager defaultManager] removeItemAtPath:dict[@"path"] error:nil];
            }
        }
        [self.videoArray removeAllObjects];
    }
}

#pragma 相机相关设置
/**
 *  设置闪光灯模式
 *
 *  @param flashMode 闪光灯模式
 */
-(void)setFlashMode:(AVCaptureFlashMode )flashMode{
    [self changeDeviceProperty:^(AVCaptureDevice *captureDevice) {
        if ([captureDevice isFlashModeSupported:flashMode]) {
            [captureDevice setFlashMode:flashMode];
            switch (flashMode) {
                case AVCaptureFlashModeAuto:
                    [captureDevice setTorchMode:AVCaptureTorchModeAuto];
                    break;
                case AVCaptureFlashModeOn:
                    [captureDevice setTorchMode:AVCaptureTorchModeOn];
                    break;
                case AVCaptureFlashModeOff:
                    [captureDevice setTorchMode:AVCaptureTorchModeOff];
                    break;
                default:
                    break;
            }
        }
    }];
}

/**
 *  改变设备属性的统一操作方法
 *
 *  @param propertyChange 属性改变操作
 */
-(void)changeDeviceProperty:(PropertyChangeBlock)propertyChange{
    AVCaptureDevice *captureDevice= self.videoCamera.inputCamera;
    NSError *error;
    //注意改变设备属性前一定要首先调用lockForConfiguration:调用完之后使用unlockForConfiguration方法解锁
    if ([captureDevice lockForConfiguration:&error]) {
        //自动对焦
        if ([captureDevice isFocusModeSupported:AVCaptureFocusModeContinuousAutoFocus]) {
            [captureDevice setFocusMode:AVCaptureFocusModeContinuousAutoFocus];
        }
        //自动曝光
        if ([captureDevice isExposureModeSupported:AVCaptureExposureModeContinuousAutoExposure]) {
            [captureDevice setExposureMode:AVCaptureExposureModeContinuousAutoExposure];
        }
        //自动白平衡
        if ([captureDevice isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance]) {
            [captureDevice setWhiteBalanceMode:AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance];
        }
        propertyChange(captureDevice);
        [captureDevice unlockForConfiguration];
    }else{
        NSLog(@"设置设备属性过程发生错误，错误信息：%@",error.localizedDescription);
    }
}


#pragma mark GPUImageVideoCameraDelegate
- (void)willOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
{
    [DTPubUtil addBlockOnMainThread:^{
        if (_progressView == nil) {//首次成功加载摄像头数据
            [self addUI];
            [DTPubUtil stopLoading];
        }
    }];
}


//        if (self.videoArray.count > 1) {
//            //需要合并多段视频
//            if (!self.outPath) {
//                self.outPath = [SCCreateVideoManager getVideoOutPath];//合成后的输出路径
//            }
//            //判断本地是否已有合成后的视频文件
//            if ([[NSFileManager defaultManager] fileExistsAtPath:self.outPath]) {
//                //如果存在就删除，重新合成
//                [[NSFileManager defaultManager] removeItemAtPath:self.outPath error:nil];
//            }
//            //音视频合成工具
//            AVMutableComposition *composition = [AVMutableComposition composition];
//            //音频
//            AVMutableCompositionTrack *audioTrack = [composition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
//            //视频
//            AVMutableCompositionTrack *videoTrack = [composition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
//            //开始合成
//            CMTime totalDuration = kCMTimeZero;
//            for (int i = 0; i < self.videoArray.count; i ++) {
//                NSDictionary *localDict = self.videoArray[i];
//                NSDictionary* options = @{AVURLAssetPreferPreciseDurationAndTimingKey:@YES};
//
//                AVAsset *asset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:localDict[@"path"]] options:options];
//                //获取asset中的音频
//                NSArray *audioArray = [asset tracksWithMediaType:AVMediaTypeAudio];
//                AVAssetTrack *assetAudio = audioArray.firstObject;
//                //向audioTrack中加入音频
//                NSError *audioError = nil;
//                BOOL isComplete_audio = [audioTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, asset.duration)
//                                                               ofTrack:assetAudio
//                                                                atTime:totalDuration
//                                                                 error:&audioError];
//                NSLog(@"加入音频%d  isComplete_audio：%d error：%@", i, isComplete_audio, audioError);
//
//                //获取asset中的视频
//                NSArray *videoArray = [asset tracksWithMediaType:AVMediaTypeVideo];
//                AVAssetTrack *assetVideo = videoArray.firstObject;
//                //向videoTrack中加入视频
//                NSError *videoError = nil;
//                BOOL isComplete_video = [videoTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, asset.duration)
//                                                               ofTrack:assetVideo
//                                                                atTime:totalDuration
//                                                                 error:&videoError];
//                NSLog(@"加入视频%d  isComplete_video：%d error：%@", i, isComplete_video, videoError);
//
//                totalDuration = CMTimeAdd(totalDuration, asset.duration);
//            }
//            //这里可以加水印的，但在这里不做水印处理
//
//            //视频导出处理
//            AVAssetExportSession *export = [AVAssetExportSession exportSessionWithAsset:composition
//                                                                                presetName:AVAssetExportPresetHighestQuality];
//            export.outputURL = [NSURL fileURLWithPath:self.outPath];
//            export.outputFileType = AVFileTypeMPEG4;
//            export.shouldOptimizeForNetworkUse = YES;
//            WEAK_SELF;
//            [export exportAsynchronouslyWithCompletionHandler:^{
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [self clearAllVideo];
//                    [weakSelf exportAudio];
//                });
//            }];
//        } else {
//
//        }

@end
