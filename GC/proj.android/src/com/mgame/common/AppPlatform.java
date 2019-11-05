package com.mgame.common;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Random;

import android.accounts.Account;
import android.accounts.AccountManager;
import android.app.Activity;
import android.app.AlarmManager;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.Toast;

import com.aarki.Aarki;
import com.aarki.AarkiUserBalance;
import com.aarki.AarkiUserBalance.Status;
import com.chartboost.sdk.Chartboost;
import com.flurry.android.FlurryAdSize;
import com.flurry.android.FlurryAds;
import com.flurry.android.FlurryAgent;
import com.flurry.sdk.na;
import com.google.android.gms.internal.ba;
import com.mgame.ads.OffersWallViewActivity;
import com.mgame.amazon.AmazonMgr;
import com.mgame.extensions.MyWebView;
import com.mgame.extensions.TextInputField;
import com.mgame.facebook.FacebookMgr;
import com.mgstudio.bg.R;
//import com.mgame.extensions.TextInputField.MyWebViewClient;
import com.mobileapptracker.MATEventItem;
import com.mobileapptracker.MobileAppTracker;
import com.nativex.monetization.business.Balance;
import com.nativex.monetization.communication.RedeemCurrencyData;
import com.nativex.monetization.listeners.CurrencyListenerBase;
import com.nativex.monetization.listeners.CurrencyListenerV2;
import com.supersonicads.sdk.android.SupersonicAdsPublisherAgent;
import com.tapjoy.TapjoyConnect;
import com.tapjoy.TapjoyConnectFlag;
import com.tapjoy.TapjoyConnectNotifier;
import com.tapjoy.TapjoyNotifier;
import com.tapjoy.TapjoySpendPointsNotifier;
import com.nativex.monetization.MonetizationManager;
import com.nativex.monetization.business.Balance;
import com.nativex.monetization.communication.RedeemCurrencyData;
import com.nativex.monetization.listeners.CurrencyListenerBase;
import com.nativex.monetization.listeners.CurrencyListenerV2;
import com.nativex.monetization.listeners.OnAdEventBase;
import com.nativex.monetization.listeners.OnAdEventV2;
import com.nativex.monetization.listeners.SessionListener;
import com.nativex.monetization.mraid.AdInfo;
import com.nativex.monetization.enums.AdEvent;
import com.nativex.monetization.enums.NativeXAdPlacement;

public class AppPlatform extends ActivityDelegate{

	final static int MESSAGE_SHOW_FLURRY_VIDEO_AD = 10000;
	final static int MESSAGE_SHOW_FLURRY_AD = 10001;
	final static int MESSAGE_UPDATE_NEW_VERSION = 10002;
	final static int MESSAGE_RATE_APP = 10003;
	final static int MESSAGE_SHOW_INPUT_FIELD_DIALOG = 10004;
	final static int MESSAGE_SHOW_WEBVIEW = 10005;
	
	static Handler handler;

	boolean appUrlChanged = false;
	String newAppVersionUrl = "";
	boolean isUpdateTjPtFinish = true;
	boolean isGetAarkiPtFinish = true;
	Boolean m_NativeCreated = false;
		
	private AmazonMgr m_amazonMgr;

	
	private static AppPlatform s_appAppPlatform = null;
	public static AppPlatform getAppPlatform(){
		if (s_appAppPlatform == null) {
			s_appAppPlatform = new AppPlatform();
			s_appAppPlatform.initPlatform();
		}
		return s_appAppPlatform;
	}

