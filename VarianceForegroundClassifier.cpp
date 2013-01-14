/*
 * remover.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: michael
 */

#include "VarianceForegroundClassifier.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>

using namespace cv;
using namespace std;

VarianceForegroundClassifier::VarianceForegroundClassifier() {

}

VarianceForegroundClassifier::~VarianceForegroundClassifier() {

}



void VarianceForegroundClassifier::detectForeground(std::vector<cv::Mat> &gray_set, std::vector<cv::Mat> &color_set, std::vector<cv::Mat> &foregroundMasks)
{
	std::vector<cv::Mat> &s_gray_set(gray_set);   //smoothed gray set


	//create empty foreground masks:
	for(unsigned int f = 0; f < gray_set.size(); f++)
	{
	    foregroundMasks.push_back(cv::Mat::zeros(cv::Size(gray_set.at(0).cols, gray_set.at(0).rows), gray_set.at(0).type()));
	}


	//gaussian filtering:
	for(unsigned int f = 0; f < gray_set.size(); f++)
	{
	    cv::GaussianBlur(gray_set.at(f), s_gray_set.at(f), cv::Size(0,0), 3);


	    /*string filename;
	        filename = "out/input_";
	        ostringstream converter;
	        converter << f;
	        filename += converter.str();
	        filename += ".jpg";
	        cv::imwrite(filename, gray_set.at(f));*/

	}

	int numpics = gray_set.size();
	cv::Mat values = cv::Mat_<unsigned char>::zeros(1, numpics);

	float beta = 1;
	float alpha = 70;

	for(int y = 0; y < foregroundMasks.at(0).rows; y++)
	{
	    for(int x = 0; x < foregroundMasks.at(0).cols; x++)
	    {
	        //extract the pixel values to a cv::Mat
	        for(unsigned int f = 0; f < s_gray_set.size(); f++)
	        {
	            values.at<unsigned char>(0,f) = s_gray_set.at(f).at<unsigned char>(y,x);
	        }

	        //sort the pixel values
	        cv::sort(values, values, CV_SORT_ASCENDING + CV_SORT_EVERY_ROW);


	        //variables for remembering the maximum of the
	        //cost function.
	        float max_func = -FLT_MAX;
	        int max_start = 0;
	        int max_length = 0;


	        //iterate over all possible groups of pixel values
	        //and find a group with similar pixel values.
	        //(as a similarity measure, the variance is used)
	        for(int start = 0; start < numpics; start++)
	        {
	            float mean;
	            float variance = 0;

	            for(int length = 1; length < numpics-start+1; length++)
	            {
	                float func;

	                mean = mean*(length-1) + values.at<char>(0,start+length-1);
	                mean = mean/length;

	                int diff = (int)values.at<char>(0,start+length-1)-mean;

	                variance = variance*(length-1)+diff*diff;
	                variance = variance/length;

	                func = variance+alpha/pow(length,beta);//1/(variance+1/(float)length);

	                func = -func;

	                if(func > max_func)
	                {
	                    max_func = func;
	                    max_start = start;
	                    max_length = length;
	                }
	            }
	        }

	        for(int i = 0; i < numpics; i++)
	        {
	            int fore = 1-(s_gray_set.at(i).at<char>(y,x) >= values.at<char>(0,max_start) && s_gray_set.at(i).at<char>(y,x) <= values.at<char>(0, max_start+max_length-1));

	            foregroundMasks.at(i).at<char>(y,x) = fore*255;
	        }
	    }
	}


	//remove noise:
	for(unsigned int f = 0; f < gray_set.size(); f++)
    {
	    int morph_size = 3;

	    cv::Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );


	    cv::morphologyEx(foregroundMasks.at(f), foregroundMasks.at(f), MORPH_CLOSE, kernel);//, Point(-1,-1), 2);
	    cv::morphologyEx(foregroundMasks.at(f), foregroundMasks.at(f), MORPH_OPEN, kernel);//, Point(-1,-1), 1);
    }


	//save foreground masks:
    for(unsigned int f = 0; f < gray_set.size(); f++)
    {
        string filename;
        filename = "out/foreground_";
        ostringstream converter;
        converter << f;
        filename += converter.str();
        filename += ".jpg";
        cv::imwrite(filename, foregroundMasks.at(f));
    }

	/*cv::namedWindow( "Fore", CV_WINDOW_NORMAL);
	cv::imshow("Fore", foregroundMask.at(0));
	cv::waitKey(0);*/
}





