/*
 * preprocessor.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: michael
 */

#include "preprocessor.h"
#include <dirent.h>

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

    while ((dp = readdir(dirp)) != NULL && count < 1)
    {
        std::string fileName = image_;
        if(fileName != ".." && fileName != ".")
        {
            std::string imgFile = refdir_ + fileName;
            cv::Mat img = cv::imread(imgFile, CV_LOAD_IMAGE_COLOR);

            if(! img.data )                              // Check for invalid input
            {
                std::cout <<  "Could not open or find the image: " << image_ << std::endl ;
                return false;
            }

            image_set.push_back(img);
//            preprocess(img);

//            addResponsesForImage(img, responses);
            count++;

        }
    }
    closedir(dirp);

    return true;
}



