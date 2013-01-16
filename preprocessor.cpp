/*
 * preprocessor.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: michael
 */

#include "preprocessor.h"
#include <dirent.h>
#include "opencv2/core/core.hpp"
//#include "opencv2/features2d/features2d.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <stdio.h>

#define HISTMATCH_EPSILON 0.000001

using namespace cv;
using namespace std;

Preprocessor::Preprocessor(std::string refdir, int numberOfImagesToUse) {

	refdir_ = refdir;
	this->numberOfImagesToUse = numberOfImagesToUse;
}

Preprocessor::~Preprocessor() {
	// TODO Auto-generated destructor stub
}

bool Preprocessor::loadImageSet(std::vector<cv::Mat> &image_set)
{
	// load images
	int count = 0;
    DIR* dirp = opendir(refdir_.c_str());
    struct dirent* dp;



    while ((dp = readdir(dirp)) != NULL)
    {
        std::string fileName = std::string(dp->d_name);
        if(fileName != ".." && fileName != ".")
        {
            std::string imgFile = refdir_ + fileName;
            cv::Mat img = cv::imread(imgFile, CV_LOAD_IMAGE_COLOR);

            if(! img.data )                              // Check for invalid input
            {
                std::cout <<  "Could not open image: " << image_ << std::endl ;
                return false;
            }

            image_set.push_back(img);


            count++;

            if(count == numberOfImagesToUse)
                break;

        }
    }
    std::cout << image_set.size( )<<" input images loaded." << std::endl;
    closedir(dirp);


    //match all images to image 0

    for(unsigned i = 1; i < image_set.size();i++)
    {
        //histMatchRGB(image_set.at(i),cv::Mat_<uchar>::ones(image_set.at(1).size()), image_set.at(0),cv::Mat_<uchar>::ones(image_set.at(1).size()));
        //cv::imwrite("out/after.jpg", image_set.at(1));
    }




    return true;
}

