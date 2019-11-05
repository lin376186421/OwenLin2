package com.mgame.common;

import java.util.HashMap;

import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import com.mgame.utils.IabHelper;
import com.mgame.utils.IabResult;
import com.mgame.utils.Inventory;
import com.mgame.utils.Purchase;
import com.mgstudio.bg.R;

public class IAPBilling {
	// Debug tag, for logging
    static final String TAG = "IAP";

    // Does the user have the premium upgrade?
    boolean mIsPremium = false;

    // SKUs for our two products: the premium upgrade (non-consumable) and gas (consumable)
//    static final String SKU_PREMIUM = "add_coins";
//    static final String SKU_GAS = "add_coins";
    
    // (arbitrary) request code for the purchase flow
    static final int RC_REQUEST = 10001;

    public String productId = "";
    
    boolean isPaymenting = false;
    boolean isConsume = false;
        
    // The helper object
    IabHelper mHelper = null;
    
    HashMap<String, Purchase> _mapPurchase = new HashMap<String, Purchase>();
    
    // Message
    static final int MESSAGE_SETUP_BILLING = 1001;
    static final int MESSAGE_LAUNCH_PURCHASE = 1002;
    static final int MESSAGE_RESUME_PURCHASE = 1003;
    static final int MESSAGE_CONSUME_PURCHASE = 1004;
    static final int MESSAGE_PAYMENT_SUCCEED = 1005;
    static final int MESSAGE_PAYMENT_FAILED = 1006;
    
    public void onPayment(String pid)
    {
    		if (!isPaymenting) {
    			this.productId = pid;
        		mHandler.sendEmptyMessage(MESSAGE_LAUNCH_PURCHASE);
        		
        		isPaymenting = true;		
		}
    }
    
    public void onResume()
    {
    		mHandler.sendEmptyMessage(MESSAGE_RESUME_PURCHASE);
    }
    
    public void onConsume(String pid)
    {
    		this.productId = pid;
    		mHandler.sendEmptyMessage(MESSAGE_CONSUME_PURCHASE);
    }
    
