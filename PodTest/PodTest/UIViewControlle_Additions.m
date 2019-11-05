//
//  UIViewController+UIViewControlle_Additions.m
//  DrumStore
//
//  Created by Medeli on 16/1/28.
//  Copyright © 2016年 Medeli. All rights reserved.
//

#import "UIViewControlle_Additions.h"

@implementation UIViewController (Additions)

-(void)dismissSelfAnimated:(BOOL)flag completion:(void (^)(void))completion
{
	if(self.navigationController != nil)
	{
		[self.navigationController popViewControllerAnimated:flag];
	}
	else
	{
		[self dismissViewControllerAnimated:flag completion:completion];
	}
}

-(void)presentOtherViewController:(UIViewController *)v animate:(BOOL)flag completion:(void (^)(void))completion
{
	if(self.navigationController != nil)
	{
		[self.navigationController pushViewController:v animated:flag];
	}
	else
	{
		[self presentViewController:v animated:flag completion:completion];
	}
}

@end
