/*
 * remover.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: michael
 */

#include "remover.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Remover::Remover() {

}

Remover::~Remover() {

}

//std::vector<cv::Mat> Remover::invertSet(std::vector<cv::Mat> input_set)
//{
//	std::vector<cv::Mat> inverted_set;
//
//	for(int f = 0; f < input_set.size(); f++)
//	{
//		cv::invert(inverted_set.at(f), input_set.at(0));
//	}
//
//
//
//
//	return inverted_set;
//}

void Remover::detectForeground(std::vector<cv::Mat> &gray_set, cv::Mat &foregroundMask)
{
	cv::Mat temp;
	foregroundMask = cv::Mat::zeros(cv::Size(gray_set.at(0).cols, gray_set.at(0).rows), gray_set.at(0).type());
	cv::namedWindow( "Out", CV_WINDOW_NORMAL);
	cv::imshow("Out", gray_set.at(0));
	cv::waitKey(0);

	for(unsigned int f = 1; f < gray_set.size(); f++)
	{
		temp = cv::abs(gray_set.at(0) - gray_set.at(f));
//		cv::threshold(temp, temp, 50, 255, CV_THRESH_BINARY);
		foregroundMask = foregroundMask + temp;
	}

	// threshold
//	cv::threshold(foregroundMask, foregroundMask, 150, 255, CV_THRESH_BINARY);
//
//
//	// remove noise
//	int morph_size = 1;
//	cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
////	cout << "kernel: " << kernel << endl;
//	cv::morphologyEx(foregroundMask, foregroundMask, MORPH_ERODE, kernel, Point(-1,-1), 1);
//	cv::morphologyEx(foregroundMask, foregroundMask, MORPH_CLOSE, kernel, Point(-1,-1), 1);
//
//	// get "moved" regions
//
//
//	morph_size = 5;
//	kernel = cv::getStructuringElement(MORPH_RECT, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );

//	cv::morphologyEx(foregroundMask, foregroundMask, MORPH_DILATE, kernel, Point(-1,-1), 1);
//	cv::morphologyEx(foregroundMask, foregroundMask, MORPH_CLOSE, kernel, Point(-1,-1), 2);


	cv::namedWindow( "Fore", CV_WINDOW_NORMAL);
	cv::imshow("Fore", foregroundMask);
	cv::waitKey(0);
}
