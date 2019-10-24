#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "L3G4200D.h"
#include "ADXL345.h"
#include <string.h>
#include <unistd.h>
#include <string>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>

const int CHUNKSIZE = 30;
const int NUMGYROAXIS = 3;
#define PORT 8080

int main(){

  L3G4200D gyroCtrl;
  ADXL345 acclCtrl(true);
  
  int server_fd,new_socket,valread;

  struct sockaddr_in address;
  int opt = 1;
  
  int addrlen = sizeof(address);
  char buffer[100] = {0};

  const char* sensorstart = "GYRODATA";
  const char* ok = "OK";
  
  float* gyroData = new float[CHUNKSIZE];
  float* accData  = new float[CHUNKSIZE];
  
  if((server_fd = socket(AF_INET,SOCK_STREAM,0)) == 0){
    std::cout<<"socket failed"<<std::endl;
    exit(EXIT_FAILURE);
  }

  if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))){
    std::cout<<"setsockopt failed"<<std::endl;
    exit(EXIT_FAILURE);    
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if(bind(server_fd,(struct sockaddr*)&address,sizeof(address))<0){
    std::cout<<"bind failed"<<std::endl;
    exit(EXIT_FAILURE);        
  }

  if(listen(server_fd,3)<0){
    std::cerr<<"listen failed"<<std::endl;
    exit(EXIT_FAILURE);        
  }

  if((new_socket = accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&addrlen))<0){
    std::cerr<<"accept failed"<<std::endl;
    exit(EXIT_FAILURE);            
  }
  
  send(new_socket,sensorstart,strlen(sensorstart),0);

  valread = recv(new_socket,buffer,100,0);

  std::string receivedData = buffer;
  std::cout<<"client response:"<<buffer<<std::endl;
  memset(buffer,0,sizeof(buffer));

  if(!receivedData.compare("OK")){
    std::cout<<"Gyro reading"<<std::endl;
    while(1){
      valread = recv(new_socket,buffer,100,0);
      receivedData = buffer;

      memset(buffer,0,sizeof(buffer));
      memset(gyroData,0,CHUNKSIZE*sizeof(float));

      if(!receivedData.compare("SEND")){
	for(int i = 0;i<CHUNKSIZE;i=i+3){
	  acclCtrl.update();
	  gyroData[i] = acclCtrl.getAccX();
	  gyroData[i+1] = acclCtrl.getAccY();
	  gyroData[i+2] = acclCtrl.getAccZ();
	}
	send(new_socket,gyroData,CHUNKSIZE*sizeof(float),0);
      }
    }      
  }
}
