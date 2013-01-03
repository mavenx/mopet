#ifndef HELPER_H_
#define HELPER_H_

#include <iostream>
#include <string>
#include <vector>

class Helper {
public:
  Helper();

  // This methods verifies the command line arguments.
  // If the arguments are not correct, errors will be displayed.
  bool verifyInputArguments(int argc, char* argv[]);


  // This method returns the path to reference directory defined via command line arguments
  // @return std::string
  //         holds the path to reference directory
  std::string getRefDir();

  // This method returns the path to the image via command line arguments
  // @return std::string
  //         holds the path to the image
  std::string getImage();

private:
  std::string refdir_;
  std::string image_;
};

#endif
