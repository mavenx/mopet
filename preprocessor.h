/*
 * preprocessor.h
 *
 *  Created on: Jan 2, 2013
 *      Author: michael
 */
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/highgui/highgui.hpp>

#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

class Preprocessor {
public:
	Preprocessor(std::string refdir, int numberOfImagesToUse);
	virtual ~Preprocessor();

	bool loadImageSet(std::vector<cv::Mat> &image_set);

	// all images of image set should be nearly the same!
	// SURF is used
	//
	// @return transformed imageSet (all images aligned
	void matchImages(std::vector<cv::Mat> &image_set, std::vector<cv::Mat> &transformed_set, std::vector<cv::Mat> &transformed_gray_set);

private:

	void do1ChnHist(const cv::Mat &_i, const cv::Mat &mask, double* h, double* cdf);
	void histMatchRGB(cv::Mat& src, const cv::Mat& src_mask, const cv::Mat& dst, const cv::Mat& dst_mask);

	std::string refdir_;
	std::string image_;
	int numberOfImagesToUse;

};

#endif /* PREPROCESSOR_H_ */
