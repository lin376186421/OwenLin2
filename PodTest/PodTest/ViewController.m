//
//  ViewController.m
//  TestProgress
//
//  Created by 林 on 16/6/13.
//  Copyright © 2016年 林. All rights reserved.
//

#import "ViewController.h"
#import "ProgressView.h"
#import "TabViewController.h"
#import "ViewController11.h"
#import "ViewController2.h"
#import "ViewController3.h"
#import "TableViewController.h"
#import "CollectViewController.h"
#import "SocketTestControll.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import <MediaPlayer/MediaPlayer.h>
//#import <TestFrameWork/TTViewController.h>
#import <TestFrameWork/TTViewController.h>
#import "TestAClass.h"
#import <AVFoundation/AVFoundation.h>
#import "DrawTest2ViewController.h"
#import "TestClass1.h"
#import "TestTT2.h"
#import <CL_FFmpeg_Framework/CL_FFmpeg_Framework.h>
//#import "CL_GPUVideo.h"
//#import "CLGPUCreateVideoControll.h"
//#import "CLGPUVideoCommonManager.h"

@implementation OWActionSheet



@end

@implementation NumCal

@end

@interface ViewController ()
{
    float perent;
    IBOutlet UIImageView *photoImg;
    ProgressView *pView;
    UIPopoverController *_popVC;
}

@end

int r_count = 110;

@implementation ViewController

/**
 *  十进制 转换其他进制
 *
 *  @param n       需要转换的进制
 *  @param decimal 十进制 值
 *
 *  @return 该进制下的 值
 */

+ (NSInteger)changeSystem:(NSInteger)n withDecimalSystem:(NSInteger)decimal
{
    NSInteger num = decimal;
    NSInteger remainder = 0;      //余数
    NSInteger divisor = 0;        //除数
    NSString * prepare = @"";
    
    while (true)
    {
        remainder = num%n;
        divisor = num/n;
        num = divisor;
        prepare = [prepare stringByAppendingFormat:@"%zd",remainder];
        
        if (divisor == 0)
        {
            break;
        }
    }
    
    NSString * result = @"";
    for (NSInteger i = prepare.length - 1; i >= 0; i --)
    {
        result = [result stringByAppendingFormat:@"%@",
                  [prepare substringWithRange:NSMakeRange(i , 1)]];
    }
    
    return [result integerValue];
}

-(void)test
{
    NSMutableArray *l_arr = [NSMutableArray array];
    int l_count = 9;
    for(int i = 1; i <= l_count; i++)
    {
        [l_arr addObject:[NSString stringWithFormat:@"%zd",i]];
        if(i != l_count)
        {
            [l_arr addObject:@" "];
        }
    }
    
    NSString *int3Str ;
    
    NSMutableArray *enpty_index_arr = [NSMutableArray array];//可插入符号的位置
    for(int i = 1; i < l_count; i++)
    {
        [enpty_index_arr addObject:@(2*i-1)];
    }
    for(int i = 0; i < pow(3, 8);i ++)
    {
        NSInteger int3 = [[self class] changeSystem:3 withDecimalSystem:i];
        int3Str =  [NSString stringWithFormat:@"%zd",int3];
        //补满8位
        if(int3Str.length > 8)
        {
            break;
        }
        while (int3Str.length<l_count-1) {
            int3Str = [@"0" stringByAppendingString:int3Str];
        }
        
        for(int j = 0; j < int3Str.length;j++)
        {
            NSInteger index = [enpty_index_arr[j] integerValue];
            NSString *str;
            if([int3Str characterAtIndex:j] == '0')
            {
                str = @" ";
            }
            else if([int3Str characterAtIndex:j] == '1')
            {
                str = @"+";
            }
            else if([int3Str characterAtIndex:j] == '2')
            {
                str = @"-";
            }
            
            [l_arr replaceObjectAtIndex:index withObject:str];
            
        }
        
        if([self calculate:l_arr])
        {
            NSMutableString *str = [NSMutableString stringWithString:[l_arr componentsJoinedByString:@""]];
            str = (NSMutableString *)[str stringByReplacingOccurrencesOfString:@" " withString:@""];//去空格
            NSLog(@"%@",str);
        }
    }
    
}

