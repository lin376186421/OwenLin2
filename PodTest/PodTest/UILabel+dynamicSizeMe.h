
#import <UIKit/UIKit.h>

@interface UILabel (dynamicSizeMe)

-(float)resizeToFit;
-(float)expectedHeight;
- (CGFloat)getTextWidth;
- (CGFloat)getTextHeight;
- (void)setLabelWidthWithString:(NSString *)string;
- (void)setLabelHeightWithString:(NSString *)string;

@end
