/*
 * remover.h
 *
 *  Created on: Jan 2, 2013
 *      Author: michael
 */
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include "ForegroundClassifier.h"

#ifndef ColorVarianceForegroundClassifier_H_
#define ColorVarianceForegroundClassifier_H_

class ColorVarianceForegroundClassifier : public ForegroundClassifier {
public:
    ColorVarianceForegroundClassifier();
	virtual ~ColorVarianceForegroundClassifier();

	void detectForeground(std::vector<cv::Mat> &gray_set, std::vector<cv::Mat> &color_set, std::vector<cv::Mat> &foregroundMask);


	/**
	 * performs a parameter sweep of alpha/beta and writes the results to folder out/
	 */
	void ParameterSweep(std::vector<cv::Mat> &gray_set, std::vector<cv::Mat> &color_set, cv::Mat &foregroundMask);
};

#endif /* ColorVarianceForegroundClassifier_H_ */