-(BOOL)calculate:(NSMutableArray *)arr
{
    NSMutableString *str = [NSMutableString stringWithString:[arr componentsJoinedByString:@""]];
    
    str = (NSMutableString *)[str stringByReplacingOccurrencesOfString:@" " withString:@""];//去空格
    
    NSInteger startIndex = 0;
    NSInteger endIndex = -1;
    NSMutableArray<NumCal *> *numCalArr = [NSMutableArray array];
    char initC = '+';
    for(int i = 0; i < str.length; i++)
    {
        char c = [str characterAtIndex:i];
        
        if(c == '-' || c == '+')
        {
            endIndex = i;
            NSString *numStr = [str substringWithRange:NSMakeRange(startIndex, endIndex-startIndex)];
            NumCal *pNum = [[NumCal alloc] init];
            pNum.num = [numStr integerValue];
            pNum.cal = initC;
            initC = c;
            startIndex = endIndex + 1;
            [numCalArr addObject:pNum];
        }
        
        if(i == str.length-1)
        {
            endIndex = i+1;//最后一位 endindex要向前移一位
            NSString *numStr = [str substringWithRange:NSMakeRange(startIndex, endIndex-startIndex)];
            NumCal *pNum = [[NumCal alloc] init];
            pNum.num = [numStr integerValue];
            pNum.cal = initC;
            initC = c;
            startIndex = endIndex + 1;
            [numCalArr addObject:pNum];
        }
    }
    int leftTotal = 0;
    for(int i = 0; i < numCalArr.count;i ++)
    {
        NumCal *pNum = numCalArr[i];
        if(pNum.cal == '+')
        {
            leftTotal += pNum.num;
        }
        else if(pNum.cal == '-')
        {
            leftTotal -= pNum.num;
        }
    }
    
    if(leftTotal == r_count)
        return YES;
    return NO;
}

- (void)viewDidLoad {
    
    [super viewDidLoad];
//    [self.extensionContext.inputItems firstObject];
    
//    [self test];
    
//    self.edgesForExtendedLayout = UIRectEdgeNone;self.navigationItemself.navigationItemssself.navigationItssdddfff
    
    [self.navigationController setNavigationBarHidden:NO];
    self.navigationController.navigationBar.translucent = NO;
    self.title = @"PodTest";
    
    
     pView = [[ProgressView alloc] initWithFrame:CGRectMake(50, 50, 100, 100)];
//    pView.percent = 0;
    [self.view addSubview:pView];
    perent = 0;
    
    [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(timer) userInfo:nil repeats:YES];
    
    CGSize size = [self.topLabel sizeThatFits:CGSizeMake(self.topLabel.size.width, MAXFLOAT)];
    
    self.topLabel.size = size;
    
    self.topLabel.backgroundColor = [UIColor blueColor];
    self.topLabel.layer.masksToBounds = YES;
    self.topLabel.layer.cornerRadius = 4;
    
    self.bottomLabel.backgroundColor = [UIColor greenColor];
    self.bottomLabel.layer.masksToBounds = YES;
    self.bottomLabel.layer.cornerRadius = 4;
    
    self.bottomLabel.font = [UIFont boldSystemFontOfSize:10];
    
    
    UIView *view = [[UIView alloc] initWithFrame:CGRectZero];
    view.size = CGSizeMake(self.view.width/3, 100);
    view.center = CGPointMake(self.view.width/2, self.view.height/2);
    view.backgroundColor = [UIColor yellowColor];
    view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin;//水平方向 三个变量 左  宽度 右 那个要变就约束拿个
    [self.view addSubview:view];
    
    UITapGestureRecognizer *ges = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapAction:)];
    [view addGestureRecognizer:ges];
//    [TestClass1 logText:@""];
//    [TestClass1 alloc];
//    TestClass1 *obj = [[TestClass1 alloc] init];
    [TestTT2 log:@""];
    
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)tapAction:(id)tap
{
    UIStoryboard *story = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    CollectViewController *vc = [story instantiateViewControllerWithIdentifier:@"CollectViewController"];
    [self presentOtherViewController:vc animate:YES completion:^{
        
    }];
}

