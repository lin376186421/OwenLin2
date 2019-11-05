//
//  TestClass1.h
//  PodTestSubTarget
//
//  Created by 林 on 2017/9/26.
//  Copyright © 2017年 林. All rights reserved.
//

#import <Foundation/Foundation.h>

//$(PROJECT_DIR)/PodTest/dependency/PodTestSubTarget 主工程添加的路径 用 recursive

@interface TestClass1 : NSObject

+ (void)logText:(NSString*)text;

@end
