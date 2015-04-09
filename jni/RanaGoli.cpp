#include <jni.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <android/log.h>
//#include <pthread.h>
#include <time.h>
#include <cmath>
#include <queue>

#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/features2d/features2d.hpp>

#include "Camera.h"
#include "MarkerDetector.h"

using namespace std;
using namespace cv;
using namespace alvar;

// Utility for logging:
#define LOG_TAG    "RANA_JNI"
#define LOG(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

GLuint texture;
//cv::VideoCapture capture;
int bufferIndex = 0;
int rgbIndex;
int frameWidth;
int frameHeight;
int screenWidth;
int screenHeight;
int orientation;
//pthread_mutex_t FGmutex;
//pthread_t frameGrabber;
//pthread_attr_t attr;
//struct sched_param param;


cv::Mat buffer[30];
cv::Mat rgbFrame;

pthread_mutex_t FGmutex;

void drawBackground();
void createTexture();
void destroyTexture();
void *frameRetriever(void*);
void main_proccess(Mat mat);


extern "C"
JNIEXPORT void JNICALL Java_com_test_GLClearRenderer_renderBackground(JNIEnv* env, jobject thiz) 
{
 drawBackground();
}

bool kossherInited = false;
	float verticesX[] = {
			-1.0f, -1.0f,  0.0f,		// V1 - bottom left
			-1.0f,  1.0f,  0.0f,		// V2 - top left
			 1.0f, -1.0f,  0.0f,		// V3 - bottom right
			 1.0f,  1.0f,  0.0f			// V4 - top right
	};
	float textureX[] = {    		
			// Mapping coordinates for the vertices
			0.0f, 1.0f,		// top left		(V2)
			0.0f, 0.0f,		// bottom left	(V1)
			1.0f, 1.0f,		// top right	(V4)
			1.0f, 0.0f		// bottom right	(V3)
	};


void initKosSher() 
{
		glEnable(GL_TEXTURE_2D);			//Enable Texture Mapping ( NEW )
		glShadeModel(GL_SMOOTH); 			//Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f); 	//Black Background
		glClearDepthf(1.0f); 					//Depth Buffer Setup
		glEnable(GL_DEPTH_TEST); 			//Enables Depth Testing
		glDepthFunc(GL_LEQUAL); 			//The Type Of Depth Testing To Do
		
		//Really Nice Perspective Calculations
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
	
	
	
	LOG("init kos sher");
	// generate one texture pointer
	glGenTextures(1, &texture);
	// ...and bind it to our array
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// create nearest filtered texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Different possible texture parameters, e.g. GL_CLAMP_TO_EDGE
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	
	cv::Mat mMat = cv::imread("/sdcard/logo.jpg");
	if(!mMat.data)
		LOG("logo failed to load ");
	else
		LOG("logo loaded successfully");
	//cv::imwrite("/sdcard/logoRead.jpg",mMat);
	
	// Use Android GLUtils to specify a two-dimensional texture image from our bitmap 
	//glTexImage2D(GL_TEXTURE_2D, 0, mMat.ptr(), 0);

	cv::cvtColor(mMat, mMat, CV_BGR2RGBA);
	//cv::imwrite("/sdcard/logoRead2.jpg",mMat);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3264, 1840, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) mMat.ptr());
	LOG("init finished");
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 750, 500, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) mMat.ptr());
}

void drawKosSher()
{
	LOG("draw kos sher! start");
	// bind the previously generated texture
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Point to our buffers
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	// Set the face rotation
	glFrontFace(GL_CW);
	
	// Point to our vertex buffer
	glVertexPointer(3, GL_FLOAT, 0, verticesX);
	glTexCoordPointer(2, GL_FLOAT, 0, textureX);
	
	// Draw the vertices as triangle strip
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	LOG("draw kos sher! finish");

	//Disable the client state before leaving
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//LOG("done kos o sher!");
}

