//
//  FlurryAdsDelegate.h
//  RobinHood
//
//  Created by qsun on 13-2-1.
//
//

#import <Foundation/Foundation.h>

@interface CWebVIewDelegate : NSObject <UIWebViewDelegate>
{
    
}

- (id) init;
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType;
- (void)webViewDidStartLoad:(UIWebView *)webView;
- (void)webViewDidFinishLoad:(UIWebView *)webView;
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error;
@end
