//
//  SCGPUCreatVideoController.h
//  SuperCoach
//
//  Created by 林 on 2018/12/20.
//  Copyright © 2018 Lin Feihong. All rights reserved.
//

#import <UIKit/UIKit.h>


typedef NS_ENUM(NSInteger, SCGPUCreateVideoRecordState) {
    SCGPUCreateVideoRecordStateInit = 0,
    SCGPUCreateVideoRecordStateRecording,
    SCGPUCreateVideoRecordStateFinish,
};

@interface SCGPUCreatVideoController : UIViewController

@property (nonatomic, strong) NSString *sourceId;

@end