void Preprocessor::matchImages(std::vector<cv::Mat> &image_set, std::vector<cv::Mat> &transformed, std::vector<cv::Mat> &transformed_gray_set)
{
	bool eq_hist = false;

	Mat temp;
	Mat first_img = image_set.at(0);
	Mat next_img;

	const int maxNumKeypoints = -1;
	const unsigned minNumKeypoints = 150;

	transformed.push_back(first_img);

	cv::cvtColor(image_set.at(0), temp, CV_BGR2GRAY);
	if(eq_hist)
		cv::equalizeHist(temp, temp);

	transformed_gray_set.push_back(temp);

	first_img = image_set.at(0);
//	resize(image_set.at(0), first_img, Size(image_set.at(0).cols*scale, image_set.at(0).rows*scale));


//	int minHessian = 500;
//	cv::SurfFeatureDetector detector( minHessian, 2, 2, false, false);
	cv::FastFeatureDetector detector(40, false);
//	cv::GFTTDetector detector( int maxCorners=1000, double qualityLevel=0.01, double minDistance=1,
//	                  int blockSize=3, bool useHarrisDetector=false, double k=0.04 );

	cv::SurfDescriptorExtractor extractor;
	cv::FlannBasedMatcher matcher;

	std::vector< DMatch > matches;
	std::vector<KeyPoint> keypts_next, keypts_first;
	Mat descriptors_next, descriptors_first;

	detector.detect( first_img, keypts_first );

	while(keypts_first.size() > (int)maxNumKeypoints && maxNumKeypoints > 0)
	{
	    //remove all keypoints after numberOfImagesToUse

	    keypts_first.pop_back();
	}

	extractor.compute( first_img, keypts_first, descriptors_first );


	for(unsigned int frames = 1; frames < image_set.size(); frames++)
	{
		cout << "Transforming frame #" << frames << endl;

		next_img = image_set.at(frames);
//		resize(image_set.at(0), next_img, Size(image_set.at(0).cols*scale, image_set.at(0).rows*scale));

		//-- Step 1: Detect the keypoints using SURF Detector
		detector.detect( next_img, keypts_next );

		while(keypts_next.size() > (int)maxNumKeypoints && maxNumKeypoints > 0)
        {
            //remove all keypoints after maxNumKeypoints

		    keypts_next.pop_back();
        }

		/*
		 * Draw position of keypoints (for debugging):
		 *
		 * for(unsigned i = 0; i < keypts_next.size(); i++)
		{
		    int x,y;

		    x = keypts_next.at(i).pt.x;
		    y = keypts_next.at(i).pt.y;

		    if(x > 0 && x < next_img.cols && y > 0 && y < next_img.rows)
		    {
		        Vec3b &v = next_img.at<Vec3b>(y,x);

		        v[0] = 0;
		        v[1] = 0;
		        v[2] = 255;
		    }
		}

		cv::namedWindow( "Fore", CV_WINDOW_NORMAL);
        cv::imshow("Fore", next_img);
        cv::waitKey(0);*/


		//-- Step 2: Calculate descriptors (feature vectors)
		extractor.compute( next_img, keypts_next, descriptors_next );

		//-- Step 3: Matching descriptor vectors using FLANN matcher
		matcher.match( descriptors_next, descriptors_first, matches );

		double max_dist = 0; double min_dist = 100;
		cout << descriptors_next.rows << endl;
		//-- Quick calculation of max and min distances between keypoints
		for( int i = 0; i < descriptors_next.rows; i++ )
		{
			double dist = matches[i].distance;
			if( dist < min_dist ) min_dist = dist;
			if( dist > max_dist ) max_dist = dist;
		}

		cout << "max distance: " << max_dist << endl;
		cout << "min distance: " << min_dist << endl;

		//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
		std::vector< DMatch > good_matches;

		if(min_dist == 0)
		    min_dist = 0.05;

		for( int i = 0; i < descriptors_next.rows; i++ )
		{
			if( matches[i].distance < 2*min_dist )
			{
				good_matches.push_back( matches[i]);
			}
		}

		int prev_factor = 2;
		int cur_factor;

		while(good_matches.size() < minNumKeypoints)
		{
		    //we need at least 4 matches for a homography
		    //but use some more, since the moving objects in the image may produce some outliers

		    //so add some other matches, which have not been added yet:

		    cur_factor = prev_factor+1;

		    for( int i = 0; i < descriptors_next.rows; i++ )
            {
                if( matches[i].distance > prev_factor*min_dist &&  matches[i].distance < cur_factor*min_dist)
                {
                    good_matches.push_back( matches[i]);

                    if(good_matches.size() >= minNumKeypoints)
                        break;
                }
            }

		    prev_factor = cur_factor;

		}

		//-- Localize the object
		std::vector<Point2f> next;
		std::vector<Point2f> first;

		for(unsigned int i = 0; i < good_matches.size(); i++ )
		{
			//-- Get the keypoints from the good matches
			next.push_back( keypts_next[ good_matches[i].queryIdx ].pt );
			first.push_back( keypts_first[ good_matches[i].trainIdx ].pt );
		}

		Mat H = findHomography( next, first, CV_RANSAC, 3);

//		cout << "H: "<<H << endl;

		//// create matching image:

		//-- Get the corners from the image_1 ( the object to be "detected" )
		std::vector<Point2f> next_corners(4);
		next_corners[0] = cvPoint(0,0); next_corners[1] = cvPoint( next_img.cols, 0 );
		next_corners[2] = cvPoint( next_img.cols, next_img.rows ); next_corners[3] = cvPoint( 0, next_img.rows );
		std::vector<Point2f> first_corners(4);

		perspectiveTransform( next_corners, first_corners, H);

		// COLORED
		warpPerspective(next_img, temp, H, Size(image_set.at(0).cols, image_set.at(0).rows),INTER_LINEAR, BORDER_CONSTANT);
		transformed.push_back(temp);

		// GRAYSCALE
		cv::cvtColor(image_set.at(frames), next_img, CV_BGR2GRAY);
		if(eq_hist)
			cv::equalizeHist(next_img, next_img);

		warpPerspective(next_img, temp, H, Size(image_set.at(0).cols, image_set.at(0).rows),INTER_LINEAR, BORDER_CONSTANT, 255);
		transformed_gray_set.push_back(temp);
	}
}


