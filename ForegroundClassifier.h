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

#ifndef ForegroundClassifier_H_
#define ForegroundClassifier_H_

class ForegroundClassifier {
public:
    virtual void detectForeground(std::vector<cv::Mat> &gray_set, std::vector<cv::Mat> &color_set, std::vector<cv::Mat> &foregroundMasks) = 0;


    virtual ~ForegroundClassifier()
    {

    }
};

#endif /* VarianceForegroundClassifier_H_ */
