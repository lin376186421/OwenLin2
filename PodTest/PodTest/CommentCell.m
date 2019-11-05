//
//  CommentCell.m
//  PodTest
//
//  Created by 林 on 16/7/7.
//  Copyright © 2016年 林. All rights reserved.
//

#import "CommentCell.h"


@interface CommentLabel()
{
    UILongPressGestureRecognizer *_ges;
}
@end

@implementation CommentLabel
-(void)awakeFromNib
{
    [super awakeFromNib];
    self.userInteractionEnabled = YES;
    self.multipleTouchEnabled = NO;
}


-(void)longAction:(UILongPressGestureRecognizer *)ges
{
    if (ges.state!=UIGestureRecognizerStateBegan) {
        return;
    }
    
    if (!self.isFirstResponder) {
        [self becomeFirstResponder];
    }
    
    UIMenuController *menuController = [UIMenuController sharedMenuController];
    [menuController setTargetRect:self.frame inView:self.superview];
    NSMutableArray *items = [NSMutableArray array];
    [items addObject:[[UIMenuItem alloc] initWithTitle:@"拷贝" action:@selector(insideCopy:)]];
    [items addObject:[[UIMenuItem alloc] initWithTitle:@"分享" action:@selector(share:)]];
    [items addObject:[[UIMenuItem alloc] initWithTitle:@"举报" action:@selector(report:)]];
    [menuController setMenuItems:[NSArray arrayWithArray:items]];
    [menuController setMenuVisible:YES animated:YES];
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (BOOL)canPerformAction:(SEL)action withSender:(id)sender
{
    if (action == @selector(insideCopy:)||
        action == @selector(report:)||
        action == @selector(share:))
        return YES;
    
    return NO;
}

- (void)insideCopy:(id)sender
{
    NSLog(@"tap %@",sender);
}

- (void)share:(id)sender
{
    NSLog(@"tap %@",sender);
}

- (void)report:(id)sender
{
    NSLog(@"tap %@",sender);
}

- (void)menuWillHide
{
    if (self.alpha!=1.0f) {
        self.alpha = 1.0f;
    }
}

- (void)menuWillShow
{
    if ([self isFirstResponder]) {
        self.alpha = 0.6f;
    }
}
-(BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    return YES;
}

-(void)setText:(NSString *)text
{
    [super setText:text];
    
    if(_ges)
    {
        [self removeGestureRecognizer:_ges];
        _ges = nil;
    }
    
    if(_ges == nil)
    {
        UILongPressGestureRecognizer *ges = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longAction:)];
        
        ges.minimumPressDuration = 1.f;
        [self addGestureRecognizer:ges];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(menuWillHide) name:UIMenuControllerWillHideMenuNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(menuWillShow) name:UIMenuControllerWillShowMenuNotification object:nil];
        
        _ges = ges;
    }
}

// Initialization code

@end

@interface CommentCell()
{
    
    IBOutlet UILabel *nameLab;
    IBOutlet UILabel *numLab;
    IBOutlet CommentLabel *comLab;

}
@end

@implementation CommentCell

- (void)awakeFromNib {
    [super awakeFromNib];
    
   
}



- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

-(void)setObj:(id)obj
{
    self.selectionStyle = UITableViewCellSelectionStyleNone;
    comLab.text = obj;
}

@end
