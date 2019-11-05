package com.mgame.ads;

import com.chartboost.sdk.ChartboostDelegate;
import com.mgame.common.AppPlatform;

public class ChartboostAdDelegate implements ChartboostDelegate{
	@Override
	public void didCacheInterstitial(String arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didCacheMoreApps() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didClickInterstitial(String arg0)
	{
		// TODO Auto-generated method stub
		AppPlatform.nativeGetChartBoostClickGem();
		System.out.println("chartboost didClickInterstitial");

	}

	@Override
	public void didClickMoreApps() {
		// TODO Auto-generated method stub
		AppPlatform.nativeGetChartBoostClickGem();
		System.out.println("chartboost didClickMoreApps");

	}

	@Override
	public void didCloseInterstitial(String arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didCloseMoreApps() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didDismissInterstitial(String arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didDismissMoreApps() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didFailToLoadInterstitial(String arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didFailToLoadMoreApps() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didShowInterstitial(String arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void didShowMoreApps() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean shouldDisplayInterstitial(String arg0) {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public boolean shouldDisplayLoadingViewForMoreApps() {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public boolean shouldDisplayMoreApps() {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public boolean shouldRequestInterstitial(String arg0) {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public boolean shouldRequestMoreApps() {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public boolean shouldRequestInterstitialsInFirstSession() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void didFailToLoadUrl(String arg0) {
		// TODO Auto-generated method stub
		
	}
}
