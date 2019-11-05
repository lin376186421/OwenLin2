//
//  FFActionSheet.h
//  FFStory
//
//  Created by PageZhang on 16/3/17.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import "FFAlertController.h"
@interface FFActionSheet : FFAlertController

+ (void)callTel:(NSString *)tel
          title:(NSString *)title 
           from:(UIViewController *)from
         sender:(UIView *)sender;

- (BOOL)showInViewController:(UIViewController *)vc sender:(UIView *)sender;

@end
