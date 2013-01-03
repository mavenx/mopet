#include "helper.h"

#include <dirent.h>
#include <fstream>
#include <string>


Helper::Helper()
{

}

bool Helper::verifyInputArguments(int argc, char* argv[])
{
  bool success = true;
  if (argc < 2)
  {
    std::cerr
        << "Usage: ./mopet imageSet_path first_image_name"
        << std::endl
        << std::endl
        << "MOPET Software v1.0"
        << std::endl;
    return false;
  }

  DIR *dir;
  std::string dirname = argv[1];
  if ((dir = opendir(dirname.c_str())) == NULL)
  {
    std::cerr << "[ERROR] Path to image set directory (" << dirname <<") does not exist" << std::endl;
    success = false;
  }
  else
  {
    refdir_ = dirname;

  }

//  for (int i = 1; i <= 3; i++)
//  {
//    dirname.clear();
//    dirname = std::string(argv[1]) + std::string(argv[1 + i]) + "/";
//    if ((dir = opendir(dirname.c_str())) == NULL)
//    {
//      std::cerr << "[ERROR] Class " << std::string(argv[1 + i]) << " does not exist" << std::endl;
//      success = false;
//    }
//    else
//    {
////      classes_[i - 1] = std::string(argv[1 + i]) + "/";
//    }
//  }
  std::string img_path = std::string(argv[1]) + std::string(argv[2]);
  std::ifstream filetest(img_path.c_str());
  if (filetest.fail())
  {
    std::cerr << "[ERROR] No image (" << img_path.c_str() <<") found" << std::endl;
    success = false;
  }
  else
  {
    image_ = argv[2];
  }
  closedir(dir);
  return success;
}


std::string Helper::getRefDir()
{
  return refdir_;
}

std::string Helper::getImage()
{
  return image_;
}
