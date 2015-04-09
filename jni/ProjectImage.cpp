#include "ProjectImage.h"
#include "Filter.h"
#include "rana.h"
#include <iostream>

using namespace alvar;
using namespace cv;
using namespace std;

#include <android/log.h>
#define  LOG_TAG    "OCV:libnative_activity"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
ProjectImage::ProjectImage(Camera * camera, cv::Mat mat)
{
	image = mat;
	cam = camera;
	imagePos.resize(4);
	projectPos.resize(4);
	
	//filter.resize(8, FilterDoubleExponentialSmoothing(0.3,0.8));//sexy
	//filter.resize(8, FilterRunningAverage(0.2));//stable but not sexy
	filter.resize(8, FilterDoubleExponentialSmoothing(0.3,0.3));

	double w = 1;
	double h = 1;

/*	
	double visualize3d_points[4][3] = {
			{ -(0.5), -(0.5), 0 },
			{ -(0.5),  (h - 0.5), 0 },
			{  (w - 0.5),  (h - 0.5), 0 },
			{  (w - 0.5), -(0.5), 0 }
		};
*/

  	double visualize3d_points[4][3] = {
		{ -(0.5), -(1.0), 0 },
		{ -(0.5),  -2.5, 0 },
		{  (2.5),  -(2.5), 0 },
		{  (2.5), -(1.0), 0 }
	};


	for(unsigned int i = 0; i < 4; i++)
		for(unsigned int j = 0; j < 3; j++) 
		{
			realPos [ i ] [ j ] = visualize3d_points [ i ] [ j ];
		}
	
	imagePos[0] = cv::Point2f(float(0),float(0));
	imagePos[1] = cv::Point2f(float(0),float(image.rows));
	imagePos[2] = cv::Point2f(float(image.cols),float(image.rows));
	imagePos[3] = cv::Point2f(float(image.cols),float(0));
}

bool ProjectImage::resizeImage(std::vector <cv::Point2f> newImagePos) 
{
	if ( newImagePos.size() != 4 )
	{
			return false;
	}
	
	for(unsigned int i = 0; i < newImagePos.size(); i++ ) 
	{
		imagePos [ i ] = newImagePos [ i ];
	}

	
}

void ProjectImage::updatePose(alvar::Pose * pose, double size) 
{
		CvMat visualize3d_points_mat;
		
		double sizeW = size;
		double sizeH = size;
		
		double visualize3d_points[4][3] = {
				{realPos[0][0]*sizeW, realPos[0][1]*sizeH, realPos[0][2] * size},
				{realPos[1][0]*sizeW, realPos[1][1]*sizeH, realPos[1][2] * size},
				{realPos[2][0]*sizeW, realPos[2][1]*sizeH, realPos[2][2] * size},
				{realPos[3][0]*sizeW, realPos[3][1]*sizeH, realPos[3][2] * size}
		};
		
		CvMat visualize2d_points_mat;
		double visualize2d_points[4][2];

		cvInitMatHeader(&visualize3d_points_mat, 4, 3, CV_64F, visualize3d_points);
		cvInitMatHeader(&visualize2d_points_mat, 4, 2, CV_64F, visualize2d_points);
		cam->ProjectPoints(&visualize3d_points_mat, pose, &visualize2d_points_mat);

		for(unsigned int i = 0; i < 4; i++) 
		{
			projectPos [ i ] = cv::Point2f( filter[2*i].next(visualize2d_points [ i ] [ 0 ]), filter[2*i+1].next(visualize2d_points [ i ] [ 1 ]) );
		}
		
		 homography= cv::findHomography(imagePos,projectPos,0 );
		//CvMat logoWarped;
		//CvwarpPerspective(image,logoWarped,H,imageMain.size() );

}
void ProjectImage::updateImage(Mat newImage)
{
	image = newImage;
}

