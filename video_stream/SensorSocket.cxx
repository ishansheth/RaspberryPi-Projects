#include "SensorSocket.hxx"


rpi::connect::SensorSocket::SensorSocket(int type,int protocol){
  if((m_sockDesc = socket(AF_INET,type,protocol)) < 0){
    std::cerr<<"Socket can not be created"<<std::endl;
  }
}

rpi::connect::SensorSocket::SensorSocket(int newConnSD){
  m_sockDesc = newConnSD;
}

rpi::connect::SensorSocket::~SensorSocket(){
  ::close(m_sockDesc);
  m_sockDesc = -1;
}

void
rpi::connect::SensorSocket::connect(const std::string& foreignAddress,unsigned short foreignPort){
  sockaddr_in destAddr;
  unsigned int addr_len = sizeof(destAddr);
  
  fillAddr(foreignAddress,foreignPort,destAddr);
  
  if(::connect(m_sockDesc,(sockaddr*)&destAddr,addr_len)<0){
    std::cerr<<"Could not connect to foreign host"<<std::endl;
  }
}

void
rpi::connect::SensorSocket::send(const void* buffer,int bufferlen){
  if(::send(m_sockDesc,(void*)buffer,bufferlen,0)<0){
    std::cerr<<"could not send buffer data to foreign host"<<std::endl;
  }
}

int
rpi::connect::SensorSocket::recv(void* buffer,int bufferlen){
  int returnVal;
  if((returnVal = ::recv(m_sockDesc,(void*)buffer,bufferlen,0)) < 0){
    std::cerr<<"could not receive buffer data to foreign host"<<std::endl;      
    }
  return returnVal;
}

std::string
rpi::connect::SensorSocket::getLocalAddress(){
  sockaddr_in Addr;
  
  unsigned int addr_len = sizeof(Addr);

  if(getsockname(m_sockDesc,(sockaddr*)&Addr,(socklen_t*)&addr_len)<0){
    std::cerr<<"Can not get local address"<<std::endl;
  }
  
  return inet_ntoa(Addr.sin_addr);
}

unsigned short
rpi::connect::SensorSocket::getLocalPort(){
  sockaddr_in Addr;
  
  unsigned int addr_len = sizeof(Addr);

  if(getsockname(m_sockDesc,(sockaddr*)&Addr,(socklen_t*)&addr_len)<0){
    std::cerr<<"Can not get local address"<<std::endl;
  }
  
  return ntohs(Addr.sin_port);
}


void
rpi::connect::SensorSocket::setLocalPort(unsigned short port){
  sockaddr_in localaddr;
  
  memset(&localaddr,0,sizeof(localaddr));
  
  localaddr.sin_family = AF_INET;
  localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localaddr.sin_port = htons(port);

  if(bind(m_sockDesc,(sockaddr*)&localaddr,sizeof(sockaddr_in))<0){
    std::cerr<<"can not bind socket to port"<<std::endl;
  }
}

void
rpi::connect::SensorSocket::setLocalAddressAndPort(const std::string& localAddr,unsigned short localPort){
  sockaddr_in localaddr;
  fillAddr(localAddr,localPort,localaddr);
  
  if(bind(m_sockDesc,(sockaddr*)&localaddr,sizeof(sockaddr_in))<0){
    std::cerr<<"can not bind socket to port"<<std::endl;
  }
}

std::string
rpi::connect::SensorSocket::getForeignAddress(){
  sockaddr_in Addr;
  
  unsigned int addr_len = sizeof(Addr);
  
  if(getpeername(m_sockDesc,(sockaddr*)&Addr,(socklen_t*)&addr_len)<0){
      std::cerr<<"Can not get foreign address"<<std::endl;
  }
  return inet_ntoa(Addr.sin_addr);
}

unsigned short
rpi::connect::SensorSocket::getForeignPort(){
  sockaddr_in Addr;
  
  unsigned int addr_len = sizeof(Addr);
  
  if(getpeername(m_sockDesc,(sockaddr*)&Addr,(socklen_t*)&addr_len)<0){
      std::cerr<<"Can not get foreign address"<<std::endl;
  }    
  return ntohs(Addr.sin_port);
}






