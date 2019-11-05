//
//  FlurryAdsDelegate.m
//  RobinHood
//
//  Created by qsun on 13-2-1.
//
//

#import "CWebVIewDelegate.h"
#import "CAppPlatform.h"

@implementation CWebVIewDelegate
- (id) init
{
    if(self = [super init]){
        
    }
    return self;
}
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    NSString *requestString = [[request URL] absoluteString];
    NSURL *webURL = [NSURL URLWithString:[NSString stringWithUTF8String:LogicInfo::getSingleton().m_webURL.c_str()]];
    NSString *webString = [webURL absoluteString];
    if(![webString isEqualToString:requestString])//网页里面的链接 用自带浏览器打开
    {
        [[UIApplication sharedApplication] openURL:[request URL]];
    }
    return YES;
}
- (void)webViewDidStartLoad:(UIWebView *)webView
{
    NSLog(@"webViewDidStartLoad");
}
- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    CAppPlatform::sharedAppPlatform()->webViewDidFinishLoad(1);
    NSLog(@"webViewDidFinishLoad");
}
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    CAppPlatform::sharedAppPlatform()->webViewDidFinishLoad(0);
    NSLog(@"didFailLoadWithError:%@",[error description]);
}

@end
