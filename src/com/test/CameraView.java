package com.test;

import java.io.IOException;

import android.content.Context;
import android.hardware.Camera;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.SurfaceHolder.Callback;
import android.graphics.ImageFormat;

import android.util.Log;
import android.graphics.PixelFormat;
import android.graphics.ImageFormat;

import org.opencv.core.Mat;
import org.opencv.core.CvType;
import org.opencv.highgui.*;
import android.hardware.Camera.PreviewCallback;


import java.util.List;

public class CameraView extends SurfaceView implements Callback, PreviewCallback {
    private Camera camera;
    
    
	private static String TAG = "rana"; 
    private byte mBuffer[];
    private Mat[] mFrameChain;
    private int mChainIdx = 0;
    private boolean mStopThread = false; 

    
 
    public CameraView( Context context ) {
        super( context );
        // We're implementing the Callback interface and want to get notified
        // about certain surface events.
        getHolder().addCallback( this );
        // We're changing the surface to a PUSH surface, meaning we're receiving
        // all buffer data from another component - the camera, in this case.
        getHolder().setType( SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS );
    }
 
    public void surfaceCreated( SurfaceHolder holder ) {
        // Once the surface is created, simply open a handle to the camera hardware.
        camera = Camera.open();
    }
 
 
    public void surfaceChanged( SurfaceHolder holder, int format, int width, int height ) {
        // This method is called when the surface changes, e.g. when it's size is set.
        // We use the opportunity to initialize the camera preview display dimensions.
        Camera.Parameters params = camera.getParameters();
        params.setPreviewFormat(ImageFormat.NV21);
        params.setPreviewSize( width, height );
        //params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        
 		List<String> FocusModes = params.getSupportedFocusModes();

			if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE))
				params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
			else if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
				params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
			else if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO))
				params.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
			else if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_EDOF))
				params.setFocusMode(Camera.Parameters.FOCUS_MODE_EDOF);
			else if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_FIXED))
				params.setFocusMode(Camera.Parameters.FOCUS_MODE_FIXED);
			else if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_MACRO))
				params.setFocusMode(Camera.Parameters.FOCUS_MODE_MACRO);
			else if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_INFINITY))
				params.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY);
			else 
			{
			}
				

        
        camera.setParameters( params );
 
		Log.d(TAG, "set preview call back");
		int size = width * height;
		size  = size * ImageFormat.getBitsPerPixel(params.getPreviewFormat()) / 8;
		mBuffer = new byte[size];
		camera.addCallbackBuffer(mBuffer);
		camera.setPreviewCallbackWithBuffer(this); // 
		Log.d(TAG, "done!");
		mFrameChain = new Mat[2];
		mFrameChain[0] = new Mat(height + (height/2), width, CvType.CV_8UC1);
		mFrameChain[1] = new Mat(height + (height/2), width, CvType.CV_8UC1);
 
 
        // We also assign the preview display to this surface...
        try {
            camera.setPreviewDisplay( holder );
        } catch( IOException e ) {
            e.printStackTrace();
        }
        // ...and start previewing. From now on, the camera keeps pushing preview
        // images to the surface.
        camera.startPreview();
        
        new Thread(new CameraWorker()).start();
    }
 
    public void surfaceDestroyed( SurfaceHolder holder ) {
        // Once the surface gets destroyed, we stop the preview mode and release
        // the whole camera since we no longer need it.
        camera.stopPreview();
        camera.release();
        camera = null;
    }
    
	public void onPreviewFrame(byte[] frame, Camera arg1) {
        //Log.d(TAG, "Preview Frame received. Frame size: " + frame.length);
        synchronized (this) {
            mFrameChain[1 - mChainIdx].put(0, 0, frame);
            Highgui.imwrite("/sdcard/f1/gotJava.jpg",mFrameChain[1 - mChainIdx]);
            this.notify();
        }
       
       
        //FindFeatures(0);
       
        if (camera != null)
            camera.addCallbackBuffer(mBuffer);
    }
    
    public native void FindFeatures(long matAddrGr);

   private class CameraWorker implements Runnable {

		long lastTime = 0;
        public void run() {
            do {
                synchronized (CameraView.this) {
                    try {
                        CameraView.this.wait();
                    } catch (InterruptedException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }

                if (!mStopThread) {
                    if (!mFrameChain[mChainIdx].empty()) {
                  //      deliverAndDrawFrame(cameraFrame[mChainIdx]);
				//		Log.d(TAG, "heey!");
						FindFeatures(mFrameChain[mChainIdx].getNativeObjAddr());
					
					}
                    mChainIdx = 1 - mChainIdx;
                }
            } while (!mStopThread);
            Log.d(TAG, "Finish processing thread");
        }
    }

}
