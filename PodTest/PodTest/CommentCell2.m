//
//  CommentCell2.m
//  PodTest
//
//  Created by 林 on 16/7/7.
//  Copyright © 2016年 林. All rights reserved.
//

#import "CommentCell2.h"
#import <YYText.h>
@interface CommentCell2()
{
    
    IBOutlet UILabel *nameLab;
    IBOutlet UILabel *numLab;
    IBOutlet YYLabel *comLab;
}

@end

@implementation CommentCell2


- (void)awakeFromNib {
    [super awakeFromNib];
//    [comLab setNumberOfLines:10];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

UIColor * randomColor() {
    return [UIColor colorWithRed:arc4random()%256/255.f green:arc4random()%256/255.f blue:arc4random()%256/255.f alpha:1.0];
}

-(void)setObj:(id)obj
{
    NSMutableAttributedString *tag = [NSMutableAttributedString new];
    for (NSString *item in obj) {
        YYTextBorder *border = [YYTextBorder borderWithLineStyle:YYTextLineStyleSingle lineWidth:0.5 strokeColor:randomColor()];
        border.cornerRadius = 1;
        border.insets = UIEdgeInsetsMake(0, 3, 0, 3);
        
        NSMutableAttributedString *one = [[NSMutableAttributedString alloc] initWithString:[NSString stringWithFormat:@"   %@   ", item]];
        one.yy_font = [UIFont systemFontOfSize:10];
        one.yy_color = border.strokeColor;
        one.yy_textBackgroundBorder = border;
        [tag appendAttributedString:one];
    }
    comLab.attributedText = tag;
}

@end
