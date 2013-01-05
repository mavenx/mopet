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

#ifndef REMOVER_H_
#define REMOVER_H_

class Remover {
public:
	Remover();
	virtual ~Remover();

	void detectForeground(std::vector<cv::Mat> &gray_set, cv::Mat &foregroundMask);

	void removeForeground();


};

#endif /* REMOVER_H_ */
