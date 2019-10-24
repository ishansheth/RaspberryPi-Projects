#include "SensorSocketTCP.hxx"

using namespace rpi::connect::tcp;

SensorServerSocketTCP::SensorServerSocketTCP(unsigned short localPort,int queuelen):
  SensorSocket(SOCK_STREAM,IPPROTO_TCP){
  setLocalPort(localPort);
  setListen(queuelen);
}

SensorServerSocketTCP::SensorServerSocketTCP(const std::string& localAddress,unsigned short localPort,int queuelen):
  SensorSocket(SOCK_STREAM,IPPROTO_TCP){
  setLocalAddressAndPort(localAddress,localPort);
  setListen(queuelen);
}

void
SensorServerSocketTCP::setListen(int queuelen){
  if(listen(m_sockDesc,queuelen)<0){
    std::cerr<<"can not listen for new connection"<<std::endl;      
  }
}

::rpi::connect::SensorSocket*
SensorServerSocketTCP::accept(){
  int newSD;
  if((newSD = ::accept(m_sockDesc,NULL,0))<0){
    std::cerr<<"can not accept new connection"<<std::endl;
  }
  return new rpi::connect::SensorSocket(newSD);
}