void drawBackground() {
	//LOG("drawBackground");
	static std::queue<int64> time_queue;
    int64 then;
	int64 now = cv::getTickCount();
	time_queue.push(now);

	
	if(!kossherInited) {
		kossherInited = true;
		initKosSher();
	}


	//LOG("lock!");
	pthread_mutex_lock(&FGmutex);
	int bI = bufferIndex - 1;
	pthread_mutex_unlock(&FGmutex);
	//LOG("and unlock! %d", bI);
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//LOG("native");
		
	static float mCubeRotation = 0;	
		
	glLoadIdentity();   
	//glTranslatef(0.0f, 0.0f, -4.0f);	
	//glRotatef(mCubeRotation, 1.0f, 1.0f, 1.0f);
	//colored_rect(-0.1, -0.1, 0.3, 0.3, 0.5, 0.5, 0.5);
	
	
	//drawKosSher();
	
	
	if(bI>=0){
		//LOG("let's proccess!");
		cv::flip(buffer[(bI) % 30], rgbFrame, 1);
		main_proccess(rgbFrame);
		//LOG("we're done with cv!");
	}
	
	
	glLoadIdentity();   
	mCubeRotation -= 0.5f; 


	if (time_queue.size() >= 2)
		then = time_queue.front();
	else
		then = 0;

	if (time_queue.size() >= 25)
		time_queue.pop();

	float fps = time_queue.size() * (float)cv::getTickFrequency() / (now-then);

	//LOG("draw fps %f", fps);
}

extern "C"
JNIEXPORT void JNICALL Java_com_test_CameraView_FindFeatures(
		JNIEnv* env, jobject thiz,
		jlong addr)
{
	
	
	
	
	//LOG("whatever!");
	
	static std::queue<int64> time_queue;
    int64 then;
	int64 now = cv::getTickCount();
	time_queue.push(now);
	
	if(true) 
	{
		///////////
	//	LOG("#1");
		cv::Mat& mFrame = *(cv::Mat*)addr;
		imwrite("/sdcard/f1/input.jpg" , mFrame);
	//	LOG("#2");
		cv::Mat mBGR;
	//	LOG("#3");
		
	//	LOG("mFrame's %dx%d", mFrame.cols, mFrame.rows);
		
		cv::cvtColor(mFrame, mBGR, cv::COLOR_YUV2BGR_NV21);
		imwrite("/sdcard/f1/input2.jpg" , mBGR);
		
	//	LOG("#4");
		pthread_mutex_lock(&FGmutex);
	//	LOG("#5");
		mBGR.copyTo(buffer[(bufferIndex++) % 30]);
	//	LOG("#6");
		pthread_mutex_unlock(&FGmutex);
	//	LOG("#7");
		///////////
	}

	if (time_queue.size() >= 2)
		then = time_queue.front();
	else
		then = 0;

	if (time_queue.size() >= 25)
		time_queue.pop();

	float fps = time_queue.size() * (float)cv::getTickFrequency() / (now-then);

	//LOG("============> camera fps %f", fps);
}



bool init=true;
const int marker_size=15;
Camera cam;

void folan(int width, int height) 
{
	double p[16];
	
	cam.GetOpenglProjectionMatrix(p,width,height);
	//GlutViewer::SetGlProjectionMatrix(p);
	
	
	float modelview_mat[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	float proj_mat[16];


	for(int i = 0; i < 16; i++ )
		proj_mat [ i ] = (float)(p [ i ]);
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(proj_mat);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelview_mat);
}

