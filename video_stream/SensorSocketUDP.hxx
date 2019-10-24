#ifndef SENSORSOCKETUDP_HXX_
#define SENSORSOCKETUDP_HXX_

//#include <iostream>
#include "SensorSocket.hxx"

namespace rpi{

  namespace connect{

    namespace udp{
      
      class SensorServerSocketUDP:public rpi::connect::SensorSocket{

	void setBroadcast();
  
      public:

	SensorServerSocketUDP();
    
	SensorServerSocketUDP(unsigned short localPort);
  
	SensorServerSocketUDP(const std::string& localAddress,unsigned short localPort);

	~SensorServerSocketUDP(){}
	
	void disconnect();
	
	void sendTo(const void* buffer,int bufferlen,const std::string& foreignAddress,unsigned short port);
	
	int recvFrom(void* buffer,int bufferlen,std::string& sourceAddress,unsigned short& sourcePort);
	
	void setMultiCastTTL(unsigned char multicastTTL);
	
	void joinGroup(const std::string& multicastGrp);

	void leaveGroup(const std::string& multicastGrp);  
      };
    }
  }
}
#endif
