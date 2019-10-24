#ifndef SENSORSOCKETTCP_HXX_
#define SENSORSOCKETTCP_HXX_

#include <iostream>
#include "SensorSocket.hxx"

namespace rpi{

  namespace connect{

    namespace tcp{
      
      class SensorServerSocketTCP:public rpi::connect::SensorSocket{

      public:
 
	SensorServerSocketTCP(unsigned short localPort,int queuelen);
  
	SensorServerSocketTCP(const std::string& localAddress,unsigned short localPort,int queuelen);

	~SensorServerSocketTCP(){}

	void setListen(int queuelen);

	SensorSocket* accept();
      };
    }
  }
}
#endif
