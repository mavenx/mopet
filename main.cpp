#include <iostream>
#include <string>

//#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>

#include "helper.h"
#include "preprocessor.h"
#include "remover.h"



int main(int argc, char* argv[])
{
	std::vector<cv::Mat> image_set; // input image array
	std::vector<cv::Mat> transformed_set; // transformed image array
	std::vector<cv::Mat> transformed_gray_set; // transformed image array
	cv::Mat foreground;

	Helper helper;

	bool success = helper.verifyInputArguments(argc, argv);

	if (!success)
		return -1;

	Preprocessor preprocessor(helper.getRefDir(), helper.getImage());
	Remover remover;
	preprocessor.loadImageSet(image_set);

	// preprocess images
	preprocessor.matchImages(image_set, transformed_set, transformed_gray_set);

	// detect and remove foreground objects
	remover.detectForeground(transformed_gray_set, foreground);



//	for(unsigned int n = 0; n < image_set.size(); n++)
//	{
//		cv::namedWindow( "Out", CV_WINDOW_NORMAL);
//		cv::imshow("Out", transformed_gray_set.at(n));
//		cv::waitKey(0);
//	}

	std::cout << "FIN" << std::endl;
	return 0;
}

