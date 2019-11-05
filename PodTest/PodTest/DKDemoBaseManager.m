////
////  DKDemoBaseManager.m
////  DTAnswerKitDemo
////
////  Created by 林 on 2018/1/22.
////  Copyright © 2018年 cheng. All rights reserved.
////
//
//#import "DKDemoBaseManager.h"
//#import "CLGPUVideoCommonManager.h"
//
//@interface DKDemoBaseManager()<CLGPUVideoCommonManagerDelegate>
//{
//    
//}
//@end
//
//@implementation DKDemoBaseManager
//
//+ (void)setup
//{
//    [CLGPUVideoCommonManager setup:[self sharedInstance]];
//}
//
//+ (DKDemoBaseManager *)sharedInstance
//{
//    static id instane = nil;
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        instane = [[DKDemoBaseManager alloc] init];
//    });
//    return instane;
//}
//
//- (CGFloat)gpuVideoRecordMaxTime
//{
//    return 10.f;
//}
//
//- (UIViewController *)screenTopController
//{
//    return [UIApplication sharedApplication].keyWindow.rootViewController;
//}
//
//- (BOOL)needAutoPlayWithRecordSuccessComplete:(SCRecordSuccessBlock)block
//{
//    NSString *videoPath = block();
//    NSLog(@"生成的视频路径：%@",videoPath);
//    return YES;
//}
//
//@end

