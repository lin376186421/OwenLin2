package com.mgame.extensions;

import java.io.UnsupportedEncodingException;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxEditBoxDialog;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxHelper;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Typeface;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.Editable;
import android.text.InputFilter;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.KeyEvent;
import android.view.View.OnLayoutChangeListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

import com.mgame.common.AppActivity;

public class TextInputField {
	// ===========================================================
	// Constants
	// ===========================================================

	/**
	 * The user is allowed to enter any text, including line breaks.
	 */
	private final int kEditBoxInputModeAny = 0;

	/**
	 * The user is allowed to enter an e-mail address.
	 */
	private final int kEditBoxInputModeEmailAddr = 1;

	/**
	 * The user is allowed to enter an integer value.
	 */
	private final int kEditBoxInputModeNumeric = 2;

	/**
	 * The user is allowed to enter a phone number.
	 */
	private final int kEditBoxInputModePhoneNumber = 3;

	/**
	 * The user is allowed to enter a URL.
	 */
	private final int kEditBoxInputModeUrl = 4;

	/**
	 * The user is allowed to enter a real number value. This extends
	 * kEditBoxInputModeNumeric by allowing a decimal point.
	 */
	private final int kEditBoxInputModeDecimal = 5;

	/**
	 * The user is allowed to enter any text, except for line breaks.
	 */
	private final int kEditBoxInputModeSingleLine = 6;

	/**
	 * The user is allowed to enter any text multi line mode
	 */
	private final int kTextFieldInputModeMultiLine = 7;

	/**
	 * Indicates that the text entered is confidential data that should be
	 * obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
	 */
	private final int kEditBoxInputFlagPassword = 0;

	/**
	 * Indicates that the text entered is sensitive data that the implementation
	 * must never store into a dictionary or table for use in predictive,
	 * auto-completing, or other accelerated input schemes. A credit card number
	 * is an example of sensitive data.
	 */
	private final int kEditBoxInputFlagSensitive = 1;

	/**
	 * This flag is a hint to the implementation that during text editing, the
	 * initial letter of each word should be capitalized.
	 */
	private final int kEditBoxInputFlagInitialCapsWord = 2;

	/**
	 * This flag is a hint to the implementation that during text editing, the
	 * initial letter of each sentence should be capitalized.
	 */
	private final int kEditBoxInputFlagInitialCapsSentence = 3;

	/**
	 * Capitalize all characters automatically.
	 */
	private final int kEditBoxInputFlagInitialCapsAllCharacters = 4;

	private final int kKeyboardReturnTypeDefault = 0;
	private final int kKeyboardReturnTypeDone = 1;
	private final int kKeyboardReturnTypeSend = 2;
	private final int kKeyboardReturnTypeSearch = 3;
	private final int kKeyboardReturnTypeGo = 4;

	// ===========================================================
	// Fields
	// ===========================================================

	private LinearLayout mInputEditLayout;
	private EditText mInputEditText;

	private final String mTitle;
	private final String mMessage;
	private final int mPosX;
	private final int mPosY;
	private final int mWidth;
	private final int mHeight;
	private final int mFontSize;
	private final int mInputMode;
	private final int mInputFlag;
	private final int mReturnType;
	private final int mMaxLength;
	
	private String mText = "";

	private int mInputFlagConstraints;
	private int mInputModeContraints;
	private boolean mIsMultiline;
	
	final static int MESSAGE_REMOVE_TEXT_INPUT_FIELD = 10001;

	// ===========================================================
	// Constructors
	// ===========================================================

	public TextInputField(final String pTitle, final String pMessage, int posX,
			int posY, int pWidth, int pHeight, final int fontSize, final int pInputMode,
			final int pInputFlag, final int pReturnType, final int pMaxLength) {

		this.mTitle = pTitle;
		this.mMessage = pMessage;
		this.mPosX = posX;
		this.mPosY = posY;
		this.mWidth = pWidth;
		this.mHeight = pHeight;
		this.mFontSize = fontSize;
		this.mInputMode = pInputMode;
		this.mInputFlag = pInputFlag;
		this.mReturnType = pReturnType;
		this.mMaxLength = pMaxLength;
	}

	public void showTextField() {

		View rootView = Cocos2dxGLSurfaceView.getInstance().getRootView();
		// View view = (AppActivity.getContext().findViewById())
//		 View rootView = ((ViewGroup)AppActivity.getContext().findViewById(android.R.id.content)).getChildAt(0);

		final LinearLayout.LayoutParams inputLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.FILL_PARENT);
		
		mInputEditLayout = new LinearLayout(AppActivity.getContext());
		mInputEditLayout.setLayoutParams(inputLayoutParams);
		mInputEditLayout.setOrientation(LinearLayout.VERTICAL);
		mInputEditLayout.setBackgroundColor(0);
		
		final LinearLayout.LayoutParams editTextParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
		editTextParams.leftMargin = mPosX;
		editTextParams.topMargin = mPosY;
//		editTextParams.width = mWidth;
//		editTextParams.height = mHeight;
		
