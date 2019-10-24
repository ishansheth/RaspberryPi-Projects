#include <opencv2/opencv.hpp>

void flip(cv::Mat& A)
{
  for(int i = 0; i < A.rows; i++)
  {
    cv::Mat x = A.row(i);
    
    for(int j = 0; j < x.cols/2; j++)
    {
      auto temp = x.at<double>(0,j);
      x.at<double>(0,j) = x.at<double>(0,x.cols-j-1);
      x.at<double>(0,x.cols-j-1) = temp;
    }    
  }  
}

int main(int argc,char* argv[])
{
  cv::Mat image;
  cv::Mat out_image;
  
  image = cv::imread("/home/ishan/raspberryPiApplicationsLibraries/video_stream/frame.png",CV_LOAD_IMAGE_COLOR);

  if(!image.data)
    {
      std::cout<<"image is not loaded correctly"<<std::endl;
      return -1;
    }
  else
    {
      std::cout<<"image is loaded correctly"<<std::endl;
      std::cout<<image<<std::endl;
      cv::flip(image,out_image,1);

      imwrite("out.png",out_image);
    }



  
}

