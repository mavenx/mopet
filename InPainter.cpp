/*
 * InPainter.cpp
 *
 *  Created on: Jan 13, 2013
 *      Author: tom
 */

#include "InPainter.h"


#include "VarianceForegroundClassifier.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>

using namespace cv;
using namespace std;

InPainter::InPainter() {
    // TODO Auto-generated constructor stub

}

InPainter::~InPainter() {
    // TODO Auto-generated destructor stub
}


/**
 * This method uses the image with the index 0 from the color_set
 * as a starting point.
 * It then replaces the pixels classified as foreground (foregroundMasks)
 * with the mean value over all pixels(from the other images) classified as background.
 * in addition some kind of alpha blending is performed.
 */
void InPainter::inPaint(std::vector<cv::Mat> &color_set, std::vector<cv::Mat> &foregroundMasks, cv::Mat& outputImage)
{
    const int blendradius = 300;

    outputImage = Mat::zeros(color_set.at(0).size(), color_set.at(0).type());
    cv::Mat distanceToForeground = Mat::zeros(color_set.at(0).size(), CV_32FC1);



    //first enhance foreground Mask,
    //since the neighbourhood of the moving object is
    //most likely also contaminated with some artefacts.
    for(unsigned int f = 0; f < color_set.size(); f++)
    {
        int morph_size = 1;

        cv::Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );


        //cv::morphologyEx(foregroundMasks.at(f), foregroundMasks.at(f), MORPH_DILATE, kernel);//, Point(-1,-1), 2);

        /*
        cv::namedWindow( "BG", CV_WINDOW_NORMAL);
        cv::imshow("BG", foregroundMasks.at(f));
        cv::waitKey(0);*/
    }

    //Now calculate the distance transform.
    //As a result we get the distance to all foreground objects
    //This is necessary to obtain the alpha value for alpha blending
    cv::distanceTransform(255-foregroundMasks.at(0), distanceToForeground, CV_DIST_L2, CV_DIST_MASK_PRECISE);


    for(int y = 0; y < foregroundMasks.at(0).rows; y++)
    {
        for(int x = 0; x < foregroundMasks.at(0).cols; x++)
        {
            float alpha = distanceToForeground.at<float>(y,x);

            //calculate a "linear" blendvalue
            alpha = alpha/(float)blendradius;
            if(alpha > 1)
                alpha = 1;


            if(alpha == 1)
            {
                //if alpha == 1, just use the pixel value from the first image
                outputImage.at<Vec3b>(y,x) = color_set.at(0).at<Vec3b>(y,x);
            }
            else
            {
                float c1 = 0, c2 = 0, c3 = 0;
                int sum = 0;
                //calc Mean:
                unsigned int f;
                for(f = 0; f < foregroundMasks.size(); f++)
                {
                    if(foregroundMasks.at(f).at<uchar>(y,x)==0)
                    {
                        c1 += color_set.at(f).at<Vec3b>(y,x)[0];
                        c2 += color_set.at(f).at<Vec3b>(y,x)[1];
                        c3 += color_set.at(f).at<Vec3b>(y,x)[2];
                        sum ++;
                    }
                }


                if(sum == 0)
                {
                    //no background found, all pixel have been classified as
                    //foreground at this position

                    //for debugging purposes mark them as red pixels,
                    //later on we could just use the value of the first image
                    //or the median
                    Vec3b def(0,0,255);
                    outputImage.at<Vec3b>(y,x) = def;
                }
                else
                {
                    Vec3b meanVal;

                    meanVal[0] = c1/sum;
                    meanVal[1] = c2/sum;
                    meanVal[2] = c3/sum;

                    //perform blending between the calculated mean over the background
                    //and the foreground object of image 0 in the neighbourhood.
                    outputImage.at<Vec3b>(y,x) = color_set.at(0).at<Vec3b>(y,x)*alpha + meanVal*(1-alpha);
                }
            }

        }
    }


    /*cv::namedWindow( "BG", CV_WINDOW_NORMAL);
    cv::imshow("BG", outputImage);
    cv::waitKey(0);*/
}

