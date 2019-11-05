package com.mgame.ads;

import com.aarki.Aarki;
import com.aarki.AarkiUserBalance;
import com.mgame.common.AppActivity;
import com.mgame.common.DeviceInfo;

public class AarkiUtil {
	
	private static String placementId = null;
	private static String seckey = null;

//	public static Handler mHandler = new Handler(new Looper() ) {
//		public void handleMessage(final Message msg) {
//			
//		}
//	};
	public static void initialize(final String key, final String id) {
		AppActivity.getAppContext().runOnUiThread(new Runnable(){

			@Override
			public void run() {
				placementId = id;
				seckey = key;
				// TODO Auto-generated method stub
				
				Aarki.registerApp(AppActivity.getAppContext(), key);
				Aarki.setUserId(DeviceInfo.getAndroidID());
				
//				Aarki.setRewardListener(new Aarki.RewardListener() {
//				    @Override
//				    public void onFinished(final String plaId, int reward) {
//				        // Update user balance
//				    	if (plaId.equals(placementId)) {
//				    		checkBalanceResult(true, reward);
//				    	}
//				    }
//				});
			}
			
		});
	}

	public static void showOffer() {
		
		AppActivity.getAppContext().runOnUiThread(new Runnable(){

			@Override
			public void run() {
				// TODO Auto-generated method stub
				Aarki.showAds(AppActivity.getAppContext(), placementId, new Aarki.AarkiListener() {
			        @Override
			        public void onFinished(final Aarki.Status status) {
			            if (status == Aarki.Status.OK) {
			            	showOfferResult(true);
			            } else  {
			            	showOfferResult(false);
			            }
			        }
			    });
			}
			
		});
	}
	
	public static void checkBalance() {
		AppActivity.getAppContext().runOnUiThread(new Runnable() {

			@Override
			public void run() {
				// TODO Auto-generated method stub
//		new Thread(new Runnable() {
//			
//		public void run () {
				AarkiUserBalance.check(placementId, new AarkiUserBalance.Listener() {
					
					@Override
					public void onFinished(AarkiUserBalance.Status status, Integer arg1) {
						// TODO Auto-generated method stub
						int credit = 0;
						if(arg1 != null) credit = arg1.intValue();
						if (status == AarkiUserBalance.Status.OK) {
							checkBalanceResult(true, credit);
			            } else  {
							checkBalanceResult(false, credit);

			            }
					}
				} );
			}
			
		});//.start();
	}
	
	private static void showOfferResult(final boolean result) {
		AppActivity.getAppContext().runOnGLThread(new Runnable(){
			public void run() {
				nativeShowOfferResult(result);
			}
		});
	}
	
	private static void checkBalanceResult(final boolean success, final int result) {
		AppActivity.getAppContext().runOnGLThread(new Runnable(){
			public void run() {
				nativeCheckBalanceResult(success, result);
			}
		});
	}
	
	private static native void nativeShowOfferResult(boolean result);
	private static native void nativeCheckBalanceResult(boolean success, int result);

}
