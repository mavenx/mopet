#include <iostream>
#include <string>

//#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>

#include "helper.h"
#include "preprocessor.h"

std::vector<cv::Mat> image_set; // input image array

int main(int argc, char* argv[])
{
  Helper helper;

  bool success = helper.verifyInputArguments(argc, argv);

  if (!success)
    return -1;

  Preprocessor preprocessor(helper.getRefDir(), helper.getImage());
  preprocessor.loadImageSet(image_set);

//   preprocess images (matching,stitching,....)



  // detect and remove foreground objects

  	cv::namedWindow( "Out", CV_WINDOW_NORMAL);
  	cv::imshow("Out", image_set.at(0));
  	cv::waitKey(0);


std::cout << "FIN" << std::endl;
  return 0;
}

