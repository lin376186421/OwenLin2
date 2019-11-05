//
//  CommentCell.h
//  PodTest
//
//  Created by 林 on 16/7/7.
//  Copyright © 2016年 林. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CommentLabel : UILabel<UIGestureRecognizerDelegate>


@end

@interface CommentCell : UITableViewCell

-(void)setObj:(id)obj;

@end
