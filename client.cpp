#include <iostream>
#include <vector>
#include<string>


extern "C"{
#include<unistd.h> 
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
}

#define PORT 8080

const int CHUNKSIZE = 900;

int connectToServer(const int& sock,struct sockaddr_in serverAddress){
  
  if(connect(sock,(struct sockaddr*)&serverAddress,sizeof(serverAddress))<0){
    return -1;
  }
  return 1;
}

int startGyroProtocol(const int& sock, char* buffer,int size){
  std::string servResponse;
  int valrec = read(sock,buffer,size);
  if(valrec > 0){
    servResponse = buffer;
    std::cout<<"First Server Response:"<<servResponse<<std::endl;
    if(!servResponse.compare("GYRODATA")){
      const char* ok = "OK";
      send(sock,ok,strlen(ok),0);      
    }
    return 1;
  }else if(valrec == 0){
    std::cout<<"Connection is closed by server,client can wait and try again"<<std::endl;
  }else{
    std::cout<<"Error Occured, Data can not be received"<<std::endl;
    return -1;
  }
  return 1;
}

void readBytes(const int& sock,int size,float* buffer){

  int readbytes = 0;
  int receivedBytesChunk = 0;
  while(readbytes != size){
      receivedBytesChunk = read(sock,buffer+readbytes,CHUNKSIZE*sizeof(float)-readbytes);
      std::cout<<"Received-----"<<receivedBytesChunk<<std::endl;

      if(receivedBytesChunk > 0)
	readbytes += receivedBytesChunk;
  }
  
  for(int i = 0;i<readbytes/(3*4);i=i+3){
      std::cout<<buffer[i]<<"  "<<buffer[i+1]<<"  "<<buffer[i+2]<<std::endl;
  }

}

int main(){
  struct sockaddr_in address;
  int sock = 0,valread;
  struct sockaddr_in serv_addr;
  const char* hello = "hello from client";
  char buffer[1024] = {0};
  float* gyroData =  new float[CHUNKSIZE];
  const char* ok = "OK";

  if((sock = socket(AF_INET,SOCK_STREAM,0))<0){
    printf("socket can not be created");
    return -1;
  }

  serv_addr.sin_addr.s_addr = inet_addr("169.254.133.250");
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  int connRes = connectToServer(sock,serv_addr);
  if(connRes > 0)
    std::cout<<"Connection Result:"<<connRes<<std::endl;
  else{
    std::cout<<"Connection Failed!!"<<std::endl;
    return -1;
  }

  int gyroProtoRes =  startGyroProtocol(sock,buffer,1024);  
  
  if(gyroProtoRes>0){
    while(1){
      readBytes(sock,CHUNKSIZE*sizeof(float),gyroData);
    }
  }else{
    std::cout<<"GyroProtocol could not be started"<<std::endl;
    return -1;
  }
  return 0;
}
