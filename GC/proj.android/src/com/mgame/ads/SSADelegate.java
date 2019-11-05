package com.mgame.ads;

import com.supersonicads.sdk.android.BrandConnectParameters;
import com.supersonicads.sdk.android.listeners.OnBrandConnectStateChangedListener;



public class SSADelegate implements OnBrandConnectStateChangedListener {
	@Override
	public void onAdFinished(String campaignName, int receivedCredits) {
        System.out.println("SSADelegate onAdFinished");
	}

	@Override
	public void onInitFail(String description) {
		System.out.println("SSADelegate onInitFail");
	}

	@Override
	public void onInitSuccess(BrandConnectParameters campaign) {
		System.out.println("SSADelegate onInitSuccess");
	}

	@Override
	public void noMoreOffers() {
		System.out.println("SSADelegate noMoreOffers");
	}

}
