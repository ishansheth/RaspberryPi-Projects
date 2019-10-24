#include "SensorSocketUDP.hxx"
using namespace rpi::connect::udp;

void
SensorServerSocketUDP::setBroadcast(){
  int broadcastpermission = 1;
  setsockopt(m_sockDesc,SOL_SOCKET,SO_BROADCAST,(void*)&broadcastpermission,sizeof(broadcastpermission));
}

SensorServerSocketUDP::SensorServerSocketUDP():
  SensorSocket(SOCK_DGRAM,IPPROTO_UDP){
  setBroadcast();
}

SensorServerSocketUDP::SensorServerSocketUDP(unsigned short localPort):
  SensorSocket(SOCK_DGRAM,IPPROTO_UDP){
  setLocalPort(localPort);
  setBroadcast();
}

SensorServerSocketUDP::SensorServerSocketUDP(const std::string& localAddress,unsigned short localPort):
  SensorSocket(SOCK_DGRAM,IPPROTO_UDP){
  setLocalAddressAndPort(localAddress,localPort);
  setBroadcast();
}

void
SensorServerSocketUDP::disconnect(){
  sockaddr_in nullAddr;
  memset(&nullAddr,0,sizeof(nullAddr));
  nullAddr.sin_family = AF_UNSPEC;
  
  if(::connect(m_sockDesc,(sockaddr*)&nullAddr,sizeof(nullAddr)) < 0){
    if(errno != EAFNOSUPPORT)
      std::cout<<"Disconnect failed"<<std::endl;
  }
}

void
SensorServerSocketUDP::sendTo(const void* buffer,int bufferlen,const std::string& foreignAddress,unsigned short port){
  sockaddr_in destAddr;
  
  fillAddr(foreignAddress,port,destAddr);
  
  if(sendto(m_sockDesc,(void*)buffer,bufferlen,0,(sockaddr*)&destAddr,sizeof(destAddr)) != bufferlen){
      std::cout<<"sendto failed"<<std::endl;
  }
}

int
SensorServerSocketUDP::recvFrom(void* buffer,int bufferlen,std::string& sourceAddress,unsigned short& sourcePort){
  struct sockaddr_in clntAddr;
  socklen_t addrLen = sizeof(clntAddr);
  bzero(&clntAddr, sizeof(clntAddr));
  
  int rtn;

  if((rtn = recvfrom(m_sockDesc,(void*)buffer,bufferlen,0,(sockaddr*)&clntAddr,(socklen_t*)&addrLen))<0){
    std::cout<<"Receive failed"<<std::endl;
  }

  sourceAddress = inet_ntoa(clntAddr.sin_addr);
  sourcePort = ntohs(clntAddr.sin_port);
  
  return rtn;
}

void
SensorServerSocketUDP::setMultiCastTTL(unsigned char multicastTTL){
  if(setsockopt(m_sockDesc,IPPROTO_IP,IP_MULTICAST_TTL,(void*)&multicastTTL,sizeof(multicastTTL))<0)
    std::cout<<"Multicast TTL failed"<<std::endl;
}

void
SensorServerSocketUDP::joinGroup(const std::string& multicastGrp){
  struct ip_mreq multicastRequest;
  
  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGrp.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

  if(setsockopt(m_sockDesc,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void*)&multicastRequest,sizeof(multicastRequest))<0)
    std::cout<<"Multicast group join failed"<<std::endl;
}

void
SensorServerSocketUDP::leaveGroup(const std::string& multicastGrp){
  struct ip_mreq multicastRequest;
  
  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGrp.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  
  if(setsockopt(m_sockDesc,IPPROTO_IP,IP_DROP_MEMBERSHIP,(void*)&multicastRequest,sizeof(multicastRequest))<0)
    std::cout<<"Multicast group join failed"<<std::endl;
}
