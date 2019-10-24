#include <cstdlib>
#include <iostream>
#include "SensorSocketUDP.hxx"
#include <opencv2/opencv.hpp>
#include <vector>

#define ENCODE_QUALITY 80

#define FRAME_WIDTH (1280/2)
#define FRAME_HEIGHT 720
#define FRAME_INTERVAL (1000/30)
#define PACK_SIZE 4096
#define BUF_LEN 65540

int main(int argc,char* argv[]){

  if(argc != 2){
    std::cerr<<"Usage:"<<argv[0]<<" <server port>\n"<<std::endl;
    exit(1);
  }


  std::cout<<"Starting UDP server..."<<std::endl;
  
  unsigned short port = atoi(argv[1]);
  
  if(port <= 0 || port >= 65535){
    std::cerr<<"Invalid port number given"<<std::endl;
    exit(1);
  }

  cv::Mat frame,send;
  std::vector<unsigned char> encoded;

  cv::VideoCapture cap(0);
  
  if(!cap.isOpened()){
    std::cerr<<"OpenCV failed to open camera"<<std::endl;
    exit(1);
  }
  
  char buffer[BUF_LEN] = {0};

  rpi::connect::udp::SensorServerSocketUDP sock(port);
  std::string srcAddr;
  unsigned short srcPort;

  int bytesreceived = sock.recvFrom(buffer,BUF_LEN,srcAddr,srcPort);

  if(strcmp(buffer,"SEND")){
    std::cout<<"Did not receive correct command from client"<<std::endl;
    exit(1);
  }

  std::cout<<"Received from:->"<<srcAddr<<":"<<srcPort<<std::endl;


  while(1){
    cap >> frame;

    if(frame.size().width == 0)
      continue;

    resize(frame,send,cv::Size(FRAME_WIDTH,FRAME_HEIGHT),0,0,cv::INTER_LINEAR);

    std::vector<int> compression_params;

    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(ENCODE_QUALITY);

    imencode(".jpg",send,encoded,compression_params);

    int total_pack = 1 + (encoded.size() - 1)/PACK_SIZE;

    int ibuf[1];
    ibuf[0] = total_pack;

    std::cout<<"sending frame in "<<total_pack<<" packets"<<std::endl;
    
    sock.sendTo(ibuf,sizeof(ibuf),srcAddr,srcPort);

    for(int i = 0;i<total_pack;i++)
      sock.sendTo(&encoded[i*PACK_SIZE],PACK_SIZE,srcAddr,srcPort);

    std::cout<<"frame sent....."<<std::endl;
  }

  return 0;

}
