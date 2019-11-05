//
//  UILabel+Additions.m
//  DrivingTest
//
//  Created by cheng on 15/12/31.
//  Copyright © 2015年 eclicks. All rights reserved.
//

#import "UILabel+Additions.h"
#import <CoreText/CoreText.h>

//CTFontRef CTFontCreateFromUIFont(UIFont *font) {
//    if (!font) return NULL;
//    CTFontRef ctFont = CTFontCreateWithName((__bridge CFStringRef)font.fontName, font.pointSize, NULL);
//    return ctFont;
//}

@implementation UILabel (Additions)

//- (CGSize)measureSize
//{
//    NSDictionary *attributes = @{ NSFontAttributeName: self.font };
//    CGSize measureSize = [self.text sizeWithAttributes:attributes];
//    return measureSize;
//}

+ (id)labelWithFrame:(CGRect)frame font:(UIFont *)font color:(UIColor *)color
{
    UILabel *label = [[self alloc] initWithFrame:frame];
    label.backgroundColor = [UIColor clearColor];
    if (color) {
        label.textColor = color;
    }
    label.font = font;
    return label;
}

+ (id)labelWithFrame:(CGRect)frame font:(UIFont *)font colorString:(NSString *)colorString
{
    return [self labelWithFrame:frame font:font color:[UIColor colorWithHexString:colorString]];
}

+ (id)labelWithFrame:(CGRect)frame fontSize:(CGFloat)fontSize color:(UIColor *)color
{
    return [self labelWithFrame:frame font:[UIFont systemFontOfSize:fontSize] color:color];
}

+ (id)labelWithFrame:(CGRect)frame fontSize:(CGFloat)fontSize colorString:(NSString *)colorString
{
    return [self labelWithFrame:frame font:[UIFont systemFontOfSize:fontSize] color:[UIColor colorWithHexString:colorString]];
}

- (void)setText:(NSString *)text highLightText:(NSString *)string highLightTextColor:(UIColor *)color
{
    NSRange range = [text rangeOfString:string];
    if (range.length && color) {
        NSMutableAttributedString *attString = [[NSMutableAttributedString alloc] initWithString:text];
        [attString addAttribute:(id)NSForegroundColorAttributeName value:(id)color range:range];
        self.attributedText = attString;
    } else {
        self.text = text;
    }
}

- (void)setLineBreakByTruncatingLastTailIndent:(CGFloat)tailIndent
{
    if ( self.numberOfLines <= 1) {
        return;
    }
    NSArray *separatedLines = [self getSeparatedLinesArray];
    
    NSMutableString *limitedText = [NSMutableString string];
    if ( separatedLines.count >= self.numberOfLines ) {
        
        for (int i = 0 ; i < self.numberOfLines; i++) {
            if ( i == self.numberOfLines - 1) {
                UILabel *lastLineLabel = [[UILabel alloc]initWithFrame:CGRectMake(0, 0, self.frame.size.width-tailIndent, MAXFLOAT)];
                lastLineLabel.text = separatedLines[self.numberOfLines - 1];
                
                NSArray *subSeparatedLines = [lastLineLabel getSeparatedLinesArray];
                NSString *lastLineText = [subSeparatedLines firstObject];
                NSInteger lastLineTextCount = lastLineText.length;
                CGSize size = [lastLineLabel measureSize];
                if (size.width>self.frame.size.width-tailIndent) {
                    [limitedText appendString:[NSString stringWithFormat:@"%@...",[lastLineText substringToIndex:lastLineTextCount]]];
                } else {
                    [limitedText appendString:lastLineText];
                }
            }else{
                [limitedText appendString:separatedLines[i]];
            }
        }
    }else{
        [limitedText appendString:self.text];
    }
    
    self.text = limitedText;
}

- (CGSize)measureSize {
    NSDictionary *attributes = @{ NSFontAttributeName: self.font };
    CGSize measureSize = [self.text sizeWithAttributes:attributes];
    return measureSize;
}

