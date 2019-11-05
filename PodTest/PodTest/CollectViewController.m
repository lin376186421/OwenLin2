//
//  CollectViewController.m
//  PodTest
//
//  Created by 林 on 16/6/22.
//  Copyright © 2016年 林. All rights reserved.
//

#import "CollectViewController.h"
//#import "SFFocusViewLayout.h"

@interface CollectViewController ()
{
    
}

@end

@implementation CollectViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
//    SFFocusViewLayout *layout = [[SFFocusViewLayout alloc] init];
//    layout.itemSize = CGSizeMake(40, 40);
//    layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    
//   [self.collectionView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:@"CollectionCell"];
    [self.collectionView registerClass:[UICollectionReusableView class] forSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@"Identifierhead"];
    
    
    [self.collectionView setCollectionViewLayout:layout];
    
    [self.collectionView setBackgroundColor:[UIColor whiteColor]];
    
    self.title = @"test";

    
    // Do any additional setup after loading the view.
}

#pragma mark UICollectionDelegate

-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return 131;
}

-(NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 1;
}

-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CollectionCell" forIndexPath:indexPath];
    if(cell == nil)
    {
        cell = [UICollectionViewCell new];
    }
     cell.backgroundColor = [ UIColor colorWithRed :(( arc4random ()% 255 )/ 255.0 ) green :(( arc4random ()% 255 )/ 255.0 ) blue :(( arc4random ()% 255 )/ 255.0 ) alpha : 1.0f ];
    cell.layer.masksToBounds = YES;
    cell.layer.cornerRadius = 3;
    cell.layer.borderColor = [UIColor blackColor].CGColor;
    cell.layer.borderWidth = 2;
    
    UILabel *label = [cell viewWithTag:1];
    if(label)
    {
        [label setText:[NSString stringWithFormat:@"%zd",indexPath.item]];
    }
    
    return cell;
}

-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewCell * cell = ( UICollectionViewCell *)[collectionView cellForItemAtIndexPath :indexPath];
    
    cell. backgroundColor = [ UIColor colorWithRed :(( arc4random ()% 255 )/ 255.0 ) green :(( arc4random ()% 255 )/ 255.0 ) blue :(( arc4random ()% 255 )/ 255.0 ) alpha : 1.0f ];
}

-(BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    return YES;
}

//行间距最小值
-(CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section
{
    return 5;
}

//item间距最小值
-(CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section
{
    return 20;
}

//组的头视图创建
- (UICollectionReusableView *)collectionView:(UICollectionView *)collectionView viewForSupplementaryElementOfKind:(NSString *)kind atIndexPath:(NSIndexPath *)indexPath
{
    //头
     if (kind == UICollectionElementKindSectionHeader)
     {
         
     }
    UICollectionReusableView *headView = [collectionView dequeueReusableSupplementaryViewOfKind:kind withReuseIdentifier:@"Identifierhead" forIndexPath:indexPath];
    
    if(headView == nil)
    {
        headView = [[UICollectionReusableView alloc] init];
    }
    headView.backgroundColor = [UIColor blueColor];
    return headView;
}

//section 头
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout referenceSizeForHeaderInSection:(NSInteger)section
{
    return CGSizeMake(20, 20);
}

//item size 但是结合item的间距算一排放几个
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    CGFloat rwd = arc4random()%130 + 40;
    CGFloat rhd = arc4random()%130 + 40;
    
    rwd = self.view.bounds.size.width/4-1;
    rhd = rwd;
    return CGSizeMake(rwd, rhd);
}

-(UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section
{
    return UIEdgeInsetsMake(0, 0, 0,0);
}

@end