    public void onSetup()
    {
    		// load game data
        loadData();

        /* base64EncodedPublicKey should be YOUR APPLICATION'S PUBLIC KEY
         * (that you got from the Google Play developer console). This is not your
         * developer public key, it's the *app-specific* public key.
         *
         * Instead of just storing the entire literal string here embedded in the
         * program,  construct the key at runtime from pieces or
         * use bit manipulation (for example, XOR with some other string) to hide
         * the actual key.  The key itself is not secret information, but we don't
         * want to make it easy for an attacker to replace the public key with one
         * of their own and then fake messages from the server.
         */								        
        
        String base64EncodedPublicKey = AppRes.getRString(R.string.APP_LICENSE_KEY);
        
        // Some sanity checks to see if the developer (that's you!) really followed the
        // instructions to run this sample (don't put these checks on your app!)
        if (base64EncodedPublicKey.contains("CONSTRUCT_YOUR")) {
            throw new RuntimeException("Please put your app's public key in MainActivity.java. See README.");
        }
        if (AppActivity.getAppContext().getPackageName().startsWith("com.example")) {
            throw new RuntimeException("Please change the sample's package name! See README.");
        }
        
//        if (AppConfig.APP_DEBUG) {
//			productId = "android.test.purchased";
//		}
                
        // Create the helper, passing it our context and the public key to verify signatures with
        Log.d(TAG, "Creating IAB helper.");
        if (mHelper != null) {
        		mHelper.dispose();
        		mHelper = null;
		}
        
        mHelper = new IabHelper(AppActivity.getAppContext(), base64EncodedPublicKey);
        
        // enable debug logging (for a production application, you should set this to false).
        mHelper.enableDebugLogging(true);
        
        // Start setup. This is asynchronous and the specified listener
        // will be called once setup completes.
        Log.d(TAG, "Starting setup.");
        mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            public void onIabSetupFinished(IabResult result) {
                Log.d(TAG, "Setup finished.");

                if (!result.isSuccess()) {
                    // Oh noes, there was a problem.
                    complain("Problem setting up in-app billing: " + result);
                    paymentResult(false, null);
                    return;
                }

                // Hooray, IAB is fully set up. Now, let's get an inventory of stuff we own.
                Log.d(TAG, "Setup successful.");
                mHelper.queryInventoryAsync(mGotInventoryListener);
            }
        });
    }
    
    public Handler mHandler = new Handler(Looper.getMainLooper()) {
		public void handleMessage(Message msg) {
			if (msg.what == MESSAGE_LAUNCH_PURCHASE) {
	 			onSetup();
			}
			else if(msg.what == MESSAGE_RESUME_PURCHASE) {
				onSetup();	
			}
			else if(msg.what == MESSAGE_CONSUME_PURCHASE) {
				consumePurchase();
			}
		}
	};
    
 	public void launchPurchase() //�����ㄥ��璐�椤�
 	{
 		mHelper.launchPurchaseFlow(AppActivity.getAppContext(), productId, RC_REQUEST, mPurchaseFinishedListener, AppConfig.DEVICE_ID);
 	}
 	 	
 	public void consumePurchase() //纭�璁や��璐�
 	{
 		Purchase purchase = _mapPurchase.get(productId);
 		
 		if (purchase != null) {
 			
 			Log.d(TAG, "Purchase successful.");
 	        if (purchase.getSku().equals(productId)) {
 	        		mHelper.consumeAsync(purchase, mConsumeFinishedListener);
 	        }
// 	        else paymentResult(false, null);
 	        
 	        _mapPurchase.remove(productId);
		}
 	}

 	public void paymentResult(boolean result, Purchase purchase)
 	{ 		 		 		
 		String purchaseInfo = "";
 		if (purchase != null) {
 			
 	 		JSONObject receipt = new JSONObject();
 			try {
 				
 				JSONObject receiptInfo = new JSONObject();
 				
 				receiptInfo.put("purchaseInfo", purchase.getOriginalJson());
 				receiptInfo.put("signature", purchase.getSignature());
 				
 				receipt.put("receipt", receiptInfo);
 				
 				purchaseInfo = receipt.toString();
 				
 				Log.d(TAG, receipt.toString());

 			} catch (JSONException e) {
 				e.printStackTrace();
 			}
		}
 		
 		AppPlatform.getAppPlatform().onPaymentResult(productId, result, purchaseInfo);
 		if (productId != "") {
 			_mapPurchase.put(productId, purchase);
		}
 		
 		if (!result) {
 			onDestroy();	
		}
 	}
 	
    // Listener that's called when we finish querying the items we own
    IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener() {
        public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
            Log.d(TAG, "Query inventory finished.");
            if (result.isFailure()) {
                complain("Failed to query inventory: " + result);
                paymentResult(false, null);
            }else{
            		Log.d(TAG, "Query inventory was successful.");

            		 // Do we have restore purchase
            		 
            		if (isConsume) { //纭�璁ゆ��璐规�����
            			if(inventory.hasPurchase(productId))
            			{
            				mHelper.consumeAsync(inventory.getPurchase(productId), mConsumeFinishedListener);
            			}
				}
            		else if (isPaymenting) { //娑�璐规��浠�
            			
            			if (inventory.hasPurchase(productId)) {
            				paymentResult(true, inventory.getPurchase(productId));
					}
            			else{
            				launchPurchase();
            			}
				}
            		else { //��㈠�����澶����璁㈠��
            			if (inventory.getAllPurchases().size() > 0) {
					
            				Purchase purchase = inventory.getAllPurchases().get(0); //�����跺��澶���������℃��澶�������璐�椤�
            				
            				productId = purchase.getSku();
                			paymentResult(true, purchase);    
					}
            			else{
            				
            				paymentResult(false, null);
            			}
            		}
            }

            Log.d(TAG, "Initial inventory query finished; enabling main UI.");
        }

    };

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + data);

        if (resultCode == Activity.RESULT_CANCELED) {
        		paymentResult(false, null);
        		onDestroy();
		}else if (mHelper != null && mHelper.isSetupDone() ){
			// Pass on the activity result to the helper for handling
	        if (mHelper.handleActivityResult(requestCode, resultCode, data)) {
	        		Log.d(TAG, "onActivityResult handled by IABUtil.");
	        }
		}        
    }

    // Callback for when a purchase is finished
    IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
        public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
            Log.d(TAG, "Purchase finished: " + result + ", purchase: " + purchase);
            if (result.isFailure()) {
                complain("Error purchasing: " + result);
                setWaitScreen(false);
                paymentResult(false, purchase);
            }
            else if (!verifyDeveloperPayload(purchase)) {
                complain("Error purchasing. Authenticity verification failed.");
                setWaitScreen(false);
                paymentResult(false, purchase);
            }
            else {
            		paymentResult(true, purchase);
            }
            
        }
    };
    
    // Called when consumption is complete
    IabHelper.OnConsumeFinishedListener mConsumeFinishedListener = new IabHelper.OnConsumeFinishedListener() {
        public void onConsumeFinished(Purchase purchase, IabResult result) {

        		Log.d(TAG, "Consumption finished. Purchase: " + purchase + ", result: " + result);
            // successfully consumed
            if (result.isSuccess()) {
            	            		        			        		
            		Log.d(TAG, "Consume Successed!");
            }
            else {
            		Log.d(TAG, "Error while consuming: " + result);
            }
            updateUi();
            setWaitScreen(false);
            Log.d(TAG, "End consumption flow.");
            
            onDestroy();
        }
    };

    /** Verifies the developer payload of a purchase. */
    boolean verifyDeveloperPayload(Purchase p) {
        String payload = p.getDeveloperPayload();

        return true;
    }
    
    // We're being destroyed. It's important to dispose of the helper here!
    public void onDestroy() {
        Log.d(TAG, "Destroying helper.");
        if (mHelper != null) mHelper.dispose();
        mHelper = null;
        
        isPaymenting = false;
        isConsume = false;
    }

    // updates UI to reflect model
    public void updateUi() {
        
    }

    // Enables or disables the "please wait" screen.
    void setWaitScreen(boolean set) {
      
    }

    void complain(String message) {
//        alert("Error: " + message);
    		Log.d(TAG, message);
    }

    void alert(String message) {
        AlertDialog.Builder bld = new AlertDialog.Builder(AppActivity.getAppContext());
        bld.setMessage(message);
        bld.setNeutralButton("OK", null);
        bld.create().show();
    }

    void saveData() {
//        SharedPreferences.Editor spe = getPreferences(MODE_PRIVATE).edit();
//        spe.commit();
    }

    void loadData() {
//        SharedPreferences sp = getPreferences(MODE_PRIVATE);
    }

}
