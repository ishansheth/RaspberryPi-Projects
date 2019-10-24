#include "GyroDataSource.hxx"

using namespace rpi::gyroconnect;

void
GyroSensorDataSource::restartGyroModule(){
  // TODO: start the whole gyro stack here when server breaks the connection
  int connres= -1;
  int handshakeRes = -1;
  while(connres < 0){
    connres =  connectToServer();
      std::cout<<"Trying to connect................"<<std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(2));
  }    
  handshakeRes = startGyroProtocol();
  if(handshakeRes > 0)
    std::cout<<"Connection established again!!"<<std::endl;
  else
    std::cout<<"Connection cant be established"<<std::endl;
}


int
GyroSensorDataSource::readBytesQueue(){
  int readbytes = 0;
  int receivedBytesChunk = 0;
    
  if(!m_connectionStatus){
    std::cout<<"Connection is not yet established"<<std::endl;
    return -1;
  }
  
  const char* sendCmd = "SEND";
  m_sensorConnSocket.send(sendCmd,strlen(sendCmd));      
  
  memset(m_sensorData,0,CHUNKSIZE*sizeof(float));
  
  while(readbytes != CHUNKSIZE*sizeof(float)){
    /**
       The 'read' is told to read 'CHUNKSIZE*sizeof(float)' bytes from the socket, but it is possible that it will read
       less bytes than the reuqested and return the number of bytes read. So there is a need of loop which will keep track
       of number of total bytes requested and total bytes read from the socket. And when both of them are equal, then 
       leave the loop with full buffer. If this loop is not used, then it will give garbage value in the provided buffer
      **/
    receivedBytesChunk = m_sensorConnSocket.recv(m_sensorData+readbytes,CHUNKSIZE*sizeof(float)-readbytes);
    if(receivedBytesChunk > 0)
      readbytes += receivedBytesChunk;
    else if(receivedBytesChunk == 0){
      std::cout<<"Server closed the connection"<<std::endl;
      return -1;
    }      
  }
  
  for(int i = 0;i<CHUNKSIZE;i = i+3){
    // creating in place structure
    dataBufferQueue.push({m_sensorData[i],m_sensorData[i+1],m_sensorData[i+2]});      
  }
  return 1;
}

void
GyroSensorDataSource::clientWorkerThread(){
  while(1){
    std::cout<<"queue size:"<<dataBufferQueue.size()<<std::endl;
    
    std::unique_lock<std::mutex> lk(m_bufferSyncMutex);
    m_bufferSyncVar.wait(lk,[this](){return dataBufferQueue.size() == 0;});
    if(readBytesQueue()<0){
      std::cout<<"Bytes can not be read from server,terminating thread"<<std::endl;
      break;
    }
    lk.unlock();
    m_bufferSyncVar.notify_all();
  }
}

GyroSensorDataSource::GyroSensorDataSource():m_sensorData(new float[CHUNKSIZE]),
					     m_connectionStatus(false),
					     m_sensorConnSocket(SOCK_STREAM,0),
					     m_dataLoaded(false)
{}

GyroSensorDataSource::~GyroSensorDataSource(){
  // always check if thread is joinable, then join, otherwise it will crash
  if(m_clientThread.joinable())
    m_clientThread.join();
  delete[] m_sensorData;
}

void
GyroSensorDataSource::startClientThread(){
  m_clientThread = std::thread(&GyroSensorDataSource::clientWorkerThread,this);
}

GyroData
GyroSensorDataSource::getGyroData(){
  std::unique_lock<std::mutex> lk(m_bufferSyncMutex);
  
  m_bufferSyncVar.wait(lk,[this]{return dataBufferQueue.size() > 0;});
  
  GyroData x;
  
  x = dataBufferQueue.front();
  dataBufferQueue.pop();
  
  lk.unlock();
  m_bufferSyncVar.notify_all();
  
  return x;
}

int
GyroSensorDataSource::connectToServer(){
  m_sensorConnSocket.connect(serverIP,PORT);
  std::cout<<"Local Address:----->"<<m_sensorConnSocket.getLocalAddress()<<std::endl;
  
  std::cout<<"connect to server successful"<<std::endl;
  m_connectionStatus = true;
  return 1;
}

int
GyroSensorDataSource::startGyroProtocol(){
  std::string servResponse;
  char buffer[1024] = {0};
  if(!m_connectionStatus){
    std::cout<<"Connection is not yet established"<<std::endl;
    return -1;
  }
  
  int valrec = m_sensorConnSocket.recv(buffer,1024);
  if(valrec > 0){
    servResponse = buffer;
    std::cout<<"First Server Response:"<<servResponse<<std::endl;
    if(!servResponse.compare("GYRODATA")){
      const char* ok = "OK";
      m_sensorConnSocket.send(ok,strlen(ok));      
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