		this.mInputEditText = new EditText(AppActivity.getContext());
		this.mInputEditText.setText(this.mMessage);
//		this.mInputEditText.setLeft(mPosX);
//		this.mInputEditText.setTop(mPosY);
		this.mInputEditText.setWidth(mWidth);
		this.mInputEditText.setHeight(mHeight);
		this.mInputEditText.setTextSize(mFontSize);
		this.mInputEditText.setBackgroundColor(0);
		this.mInputEditText.setIncludeFontPadding(false);
		this.mInputEditText.setGravity(Gravity.TOP|Gravity.LEFT);
		this.mInputEditText.setPadding(4, 6, 4, 4);
//		this.mInputEditText.setTypeface(Typeface.defaultFromStyle(Typeface.BOLD));
		
		this.mInputEditText.setLayoutParams(editTextParams);
		
		mInputEditLayout.addView(mInputEditText);
		((ViewGroup) rootView).addView(mInputEditLayout);
						
//		((Activity) AppActivity.getContext()).getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		int oldImeOptions = this.mInputEditText.getImeOptions();
		this.mInputEditText.setImeOptions(oldImeOptions | EditorInfo.IME_FLAG_NO_EXTRACT_UI | EditorInfo.IME_FLAG_NO_FULLSCREEN);
		oldImeOptions = this.mInputEditText.getImeOptions();
		
		switch (this.mInputMode) {
		case kEditBoxInputModeAny:
			this.mInputModeContraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_MULTI_LINE;
			break;
		case kEditBoxInputModeEmailAddr:
			this.mInputModeContraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS;
			break;
		case kEditBoxInputModeNumeric:
			this.mInputModeContraints = InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_SIGNED;
			break;
		case kEditBoxInputModePhoneNumber:
			this.mInputModeContraints = InputType.TYPE_CLASS_PHONE;
			break;
		case kEditBoxInputModeUrl:
			this.mInputModeContraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_URI;
			break;
		case kEditBoxInputModeDecimal:
			this.mInputModeContraints = InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL | InputType.TYPE_NUMBER_FLAG_SIGNED;
			break;
		case kEditBoxInputModeSingleLine:
			this.mInputModeContraints = InputType.TYPE_CLASS_TEXT;
			break;
		case kTextFieldInputModeMultiLine:
			this.mInputModeContraints = InputType.TYPE_CLASS_TEXT;
			this.mInputEditText.setSingleLine(false);

			this.mIsMultiline = true;
			break;
		default:

			break;
		}

		if (this.mIsMultiline) {
			this.mInputModeContraints |= InputType.TYPE_TEXT_FLAG_MULTI_LINE;
		}

		this.mInputEditText.setInputType(this.mInputModeContraints | this.mInputFlagConstraints);

		switch (this.mInputFlag) {
		case kEditBoxInputFlagPassword:
			this.mInputFlagConstraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD;
			break;
		case kEditBoxInputFlagSensitive:
			this.mInputFlagConstraints = InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS;
			break;
		case kEditBoxInputFlagInitialCapsWord:
			this.mInputFlagConstraints = InputType.TYPE_TEXT_FLAG_CAP_WORDS;
			break;
		case kEditBoxInputFlagInitialCapsSentence:
			this.mInputFlagConstraints = InputType.TYPE_TEXT_FLAG_CAP_SENTENCES;
			break;
		case kEditBoxInputFlagInitialCapsAllCharacters:
			this.mInputFlagConstraints = InputType.TYPE_TEXT_FLAG_CAP_CHARACTERS;
			break;
		default:
			break;
		}

		this.mInputEditText.setInputType(this.mInputFlagConstraints | this.mInputModeContraints);

		switch (this.mReturnType) {
		case kKeyboardReturnTypeDefault:
			this.mInputEditText.setImeOptions(oldImeOptions | EditorInfo.IME_ACTION_NONE);
			break;
		case kKeyboardReturnTypeDone:
			this.mInputEditText.setImeOptions(oldImeOptions | EditorInfo.IME_ACTION_DONE);
			break;
		case kKeyboardReturnTypeSend:
			this.mInputEditText.setImeOptions(oldImeOptions | EditorInfo.IME_ACTION_SEND);
			break;
		case kKeyboardReturnTypeSearch:
			this.mInputEditText.setImeOptions(oldImeOptions | EditorInfo.IME_ACTION_SEARCH);
			break;
		case kKeyboardReturnTypeGo:
			this.mInputEditText.setImeOptions(oldImeOptions | EditorInfo.IME_ACTION_GO);
			break;
		default:
			this.mInputEditText.setImeOptions(oldImeOptions | EditorInfo.IME_ACTION_NONE);
			break;
		}

		this.mInputEditText.setMaxWidth(mWidth);
		this.mInputEditText.setMaxHeight(mHeight);
		if (this.mMaxLength > 0) {
			this.mInputEditText.setFilters(new InputFilter[] { new InputFilter.LengthFilter(this.mMaxLength) });
		}
		
		final Handler initHandler = new Handler();
		initHandler.postDelayed(new Runnable() {
			@Override
			public void run() {
		
				TextInputField.this.mInputEditText.setFocusable(true);
				TextInputField.this.mInputEditText.requestFocus();
				TextInputField.this.mInputEditText.setSelection(TextInputField.this.mInputEditText.length());
				
				TextInputField.this.openKeyboard();
			}
		}, 200);
										