// Compute histogram and CDF for an image with mask
void Preprocessor::do1ChnHist(const cv::Mat &_i, const cv::Mat &mask, double* h, double* cdf)
{
    cv::Mat _t = _i.reshape(1,1);
    cv::Mat _tm;

    mask.copyTo(_tm);
    _tm = _tm.reshape(1,1);

    for(int p=0;p<_t.cols;p++)
    {
        if(_tm.at<uchar>(0,p) > 0)
        {
            uchar c = _t.at<uchar>(0,p);
            h[c] += 1.0;
        }
    }

    //normalize hist
    Mat _tmp(1,256,CV_64FC1,h);
    double minVal,maxVal;
    cv::minMaxLoc(_tmp,&minVal,&maxVal);
    _tmp = _tmp / maxVal;

    cdf[0] = h[0];
    for(int j=1;j<256;j++)
    {
        cdf[j] = cdf[j-1]+h[j];
    }

    //normalize CDF
    _tmp.data = (uchar*)cdf;
    cv::minMaxLoc(_tmp,&minVal,&maxVal);
    _tmp = _tmp / maxVal;
}

//#define BTM_DEBUG


// match histograms of 'src' to that of 'dst', according to both masks
void Preprocessor::histMatchRGB(cv::Mat& src, const cv::Mat& src_mask, const cv::Mat& dst, const cv::Mat& dst_mask)
{
#ifdef BTM_DEBUG
    namedWindow("original source",CV_WINDOW_AUTOSIZE);
    imshow("original source",src);
    namedWindow("original query",CV_WINDOW_AUTOSIZE);
    imshow("original query",dst);
#endif

    vector<Mat> chns;
    split(src,chns);
    vector<Mat> chns1;
    split(dst,chns1);
    Mat src_hist = Mat::zeros(1,256,CV_64FC1);
    Mat dst_hist = Mat::zeros(1,256,CV_64FC1);
    Mat src_cdf = Mat::zeros(1,256,CV_64FC1);
    Mat dst_cdf = Mat::zeros(1,256,CV_64FC1);
    Mat Mv(1,256,CV_8UC1);
    uchar* M = Mv.ptr<uchar>();

    for(int i=0;i<3;i++) {
        src_hist.setTo(0);
        dst_hist.setTo(0);
        src_cdf.setTo(0);
        src_cdf.setTo(0);

        double* _src_cdf = src_cdf.ptr<double>();
        double* _dst_cdf = dst_cdf.ptr<double>();
        double* _src_hist = src_hist.ptr<double>();
        double* _dst_hist = dst_hist.ptr<double>();

        do1ChnHist(chns[i],src_mask,_src_hist,_src_cdf);
        do1ChnHist(chns1[i],dst_mask,_dst_hist,_dst_cdf);

        uchar last = 0;


        for(int j=0;j<src_cdf.cols;j++) {
            double F1j = _src_cdf[j];

            for(uchar k = last; k<dst_cdf.cols; k++) {
                double F2k = _dst_cdf[k];
                if(abs(F2k - F1j) < HISTMATCH_EPSILON || F2k > F1j) {
                    M[j] = k;
                    last = k;
                    break;
                }
            }
        }

        Mat lut(1,256,CV_8UC1,M);
        LUT(chns[i],lut,chns[i]);
    }

    Mat res;
    merge(chns,res);

#ifdef BTM_DEBUG
    namedWindow("matched",CV_WINDOW_AUTOSIZE);
    imshow("matched",res);

    waitKey(0);
#endif

    res.copyTo(src);
}