void VarianceForegroundClassifier::ParameterSweep(std::vector<cv::Mat> &gray_set, std::vector<cv::Mat> &color_set, cv::Mat &foregroundMask)
{
    cv::Mat temp;
    foregroundMask = cv::Mat::zeros(cv::Size(gray_set.at(0).cols, gray_set.at(0).rows), gray_set.at(0).type());
    cv::Mat test_image = cv::Mat::zeros(cv::Size(gray_set.at(0).cols, gray_set.at(0).rows), color_set.at(0).type());
    /*cv::namedWindow( "Out", CV_WINDOW_NORMAL);
    cv::imshow("Out", gray_set.at(0));
    cv::waitKey(0);*/


    //gaussian filtering:
    for(unsigned int f = 0; f < gray_set.size(); f++)
    {
        //temp = cv::abs(gray_set.at(0) - gray_set.at(f));
//      cv::threshold(temp, temp, 50, 255, CV_THRESH_BINARY);
        //foregroundMask = foregroundMask + temp;
        cv::GaussianBlur(gray_set.at(f), gray_set.at(f), cv::Size(0,0), 3);


        string filename;
            filename = "out/input_";
            ostringstream converter;
            converter << f;
            filename += converter.str();
            filename += ".jpg";
            cv::imwrite(filename, gray_set.at(f));

    }

    int numpics = gray_set.size();
    //int *array = new int[numpics];
    cv::Mat values = cv::Mat_<unsigned char>::zeros(1, numpics);

    float beta = 1;
    for(float alpha = 10; alpha < 400; alpha+=10)
    {
    for(int y = 0; y < foregroundMask.rows; y++)
    {
        for(int x = 0; x < foregroundMask.cols; x++)
        {
            for(unsigned int f = 0; f < gray_set.size(); f++)
            {
                values.at<unsigned char>(0,f) = gray_set.at(f).at<unsigned char>(y,x);
            }

            cv::sort(values, values, CV_SORT_ASCENDING + CV_SORT_EVERY_ROW);

            float max_func = -FLT_MAX;
            int max_start = 0;
            int max_length = 0;

            for(int start = 0; start < numpics; start++)
            {
                float mean;
                float variance = 0;

                for(int length = 1; length < numpics-start+1; length++)
                {
                    float func;

                    mean = mean*(length-1) + values.at<char>(0,start+length-1);
                    mean = mean/length;

                    int diff = (int)values.at<char>(0,start+length-1)-mean;

                    variance = variance*(length-1)+diff*diff;
                    variance = variance/length;

                    func = variance+alpha/pow(length,beta);//1/(variance+1/(float)length);

                    func = -func;

                    if(func > max_func)
                    {
                        max_func = func;
                        max_start = start;
                        max_length = length;
                    }
                }
            }

            cv::Mat fore = cv::Mat_<int>::zeros(1, numpics);

            for(int i = 0; i < numpics; i++)
            {
                fore.at<int>(0, i) = 1-(gray_set.at(i).at<char>(y,x) >= values.at<char>(0,max_start) && gray_set.at(i).at<char>(y,x) <= values.at<char>(0, max_start+max_length-1));
            }



            foregroundMask.at<char>(y,x) = fore.at<int>(0,0)*255;
        }
    }

    string filename;
    filename = "out/alpha_";
    ostringstream converter;
    converter << alpha;
    filename += converter.str();
    filename += ".jpg";
    cv::imwrite(filename, foregroundMask);


    }

    //foregroundMask = cv::abs(foregroundMask - gray_set.at(0));


    //cv::threshold(foregroundMask, foregroundMask, 1, 255, cv::THRESH_BINARY);

    /*for(unsigned int f = 1; f < gray_set.size(); f++)
    {
            temp = cv::abs(gray_set.at(0) - gray_set.at(f));
    //      cv::threshold(temp, temp, 50, 255, CV_THRESH_BINARY);
            foregroundMask = foregroundMask + temp;
    }*/



    // threshold
//  cv::threshold(foregroundMask, foregroundMask, 150, 255, CV_THRESH_BINARY);
//
//
//  // remove noise
    int morph_size = 2;
//  cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
////    cout << "kernel: " << kernel << endl;
//  cv::morphologyEx(foregroundMask, foregroundMask, MORPH_ERODE, kernel, Point(-1,-1), 1);
//  cv::morphologyEx(foregroundMask, foregroundMask, MORPH_CLOSE, kernel, Point(-1,-1), 1);
//
//  // get "moved" regions
//
//
    morph_size = 3;
    cv::Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );


    //cv::morphologyEx(foregroundMask, foregroundMask, MORPH_CLOSE, kernel);//, Point(-1,-1), 2);
    //cv::morphologyEx(foregroundMask, foregroundMask, MORPH_OPEN, kernel);//, Point(-1,-1), 1);

    cv::namedWindow( "Fore", CV_WINDOW_NORMAL);
    cv::imshow("Fore", foregroundMask);
    cv::waitKey(0);
}
