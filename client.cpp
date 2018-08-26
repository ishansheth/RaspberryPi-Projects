#include <iostream>
#include <vector>
#include<string>
#include<chrono>
#include <thread>
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

int startGyroProtocol(const int& sock){
  std::string servResponse;
  char buffer[1024] = {0};
  int valrec = read(sock,buffer,1024);
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
    return -1;
  }else{
    std::cout<<"Error Occured, Data can not be received"<<std::endl;
    return -1;
  }
  return 1;
}

int readBytes(const int& sock,int size,float* buffer){

  int readbytes = 0;
  int receivedBytesChunk = 0;
  while(readbytes != size){
    /**
       The 'read' is told to read 'CHUNKSIZE*sizeof(float)' bytes from the socket, but it is possible that it will read
       less bytes than the reuqested and return the number of bytes read. So there is a need of loop which will keep track
       of number of total bytes requested and total bytes read from the socket. And when both of them are equal, then 
       leave the loop with full buffer. If this loop is not used, then it will give garbage value in the provided buffer
     **/
      receivedBytesChunk = read(sock,buffer+readbytes,CHUNKSIZE*sizeof(float)-readbytes);
      std::cout<<"Received-----"<<receivedBytesChunk<<std::endl;
      if(receivedBytesChunk > 0)
	readbytes += receivedBytesChunk;
      else if(receivedBytesChunk == 0){
	std::cout<<"Server closed the connection"<<std::endl;
	return -1;
      }      
  }
  
  for(int i = 0;i<readbytes/(3*4);i=i+3){
      std::cout<<buffer[i]<<"  "<<buffer[i+1]<<"  "<<buffer[i+2]<<std::endl;
  }
  return 1;
}

void restartGyroModule(const int& sock,struct sockaddr_in serverAddress){
  // TODO: start the whole gyro stack here when server breaks the connection
  int connres= -1;
  int handshakeRes = -1;
  while(connres < 0){
     connres =  connectToServer(sock,serverAddress);
     std::cout<<"Trying to connect................"<<std::endl;
     std::this_thread::sleep_for(std::chrono::seconds(2));
  }
  
  handshakeRes = startGyroProtocol(sock);
  if(handshakeRes > 0)
    std::cout<<"Connection established again!!"<<std::endl;
  else
    std::cout<<"Connection cant be established"<<std::endl;
}

int main(){
  struct sockaddr_in address;
  int sock = 0,valread;
  struct sockaddr_in serv_addr;
  const char* hello = "hello from client";
  float* gyroData =  new float[CHUNKSIZE];

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

  int gyroProtoRes =  startGyroProtocol(sock);  
  
  if(gyroProtoRes>0){
    while(1){
      if(readBytes(sock,CHUNKSIZE*sizeof(float),gyroData) < 0){
	restartGyroModule(sock,serv_addr);
      }
    }
  }else{
    std::cout<<"GyroProtocol could not be started"<<std::endl;
    return -1;
  }
  return 0;
}
