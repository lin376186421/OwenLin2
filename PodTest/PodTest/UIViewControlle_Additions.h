//
//  UIViewController+UIViewControlle_Additions.h
//  DrumStore
//
//  Created by Medeli on 16/1/28.
//  Copyright © 2016年 Medeli. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIViewController (Additions)

- (void)dismissSelfAnimated: (BOOL)flag completion: (void (^ __nullable)(void))completion ;

-(void)presentOtherViewController:(UIViewController *)v animate:(BOOL)flag completion:(void (^)(void))completion;

@end
