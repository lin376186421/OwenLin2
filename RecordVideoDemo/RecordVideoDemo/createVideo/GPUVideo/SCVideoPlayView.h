//
//  SCVideoPlayView.h
//  SuperCoach
//
//  Created by 林 on 2019/1/3.
//  Copyright © 2019 Lin Feihong. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SCVideoPlayView : UIView

@property (nonatomic, strong) NSURL *videoUrl;
@property (nonatomic, assign) BOOL hideCoverImage;//是否隐藏封面 针对本地视频 默认显示
@property (nonatomic, strong) NSNumber *playBtnScale;
@property (nonatomic, strong) UIImage *coverImage;

- (void)startPlay;
- (void)deallocPlayer;

@end