	public boolean isAppDebug()
	{
		if(AppConfig.APP_DEBUG.contentEquals("0"))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	public void initPlatform()
	{
		m_amazonMgr = new AmazonMgr();
	}
	
	
	public void onResume()
	{
		m_amazonMgr.onResume();
	}

	public String getPackageName()
	{
		String packageName = AppActivity.getAppContext().getPackageName();
		return packageName;
	}

	public String getPackageVersion()
	{
		try {
			PackageManager packageManager = AppActivity.getAppContext().getPackageManager();
			PackageInfo packInfo = packageManager.getPackageInfo(getPackageName(), 0); 
			String version = packInfo.versionName;

			return version;		
		} catch (Exception e) {
			return "0";
		}
	}
	
	public final String NATIVE_X_NOT_GEMS = "NotEnoughGems";
	public final String NATIVE_X_GET_GEMS = "FreeGemsStore";
	public final String NATIVE_X_GAMELAUNCH = "Game Launch";
	private String m_pendingPlacement = null;

		OnAdEventBase eventListener = new OnAdEventV2() {
		
		@Override
		public void onEvent(AdEvent event, AdInfo info, String arg2) {
			// TODO Auto-generated method stub
			
			if(event == AdEvent.FETCHED)
			{
				if(m_pendingPlacement != null && m_pendingPlacement.contentEquals(NATIVE_X_GAMELAUNCH) == false)
				{
					AppPlatform.nativeNativeXDidFinishLoad(1);
					MonetizationManager.showAd(AppActivity.getAppContext(), m_pendingPlacement);
				}
			}
			else if(event == AdEvent.DISMISSED)
			{
				if(m_pendingPlacement != null && m_pendingPlacement.contentEquals(NATIVE_X_GAMELAUNCH) == true)
				{
					MonetizationManager.fetchAd(AppActivity.getAppContext(), m_pendingPlacement, eventListener);
				}
				getNativeXPoint();
			}
			else if(event == AdEvent.DISPLAYED)//关闭广告
			{
//				if(m_pendingPlacement != null && m_pendingPlacement.length() > 0)
//				{
//					MonetizationManager.fetchAd(AppActivity.getAppContext(), m_pendingPlacement, eventListener);
//				}
			}
			else if(event == AdEvent.ALREADY_FETCHED)
			{

			}
			else if(event == AdEvent.ERROR)
			{
				AppPlatform.nativeNativeXDidFinishLoad(0);
			}
			else if(event == AdEvent.EXPIRED)
			{
				AppPlatform.nativeNativeXDidFinishLoad(0);
			}
			else if(event == AdEvent.NO_AD)
			{
				AppPlatform.nativeNativeXDidFinishLoad(0);
			}
		}
	};

	
	public void showNativeXOfferAds(String placementStr)
	{
		m_pendingPlacement = placementStr;
		if(MonetizationManager.isAdReady(placementStr))
		{
	        MonetizationManager.showAd(AppActivity.getAppContext(), placementStr);
		}
		else
		{
			MonetizationManager.fetchAd(AppActivity.getAppContext(), placementStr, eventListener);
		}
	}

	CurrencyListenerV2 currencyListener =  new CurrencyListenerV2() {
		
		@Override
		public void onRedeem(RedeemCurrencyData info) {
			// TODO Auto-generated method stub
			System.out.println("RedeemCurrencyData");
			List<Balance> balanceList = info.getBalances();
			if(balanceList.size() > 0)
			{
				System.out.println("onRedeem"+balanceList.size());
			}
//			int count = 0;
			ArrayList<String> amonutList = new ArrayList<String>();
			for(Balance balance:balanceList)
			{
				String amountStr = balance.getAmount();
				amonutList.add(amountStr);
				
			}
			if(amonutList.size() > 0)
			{
				AppPlatform.nativeGetAdsGemsForNativeX(amonutList);
			}
		}
	};
	
	public CurrencyListenerV2 getNativeXCurrencyListener()
	{
		return currencyListener;
	}
	
	public void nativeXCreated()
	{
//		MonetizationManager.setCurrencyListener(currencyListener);
//		MonetizationManager.fetchAd(AppActivity.getAppContext(), NATIVE_X_NOT_GEMS, eventListener);
//		MonetizationManager.fetchAd(AppActivity.getAppContext(), NATIVE_X_GET_GEMS, eventListener);
		if(m_NativeCreated == false)
		{
			MonetizationManager.fetchAd(AppActivity.getAppContext(), NATIVE_X_GAMELAUNCH, eventListener);
			m_NativeCreated = true;
		}
		
	}
	
	public void getNativeXPoint()
	{
		if(MonetizationManager.isInitialized())
		{
			MonetizationManager.setCurrencyListener(currencyListener);
			MonetizationManager.redeemCurrency();
		}
	}
	
	public String getAppName()
	{
		return AppRes.getRString(R.string.app_name);
	}

	private void requestTapjoyConnect()
	{
		// Tapjoy
		Hashtable<String, String> flags = new Hashtable<String, String>();
		flags.put(TapjoyConnectFlag.ENABLE_LOGGING, "true");
		flags.put(TapjoyConnectFlag.DISABLE_ADVERTISING_ID_CHECK, "true");
		if(TapjoyConnect.getTapjoyConnectInstance() == null)
		{
			TapjoyConnect.requestTapjoyConnect(AppActivity.getAppContext(),AppConfig.TAPJOY_APP_ID, AppConfig.TAPJOY_SECRET_KEY, flags,new TapjoyConnectNotifier() 
			{
				@Override
				public void connectSuccess() 
				{
//					TapjoyConnect.getTapjoyConnectInstance().setTapjoyViewNotifier(OffersWallViewActivity.tjViewNotifier);
//					TapjoyConnect.getTapjoyConnectInstance().setVideoNotifier(OffersWallViewActivity.tjVideoNotifier);
//					tapjoyGetUpdatePoints();
//					System.out.println("connectSuccess");
				}
				@Override
				public void connectFail()
				{
					System.out.println("connectFail");
				}
			});
		}
	}

	public void tapjoyGetUpdatePoints()
	{
//		if (!isUpdateTjPtFinish)
//		{
//			return;	
//		}
//		isUpdateTjPtFinish = false;
//		TapjoyConnect.getTapjoyConnectInstance().getTapPoints(new TapjoyNotifier()
//		{
//			@Override
//			public void getUpdatePointsFailed(String error)
//			{
//				isUpdateTjPtFinish = true;
//				System.out.println("getUpdatePointsFailed" + error);
//			}
//
//			@Override
//			public void getUpdatePoints(String currencyName, int pointTotal)
//			{
//				if (pointTotal >0)
//				{
//					AppPlatform.nativeGetAdsFreeGems(pointTotal,2);
//					tapjoySpendPoint(pointTotal);
//				}
//				isUpdateTjPtFinish = true;
//				System.out.println("getUpdatePoints" + pointTotal);
//			}
//		});

	}

	public void tapjoySpendPoint(int val)
	{
//		TapjoyConnect.getTapjoyConnectInstance().spendTapPoints(val, new TapjoySpendPointsNotifier()
//		{
//			@Override
//			public void getSpendPointsResponseFailed(String error)
//			{
//				System.out.println("getSpendPointsResponseFailed" + error);
//			}
//			@Override
//			public void getSpendPointsResponse(String currencyName, int spendPoint)
//			{
//				System.out.println("getSpendPointsResponse" + spendPoint);
//			}
//		});
	}


	public void showTapJoyOfferswall()
	{
//		Intent intent = new Intent(AppActivity.getContext(),OffersWallViewActivity.class);
//		AppActivity.getAppContext().startActivity(intent);
	}

	public void showTapjoyFullScreenAds()
	{
//		TapjoyConnect.getTapjoyConnectInstance().getFullScreenAd(OffersWallViewActivity.tjFullScreenAdNotifier);

	}

	public void showChartBoostAds()
	{
		Chartboost.sharedChartboost().showInterstitial();
	}
	public void showFlurryVideoAds()
	{
		Random random = new Random();
		if (Math.abs(random.nextInt())%2 == 0)
		{
			ViewGroup view = (ViewGroup)((Activity) AppActivity.getAppContext()).getWindow().getDecorView();
			if(FlurryAds.isAdReady(AppConfig.FLURRY_AD_SPACE)) {
				FlurryAds.displayAd(AppActivity.getAppContext(), AppConfig.FLURRY_AD_SPACE, view);
			} else {
				FlurryAds.fetchAd(AppActivity.getAppContext(), AppConfig.FLURRY_AD_SPACE, view, FlurryAdSize.FULLSCREEN);
			}
		} else
		{
			showTapjoyFullScreenAds();
		}
	}


	public void getAarkiPoint()
	{
		
		if(!isGetAarkiPtFinish)
		{
			return;
		}
		isGetAarkiPtFinish = false;
		AarkiUserBalance.check(AppConfig.AARKI_APP_ID, new AarkiUserBalance.Listener() 
		{
			@Override
			public void onFinished(final Status status, final Integer balance)
			{
				if (status == AarkiUserBalance.Status.OK)
				{
                	System.out.println("check onFinished" + balance.intValue());
					if (balance.intValue() > 0)
					{
						addAarkiBalance(-balance.intValue());
					}
					else
					{
						isGetAarkiPtFinish = true;

					}
				}
				else
					
				{
					isGetAarkiPtFinish = true;
				}
			}
		});
	}
	
	public void addAarkiBalance(final int val)
	{
		 AarkiUserBalance.add(AppConfig.AARKI_APP_ID, val, new AarkiUserBalance.Listener() 
		 {	            
	            @Override
	            public void onFinished(final Status status, final Integer balance) 
	            {
	                if (status == AarkiUserBalance.Status.OK)
	                {
						AppPlatform.nativeGetAdsFreeGems(-val,4);
	                    // Update stats: item was successfully purchased
	                	System.out.println("add onFinished" + val);
	                } else if (status == AarkiUserBalance.Status.InsufficientFunds)
	                {
	                	System.out.println("add InsufficientFunds");

	                    // Update balance
	                    
	                    // Here the user can be alerted that there were not enough funds to buy the item
	                }
					isGetAarkiPtFinish = true;
	            }
	        });
	}

	public void showAarkiOfferAds()
	{
		  
		  Aarki.showAds(AppActivity.getAppContext(), AppConfig.AARKI_APP_ID, new Aarki.AarkiListener() {
	            @Override
	            public void onFinished(Aarki.Status status) 
	            {
	                if (status == Aarki.Status.OK) 
	                {
	                }
	                else if (status == Aarki.Status.AppNotRegistered) 
	                {
	                }
	            }
	        });
	}

    public void showSSAOfferAds()
	{
		SupersonicAdsPublisherAgent mSupersonicAdsAgent = SupersonicAdsPublisherAgent.getInstance(AppActivity.getAppContext());
		mSupersonicAdsAgent.showOfferWall(AppActivity.getAppContext(),AppConfig.SSA_APP_KEY, AppConfig.DEVICE_ID, null);
	}
    
	public void displayFlurryAds(String pid)
	{
		HashMap<String, String> userCookies = new HashMap<String, String>();
		userCookies.put("game", AppConfig.APP_GAME_NAME);
		userCookies.put("appId", AppConfig.APP_ID);
		userCookies.put("pid", pid);
		userCookies.put("user", AppConfig.DEVICE_ID);
		//		userCookies.put("deviceId", AppConfig.DEVICE_ID);
		//			Log.d("vneurifuhreiws", AppConfig.DEVICE_ID);
		FlurryAds.setUserCookies(userCookies);

		handler.sendEmptyMessage(MESSAGE_SHOW_FLURRY_AD);
	}

	public void logFlurryEvent(String key, HashMap<String, String> params)
	{
		FlurryAgent.logEvent(key, params);
	}
	
	
	
	
	public void showWebView(String webURL,int xPos,int yPos,int width, int height,int diwWidth,int dirHeight) { 
		Message msg = new Message();
		msg.what = MESSAGE_SHOW_WEBVIEW;
		msg.obj = new MyWebView.MyWebViewMessage(webURL,xPos,yPos,width,height,diwWidth,dirHeight);
		AppActivity.getAppContext().mHandler.sendMessage(msg);		
	}
	public void hideWebView()
	{
 		if(AppActivity.s_myWebView != null)
		{
			AppActivity.s_myWebView.hideMyWebView();
		}
	}
	
	public void logMobileAppEvent(String productId, float price)
	{
		MobileAppTracker.getInstance().measureAction("purchase", new MATEventItem(productId, 1, price, price*0.7), price*0.7, null, AppConfig.MOBILE_APP_ID);
	}

	public void getMoregame()
	{
		Uri uri = Uri.parse("market://details?id=com.vcnllc.cs");  
		Intent intent=new Intent(Intent.ACTION_VIEW, uri); 
		AppActivity.getAppContext().startActivity(intent);
	}
	
	public void onLoad(){     
		handler = new Handler(Looper.myLooper()) {
			public void handleMessage(final Message msg) 
			{
				ViewGroup view = (ViewGroup)((Activity) AppActivity.getAppContext()).getWindow().getDecorView();

				switch (msg.what) { 
				case MESSAGE_SHOW_FLURRY_AD: //flurry ad


					//					FlurryAgent.setUserId(Utils.getUserId(AppActivity.getContext()));

					//					FlurryAgent.enableTestAds(true);

					//					FlurryAds.fetchAd(AppActivity.getAppContext(), "BANNER_MAIN_VC", view, FlurryAdSize.FULLSCREEN);

					//FlurryAgent.displayAd(AppActivity.getContext(), "BANNER_MAIN_VC", view); 
					break;
				case MESSAGE_UPDATE_NEW_VERSION:

					String title = "Update is available!";
					String message = (String)msg.obj;					
					
					if (message.length() <= 0)
					{
						message = "Good news! A new version of the game is available.";
					}

					new AlertDialog.Builder(AppActivity.getAppContext())
					.setTitle(title) 
					.setMessage(message)
					.setPositiveButton("Update",  new DialogInterface.OnClickListener() {

						public void onClick(DialogInterface dialog, int which) {

							Uri uri = Uri.parse(newAppVersionUrl);  

							Intent intent=new Intent(Intent.ACTION_VIEW, uri); 
							AppActivity.getAppContext().startActivity(intent);
						}  			
					}).show(); 
					break;
				case MESSAGE_RATE_APP:

					new AlertDialog.Builder(AppActivity.getAppContext())
					.setTitle("Rate " + AppActivity.getAppContext().getString(R.string.app_name)) 
					.setMessage("You will get 10 FREE gems if you give us good feedback rating!")
					.setPositiveButton("OK",  new DialogInterface.OnClickListener() {

						public void onClick(DialogInterface dialog, int which) {

							nativeRateAppOption(true);

							Uri uri = Uri.parse(AppConfig.APP_STORE_URL);  

							Intent intent=new Intent(Intent.ACTION_VIEW, uri); 
							AppActivity.getAppContext().startActivity(intent);
						}  			
					})
					.setNegativeButton("Cancel", null)
					.show(); 
					break;
				}
			}
		};

		//load native config
		nativeLoadConfig();
	}

	public String getGmailName()
	{
		AccountManager accountManager = AccountManager.get(AppActivity.getAppContext());
		Account[] accounts = accountManager.getAccountsByType("com.google");
		String gmailName = "";
		if (accounts.length > 0) {
			gmailName = accounts[0].name;

			//list all account
			for(Account account:accounts){
				Log.i("--Get Account--" , account.name) ;
				Log.i("--Get Account--" , account.type) ;
			}
		}

		return gmailName;
	}

	public void paymentPurchase(String productId, int payType){
		
		if (payType == AppConfig.PAY_TYPE_PAYPAL) {
			
			String paypal = productId;
			
			Uri uri = Uri.parse(paypal);  

			Intent intent=new Intent(Intent.ACTION_VIEW, uri); 
			AppActivity.getAppContext().startActivity(intent);
		}
		else if (payType == AppConfig.PAY_TYPE_AMAZON) {
			amazonPaymentPurchase(productId);
		}
		else {
			AppActivity.s_iapBilling.onPayment(productId);	
			AppActivity.isWaitPurchase = true;
		}
	}
	
	public void onPaymentResult(String productId, boolean result, String purchaseInfo)
	{
		AppActivity.isWaitPurchase = false;
		nativePaymentResult(productId, result, purchaseInfo);
	}

	public void resumePurchase(){
		AppActivity.s_iapBilling.onResume();
	}

	public void consumePurchase(String productId){
		AppActivity.s_iapBilling.onConsume(productId);
	}

	public void amazonPaymentPurchase(String productId){

		m_amazonMgr.makePurchaseRequest(productId);
	}
	
	public void updateNewVersion(String message, String newUrl)
	{
		if (newUrl.length() > 0) {
			newAppVersionUrl  = newUrl;
			appUrlChanged = true;
		}else{
			newAppVersionUrl = AppConfig.APP_STORE_URL;
			appUrlChanged = false;
		}

		Message msg = new Message();
		msg.what = MESSAGE_UPDATE_NEW_VERSION;
		msg.obj = message;
		
		handler.sendMessage(msg);
	}

	public void rateApp()
	{
		handler.sendEmptyMessage(MESSAGE_RATE_APP);
	}

	static String getRealtime(){
		return String.valueOf(SystemClock.elapsedRealtime()/1000);
	}

	public static boolean connectedToNetwork(){
		ConnectivityManager connMgr = (ConnectivityManager) AppActivity.getAppContext().getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo networkInfo = connMgr.getActiveNetworkInfo();
		if (networkInfo != null && networkInfo.isConnected()) {
			// fetch data
			return true;
		} else {
			// display error
			return false;
		}
	}

	static int pid = 0;
	public static void fireNotification(int delay, String message) {

		long time = System.currentTimeMillis() + (delay * 1000);

		Intent intent = new Intent(AppActivity.getContext(), NotificationReceiver.class);
		intent.putExtra("title", AppRes.getRString(R.string.app_name));
		intent.putExtra("message", message);

		PendingIntent pendingintent = PendingIntent.getBroadcast(AppActivity.getContext(), pid, intent,
				0);

		// Get the AlarmManager service
		AlarmManager am = (AlarmManager) AppActivity.getContext().getSystemService(Activity.ALARM_SERVICE);
		am.set(AlarmManager.RTC_WAKEUP, time, pendingintent);
		
		pid++;
	}

	public static void cancelAllNotifications() {

		Intent intent = new Intent(AppActivity.getContext(), NotificationReceiver.class);	

		for (int i = 0; i < pid; i++) {
			AlarmManager am = (AlarmManager) AppActivity.getContext().getSystemService(Activity.ALARM_SERVICE);
			am.cancel(PendingIntent.getBroadcast(AppActivity.getContext(), i, intent, 0));	
		}
		pid = 0;
	}

	public static void showTextInputFieldDialog(final String pTitle, final String pContent, int x, int y,int width, int height, int fontSize, final int pInputMode, final int pInputFlag, final int pReturnType, final int pMaxLength) { 
		Message msg = new Message();
		msg.what = MESSAGE_SHOW_INPUT_FIELD_DIALOG;
		msg.obj = new TextInputField.TextInputFieldMessage(pTitle, pContent, x, y, width, height, fontSize, pInputMode, pInputFlag, pReturnType, pMaxLength);
		AppActivity.getAppContext().mHandler.sendMessage(msg);		
	}
	
	public static void openUrl(final String url)
	{
		Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		AppActivity.getAppContext().startActivity(intent);
//		AppActivity.getAppContext().runOnUiThread(new Runnable() {
//			@Override
//			public void run() {
//				// TODO Auto-generated method stub
//			}
//		});
	}
	
	public static void exit()
	{
		AppActivity.getAppContext().finish();
		AppActivity.getAppContext().onDestroy();
	}
	
	
	
	public static native void nativeNativeXDidFinishLoad(int state);
	public static native void nativeWebViewDidFinishLoad(int state);
	public static native void nativeGetSSAPoint();
	public static native void nativeGetAdsGemsForNativeX(ArrayList<String> amonutList);
	public static native void nativeGetAdsFreeGems(int value,int type);
	public static native void nativeGetChartBoostClickGem();
	public static native void nativePaymentResult(String productId, boolean result, String purchaseInfo);
	public static native void nativeRateAppOption(boolean rate);
	public static native void flurryDidFailToReceiveAd();
	public static native void flurryDidReceiveAd();
	public static native void nativeLoadConfig();
	public static native void nativeRegisterServerPush(String regId);
}