		this.mInputEditText.setOnEditorActionListener(new OnEditorActionListener() {
			@Override
			public boolean onEditorAction(final TextView v, final int actionId, final KeyEvent event) {
				/* If user didn't set keyboard type, this callback will be invoked twice with 'KeyEvent.ACTION_DOWN' and 'KeyEvent.ACTION_UP'. */
				if (actionId != EditorInfo.IME_NULL || (actionId == EditorInfo.IME_NULL && event != null && event.getKeyCode() != KeyEvent.KEYCODE_ENTER)) {
					onEndEditing();
					return true;
				}
				
				if (!mIsMultiline) {
				
					if (event.getKeyCode() == KeyEvent.KEYCODE_ENTER) {
						onEndEditing();
						return true;
					}
				}
				
				return false;
			}
		});
		
		this.mInputEditText.addTextChangedListener(new TextWatcher(){ 
			 
	            @Override 
	            public void afterTextChanged(Editable s) { 
	                Log.d("EditText", "afterTextChanged"); 
	            } 
	 
	            @Override 
	            public void beforeTextChanged(CharSequence s, int start, int count, 
	                    int after) { 
	                Log.d("EditText", "beforeTextChanged:" + s + "-" + start + "-" + count + "-" + after); 
	                 
	            }

				@Override
				public void onTextChanged(CharSequence s, int arg1,
						int arg2, int arg3) {
					
					if (mIsMultiline && mMaxLength > 0) {
						int fontw = (int) (mFontSize*1.2);
						
						String text = mInputEditText.getText().toString();
						
						int length = mInputEditText.getText().length();
						
						int len = 0;
						for (int i = 0; i < mInputEditText.getText().length(); i++) {
							char ch = mInputEditText.getText().charAt(i);
							
							if (ch == '\n') {
								len += mWidth/fontw;
							}
							
							if (length + len >= mMaxLength) {
								text = text.substring(0, i);
								break;
							}
						}
											
						if ((length + len) >= mMaxLength) {
														
							mInputEditText.setText(text);
							mInputEditText.setSelection(text.length());
							return;
						}
					}
				}
		});
	}
	
	public void onEndEditing()
	{
		TextInputField.this.setTextInputFieldResult(TextInputField.this.mInputEditText.getText().toString());
		TextInputField.this.closeKeyboard();
		
		AppActivity.s_textInputField = null;
	}

	private void openKeyboard() {
		final InputMethodManager imm = (InputMethodManager) AppActivity.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.showSoftInput(this.mInputEditText, 0);
//		imm.setInputMethod(this.mInputEditText.getWindowToken(), "com.android.inputmethod.pinyin");
		
		((Activity) AppActivity.getContext()).getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);
	}

	private void closeKeyboard() {
		final InputMethodManager imm = (InputMethodManager) AppActivity.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.hideSoftInputFromWindow(this.mInputEditText.getWindowToken(), 0);
	}

	public Handler mHandler = new Handler(Looper.getMainLooper()) {
		public void handleMessage(Message msg) {
			if (msg.what == MESSAGE_REMOVE_TEXT_INPUT_FIELD) {
				View rootView = Cocos2dxGLSurfaceView.getInstance().getRootView();
				((ViewGroup) rootView).removeView(mInputEditLayout);
			}
		}
	};
	
	public void setTextInputFieldResult(final String pResult) {
		try {
			final byte[] bytesUTF8 = pResult.getBytes("UTF8");

			AppActivity.getAppContext().runOnGLThread(new Runnable() {
				@Override
				public void run() {
					nativeSetTextInputFieldResult(bytesUTF8);
					mHandler.sendEmptyMessage(MESSAGE_REMOVE_TEXT_INPUT_FIELD);
				}
			});
		} catch (UnsupportedEncodingException pUnsupportedEncodingException) {
			/* Nothing. */
		}
	}

	private static native void nativeSetTextInputFieldResult(final byte[] pBytes);

	public static class TextInputFieldMessage {
		public String title;
		public String content;
		public int posX;
		public int posY;
		public int width;
		public int height;
		public int fontSize;
		public int inputMode;
		public int inputFlag;
		public int returnType;
		public int maxLength;

		public TextInputFieldMessage(String title, String content, int x,
				int y, int width, int height, int fontSize, int inputMode, int inputFlag,
				int returnType, int maxLength) {
			this.content = content;
			this.title = title;
			this.posX = x;
			this.posY = y;
			this.width = width;
			this.height = height;
			this.fontSize = fontSize;
			this.inputMode = inputMode;
			this.inputFlag = inputFlag;
			this.returnType = returnType;
			this.maxLength = maxLength;
		}

		public void showTextField() {
			
			TextInputField textInputField = new TextInputField(title, content,
					posX, posY, width, height, fontSize, inputMode, inputFlag,
					returnType, maxLength);
			textInputField.showTextField();
			
			AppActivity.s_textInputField = textInputField;
		}

	}
}