- (IBAction)testAction:(id)sender {
    
    UIStoryboard *story = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    UIViewController *vc = nil;
    UIButton *button = (UIButton *)sender;
    if(button.tag == 1)
    {
        TabViewController *tabVC = [story instantiateViewControllerWithIdentifier:@"TabViewController"];
        ViewController11 *tabVC_1 = [story instantiateViewControllerWithIdentifier:@"ViewController11"];
        ViewController2 *tabVC_2 = [story instantiateViewControllerWithIdentifier:@"ViewController2"];
        ViewController3 *tabVC_3 = [story instantiateViewControllerWithIdentifier:@"ViewController3"];
        NSArray *list = [NSArray arrayWithObjects:tabVC_1,tabVC_2,tabVC_3, nil];
        tabVC.controllerList = list;
        vc = tabVC;
    }
    else if(button.tag == 2)
    {
        vc = [story instantiateViewControllerWithIdentifier:@"CollectViewController"];
//        vc = [[SCCreateShortVideoController alloc] init];
//        UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:vc];
//        [self.navigationController presentViewController:nav animated:YES completion:nil];
//        [self click:nil];
        return;
    }
    else if (button.tag == 3)
    {
        vc = [story instantiateViewControllerWithIdentifier:@"TableViewController"];
    }
    else if (button.tag == 4)
    {
        vc = [story instantiateViewControllerWithIdentifier:@"CommentViewController"];
    }
    else if (button.tag == 5)
    {
        UIImage *image = [UIImage imageNamed:@"Test.bundle/pop.png"];
        [TestAClass printA];
        vc = [TTViewController createSelf];//
//        vc = [story instantiateViewControllerWithIdentifier:@"SearchUAViewController"];
    } else if (button.tag == 6) {
        vc = [DrawTest2ViewController new];
    } else if (button.tag == 7) {
//        vc = [SocketTestControll new];
        vc = [story instantiateViewControllerWithIdentifier:@"SocketTestControll"];
    }
    
    if(vc)
    {
        [self presentOtherViewController:vc animate:YES completion:nil];
    }
}

//- (void)click:(id)sender {
//    WEAK_SELF
//    UIAlertController *vc = [UIAlertController alertControllerWithTitle:@"选择主题" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
//    [vc addAction:[UIAlertAction actionWithTitle:@"1" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
//        [weakSelf chooseId:0];
//    }]];
//    [vc addAction:[UIAlertAction actionWithTitle:@"2" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
//        [weakSelf chooseId:1];
//    }]];
//    [vc addAction:[UIAlertAction actionWithTitle:@"3" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
//        [weakSelf chooseId:2];
//    }]];
//    [vc addAction:[UIAlertAction actionWithTitle:@"4" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
//        [weakSelf chooseId:3];
//    }]];
//    [vc addAction:[UIAlertAction actionWithTitle:@"5" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
//        [weakSelf chooseId:4];
//    }]];
//    [vc addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
//        [vc dismissViewControllerAnimated:YES completion:nil];
//    }]];
//    [self presentViewController:vc animated:YES completion:NULL];
//}
//
//- (void)chooseId:(NSUInteger)gId
//{
//    CLGPUCreateVideoControll *vc = [[CLGPUCreateVideoControll alloc] init];
//    vc.stuffItem = [self.class createLocalStuffItemWithSourceId:[@(gId) stringValue]];
//    UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:vc];
//    [self presentViewController:nav animated:YES completion:nil];
//}
//
//+ (CLGPUCreateVideoStuffItem *)createLocalStuffItemWithSourceId:(NSString *)sourceId
//{
//    //    NSString *rootPath = [NSString stringWithFormat:@"gif_resource/%@%@",@"create_video_local",sourceId];
//    //    NSString *rootPath = [[NSBundle mainBundle] pathForResource:@"config" ofType:@"plist" inDirectory:rootPath];
//
//    NSString *sourceIdPath = [NSString stringWithFormat:@"gif_resource/%@%@",@"create_video_local",sourceId];
//    NSString *sourcePath = [[NSBundle mainBundle] pathForResource:@"config" ofType:@"plist" inDirectory:sourceIdPath];
//    NSString *rootPath =  [sourcePath stringByDeletingLastPathComponent];
//
//    NSString *configPlistPath = [rootPath stringByAppendingPathComponent:@"config.plist"];
//    if (configPlistPath.length) {
//        NSDictionary *dic = [NSDictionary dictionaryWithContentsOfURL:[NSURL fileURLWithPath:configPlistPath]];
//        CLGPUCreateVideoStuffItem *item = [CLGPUCreateVideoStuffItem yy_modelWithDictionary:dic];
//        item.rootPath = rootPath;
//        item.sourceId = sourceId;
//        return item;
//    }
//    return nil;
//}

