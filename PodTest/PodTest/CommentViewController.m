//
//  CommentViewController.m
//  PodTest
//
//  Created by 林 on 16/7/7.
//  Copyright © 2016年 林. All rights reserved.
//

#import "CommentViewController.h"
#import "CommentCell.h"
#import "CommentCell2.h"
#import "UITableView+FDTemplateLayoutCell.h"

#define  CELL 0

//
//-(UILabel *)addAnimationLabel
//{
//    UILabel *label = [UILabel new];
//    [_kjzView addSubview:label];
//    label.font = [UIFont systemFontOfSize:15];
//    label.textColor = [UIColor whiteColor];
//    label.text = @" 车轮考驾照 学车特简单 ";
//    [label mas_makeConstraints:^(MASConstraintMaker *make) {
//        make.top.equalTo(_kjzView);
//        make.bottom.equalTo(_kjzView);
//        make.centerY.equalTo(_kjzView);
//        make.leading.equalTo(_kjzView.mas_trailing);
//    }];
//    float dis = [label getTextWidth]+_kjzView.width;
//    ;
//    [UIView animateWithDuration:5 delay:0 options:UIViewAnimationOptionCurveLinear animations:^{
//        label.transform = CGAffineTransformMakeTranslation(-dis, 0);
//    } completion:^(BOOL finished) {
//        [label removeFromSuperview];
//    }];
//    return label;
//}
//
//-(void)kjzViewAnimation
//{
//    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
//        UILabel *label = [self addAnimationLabel];
//        float dis = [label getTextWidth]+_kjzView.width;
//        float t = [label getTextWidth]/(dis/5);
//        if(_timer){
//            [_timer invalidate];
//            _timer = nil;
//        }
//        _timer = [NSTimer bk_scheduleTimerWithTimeInterval:t repeats:YES usingBlock:^(NSTimer * _Nonnull timer) {
//            [self addAnimationLabel];
//        }];
//    });
//}


//UIViewController *inputVC = [UIViewController new];
//UIView *inputView = inputVC.view;
//inputView.translatesAutoresizingMaskIntoConstraints = NO;
//UIView *windowView = [[UIApplication sharedApplication].delegate window];
////    inputView.layer.masksToBounds = YES;
//[windowView addSubview:inputView];
//
//
////    [self.view insertSubview:inputAccessoryView belowSubview:_mediaControl];
//
//
//[windowView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[inputView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(inputView)]];
//_constraint_height = [NSLayoutConstraint constraintWithItem:inputView attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1 constant:50];
//_constraint_bottom = [NSLayoutConstraint constraintWithItem:windowView attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:inputView attribute:NSLayoutAttributeBottom multiplier:1 constant:0];
//[windowView addConstraints:@[_constraint_height, _constraint_bottom]];
//
//DTMediaInputView *inputAccessoryView = [DTMediaInputView new];
//_inputAccessoryView = inputAccessoryView;
//inputAccessoryView.translatesAutoresizingMaskIntoConstraints = NO;
//[inputView addSubview:inputAccessoryView];
//[inputView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[_inputAccessoryView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(_inputAccessoryView)]];
//[inputView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[_inputAccessoryView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(_inputAccessoryView)]];
//
//[self updateInputToReply:nil];


@interface CommentViewController ()<UITableViewDelegate,UITableViewDataSource>
{
    NSMutableArray *messageArr;
}
@property (strong, nonatomic) IBOutlet UITableView *tableview;

@end

@implementation CommentViewController

-(NSString *)createRandomMessage
{
    NSMutableString *message =[NSMutableString string];
    NSInteger count = arc4random() % 500 + 10;
    for(int i = 0; i < count; i++)
    {
        [message appendFormat:@"a"];
    }
    return message;
}


-(NSString *)creatStr:(NSInteger)maxCount
{
    NSString *string = [[NSString alloc]init];
    for (int i = 0; i < maxCount; i++) {
        int number = arc4random() % 36;
        if (number < 10) {
            int figure = arc4random() % 10;
            NSString *tempString = [NSString stringWithFormat:@"%d", figure];
            string = [string stringByAppendingString:tempString];
        }else {
            int figure = (arc4random() % 26) + 97;
            char character = figure;
            NSString *tempString = [NSString stringWithFormat:@"%c", character];
            string = [string stringByAppendingString:tempString];
        }
    }
    return string;
}

-(NSArray *)getMessageArr
{
    NSInteger count = arc4random()%10;
    NSMutableArray *arr = [NSMutableArray array];
    for (int  i =0; i < count; i++) {
        [arr addObject:[self creatStr:arc4random() % 15 + 3]];
    }
    return arr;
}

-(void)changeScreen:(NSNotification*)no
{
    [_tableview reloadData];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    if(CELL == 0)
    {
        messageArr = [NSMutableArray array];
        for(int i = 0; i < 30; i ++){
            [messageArr addObject:[self creatStr:arc4random()%500 + 10]];
        }
        [_tableview registerNib:[UINib nibWithNibName:@"CommentCell" bundle:nil] forCellReuseIdentifier:@"CommentCell"];
    }
    else if (CELL == 1)
    {
        messageArr = [NSMutableArray array];
        for(int i = 0; i < 30; i ++){
            [messageArr addObject:[self getMessageArr]];
        }
        [_tableview registerNib:[UINib nibWithNibName:@"CommentCell2" bundle:nil] forCellReuseIdentifier:@"CommentCell2"];
    }
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(changeScreen:) name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];
    [_tableview reloadData];
    // Do any additional setup after loading the view.
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
//    return [tableView fd_heightForCellWithIdentifier:@"CommentCell" cacheByIndexPath:indexPath configuration:^(CommentCell *cell) {
//        [cell setObj:[messageArr objectAtIndex:indexPath.row]];
//    }];;//使用缓存
    if(CELL ==0 )
    {
        return [tableView fd_heightForCellWithIdentifier:@"CommentCell" configuration:^(CommentCell* cell) {
            [cell setObj:[messageArr objectAtIndex:indexPath.row]];
        }];//不使用缓存
        
    }
    else if (CELL == 1)
    {
        return [tableView fd_heightForCellWithIdentifier:@"CommentCell2" configuration:^(CommentCell2* cell) {
            [cell setObj:[messageArr objectAtIndex:indexPath.row]];
        }];//不使用缓存
        
    }
    
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return messageArr.count;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (CELL == 0) {
        CommentCell *cell = [tableView dequeueReusableCellWithIdentifier:@"CommentCell"];
        [cell setObj:[messageArr objectAtIndex:indexPath.row]];
        return cell;
    } else {
        CommentCell2 *cell = [tableView dequeueReusableCellWithIdentifier:@"CommentCell2"];
        [cell setObj:[messageArr objectAtIndex:indexPath.row]];
        return cell;
    }
}


@end
