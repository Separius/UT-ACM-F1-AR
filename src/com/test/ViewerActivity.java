package com.test;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.VideoView;


//usage : 
//	image : settype("image"); addStringextra("path", $path);
//	web : settype("web"); addStringExtra("url", $url);
// video : not tested :  setdataandtype($uri, "video");
public class ViewerActivity extends Activity {

	private WebView 	webv;
	private ImageView 	imgv;
	private VideoView 	vidv;
	private Bitmap bm = null;
	@SuppressLint("SetJavaScriptEnabled")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_viewer);
		
		Log.e("rana", "viewer started");
		webv = (WebView)   findViewById(R.id.webView1);
		imgv = (ImageView) findViewById(R.id.imageView1);
		vidv = (VideoView) findViewById(R.id.videoView1);
		
		webv.setVisibility(View.GONE);
		imgv.setVisibility(View.GONE);
		webv.setVisibility(View.GONE);
		
		Intent intent=getIntent();	
		String type = intent.getType();
		
		if(type.contains("video"))
		{
			vidv.setVisibility(View.VISIBLE);
			vidv.setVideoURI(intent.getData());
		}
		else if (type.contains("image"))
		{
			imgv.setVisibility(View.VISIBLE);   	
	    	bm = BitmapFactory.decodeFile(intent.getStringExtra("path"));//URI not URL
	    	imgv.setImageBitmap(bm);
		}
		else if (type.equals("web"))
		{
			webv.setVisibility(View.VISIBLE);
			webv.getSettings().setJavaScriptCanOpenWindowsAutomatically(true);
			webv.getSettings().setJavaScriptEnabled(true);
			webv.clearCache(true);
			webv.getSettings().setCacheMode(WebSettings.LOAD_NO_CACHE);
			webv.getSettings().setAppCacheEnabled(false);
			webv.setWebViewClient(new HelloWebViewClient());
			
			webv.loadUrl(intent.getStringExtra("url"));//URL not URI
		}
		else Toast.makeText(this, "Unable to show Content", Toast.LENGTH_SHORT).show();	
	}
	
	@Override
	public void onDestroy() {
		setResult(RESULT_OK);
        super.onDestroy();
        Log.e("rana","viewer destroyed");
        
        if(bm != null)
        	bm.recycle();
    }	
	private class HelloWebViewClient extends WebViewClient {
	    @Override
	    public boolean shouldOverrideUrlLoading(WebView view, String url) {
	        // This line right here is what you're missing.
	        // Use the url provided in the method.  It will match the member URL!
	        view.loadUrl(url);
	        return true;
	    }
	}
	
}
