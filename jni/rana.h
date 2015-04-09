#ifndef __RANA_H
#define __RANA_H
#include "Util.h"
#include <cassert>

template<class PointType>//TODO clean it up
float PointSquaredDistanceFloat(PointType p1, PointType p2) {
	return (((float)(p1.x-p2.x))*((float)(p1.x-p2.x))) +
		(((float)(p1.y-p2.y))*((float)(p1.y-p2.y)));
}

/** \brief Returns the min element among 4 input arguments.
  * \param a0	First elem.
  * \param a1	Second elem.
  * \param a2	3rd elem.
  * \param a3	4th elem.
  * \return min elem.
*/
template<class T>
T ranaMinElement(T a0, T a1, T a2, T a3) {

	T m1 = a0 < a1 ? a0 : a1;
	T m2 = a2 < a3 ? a2 : a3;
	return m1 < m2 ? m1 : m2;
}

template<class T>
std::size_t ranaMinElementIndex(T a0, T a1, T a2, T a3) {

	std::size_t id1 = a0 < a1 ? 0 : 1;
	std::size_t id2 = a2 < a3 ? 2 : 3;
	T m1 = a0 < a1 ? a0 : a1;
	T m2 = a2 < a3 ? a2 : a3;
	return m1 < m2 ? id1 : id2;
}

template<class T>
T ranaMaxElement(T a0, T a1, T a2, T a3) {

	T m1 = a0 > a1 ? a0 : a1;
	T m2 = a2 > a3 ? a2 : a3;
	return m1 > m2 ? m1 : m2;
}

template<class T>
std::size_t ranaMaxElementIndex(T a0, T a1, T a2, T a3) {

	std::size_t id1 = a0 > a1 ? 0 : 1;
	std::size_t id2 = a2 > a3 ? 2 : 3;
	T m1 = a0 > a1 ? a0 : a1;
	T m2 = a2 > a3 ? a2 : a3;
	return m1 > m2 ? id1 : id2;
}


/** \brief Returns a new Rectange around the points but bigger
  * \param points	 Set of points.
  * \param margin	 margin.
  * \param maxW      max width
  * \param maxH		 max height
  * \return rectangle around the points.
*/

cv::Rect ranaCropRectangle(std::vector<alvar::PointDouble> points, int margin, int maxW, int maxH);
cv::Rect ranaCropRectangle(std::vector<cv::Point2f> points, int margin, int maxW, int maxH);


#endif // __RANA_H
