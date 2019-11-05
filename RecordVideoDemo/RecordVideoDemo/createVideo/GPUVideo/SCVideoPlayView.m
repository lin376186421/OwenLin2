//
//  SCVideoPlayView.m
//  SuperCoach
//
//  Created by 林 on 2019/1/3.
//  Copyright © 2019 Lin Feihong. All rights reserved.
//

#import "SCVideoPlayView.h"
#import <AVFoundation/AVFoundation.h>
#import "SCCreateVideoManager.h"
#import <AVKit/AVKit.h>
@interface SCVideoPlayView()
{
    UIImageView *_coverImageView;
}

@property (nonatomic, strong) AVPlayer *avPlayer;
@property (nonatomic, strong) AVPlayerLayer *playLayer;
@property (nonatomic, assign) CMTime time;
@property (nonatomic, strong) UIButton *playBtn;

@end

@implementation SCVideoPlayView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self setup];
    }
    return self;
}

- (void)awakeFromNib
{
    [super awakeFromNib];
    [self setup];
}

- (void)setup
{
    self.backgroundColor = [UIColor blackColor];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    self.playLayer.frame = self.bounds;
}

- (void)setVideoUrl:(NSURL *)videoUrl
{
    _videoUrl = videoUrl;
    AVPlayerItem *item = [AVPlayerItem playerItemWithURL:_videoUrl];
    AVPlayer* player = [AVPlayer playerWithPlayerItem:item];
    self.avPlayer = player;
    
    AVPlayerLayer *playLayer = [AVPlayerLayer playerLayerWithPlayer:player];
    playLayer.frame = self.bounds;
    playLayer.videoGravity = AVLayerVideoGravityResizeAspect;
    [self.layer addSublayer:playLayer];
    
//    AVPlayerViewController *avPlayerVC =[[AVPlayerViewController alloc] init];
//    avPlayerVC.player = player;
//    //步骤4：设置播放器视图大小
//    avPlayerVC.view.frame = self.bounds;
//    //特别注意:AVPlayerViewController不能作为局部变量被释放，否则无法播放成功
////    [self addChildViewController:avPlayerVC];
//    [self addSubview:avPlayerVC.view];

    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(playbackFinished:) name:AVPlayerItemDidPlayToEndTimeNotification object:nil];
    
 
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(appBecomeActive:)
                                                 name:UIApplicationDidBecomeActiveNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(appWillResignActive:)
                                                 name:UIApplicationWillResignActiveNotification
                                               object:nil];
    
    if (!_hideCoverImage && [[NSFileManager defaultManager] fileExistsAtPath:videoUrl.path]) {
        
        //本地视频 设置封面
        if (_coverImage == nil) {
            _coverImage = [SCCreateVideoManager getScreenShotImageFromVideoPath:[AVURLAsset assetWithURL:self.videoUrl] withStart:2.f withTimescale:0];//暂时从第一秒开始取
        }
        
        if (_coverImage) {
            //视频宽高
            _coverImageView = [[UIImageView alloc] initWithFrame:self.bounds];
            _coverImageView.center = CGPointMake(self.width/2, self.height/2);
            _coverImageView.autoresizingMask = UIViewAutoresizingFlexibleHeight|UIViewAutoresizingFlexibleWidth;
             _coverImageView.image = _coverImage;
            [self addSubview:_coverImageView];
        }
    }
    
    CGFloat scale = (_playBtnScale?_playBtnScale.floatValue:1);
    self.playBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.playBtn.frame = CGRectMake(0, 0, 65*scale, 65*scale);
    self.playBtn.center = CGPointMake(self.width/2, self.height/2);
    [self.playBtn setImage:[UIImage imageNamed:@"sc_creat_video_play"] forState:UIControlStateNormal];
    [self.playBtn addTarget:self action:@selector(playAction) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:self.playBtn];
    
    [self addTarget:self singleTapAction:@selector(clickBgAction:)];
}

- (void)clickBgAction:(id)sender
{
    if (_playBtn.hidden == NO) {
        [self.avPlayer play];
        _playBtn.hidden = YES;
        _coverImageView.hidden = YES;
    } else {
        if (self.avPlayer) {
            [self.avPlayer pause];
            _playBtn.hidden = NO;
            _coverImageView.hidden = NO;
            self.time = self.avPlayer.currentTime;
        }
    }
}

- (void)startPlay
{
    [self playAction];
}

- (void)playAction
{
    [self.avPlayer seekToTime:CMTimeMake(0, 1)];
    [self.avPlayer play];
    _playBtn.hidden = YES;
    _coverImageView.hidden = YES;
}

- (void)playbackFinished:(NSNotification *)notify
{
    NSLog(@"视频播放完成.");
    AVPlayerItem *playerItem = notify.object;
    if (playerItem) {
       [self.avPlayer seekToTime:CMTimeMake(0, 1)];
        _playBtn.hidden = NO;
        _coverImageView.hidden = NO;
    }
}

- (void)appWillResignActive:(NSNotification *)notification
{
    if (self.avPlayer) {
        [self.avPlayer pause];
        _playBtn.hidden = NO;
        _coverImageView.hidden = NO;
        self.time = self.avPlayer.currentTime;
    }
}

- (void)appBecomeActive:(NSNotification *)notification
{
//    @try {
//        [self.avPlayer seekToTime:self.time toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero completionHandler:^(BOOL finished) {
//            if (finished) {
//                [DTPubUtil addBlockOnMainThread:^{
//                    _playBtn.hidden = YES;
//                    _coverImageView.hidden = YES;
//                    [self.avPlayer play];
//                }];
//            }
//        }];
//    } @catch (NSException *exception) {
//        [DTPubUtil addBlockOnMainThread:^{
//            _playBtn.hidden = YES;
//            _coverImageView.hidden = YES;
//            [self.avPlayer play];
//        }];
//
//    }
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [self deallocPlayer];
}

- (void)deallocPlayer
{
//    [self.videoPlayer stop];
//    self.videoPlayer = nil;
    [self.avPlayer pause];
    self.avPlayer = nil;
//    self.avPlayerVC = nil;
}

@end
