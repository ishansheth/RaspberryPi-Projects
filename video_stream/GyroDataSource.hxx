#ifndef GYRODATASOURCE_HXX_
#define GYRODATASOURCE_HXX_

#include <iostream>
#include "SensorSocket.hxx"

const char* serverIP = "169.254.133.250";
namespace rpi{

  namespace gyroconnect{
    
    class GyroSensorDataSource{
  
      ::rpi::connect::SensorSocket m_sensorConnSocket;
  
      std::thread m_clientThread;
  
      float* m_sensorData;

      bool m_connectionStatus;

      bool m_dataLoaded;
  
      static constexpr int CHUNKSIZE = 30;
  
      std::queue<GyroData> dataBufferQueue;

      std::mutex m_bufferSyncMutex;
      
      std::condition_variable m_bufferSyncVar;

      void restartGyroModule();

      int readBytesQueue();

      void clientWorkerThread();

    public:
      // ctor
      GyroSensorDataSource();
  
      //dtor, joining thread, deleting the receiver buffer
      ~GyroSensorDataSource();

      void startClientThread();
      
      GyroData getGyroData();
  
      int connectToServer();

      int startGyroProtocol();
    };
    
  }// namespace gyroconnect

}// namespace rpi

#endif
