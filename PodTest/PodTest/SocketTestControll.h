//
//  SocketTestControll.h
//  PodTest
//
//  Created by 林 on 2017/6/22.
//  Copyright © 2017年 林. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef enum : NSUInteger {
    DT_WEB_STATUS_CONNETING,//连接中。。。
    DT_WEB_STATUS_FAILD,//连接失败
    DT_WEB_STATUS_CLOSE,//连接关闭
    DT_WEB_STATUS_CONNETED,//连接成功
} DT_WEB_STATUS;


@interface SocketTestControll : UIViewController

@property (nonatomic, assign) BOOL isConnet;
@property (nonatomic, assign) DT_WEB_STATUS webStatus;

@end