- (BOOL)lastLineShouldTailIndent:(CGFloat)tailIndent
{
    BOOL ret = NO;
    if ( self.numberOfLines <= 1) {
        return ret;
    }
    NSArray *separatedLines = [self getSeparatedLinesArray];
    
    NSMutableString *limitedText = [NSMutableString string];
    if ( separatedLines.count >= self.numberOfLines ) {
        
        for (int i = 0 ; i < self.numberOfLines; i++) {
            if ( i == self.numberOfLines - 1) {
                UILabel *lastLineLabel = [[UILabel alloc]initWithFrame:CGRectMake(0, 0, self.frame.size.width-tailIndent, MAXFLOAT)];
                lastLineLabel.text = separatedLines[self.numberOfLines - 1];
                
                NSArray *subSeparatedLines = [lastLineLabel getSeparatedLinesArray];
                NSString *lastLineText = [subSeparatedLines firstObject];
                NSInteger lastLineTextCount = lastLineText.length;
                CGSize size = [lastLineLabel measureSize];
                if (size.width>self.frame.size.width-tailIndent) {
                    [limitedText appendString:[NSString stringWithFormat:@"%@...",[lastLineText substringToIndex:lastLineTextCount]]];
                    ret = YES;
                } else {
                    [limitedText appendString:lastLineText];
                }
            }else{
                [limitedText appendString:separatedLines[i]];
            }
        }
    }else{
        [limitedText appendString:self.text];
    }
    
    self.text = limitedText;
    
    return ret;
}

- (NSArray *)getSeparatedLinesArray
{
    NSString *text = [self text];
    UIFont   *font = [self font];
    CGRect    rect = [self frame];
    
    CTFontRef myFont = CTFontCreateWithName((__bridge CFStringRef)([font fontName]), [font pointSize], NULL);
    NSMutableAttributedString *attStr = [[NSMutableAttributedString alloc] initWithString:text];
    [attStr addAttribute:(NSString *)kCTFontAttributeName value:(__bridge id)myFont range:NSMakeRange(0, attStr.length)];
    
    CTFramesetterRef frameSetter = CTFramesetterCreateWithAttributedString((__bridge CFAttributedStringRef)attStr);
    
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathAddRect(path, NULL, CGRectMake(0,0,rect.size.width,100000));
    
    CTFrameRef frame = CTFramesetterCreateFrame(frameSetter, CFRangeMake(0, 0), path, NULL);
    
    NSArray *lines = (__bridge NSArray *)CTFrameGetLines(frame);
    NSMutableArray *linesArray = [[NSMutableArray alloc]init];
    
    for (id line in lines)
    {
        CTLineRef lineRef = (__bridge CTLineRef )line;
        CFRange lineRange = CTLineGetStringRange(lineRef);
        NSRange range = NSMakeRange(lineRange.location, lineRange.length);
        
        NSString *lineString = [text substringWithRange:range];
        [linesArray addObject:lineString];
    }
    return (NSArray *)linesArray;
}

- (void)textAlignmentLeftAndRightWith:(CGFloat)labelWidth{
    
    CGSize size = [self.text boundingRectWithSize:CGSizeMake(labelWidth,MAXFLOAT)  options:NSStringDrawingUsesLineFragmentOrigin |NSStringDrawingTruncatesLastVisibleLine|NSStringDrawingUsesFontLeading  attributes:@{NSFontAttributeName :self.font}  context:nil].size;
    
    CGFloat margin = (labelWidth - size.width)/(self.text.length - 1);
    
    NSNumber *number = [NSNumber  numberWithFloat:margin];
    
    NSMutableAttributedString *attribute = [[NSMutableAttributedString  alloc] initWithString:self.text];
    
    [attribute  addAttribute:NSKernAttributeName  value:number range:NSMakeRange(0,self.text.length -1 )];
    
    self.attributedText = attribute;
}

@end
//
//@implementation UILabel (Attributed)
//
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withFont:(UIFont *)font
//{
//    [self setText:text highLightText:highLightText withColor:nil withFont:font];
//}
//
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withColor:(UIColor *)color
//{
//    [self setText:text highLightText:highLightText withColor:color withFont:nil];
//}
//
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withColor:(UIColor *)color withFont:(UIFont *)font
//{
//    self.attributedText = [NSAttributedString string:text rangeText:highLightText font:font color:color];
//}
//
//- (void)setText:(NSString *)text highLightTextArray:(NSArray *)highLightTextArray withColor:(UIColor *)color
//{
//    if (text && color) {
//        NSMutableAttributedString *mString = [NSMutableAttributedString stringWithString:text];
//        NSInteger loc = 0;
//        NSString *ctext = [text copy];
//        for (NSString *string in highLightTextArray) {
//            NSRange range = [ctext rangeOfString:string];
//            range.location = loc+range.location;
//            if (range.length>0) {
//                [mString addAttributeColor:color range:range];
//            }
//            //从后面截断，防止高亮数字有相同的，比如 @[@"500", @"0"] 都有0
//            loc = range.location+range.length;
//            if (loc<text.length) {
//                ctext = [text substringFromIndex:loc];
//            } else {
//                ctext = @"";
//            }
//        }
//        self.attributedText = mString;
//        return;
//    }
//    self.text = text;
//}
//
//@end

