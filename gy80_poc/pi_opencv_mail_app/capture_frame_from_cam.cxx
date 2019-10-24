/**
This programm is to capture the image frame and video from webcam on linux machine 
using openCV APIs. If this program should be run on the raspberrypi, then it has to be cross compiled
**/

#include "opencv2/opencv.hpp"
//#include <opencv2/highgui/highgui.hpp> 
//#include "opencv2/videoio.hpp"
//#include "opencv2/videoio/videoio_c.h" // This header file is needed in openCV 4.0.0 version of the 

#include <iostream>
using namespace cv;

void* grab_video(){
    VideoCapture capture(0);
    double dwidth=capture.get(CV_CAP_PROP_FRAME_WIDTH);
    double dheight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    std::cout<<"width:"<<dwidth<<std::endl;
    std::cout<<"height:"<<dheight<<std::endl;

    Size framesize(static_cast<int>(dwidth),static_cast<int>(dheight));
    VideoWriter owriter("MyVideo.avi",CV_FOURCC('M','J','P','G'),20,framesize,true);

    if(!capture.isOpened()){
            std::cout << "Failed to connect to the camera." << std::endl;
            return 0;
    }

    if(!owriter.isOpened()){
        std::cout<<"can not open videowriter class"<<std::endl;
        return 0;
    }
    while(1){
        Mat frame;
        bool bsuccess = capture.read(frame);
        if(!bsuccess){
                std::cout<<"can not read a frame from camera";
                return 0;
        }
        owriter.write(frame);
//        imshow("MyVideo",frame);

//        if(waitKey(10) == 27){
//                std::cout<<"esc key is pressed"<<std::endl;
//                break;
//        }

    }

}

void* grap_image(){
  VideoCapture cap;

  if(!cap.open(0)){
    return 0;
  }

  for(;;){
    Mat frame;

    cap >> frame;
    if(frame.empty()){
      break;
    }
    imshow("this is you, smile!!",frame);

    if(waitKey(10)==27)
      break;
  }
}

int main(){

	grab_video();

	return 0;
    
}
