#include<iostream>
#include<errno.h>
#include<wiringPiI2C.h>
#include<unistd.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include "L3G4200D.h"
#include "ADXL345.h"
#include <string.h>
#include <unistd.h>
#include <string>

#define PORT 8080

int main(){

  L3G4200D gyroCtrl;
  ADXL345 acclCtrl(true);
  
  int server_fd,new_socket,valread;

  struct sockaddr_in address;
  int opt = 1;
  
  int addrlen = sizeof(address);
  char buffer[10] = {0};

  const char* gyrostart = "GYRODATA";
  const char* ok = "OK";
  
  float* gyroData = new float[3];
  
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
  
  send(new_socket,gyrostart,strlen(gyrostart),0);

  valread = recv(new_socket,buffer,5,0);
  std::string receivedData = buffer;
  std::cout<<"client response:"<<buffer<<std::endl;
  if(!receivedData.compare("OK")){
    std::cout<<"Gyro reading"<<std::endl;
    while(1){
	    	  gyroCtrl.update();
		  gyroData[0] = gyroCtrl.getGyroX();
		  gyroData[1] = gyroCtrl.getGyroY();
		  gyroData[2] = gyroCtrl.getGyroZ();
		  std::cout<<"Sending:"<<gyroData[0]<<"  "<<gyroData[1]<<"  "<<gyroData[2]<<"  "<<std::endl;
		  sleep(1);
		  send(new_socket,gyroData,3*sizeof(float),0);
		  valread = read(new_socket,buffer,2);
		  std::string response = buffer;
		  std::cout<<"Received:"<<response<<std::endl;
		  if(response.compare("OK")){
		    std::cout<<"client did not say OK, stopping sending"<<std::endl;
    		    break;
		  }
	  }
      
    }
  
}
