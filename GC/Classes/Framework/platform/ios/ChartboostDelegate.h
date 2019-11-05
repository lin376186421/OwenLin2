//
//  ChartboostDelegate.h
//  GameClient
//
//  Created by Lee  on 14-2-17.
//
//

#import <Foundation/Foundation.h>
#import <Chartboost/Chartboost.h>


@interface ChartboostDelegate : NSObject<ChartboostDelegate>
{
    
}

- (void)didClickInterstitial:(CBLocation )location;
- (void)didClickMoreApps:(CBLocation)location;

@end