- (IBAction)import:(id)sender {
    
    [[FFmpegManager sharedManager] converWithCommandStr:nil processBlock:^(float process) {
        
    } completionBlock:^(NSError *error) {
        
    }];
    
    OWActionSheet *sheet = [[OWActionSheet alloc] initWithTitle:@"上传图片或视频" delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil otherButtonTitles:nil, nil];
    [sheet addButtonWithTitle:@"拍照"];
    [sheet addButtonWithTitle:@"相册"];
    [sheet addButtonWithTitle:@"取消"];
    [sheet setCancelButtonIndex:sheet.numberOfButtons - 1];
    [sheet showInView:self.view];
}

- (void)actionSheet:(OWActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    NSString *btnTitle = [actionSheet buttonTitleAtIndex:buttonIndex];
    
    if ([btnTitle isEqualToString:@"取消"]) {
        return;
    }
    
    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (status == AVAuthorizationStatusDenied || status == AVAuthorizationStatusRestricted) {
        return;
    }
    
    UIImagePickerControllerSourceType type;
    if([btnTitle isEqualToString:@"拍照"]){
        type = UIImagePickerControllerSourceTypeCamera;
    }
    else if ([btnTitle isEqualToString:@"相册"]){
        type = UIImagePickerControllerSourceTypePhotoLibrary;
    }
    
    UIImagePickerController *vc = [[UIImagePickerController alloc] init];
    vc.delegate = self;
    vc.allowsEditing = YES;
    vc.sourceType = type;
    vc.mediaTypes = [UIImagePickerController availableMediaTypesForSourceType:UIImagePickerControllerSourceTypePhotoLibrary];
    if((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)){
        [self.navigationController presentViewController:vc animated:YES completion:^{
            
        }];
    }
    else{
        _popVC = [[UIPopoverController alloc] initWithContentViewController:vc];
        CGRect rect = [self.view convertRect:photoImg.bounds fromView:photoImg];
        [self dismissViewControllerAnimated:YES completion:^{
            
        }];
        [_popVC presentPopoverFromRect:rect inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
    }
}


- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<NSString *,id> *)info
{
    if ([[info objectForKey:UIImagePickerControllerMediaType] isEqualToString:@"public.movie"]) {
        NSURL *videoURL = [info objectForKey:UIImagePickerControllerMediaURL];
        //获取视频的thumbnail
        MPMoviePlayerController *player = [[MPMoviePlayerController alloc] initWithContentURL:videoURL];
        UIImage *image = [player thumbnailImageAtTime:0 timeOption:MPMovieTimeOptionNearestKeyFrame];
        photoImg.image = image;
        [picker dismissViewControllerAnimated:YES completion:nil];
    } else {
        UIImage *image = [info valueForKey:UIImagePickerControllerOriginalImage];
        [picker dismissViewControllerAnimated:YES completion:nil];
        photoImg.image = image;
    }
    
    if (_popVC.popoverVisible) {
        [_popVC dismissPopoverAnimated:YES];
        _popVC = nil;
    }
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker{
    [picker dismissViewControllerAnimated:YES completion:^{}];
}

-(void)timer
{
    perent  += 10;
    pView.percent = perent/100;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
