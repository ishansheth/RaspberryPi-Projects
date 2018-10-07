#include <iostream>
#include <vector>
#include<string>
#include<chrono>
#include <thread>
#include<cmath>
#include "GY80SensorDataSource.hxx"

// TODO: create a class to get the sensor data from source, connecting to server socket and receiving data thread;
// The user can start the module to start receiving the data from server and then get data from it
// 1. may be using callback 2. or simply using the member function of the class to fetch continuous data 


int main(){
  SensorDataSource m_dataSource;

  //  m_dataSource.startClientThread();
  
  if(m_dataSource.connectToServer()>0){
      m_dataSource.startGyroProtocol();
      m_dataSource.startClientThread();
      while(1)
	std::cout<<"data from queue:"<<m_dataSource.getSingleAxisData()<<":"<<m_dataSource.getSingleAxisData()<<":"<<m_dataSource.getSingleAxisData()<<std::endl;
      std::cout<<"reading bytes finished"<<std::endl;
  }else{
        std::cout<<"Server could not be connected"<<std::endl;
  }
    
  return 0;
}