//@implementation TTTAttributedLabel (HighLightText)
//
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withFont:(UIFont *)font
//{
//    NSRange range = [text rangeOfString:highLightText];
//    [self setText:text afterInheritingLabelAttributesAndConfiguringWithBlock:^NSMutableAttributedString *(NSMutableAttributedString *mutableAttributedString) {
//        if (font) {
//            CTFontRef ctFont = CTFontCreateWithName((__bridge CFStringRef)font.fontName, font.pointSize, NULL);
//            [mutableAttributedString addAttribute:(id)kCTFontAttributeName value:(__bridge id _Nonnull)(ctFont) range:range];
//        }
//        return mutableAttributedString;
//    }];
//}
//
//- (void)setText:(NSString *)text highLightRange:(NSRange)range withColor:(UIColor *)color
//{
//    [self setText:text afterInheritingLabelAttributesAndConfiguringWithBlock:^NSMutableAttributedString *(NSMutableAttributedString *mutableAttributedString) {
//        if (color) {
//            [mutableAttributedString addAttribute:(id)kCTForegroundColorAttributeName value:(id)color.CGColor range:range];
//        }
//        return mutableAttributedString;
//    }];
//}
//
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withColor:(UIColor *)color
//{
//    if (text&&highLightText&&color) {
//        NSRange range = [text rangeOfString:highLightText];
//        if (range.length>0) {
//            [self setText:text highLightRange:range withColor:color];
//            return;
//        }
//    }
//    [self setText:text];
//}
//
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withColor:(UIColor *)color withFont:(UIFont*)font
//{
//    if (text&&highLightText&&color&&font) {
//        NSRange range = [text rangeOfString:highLightText];
//        [self setText:text afterInheritingLabelAttributesAndConfiguringWithBlock:^NSMutableAttributedString *(NSMutableAttributedString *mutableAttributedString) {
//            if (font&&color) {
//                [mutableAttributedString addAttributes:@{(id)kCTForegroundColorAttributeName:(id)color.CGColor,(id)kCTFontAttributeName:(__bridge id)CTFontCreateFromUIFont(font)} range:range];
//            }
//            return mutableAttributedString;
//        }];
//        return;
//    }
//    [self setText:text];
//}
//
//- (void)setText:(NSString *)text highLightTextArray:(NSArray *)highLightTextArray withColor:(UIColor *)color
//{
//    if (text&&highLightTextArray&&color) {
//        [self setText:text afterInheritingLabelAttributesAndConfiguringWithBlock:^NSMutableAttributedString *(NSMutableAttributedString *mutableAttributedString) {
//            if (color) {
//                NSString *ctext = [text copy];
//                NSInteger loc = 0;
//                for (NSString *string in highLightTextArray) {
//                    NSRange range = [ctext rangeOfString:string];
//                    range.location = loc+range.location;
//                    if (range.length>0) {
//                        [mutableAttributedString addAttribute:(id)kCTForegroundColorAttributeName value:(id)color.CGColor range:range];
//                    }
//                    //从后面截断，防止高亮数字有相同的，比如 @[@"500", @"0"] 都有0
//                    loc = range.location+range.length;
//                    if (loc<text.length) {
//                        ctext = [text substringFromIndex:loc];
//                    } else {
//                        ctext = @"";
//                    }
//                }
//            }
//            return mutableAttributedString;
//        }];
//        return;
//    }
//    [self setText:text];
//}
//
//- (void)setLinkTextColor:(UIColor *)color highLightColor:(UIColor *)hColor
//{
//    NSMutableDictionary *linkAttributes = [self.linkAttributes mutableCopy];
//    [linkAttributes setObject:[NSNumber numberWithBool:NO] forKey:(NSString *)kCTUnderlineStyleAttributeName];
//    if (color) {
//        [linkAttributes setObject:(id)color.CGColor forKey:(NSString *)kCTForegroundColorAttributeName];
//    }
//    self.linkAttributes = linkAttributes;
//    //    self.inactiveLinkAttributes = linkAttributes;
//
//    NSMutableDictionary *activeLinkAttributes = [self.activeLinkAttributes mutableCopy];
//    [activeLinkAttributes setObject:[NSNumber numberWithBool:NO] forKey:(NSString *)kCTUnderlineStyleAttributeName];
//    if (hColor) {
//        [activeLinkAttributes setObject:(id)hColor.CGColor forKey:(NSString *)kCTForegroundColorAttributeName];
//    }
//    self.activeLinkAttributes = activeLinkAttributes;
//}
//
//- (void)setLinkTextColorStr:(NSString *)colorString highLightColorStr:(NSString *)hColorString
//{
//    [self setLinkTextColor:[UIColor colorWithString:colorString] highLightColor:[UIColor colorWithString:hColorString]];
//}
//
//@end

