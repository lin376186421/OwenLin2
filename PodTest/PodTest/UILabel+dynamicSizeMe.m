
#import "UILabel+dynamicSizeMe.h"
#import "BPUIViewAdditions.h"

@implementation UILabel (dynamicSizeMe)

-(float)resizeToFit{
    float height = [self expectedHeight];
    CGRect newFrame = [self frame];
    newFrame.size.height = height;
    [self setFrame:newFrame];
    return newFrame.origin.y + newFrame.size.height;
}

-(float)expectedHeight{
    [self setNumberOfLines:0];
    [self setLineBreakMode:NSLineBreakByWordWrapping];

    CGSize maximumLabelSize = CGSizeMake(self.frame.size.width,9999);
    
    CGSize expectedLabelSize = [[self text] sizeWithFont:[self font] 
                                            constrainedToSize:maximumLabelSize
                                            lineBreakMode:[self lineBreakMode]]; 
    return expectedLabelSize.height;
}

- (void)setLabelWidthWithString:(NSString *)string
{
    self.text = string;
    self.width = [self getTextWidth];
}

- (void)setLabelHeightWithString:(NSString *)string
{
    self.text = string;
    self.height = [self getTextHeight];
}

- (CGFloat)getTextWidth
{
    return [self.text sizeWithFont:self.font].width;
}

- (CGFloat)getTextHeight
{
    return [self textRectForBounds:CGRectMake(0, 0, self.width, FLT_MAX) limitedToNumberOfLines:self.numberOfLines].size.height;
//    return [self.text sizeWithFont:self.font constrainedToSize:CGSizeMake(self.width, FLT_MAX)].height;
}

@end
