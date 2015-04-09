#include "rana.h"

cv::Rect ranaCropRectangle(std::vector<alvar::PointDouble> points, int margin, int maxW, int maxH)
{
	assert(points.size() == 4);

	double minX = ranaMinElement(points[0].x, points[1].x, points[2].x, points[3].x) - margin;
	double minY = ranaMinElement(points[0].y, points[1].y, points[2].y, points[3].y) - margin;

	if(minX < 0)
		minX = 0;
	if(minY < 0)
		minY = 0;

	double maxX = ranaMaxElement(points[0].x, points[1].x, points[2].x, points[3].x) + margin;
	double maxY = ranaMaxElement(points[0].y, points[1].y, points[2].y, points[3].y) + margin;

	if(maxX > maxW)
		maxX = maxW;
	if(maxY > maxH)
		maxY = maxH;

	return cv::Rect(minX, minY,maxX - minX, maxY-minY);
}

cv::Rect ranaCropRectangle(std::vector<cv::Point2f> points, int margin, int maxW, int maxH)
{
	assert(points.size() == 4);

	float minX = ranaMinElement(points[0].x, points[1].x, points[2].x, points[3].x) - margin;
	float minY = ranaMinElement(points[0].y, points[1].y, points[2].y, points[3].y) - margin;

	if(minX < 0)
		minX = 0;
	if(minY < 0)
		minY = 0;

	float maxX = ranaMaxElement(points[0].x, points[1].x, points[2].x, points[3].x) + margin;
	float maxY = ranaMaxElement(points[0].y, points[1].y, points[2].y, points[3].y) + margin;

	if(maxX > maxW)
		maxX = maxW;
	if(maxY > maxH)
		maxY = maxH;

	return cv::Rect(minX, minY,maxX - minX, maxY-minY);
}

