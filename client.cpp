#include <iostream>
#include <string>
extern "C"{
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
}

#define PORT 8080

int main(){
  struct sockaddr_in address;
  int sock = 0,valread;
  struct sockaddr_in serv_addr;
  const char* hello = "hello from client";
  const char* ok = "OK";
  char buffer[1024] = {0};
  float* gyroData =  new float[3];


  if((sock = socket(AF_INET,SOCK_STREAM,0))<0){
    printf("socket can not be created");
    return -1;
  }

  serv_addr.sin_addr.s_addr = inet_addr("169.254.133.250");
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  
  if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
    printf("could not connect");
    return -1;
  }

  valread = recv(sock,buffer,1024,0);

  std::string servResponse = buffer;
  std::cout<<"First Server Response:"<<servResponse<<std::endl;
  
  if(!servResponse.compare("GYRODATA")){
    std::cout<<"Starting to receive gyrodata from server"<<std::endl;
    send(sock,ok,strlen(ok),0);
    while(1){
      valread = recv(sock,gyroData,3*sizeof(float),0);
      std::cout<<"Received Gyro Data:"<<gyroData[0]<<"  "<<gyroData[1]<<"  "<<gyroData[2]<<std::endl;
      send(sock,ok,strlen(ok),0);
    }
  }
  return 0;
}
