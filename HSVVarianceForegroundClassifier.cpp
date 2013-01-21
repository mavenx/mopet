/*
 * remover.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: michael
 */

#include "HSVVarianceForegroundClassifier.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>

using namespace cv;
using namespace std;

HSVVarianceForegroundClassifier::HSVVarianceForegroundClassifier() {

}

HSVVarianceForegroundClassifier::~HSVVarianceForegroundClassifier() {

}



void HSVVarianceForegroundClassifier::detectForeground(std::vector<cv::Mat> &gray_set, std::vector<cv::Mat> &color_set, std::vector<cv::Mat> &foregroundMasks)
{
	std::vector<cv::Mat> s_gray_set;   //smoothed gray set
	std::vector<cv::Mat> s_hsv_set;   //smoothed hsv set


	//create empty foreground masks:
	for(unsigned int f = 0; f < gray_set.size(); f++)
	{
	    foregroundMasks.push_back(cv::Mat::zeros(cv::Size(gray_set.at(0).cols, gray_set.at(0).rows), gray_set.at(0).type()));
	}


	//gaussian filtering:
	for(unsigned int f = 0; f < gray_set.size(); f++)
	{
	    cv::Mat blur;
	    cv::Mat temp;
	    float sigma = 1;

	    cv::GaussianBlur(gray_set.at(f), blur, cv::Size(0,0), sigma);
	    s_gray_set.push_back(blur);

	    cv::GaussianBlur(color_set.at(f), temp, cv::Size(0,0), sigma);


	    // convert to hsv
//	    cv::cvtColor(temp, temp, CV_RGB2HSV);

	    s_hsv_set.push_back(temp);
//	    string filename;
//	        filename = "out/gauss_";
//	        ostringstream converter;
//	        converter << f;
//	        filename += converter.str();
//	        filename += ".jpg";
//	        cv::imwrite(filename, s_color_set.at(f));

	}

	int numpics = gray_set.size();
	cv::Mat values = cv::Mat_<unsigned char>::zeros(1, numpics);
	cv::Mat sortIdx;

	float beta = 2;
	float alpha = 2000;
	float gamma = 5;

	int debug = 0;

	for(int y = 0; y < foregroundMasks.at(0).rows; y++)
	{
	    for(int x = 0; x < foregroundMasks.at(0).cols; x++)
	    {
	        //extract the pixel values to a cv::Mat
	        for(unsigned int f = 0; f < s_gray_set.size(); f++)
	        {
	            values.at<unsigned char>(0,f) = s_hsv_set.at(f).at<Vec3b>(y,x)[0];

	        }

	         //UNCOMMENT if you want to debug certain pixels ;)
//	        if(x == 400 && y == 760)
//	            debug = 1;
//	        else
//	            debug = 0;


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

	                Vec3b newValb = s_hsv_set.at(sortIdx.at<int>(0,start+length-1)).at<Vec3b>(y,x);
                    Vec3f newVal = newValb;

                    if(debug)
                        cout << "newval(length=" << length << "): [" << newVal.val[0] << " " << newVal.val[1] << " " << newVal.val[2] << "]"<< endl;


                    Vec3f diff = newVal-mean;
                    mean = mean + (1/(float)length) * diff;
//                    cout << "sssssss: " << (newVal - mean)[0] << endl;
//	                mean = mean*(length-1) + newVal;
//	                mean = mean/length;

	                if(debug)
	                    cout << "mean: [" << mean.val[0] << " " << mean.val[1] << " " << mean.val[2] << "]"<< endl;



	                if(debug)
	                    cout << "diff: [" << diff.val[0] << " " << diff.val[1] << " " << diff.val[2] << "]"<< endl;

	                variance = variance + diff.mul(newVal - mean);
	                variance = variance/(length);
//	                variance = variance*(length-1)+diff.mul(diff);
//	                variance = variance/length;

	                if(debug)
	                    cout << "variance: [" << variance.val[0] << " " << variance.val[1] << " " << variance.val[2] << "]"<< endl;

//	                func = sqrt(pow(variance[0],2) + pow(variance[1],2) + pow(variance[2],2)) + alpha/pow(length,beta);//1/(variance+1/(float)length);
	                func = sqrt(pow(gamma * variance[0],2) + pow(gamma*variance[1],2) + pow(gamma*variance[2],2)) + alpha/pow(length,beta);//1/(variance+1/(float)length);


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
	                cout << (unsigned)foregroundMasks.at(i).at<uchar>(y,x) << endl;
	        }
	    }
	}


	//remove noise:
	for(unsigned int f = 0; f < gray_set.size(); f++)
    {
	    int morph_size = 2;
	    cv::Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );

	    cv::morphologyEx(foregroundMasks.at(f), foregroundMasks.at(f), MORPH_OPEN, kernel);//, Point(-1,-1), 2);

	    morph_size = 30;
	    kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );
	    cv::morphologyEx(foregroundMasks.at(f), foregroundMasks.at(f), MORPH_CLOSE, kernel);//, Point(-1,-1), 1);
    }

//	for(unsigned int f = 0; f < gray_set.size(); f++)
//	{
//		int morph_size = 5;
//		cv::Mat kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );
//
//		cv::morphologyEx(foregroundMasks.at(f), foregroundMasks.at(f), MOR, kernel);//, Point(-1,-1), 2);
//
////		morph_size = 10;
////		kernel = cv::getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ));//, Point( morph_size, morph_size ) );
////		cv::morphologyEx(foregroundMasks.at(f), foregroundMasks.at(f), MORPH_CLOSE, kernel);//, Point(-1,-1), 1);
//	}

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
