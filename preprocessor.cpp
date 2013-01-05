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

using namespace cv;
using namespace std;

Preprocessor::Preprocessor(std::string refdir, std::string image) {

	refdir_ = refdir;
	image_ = image;
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
//            preprocess(img);

//            addResponsesForImage(img, responses);
            count++;
//            std::cout <<  count << "..";

        }
    }
    std::cout << image_set.size( )<<" input images loaded." << std::endl;
    closedir(dirp);

    return true;
}

void Preprocessor::matchImages(std::vector<cv::Mat> &image_set, std::vector<cv::Mat> &transformed, std::vector<cv::Mat> &transformed_gray_set)
{
	bool eq_hist = true;

	Mat temp;
	Mat first_img = image_set.at(0);
	Mat next_img;

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
	extractor.compute( first_img, keypts_first, descriptors_first );


	for(unsigned int frames = 1; frames < image_set.size(); frames++)
	{
		cout << "Transforming frame #" << frames << endl;

		next_img = image_set.at(frames);
//		resize(image_set.at(0), next_img, Size(image_set.at(0).cols*scale, image_set.at(0).rows*scale));

		//-- Step 1: Detect the keypoints using SURF Detector
		detector.detect( next_img, keypts_next );

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

		for( int i = 0; i < descriptors_next.rows; i++ )
		{
			if( matches[i].distance < 2*min_dist )
			{
				good_matches.push_back( matches[i]);
			}
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