void videocallback(IplImage *image){
	bool flip_image = (image->origin?true:false);
    if (flip_image) {
        cvFlip(image);
        image->origin = !image->origin;
    }
    if (init) {
        init = false;
        cam.SetRes(image->width, image->height);

		folan(image->width, image->height);
    }
    static MarkerDetector<MarkerData> marker_detector;
    marker_detector.SetMarkerSize(marker_size);

    bool tracked = false;

    for (std::size_t i=0; i<marker_detector.markers->size(); i++) {
		int id = (*(marker_detector.markers))[i].GetId();

		{
			bool b = marker_detector.CropTrack(image, &cam, &((*(marker_detector.markers))[i]) , 0.8);
			Pose p = (*(marker_detector.markers))[i].pose;
			if(b) {
				tracked = true;
				static float mCubeRotation = 0;	
				glPushMatrix();
//	
				

				double gl_mat[16];

				float scale = marker_size;

				//scale = 1;

				p.GetMatrixGL(gl_mat);
				GLfloat glMat[16];
				for(int q = 0; q < 16; q++)
				{
					glMat [ q ] = (float)(gl_mat [ q ]);
					if ( q % 4 == 0 )//4 <= q && q < 8 ) //q % 4 == 1)
						glMat[q] *= -1;
				
				}
				for(int i = 0; i < 4; i++) 
				{
					LOG("%f\t%f\t%f\t%f", glMat[0*4 + i], glMat[1*4 + i], glMat[2*4 + i], glMat[3*4 + i]);
				}
	//			LOG("  ");
				for(int i = 0; i < 4; i++) 
				{
	//				LOG("%lf\t%lf\t%lf\t%lf", gl_mat[0*4 + i], gl_mat[1*4 + i], gl_mat[2*4 + i], gl_mat[3*4 + i]);
				}


				LOG ( " points:  " );

				GLfloat q3[] = {
					- 1.0 * scale, -1.0 * scale, 0,
					  1.0 * scale, -1.0 * scale, 0,
					  1.0 * scale,  1.0 * scale, 0,
					- 1.0 * scale,  1.0 * scale, 0
				};


				for(int i = 0; i < 4; i++) {
					float x = q3[i*3 + 0] * glMat[ 0 ] + q3[i*3 + 1] * glMat[ 4 ] + q3[i*3 + 2] * glMat[ 8 ] + glMat [ 12];
					float y = q3[i*3 + 0] * glMat[ 1 ] + q3[i*3 + 1] * glMat[ 5 ] + q3[i*3 + 2] * glMat[ 9 ] + glMat [ 13];
					float z = q3[i*3 + 0] * glMat[ 2 ] + q3[i*3 + 1] * glMat[ 6 ] + q3[i*3 + 2] * glMat[ 10] + glMat [ 14];
					
					
					q3 [i*3 + 0] = x;
					q3 [i*3 + 1] = y;
					q3 [i*3 + 2] = z;
					
					
					LOG("%f\t%f\t%f", x, y, z);
				}


				glMultMatrixf(glMat);
//				glTranslatef(1.f, 0.f, 0.f);
				glScalef(2 * scale,  2 * scale, 1);

				//glTranslatef(0.0f, 0.0f, 100.0f);
				//glRotatef(mCubeRotation, 1.0f, 1.0f, 1.0f);
			
			
				drawKosSher();
				glPopMatrix();
				mCubeRotation -= 0.5f; 
	//			LOG("done drawing!");

				//drawFrame(drawMat, (*(marker_detector.markers))[i].GetMarkerEdgeLength(), p, id);
			}
		}
    }

	static unsigned int count = 0;

    if(!tracked && count % 8 == 0) {
    	count = 0;
    	marker_detector.Detect(image, &cam, true, false);
		for (std::size_t i=0; i<marker_detector.markers->size(); i++) {
			Pose p = (*(marker_detector.markers))[i].pose;
			int id = (*(marker_detector.markers))[i].GetId();
			LOG("<marker found %d", id);
    	}
    }

    count++;

    if (flip_image) {
//		LOG("sepehr");
        cvFlip(image);
        image->origin = !image->origin;
    }
}

void main_proccess(Mat mat)
{
	IplImage* frame = new IplImage(mat);
	if (frame)
		videocallback(frame);
}
