//
//  TTViewController.h
//  TestFrameWork
//
//  Created by 林 on 17/2/8.
//  Copyright © 2017年 林. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TTViewController : UIViewController

+ (TTViewController *)createSelf;
+ (NSBundle *)getBundle;
+ (NSString *)getBundlePath: (NSString *) assetName;

@end
