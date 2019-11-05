//
//  SCVideoPlayController.m
//  PodTest
//
//  Created by 林 on 2018/12/12.
//  Copyright © 2018 林. All rights reserved.
//

#import "SCVideoPlayController.h"
#import "SCGPUCreatVideoController.h"
#import "SCFileManager.h"
#import "SCVideoPlayView.h"
#import "SCCreateVideoManager.h"
#import "SCChooseView.h"
#import "LFGPUImageEmptyFilter.h"
#import "FSKGPUImageBeautyFilter.h"

@interface SCVideoPlayController ()
{
    SCVideoPlayView *_playView;
    BOOL _isShow;
    SCChooseView *_chooseView;
    GPUImageMovie *_showMovie;
    GPUImageView *_filterView;
    GPUImageOutput<GPUImageInput> *_filter;
    AVPlayer *_player;
    
}
//音乐播放
@property (nonatomic, strong) UIButton *cancelBtn;

@end

@implementation SCVideoPlayController

- (void)viewDidLoad {
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didBecomActiveNotification:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willResignActiveNotification:) name:UIApplicationWillResignActiveNotification object:nil];
    
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor blackColor];
//    self.disableBackGesture = YES;
    self.title = @"视频播放";
    
    [self customAVPlayer];
    
//    CGSize size = [SCCreateVideoManager getVideoNaturalSizeWithVideoPath:_videoUrl.path];
//    _playView = [[SCVideoPlayView alloc] initWithFrame:CGRectMake(0, 0, size.width, size.height)];
//    _playView.center = CGPointMake(self.view.width/2, self.view.height/2);
//    _playView.coverImage = [[SCCreateVideoManager sharedInstance].coverImageArr objectAtIndex:0];
//    _playView.videoUrl = _videoUrl;
//    [_playView startPlay];
//    [self.view addSubview:_playView];

    [self buildNavUI];
    
    WEAK_SELF
    SCChooseView *chooseView = [SCChooseView showInView:self.view];
    chooseView.selectBlock = ^(id data) {
        [weakSelf selectFilter:data];
    };
    chooseView.hidden = YES;
    _chooseView = chooseView;
    _isShow = NO;
}


//应用进入前台
- (void)didBecomActiveNotification:(NSNotification *)ntf {
    [_player seekToTime:kCMTimeZero];
    [_player play];
    [_showMovie startProcessing];
}

//应用即将进入后台
- (void)willResignActiveNotification:(NSNotification *)ntf {
    [_player pause];
    [_showMovie endProcessing];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.navigationController setNavigationBarHidden:YES animated:YES];
    [[UIApplication sharedApplication] setStatusBarHidden:YES];
    //让其他应用的声音停止
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setActive:YES error:nil];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.navigationController setNavigationBarHidden:NO animated:YES];
    [[UIApplication sharedApplication] setStatusBarHidden:NO];
    [self removeNotification];
    //当离开该页面。让其他应用声音恢复
    [[AVAudioSession sharedInstance] setActive:NO withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:nil];
}

- (void)selectFilter:(NSDictionary *)dict
{
    [_showMovie removeAllTargets];
    [_filter removeAllTargets];
    _filter = [[NSClassFromString(dict[@"filter"]) alloc] init];
    if ([dict[@"filter"] isEqualToString:@"GPUImageGammaFilter"]) {
        ((GPUImageGammaFilter *)_filter).gamma = [dict[@"gamma"] floatValue];
    } else if ([dict[@"filter"] isEqualToString:@"GPUImageSaturationFilter"]) {
        ((GPUImageSaturationFilter *)_filter).saturation = [dict[@"saturation"] floatValue];
    } else if ([dict[@"filter"] isEqualToString:@"GPUImageContrastFilter"]) {
        ((GPUImageContrastFilter *)_filter).contrast = [dict[@"contrast"] floatValue];
    } else if ([dict[@"filter"] isEqualToString:@"FSKGPUImageBeautyFilter"]) {
        ((FSKGPUImageBeautyFilter *)_filter).beautyLevel = [dict[@"beautyLevel"] floatValue];
        ((FSKGPUImageBeautyFilter *)_filter).brightLevel = [dict[@"brightLevel"] floatValue];
        ((FSKGPUImageBeautyFilter *)_filter).toneLevel = [dict[@"toneLevel"] floatValue];
    } else if ([dict[@"filter"] isEqualToString:@"GPUImageRGBFilter"]) {
        ((GPUImageRGBFilter *)_filter).red = [dict[@"red"] floatValue];
        ((GPUImageRGBFilter *)_filter).blue = [dict[@"blue"] floatValue];
        ((GPUImageRGBFilter *)_filter).green = [dict[@"green"] floatValue];
    }
    
    [_showMovie addTarget:_filter];
    [_filter addTarget:_filterView];
}