void ProjectImage::fastWarp(cv::Mat drawMat, cv::Rect drawRect) 
{
	unsigned int lastX = (unsigned int) (drawRect.x + drawRect.width);
	unsigned int lastY = (unsigned int) (drawRect.y + drawRect.height);
	
    double M[9];
    
    // TODO drop the middle Mat ...
    Mat matM(3, 3, CV_64F, M);
    
    // TODO findHomography with float, no convert faster ...
    homography.convertTo(matM, matM.type());
    
    
    // TODO better invert for float? maybe?
    invert(matM, matM);

	
	int dX, dY;
		
	unsigned char *input = (unsigned char*)(drawMat.data);
	unsigned char *idata = (unsigned char*)(image.data);

/*
	for (int i = 0; i < 3; i++ )
		for (int j = 0; j < 3; j++) 
		{
			char buffer[256];
			sprintf(buffer, "%5.4f", M[j*3+i]);
			cv::putText(drawMat, std::string(buffer), cv::Point(100 + 100 * i,300 + 30 * j), //TODO
			cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 0,255,255));
		}

*/

	int dstIndex, srcIndex;
	double X0, Y0, W0, l;

	for (unsigned int x = drawRect.x; x < lastX; x++) 
		for(unsigned int y = drawRect.y; y < lastY; y++)
		{
			
			// load 4 float using vld1q_f32 ?
			X0 = M[0]*x + M[1]*y + M[2];
            Y0 = M[3]*x + M[4]*y + M[5];
            W0 = M[6]*x + M[7]*y + M[8];


			// TODO check max int limit
			// TODO hardcode :D
			if(W0 < 0.00001 && W0 > 0.00001)
				l = 0;
			else
				l = 1.0 / W0;

			// TODO hmmm?
			dX = (int) (X0 * l);
			dY = (int) (Y0 * l);
			
			if(dX > 0 && dY > 0 && dX < image.cols && dY < image.rows)
			{
				// TOFF && KASIF
				
				// IGNORE WITH ALPHA ?
					dstIndex = drawMat.step[0] * y + x * drawMat.step[1];
					srcIndex = image.step[0] * dY + dX * image.step[1];
				
				// alpha dar : 0.6 is hardcoded :))
				//~ input[drawMat.step[0] * y + x * drawMat.step[1] ] = (unsigned char)(0.4 * (float)input[drawMat.step[0] * y + x * drawMat.step[1] ] + 0.6 * (float) (idata [image.step[0] * dY + dX * image.step[1]]));
				//~ input[drawMat.step[0] * y + x * drawMat.step[1] + 1 ] = (unsigned char)(0.4 * (float)input[drawMat.step[0] * y + x * drawMat.step[1] + 1 ] + 0.6 * (float)(idata [image.step[0] * dY + dX * image.step[1] + 1]));
				//~ input[drawMat.step[0] * y + x * drawMat.step[1] + 2 ] = (unsigned char)(0.4 * (float)input[drawMat.step[0] * y + x * drawMat.step[1] + 2 ] + 0.6 * (float)(idata [image.step[0] * dY + dX * image.step[1] + 2]));

				input[ dstIndex ] = idata [ srcIndex ];
				input[ dstIndex + 1 ] = idata [srcIndex + 1];
				input[ dstIndex + 2 ] = idata [srcIndex + 2];
			}
			else
			{
			//	input[drawMat.step[0] * y + x * drawMat.step[1] + 2 ] = 255;
			}
//			input[drawMat.step * y + x ] += idata [image.cols * dY + dX];
//			input[drawMat.step * y + x ] += idata [image.cols * dY + dX];

		
			//LOGE("%u %u --> %u %u", x, y, dX, dY);
		}
}

void ProjectImage::draw(cv::Mat mainImage) 
{
	
	
	
	//~ int64 time = cv::getTickCount();
//~ 
	cv::Rect rect = ranaCropRectangle(projectPos, 0, mainImage.size().width, mainImage.size().height);

	/*
	for (int i=0; i<4; i++) {
		cv::line( mainImage,
				cv::Point((int)projectPos[i].x, (int)projectPos[i].y), cv::Point((int)projectPos[(i+1)%4].x, (int)projectPos[(i+1)%4].y),
				Scalar( 0, 255, 0,255 ), 2, 8 );
	}
	*/

	fastWarp(mainImage, rect);


	// for(unsigned int i = 0; i < 4; i++)
	//	projectPos[i] -= Point2f(rect.x, rect.y);
	// homography= cv::findHomography(imagePos,projectPos,0 );
	//cout <<"TIME BEGIN" <<cv::getTickCount() <<"\n";
	//~ LOGE("HOMO time START %lld ", time);
	//~ cv::warpPerspective(image,logoWarped,homography, mainImage.size() );
	//~ int64 then = cv::getTickCount();
	//~ LOGE("HOMO time END %lld ", then);
	//~ LOGE("HOMO FREQ %lf", cv::getTickFrequency());
	//~ LOGE("HOMO time %lf", 1000*(double)(then - time)/(double)(cv::getTickFrequency() ) );
	//~ cout <<"HOMO GOGOGO"<<"\n";

//	Point2f src[3], dst[3];
//	for(int i = 0; i < 3; i++)
//	{
//		src[i] = imagePos[i];
//		dst[i] = projectPos[i];
//	}
//
//	Mat mMat(2, 3, CV_32FC1);
//	mMat = getAffineTransform(src, dst);
//
//	cv::warpAffine(image,logoWarped, mMat, mainImage.size());


//	LOGE("logoWarped size %d, %d", logoWarped.size().width,logoWarped.size().height );
//	cv::Mat newWraped;
//	cvtColor(logoWarped, newWraped, CV_RBG2RGBA);
	//showFinal(mainImage, logoWarped);
	//LOGE("mainImage size %d, %d", mainImage.size().width,mainImage.size().height );

//	cv::Mat newSrc = cv::Mat(logoWarped.rows,logoWarped.cols,CV_8UC4);
//	newSrc.reshape(1,newSrc.rows*newSrc.cols).col(3).setTo(Scalar(126));
//	int from_to[] = { 0,0, 1,1, 2,2, 3,3 };
//
//	cv::mixChannels(&logoWarped,2,&newSrc,1,from_to,4);
//
	//LOGE("warp time : %lf", 1000*(double)(cv::getTickCount() - time)/(double)(cv::getTickFrequency()));

	//~ mainImage += logoWarped;
}


void ProjectImage::showFinal(cv::Mat src1, cv::Mat src2)
{
	cv::Mat gray,gray_inv,src1final,src2final;
	cvtColor(src2,gray,CV_BGR2GRAY);
	threshold(gray,gray,0,255,CV_THRESH_BINARY);
	//adaptiveThreshold(gray,gray,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,5,4);
	bitwise_not ( gray, gray_inv );
	src1.copyTo(src1final,gray_inv);
	src2.copyTo(src2final,gray);
	cv::Mat finalImage = src1final+src2final;
//	cv::namedWindow( "output", CV_WINDOW_AUTOSIZE );
//	cv::imshow("output",finalImage);

//	cv::imwrite("k.jpg", finalImage);
}
 
