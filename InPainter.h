/*
 * InPainter.h
 *
 *  Created on: Jan 13, 2013
 *      Author: tom
 */

#ifndef INPAINTER_H_
#define INPAINTER_H_

#include "VarianceForegroundClassifier.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>

class InPainter {
public:
    InPainter();
    virtual ~InPainter();

    /**
     * This method uses the image with the index 0 from the color_set
     * as a starting point.
     * It then replaces the pixels classified as foreground (foregroundMasks)
     * with the mean value over all pixels(from the other images) classified as background.
     * in addition some kind of alpha blending is performed.
     */
    void inPaint(std::vector<cv::Mat> &color_set, std::vector<cv::Mat> &foregroundMasks, cv::Mat& outputImage);
};

#endif /* INPAINTER_H_ */
