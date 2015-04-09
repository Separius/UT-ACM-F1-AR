#ifndef _PROJECT_IMAGE_H
#define _PROJECT_IMAGE_H

#include "Pose.h"
#include "Camera.h"
#include "Filter.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

class Point3D {
private :
	double coordinate [3];
public:
	
	Point3D ( double x, double y, double z ) {
		set (x, y, z);
	}

	void set ( double x, double y, double z ) 
	{
		coordinate [ 0 ] = x;
		coordinate [ 1 ] = y;
		coordinate [ 2 ] = z;
	}
	
	double getX() { return coordinate [ 0 ]; }
	double getY() { return coordinate [ 1 ]; }
	double getZ() { return coordinate [ 2 ]; }
	
};

class ProjectImage
{
private:
	cv::Mat image;
	cv::Mat homography;
	alvar::Camera * cam;
	std::vector <cv::Point2f> imagePos;
	std::vector <cv::Point2f> projectPos;    
	double realPos[4][3];
	std::vector<alvar::FilterDoubleExponentialSmoothing> filter;
	cv::Mat logoWarped;


	void showFinal(cv::Mat src1, cv::Mat src2);
	
	void fastWarp(cv::Mat drawMat, cv::Rect drawRect); // faster than opencv, at least :))
	
public:
	ProjectImage(alvar::Camera * camera, cv::Mat mat);	
	bool resizeImage(std::vector <cv::Point2f> newImagePos);
	void updatePose(alvar::Pose * pose, double size);
	void updateImage(cv::Mat newImage);
	void draw(cv::Mat mainImage);	
	cv::Mat getResultMat() { return logoWarped;}

};

#endif // _PROJECT_IMAGE_H
