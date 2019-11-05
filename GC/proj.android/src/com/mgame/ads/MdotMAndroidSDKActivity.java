package com.mgame.ads;

import com.mdotm.android.listener.MdotMAdEventListener;
import com.mdotm.android.utils.MdotMLogger;

import android.app.Activity;

public class MdotMAndroidSDKActivity extends Activity implements MdotMAdEventListener {
    public void onReceiveBannerAd() {
      // TODO Auto-generated method stub
       MdotMLogger.i(this, "banner Ad received ");
	      }

    public void onFailedToReceiveBannerAd() {
       MdotMLogger.i(this, "Failed to receive banner ad");
    }

    public void onReceiveInterstitialAd() {
      // TODO Auto-generated method stub
       MdotMLogger.i(this, "interstitial Ad received ");
	}

    public void onFailedToReceiveInterstitialAd() {
       MdotMLogger.i(this, "Failed to receive interstitial ad");
    }
 	public void onBannerAdClick() {
 		MdotMLogger.i(this, " banner AdClick");
 	 }

	public void onInterstitialAdClick() {
		MdotMLogger.i(this, " interstitial AdClick");
 	 }

   public void onDismissScreen() {
      MdotMLogger.i(this, "banner Dismiss");
     }

   public void onInterstitialDismiss() {
      MdotMLogger.i(this, "interstitial dismiss");
     }

   public void onLeaveApplicationFromBanner() {
	 MdotMLogger.i(this, "banner LeaveApplication");
   }

   public void onLeaveApplicationFromInterstitial() {
	 MdotMLogger.i(this, "interstitial  LeaveApplication");
   }

   public void willShowInterstitial() {
	 MdotMLogger.i(this, "will show interstitial");
   }

   public void didShowInterstitial() {
	 MdotMLogger.i(this, "did show interstitial");
   }

   public void onBannerDismissScreen() {
	// TODO Auto-generated method stub
   }
}
