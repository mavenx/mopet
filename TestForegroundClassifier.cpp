/*
 * remover.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: michael
 */

#include "TestForegroundClassifier.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>

using namespace cv;
using namespace std;

TestForegroundClassifier::TestForegroundClassifier() {

}

TestForegroundClassifier::~TestForegroundClassifier() {

}



void TestForegroundClassifier::detectForeground(std::vector<cv::Mat> &gray_set, std::vector<cv::Mat> &color_set, std::vector<cv::Mat> &foregroundMasks)
{
	std::vector<cv::Mat> s_gray_set;   //smoothed gray set
	std::vector<cv::Mat> s_color_set;   //smoothed color set


	//create empty foreground masks:
	for(unsigned int f = 0; f < gray_set.size(); f++)
	{
	    foregroundMasks.push_back(cv::Mat::zeros(cv::Size(gray_set.at(0).cols, gray_set.at(0).rows), gray_set.at(0).type()));
	}


	//gaussian filtering:
	for(unsigned int f = 0; f < gray_set.size(); f++)
	{
	    cv::Mat blur;
	    float sigma = 3;

	    cv::GaussianBlur(gray_set.at(f), blur, cv::Size(0,0), sigma);
	    s_gray_set.push_back(blur);

	    cv::GaussianBlur(color_set.at(f), blur, cv::Size(0,0), sigma);
	    s_color_set.push_back(blur);


	    string filename;
	        filename = "out/gauss_";
	        ostringstream converter;
	        converter << f;
	        filename += converter.str();
	        filename += ".jpg";
	        cv::imwrite(filename, s_color_set.at(f));

	}

	int numpics = gray_set.size();
	cv::Mat values = cv::Mat_<unsigned char>::zeros(1, numpics);
	cv::Mat sortIdx;

	float beta = 1;
	float alpha = 200;

	int debug = 0;

	for(int y = 0; y < foregroundMasks.at(0).rows; y++)
	{
	    for(int x = 0; x < foregroundMasks.at(0).cols; x++)
	    {
	        //extract the pixel values to a cv::Mat
	        for(unsigned int f = 0; f < s_gray_set.size(); f++)
	        {
	            values.at<unsigned char>(0,f) = s_gray_set.at(f).at<unsigned char>(y,x);

	        }

	        // UNCOMMENT if you want to debug certain pixels ;)
	        if(x == 230 && y == 170)
	            debug = 1;
	        else
	            debug = 0;


	        if(debug)
	            cout << "values: " << values << endl;

	        //sort the pixel values
	        cv::sortIdx(values, sortIdx, CV_SORT_ASCENDING + CV_SORT_EVERY_ROW);
	        cv::sort(values, values, CV_SORT_ASCENDING + CV_SORT_EVERY_ROW);

	        if(debug)
	        {
	            cout << "sorted values: " << values << endl;
	            cout << "sortIdx: " << sortIdx << endl;
	        }

	        //variables for remembering the maximum of the
	        //cost function.
	        float max_func = -FLT_MAX;
	        int max_start = 0;
	        int max_length = 0;

	        if(debug)
	            cout << "==============: x=" << x << ", y=" << y << endl;

	        //iterate over all possible groups of pixel values
	        //and find a group with similar pixel values.
	        //(as a similarity measure, the variance is used)
	        for(int start = 0; start < numpics; start++)
	        {
	            Vec3f mean = 0;
	            Vec3f variance = 0;


	            if(debug)
	            {
                    cout << "---------" << endl;
                    cout << "start=" << start << endl;
	            }

	            for(int length = 1; length < numpics-start+1; length++)
	            {
	                float func;

	                Vec3b newValb = s_color_set.at(sortIdx.at<int>(0,start+length-1)).at<Vec3b>(y,x);
                    Vec3f newVal = newValb;

                    if(debug)
                        cout << "newval(length=" << length << "):" << newVal << endl;




	                mean = mean*(length-1) + newVal;
	                mean = mean/length;

	                if(debug)
	                    cout << "mean: " << mean << endl;

	                Vec3f diff = newVal-mean;

	                if(debug)
	                    cout << "diff: " << diff << endl;

	                variance = variance*(length-1)+diff.mul(diff);
	                variance = variance/length;

	                if(debug)
	                    cout << "variance: " << variance << endl;

	                func = sqrt(pow(variance[0],2) + pow(variance[1],2) + pow(variance[2],2)) + alpha/pow(length,beta);//1/(variance+1/(float)length);

	                func = -func;

	                if(debug)
	                    cout << "function = " << func << endl;


	                if(func > max_func)
	                {
	                    max_func = func;
	                    max_start = start;
	                    max_length = length;
	                }
	            }
	        }

	        if(debug)
	        {
	            cout << "max_start=" << max_start << ", max_length=" << max_length << endl;
	        }

	        /*for(int i = 0; i < numpics; i++)
	        {
	            //int fore = 1-(s_gray_set.at(i).at<char>(y,x) >= values.at<char>(0,max_start) && s_gray_set.at(i).at<char>(y,x) <= values.at<char>(0, max_start+max_length-1));
	            int fore = 1 - (sortIdx.at<int>(0,max_start) >= i && sortIdx.at<int>(0,max_start+max_length-1) <= i);

	            if(debug)
	                cout << "fore(" << i << ") = " << fore << "  ";
	            foregroundMasks.at(i).at<char>(y,x) = fore*255;
	        }*/

	        for(int i = 0; i < numpics; i++)
	        {
	            if(i >= max_start && i < max_start + max_length)
	            {
	                foregroundMasks.at(sortIdx.at<int>(0,i)).at<uchar>(y,x) = 0;
	            }
	            else
	            {
	                foregroundMasks.at(sortIdx.at<int>(0,i)).at<uchar>(y,x) = 255;
	            }
	        }

	        if(debug)
	        {
	            for(int i = 0; i < numpics; i++)
	                cout << (unsigned)foregroundMasks.at(i).at<uchar>(y,x);
	        }
	    }
	}


	//remove noise:
	for(unsigned int f = 0; f < gray_set.size(); f++)
    {
	    int morph_size = 10;
	    cv::Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );


	    cv::morphologyEx(foregroundMasks.at(f), foregroundMasks.at(f), MORPH_CLOSE, kernel);//, Point(-1,-1), 2);

	    morph_size = 3;
	    kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );
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

        cv::Mat product = Mat::zeros(foregroundMasks.at(f).size(), foregroundMasks.at(f).type());
        multiply(foregroundMasks.at(f), gray_set.at(f), product, 1/(float)255);

        cv::imwrite(filename, product);
    }

	/*cv::namedWindow( "Fore", CV_WINDOW_NORMAL);
	cv::imshow("Fore", foregroundMask.at(0));
	cv::waitKey(0);*/
}





void TestForegroundClassifier::ParameterSweep(std::vector<cv::Mat> &gray_set, std::vector<cv::Mat> &color_set, cv::Mat &foregroundMask)
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
                float mean = 0;
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
