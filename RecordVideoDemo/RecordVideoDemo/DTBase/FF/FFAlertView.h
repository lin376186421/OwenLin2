//
//  FFAlertView.h
//  FFStory
//
//  Created by PageZhang on 16/3/17.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import "FFAlertController.h"
@interface FFAlertView : FFAlertController

+ (void)callTel:(NSString *)tel
          title:(NSString *)title
           from:(UIViewController *)from;

@end
