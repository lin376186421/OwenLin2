//
//  SCChooseView.m
//  RecordVideoDemo
//
//  Created by 林 on 2019/1/22.
//  Copyright © 2019 林. All rights reserved.
//

#import "SCChooseView.h"

@interface SCChooseView()<UITableViewDelegate,UITableViewDataSource>
{
    UITableView *_tableView;
    NSArray *_filterArr;
    NSArray *_musicArr;
}
@end

@implementation SCChooseView

+ (instancetype)showInView:(UIView *)superView
{
    SCChooseView *view = [[SCChooseView alloc] initWithFrame:CGRectMake(0, 0, 100, SCREEN_HEIGHT)];
    [superView addSubview:view];
    return view;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.5];
        _tableView = [[UITableView alloc] initWithFrame:self.bounds];
        _tableView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        _tableView.backgroundColor = [UIColor clearColor];
        _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
        _tableView.delegate = self;
        _tableView.dataSource = self;
        _tableView.showsHorizontalScrollIndicator = NO;
        _tableView.showsVerticalScrollIndicator = NO;
        _tableView.keyboardDismissMode =  UIScrollViewKeyboardDismissModeOnDrag; // 滑动时 收回键盘
        [self addSubview:_tableView];
        
        NSDictionary *config = [self getFilterGeojsonFile];
        _filterArr = [NSArray arrayWithArray:config[@"filters"]];
        _musicArr = [NSArray arrayWithArray:config[@"music"]];
        
        [_tableView reloadData];
    }
    return self;
}

- (NSDictionary *)getFilterGeojsonFile {
    NSDictionary *json_dict;
    NSString *strPath = [[NSBundle mainBundle] pathForResource:@"KJFilter" ofType:@"geojson"];
    if (strPath) {
        NSString *parseJson = [[NSString alloc] initWithContentsOfFile:strPath encoding:NSUTF8StringEncoding error:nil];
        //去除空格
        parseJson = [parseJson stringByReplacingOccurrencesOfString:@"\r\n" withString:@""];
        parseJson = [parseJson stringByReplacingOccurrencesOfString:@"\n" withString:@""];
        parseJson = [parseJson stringByReplacingOccurrencesOfString:@"\t" withString:@""];
        NSData *jsonData = [parseJson dataUsingEncoding:NSUTF8StringEncoding];
        NSError *error;
        json_dict = [NSJSONSerialization JSONObjectWithData:jsonData options:NSJSONReadingMutableContainers error:&error];
    }
    return json_dict;
}

#pragma mark - UITableViewDelegate

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (section == 0) {
        return 0;
        return _musicArr.count;
    }
    return _filterArr.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 100;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    
    return [self tableView:tableView numberOfRowsInSection:section] ? 30 : 0;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {
    if ([self tableView:tableView heightForHeaderInSection:section] > 0) {
        UILabel *label = [UILabel labelWithFrame:CGRectMake(0, 0, tableView.width, 20) fontSize:15 color:[UIColor whiteColor]];
        label.textAlignment = NSTextAlignmentCenter;
        label.text = section == 0?@"音效":@"滤镜";
        return label;
    }
    return nil;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 0) {
        UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"cell1"];
        if (!cell) {
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"cell1"];
            UIImageView *imageView = [[UIImageView alloc] initWithFrame:CGRectMake(10, 10, tableView.width-20, 80)];
            imageView.tag = 111;
            [cell.contentView addSubview:imageView];
            
            UILabel *label = [UILabel labelWithFrame:CGRectMake(0, imageView.bottom, tableView.width, 20) fontSize:17 color:[UIColor blackColor]];
            label.tag = 222;
            label.textAlignment = NSTextAlignmentCenter;
            [cell.contentView addSubview:label];
        }
        NSDictionary *dict = [_musicArr objectAtIndex:indexPath.row];
        UIImageView *imageView = [cell.contentView viewWithTag:111];
        imageView.image = dict[@"image"];
        UILabel *label = [cell.contentView viewWithTag:222];
        label.text = dict[@"name"];
        cell.backgroundColor = cell.contentView.backgroundColor = [UIColor clearColor];
        return cell;
    }
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"cell2"];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"cell2"];
        UIImageView *imageView = [[UIImageView alloc] initWithFrame:CGRectMake(10, 10, tableView.width-20, 100-20)];
        imageView.tag = 111;
        [cell.contentView addSubview:imageView];
    }
    NSDictionary *dict = [_filterArr objectAtIndex:indexPath.row];
    UIImageView *imageView = [cell.contentView viewWithTag:111];
    imageView.image = [self.class sc_imageProcessedUsingGPUImage:[UIImage imageNamed:@"filter.jpg"] withFilterName:dict[@"filter"]];
    cell.backgroundColor = cell.contentView.backgroundColor = [UIColor clearColor];
    return cell;
    return [[UITableViewCell alloc] init];
}

+ (UIImage *)sc_imageProcessedUsingGPUImage:(UIImage *)image
                             withFilterName:(NSString *)filterName {
    GPUImagePicture *stillImageSource = [[GPUImagePicture alloc] initWithImage:image];
    GPUImageOutput<GPUImageInput> *stillImageFilter = [[NSClassFromString(filterName) alloc] init];
    [stillImageSource addTarget:stillImageFilter];
    [stillImageFilter useNextFrameForImageCapture];
    [stillImageSource processImage];
    UIImage *newImage = [stillImageFilter imageFromCurrentFramebuffer];
    return newImage;
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    if (indexPath.section == 1) {
        NSDictionary *dic = _filterArr[indexPath.row];
        if (_selectBlock) {
            _selectBlock(dic);
        }
    }
}


@end
