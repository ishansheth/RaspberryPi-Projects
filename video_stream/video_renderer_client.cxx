#include <iostream>
#include <cstdlib>
#include "SensorSocketUDP.hxx"
#include <opencv2/opencv.hpp>
#include <chrono>

#define BUF_LEN 65540
#define PACK_SIZE 4096

void flip(cv::Mat& A)
{
  std::cout<<"flipping----"<<'\n';
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



int main(int argc,char* argv[]){

  if(argc < 4 || argc > 4){
    std::cerr<<"Usage:"<<argv[0]<<"<server addr> <server port> <local port>"<<std::endl;
    exit(1);
  }
  
  int port = atoi(argv[2]);

  if(port < 0 || port > 65535){
    fprintf(stderr,"The server port number is wrong");
    exit(1);
  }

  cv::namedWindow("Camera Feed",CV_WINDOW_AUTOSIZE);
  
  unsigned short localPort = atoi(argv[3]);
  rpi::connect::udp::SensorServerSocketUDP sock(localPort);

  std::string srcAddr;
  unsigned short srcPort;

  cv::Mat grayImage;
  cv::Mat gray3Channel;
  cv::Mat rawData;
  cv::Mat decodedFrame;
  cv::Mat combinedImage;
  cv::Mat flippedGrayChannelImage;
  
  char buffer[BUF_LEN];
  
  std::cout<<"Client using local port:"<<localPort<<std::endl;

  std::string sendCmd = "SEND";

  // sending video server a command to send the frames
  sock.sendTo(sendCmd.c_str(),sendCmd.size(),argv[1],port);

  clock_t lastcyc = clock();
  
  while(1){
    
    int bytesreceived = sock.recvFrom(buffer,BUF_LEN,srcAddr,srcPort);

    int total_pack = ((int*)buffer)[0];
    
    memset(buffer,sizeof(buffer),0);
    
    char* longbuf = new char[PACK_SIZE*total_pack];

    auto start = std::chrono::system_clock::now();
    
    for(int i = 0;i<total_pack;i++){
      bytesreceived = sock.recvFrom(buffer,BUF_LEN,srcAddr,srcPort);
      if(bytesreceived != PACK_SIZE){
	std::cerr<<"received unexpected size of packet:"<<bytesreceived<<std::endl;
	continue;
      }
      memcpy(&longbuf[i*PACK_SIZE],buffer,PACK_SIZE);
    }

    rawData = cv::Mat(1,PACK_SIZE*total_pack,CV_8UC1,longbuf);

    decodedFrame = imdecode(rawData,CV_LOAD_IMAGE_COLOR);

    if(decodedFrame.size().width == 0){
      std::cerr<<"decode failure!"<<std::endl;
      continue;
    }

    cv::cvtColor(decodedFrame,grayImage,CV_BGR2GRAY);

    /**
       A coloured and grey scale image have 3 and 1 channels respectively. You cannot just combine their 
       respective matrices together so you need to convert one to the other. With this code, I converted 
       the grey scale to a coloured one to facilitate the combination
    **/
    
    cv::cvtColor(grayImage,gray3Channel,CV_GRAY2BGR);
    
    combinedImage = cv::Mat(grayImage.size().height,grayImage.size().width*2,CV_8UC1);
    cv::flip(gray3Channel,flippedGrayChannelImage,1);

    cv::hconcat(decodedFrame,flippedGrayChannelImage,combinedImage);

    // adding blue separation line between two images
    cv::line(combinedImage,cv::Point(grayImage.size().width,0),
	                   cv::Point(grayImage.size().width,grayImage.size().height),
	                   cv::Scalar(255,0,0),4,8);
    
    imshow("Camera Feed",combinedImage);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> processingTime = end - start;
    //    std::cout<<"Processing time of frame:"<<processingTime.count()<<" sec"<<std::endl;
    
    free(longbuf);

    char k = cv::waitKey(33);
    // if ESC key is pressed, then leave the loop and exit
    if(k == 27)
      break;
    else if(k == 32){
      std::cout<<"spacebar is pressed,saving the frame to the disk"<<std::endl;
      // find out about compression para
      std::vector<int> compPara;
      compPara.push_back(CV_IMWRITE_PNG_COMPRESSION);
      compPara.push_back(9);
      imwrite("frame.png",decodedFrame);
    }
    
    clock_t nextcyc = clock();

    double duration = (nextcyc - lastcyc) / (double) CLOCKS_PER_SEC;

    //    std::cout<<"Frame rendering duration:"<<duration<<" sec"<<std::endl;

    //    std::cout<<"Effective FPS:"<<(1/duration)<<"  kbps:"<<(PACK_SIZE*total_pack / duration / 1024 * 8)<<std::endl;
    //    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    lastcyc = nextcyc;
  }
  
  return 0;
}

