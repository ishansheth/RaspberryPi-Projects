#ifndef GY80SENSORDATA_HXX_
#define GY80SENSORDATA_HXX_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <future>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <unistd.h> 
#include <stdio.h>
#include <queue>
#include <iostream>

#define PORT 8080
#define PI 3.14159265

struct GyroData{
    float xData;
    float yData;
    float zData;
};

static void fillAddr(const std::string& address,unsigned short port,sockaddr_in& addr){

  memset(&addr,0,sizeof(addr));
  addr.sin_family = AF_INET;

  hostent* host;

  if((host = gethostbyname(address.c_str())) == NULL){
    std::cerr<<"Failed to resolve hostname"<<std::endl;
  }

  addr.sin_addr.s_addr = *((unsigned long*) host->h_addr_list[0]);

  addr.sin_port = htons(port);
}


namespace rpi{

  namespace connect{
    
    class SensorSocket{

    protected:
      int m_sockDesc;      //socket descriptor

    public:

      SensorSocket(int type,int protocol);
  
      SensorSocket(int newConnSD);

      ~SensorSocket();

      void connect(const std::string& foreignAddress,unsigned short foreignPort);

      void send(const void* buffer,int bufferlen);

      int recv(void* buffer,int bufferlen);

      std::string getLocalAddress();

      unsigned short getLocalPort();  

      void setLocalPort(unsigned short port);
      
      void setLocalAddressAndPort(const std::string& localAddr,unsigned short localPort);

      std::string getForeignAddress();

      unsigned short getForeignPort();

      static unsigned short resolveService(const std::string& service, const std::string& protocol){
	struct servent* serv;

	if((serv = getservbyname(service.c_str(),protocol.c_str())) == NULL)
	  return atoi(service.c_str());
	else
	  return ntohs(serv->s_port);
      }
  
    };
    
  }

}
#endif
