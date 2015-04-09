package com.test;

import java.io.File;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.MediaController;
import android.widget.Toast;
import android.widget.VideoView;

import org.opencv.android.OpenCVLoader;



import android.view.WindowManager;


public class CameraProjectActivity extends Activity {
	
	
    static 
    {
    	if(!OpenCVLoader.initDebug())
		{
			//handle something
		}
		System.loadLibrary("rana");
    }

	
    /** Called when the activity is first created. */
    @Override
    public void onCreate( Bundle savedInstanceState ) {
        super.onCreate( savedInstanceState );
     
        // When working with the camera, it's useful to stick to one orientation.
        setRequestedOrientation( ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE );
     
        // Next, we disable the application's title bar...
        requestWindowFeature( Window.FEATURE_NO_TITLE );
        // ...and the notification bar. That way, we can use the full screen.
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,
                                WindowManager.LayoutParams.FLAG_FULLSCREEN );

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
     
     
        setContentView(R.layout.main);
        // Now let's create an OpenGL surface.
        GLSurfaceView glView = (GLSurfaceView) findViewById(R.id.gl_view);
        // To see the camera preview, the OpenGL surface has to be created translucently.
        // See link above.
        glView.setEGLConfigChooser( 8, 8, 8, 8, 16, 0 );
        glView.getHolder().setFormat( PixelFormat.TRANSLUCENT );
        // The renderer will be implemented in a separate class, GLView, which I'll show next.
        glView.setRenderer( new GLClearRenderer() );
        // Now set this as the main view.
        //setContentView( glView );
     
        // Now also create a view which contains the camera preview...
        CameraView cameraView = new CameraView( this );
        // ...and add it, wrapping the full screen size.
        addContentView( cameraView, new LayoutParams( LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT ) );
    }
    private boolean popUpState = false; 
    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // MotionEvent reports input details from the touch screen
        // and other input controls. In this case, you are only
        // interested in events where the touch position changed.
 
    	
    	//Image view
    	if(!popUpState)
    	{
	    	Intent intent = new Intent(this, ViewerActivity.class);
	    	intent.setType("image");
	    	intent.putExtra("path", "/sdcard/logo.jpg");
	    	//intent.addFlags(Intent.FLAG_ACTIVITY_PREVIOUS_IS_TOP);
	    	startActivityForResult(intent, 0);
	    	popUpState = !popUpState;
    	}
    	
    	//~ //start video Activity
    	//~ //popout
    	//~ 
    	//~ File file = new File("/sdcard/ted.mp4");
    	//~ Intent intent = new Intent(Intent.ACTION_VIEW);
    	//~ intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    	//~ intent.setDataAndType(Uri.fromFile(file), "video/mp4");
    	//~ startActivity(intent);  
    	//~ 
    	//~ 
    	//~ //web view
    	//~ if(!popUpState)
    	//~ {
	    	//~ Intent intent = new Intent(this, ViewerActivity.class);
	    	//~ //intent.putExtra("ContentType", "WEB");
	    	//~ intent.setType("web");
	    	//~ //intent.addFlags(Intent.FLAG_ACTIVITY_PREVIOUS_IS_TOP);
	    	//~ //File file = new File("/sdcard/img.jpg");
	    	//~ intent.putExtra("url", "http://185.8.172.38");
	    	//~ //startActivity(intent);
	    	//~ startActivityForResult(intent, 0);
	    	//~ 
	    	//~ popUpState = !popUpState;
    	//~ }
    	return true;
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
            // Make sure the request was successful
    	popUpState = false;
    	Log.e("rana", "recieved result");
    }
}
