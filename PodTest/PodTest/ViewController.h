//
//  ViewController.h
//  TestProgress
//
//  Created by 林 on 16/6/13.
//  Copyright © 2016年 林. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface OWActionSheet : UIActionSheet

@end

@interface ViewController : UIViewController<UIActionSheetDelegate,UINavigationControllerDelegate,UIImagePickerControllerDelegate>

@property (weak, nonatomic) IBOutlet UILabel *topLabel;

@property (weak, nonatomic) IBOutlet UILabel *bottomLabel;
@end


@interface NumCal : NSObject;

@property(nonatomic)  NSInteger num;
@property(nonatomic) char cal;

@end