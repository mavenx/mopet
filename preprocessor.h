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
	Preprocessor(std::string refdir, std::string image);
	virtual ~Preprocessor();

	bool loadImageSet(std::vector<cv::Mat> &image_set);

	// all images of image set should be nearly the same!
	// SURF is used
	//
	// @return transformed imageSet
	std::vector<cv::Mat> matchImages(std::vector<cv::Mat> &image_set);

private:
	std::string refdir_;
	std::string image_;

};

#endif /* PREPROCESSOR_H_ */