- (void)customAVPlayer {
//    WEAK_SELF
    AVPlayer *player = [[AVPlayer alloc] init];
    _player = player;
    
    AVPlayerItem *playerItem = [AVPlayerItem playerItemWithURL:_videoUrl];
    [player replaceCurrentItemWithPlayerItem:playerItem];
    
    UIImageView *imgView = [UIImageView new];
    imgView.frame = self.view.bounds;
    [self.view addSubview:imgView];

    AVPlayerLayer *playerLayer = [AVPlayerLayer playerLayerWithPlayer:player];
    playerLayer.videoGravity=AVLayerVideoGravityResizeAspectFill;
    playerLayer.frame = CGRectMake(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    [imgView.layer insertSublayer:playerLayer atIndex:0];
    
    GPUImageMovie *showMovie = [[GPUImageMovie alloc] initWithPlayerItem:playerItem];
    _showMovie = showMovie;
    showMovie.runBenchmark = YES;
    showMovie.playAtActualSpeed = YES;//滤镜渲染方式
    showMovie.shouldRepeat = YES;//是否循环播放
    
    //正常滤镜
    LFGPUImageEmptyFilter *filter = [[LFGPUImageEmptyFilter alloc] init];
    _filter = filter;
    [showMovie addTarget:filter];
    
    GPUImageView *filterView = [[GPUImageView alloc] initWithFrame:CGRectMake(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)];
    _filterView = filterView;
    [imgView addSubview:filterView];
    [filter addTarget:filterView];
//    CGAffineTransform rotate = CGAffineTransformMakeRotation([KJUtility kj_degressFromVideoFileWithURL:videoUrl] / 180.0 * M_PI );
//    _kj_filterView.transform = rotate;
    [imgView bringSubviewToFront:filterView];
    
    [self addNotification];
    
    [player play];
    
    [showMovie startProcessing];
}

//添加播放器通知
-(void)addNotification{
    //给AVPlayerItem添加播放完成通知
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(playbackFinished:) name:AVPlayerItemDidPlayToEndTimeNotification object:_player.currentItem];
}

//移除通知
-(void)removeNotification{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

//播放完成通知
-(void)playbackFinished:(NSNotification *)notification{
    NSLog(@"视频播放完成,重新播放");
    // 播放完成后重复播放
    // 跳到最新的时间点开始播放
    [_player seekToTime:kCMTimeZero];
    [_player play];
    [_showMovie startProcessing];
}

- (void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    _playView.center = CGPointMake(self.view.width/2, self.view.height/2);
}

- (void)buildNavUI
{
    
    self.cancelBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.cancelBtn.frame = CGRectMake(20, 20, 32, 32);
    [self.cancelBtn setImage:[UIImage imageNamed:@"sc_creat_video_cancel"] forState:UIControlStateNormal];
    [self.cancelBtn addTarget:self action:@selector(dismissAction) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.cancelBtn];
    
    UIButton *resetBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    resetBtn.backgroundColor = SCGreenColor;
    resetBtn.frame = CGRectMake(self.view.width/2-122/2, self.view.height - 48- 25, 122, 48);
    resetBtn.layer.cornerRadius = resetBtn.height/2;
    resetBtn.layer.masksToBounds = YES;
    [resetBtn setTitle:@"添加效果"];
    [resetBtn setTitleFont:FONT(17)];
    [resetBtn setTitleColor:[UIColor whiteColor]];
    [resetBtn addTarget:self action:@selector(editAction:)];
    [self.view addSubview:resetBtn];
}

- (void)editAction:(id)sender
{
    _isShow = !_isShow;
    _chooseView.hidden = !_isShow;
}

- (void)resetAction
{
    [self dismissAction];
}

- (void)nextAction
{
    [_playView deallocPlayer];
    [DTPubUtil showHUDMessageInWindow:@"哪有下一步"];
    //下一步
//    SCCreatVideoUploadController *vc = [[SCCreatVideoUploadController alloc] init];
//    vc.videoUrl = _videoUrl;
//    [self.navigationController pushViewControllerWithPopOneControllerAndAnimated:vc];
}

- (void)dismissAction
{
    [self.navigationController popViewControllerAnimated:YES];
}


-(void)dealloc
{
    
}


@end
