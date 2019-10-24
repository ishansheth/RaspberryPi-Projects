#include<iostream>
#include<boost/asio/write.hpp>
#include<boost/asio/buffer.hpp>
#include<boost/asio/ip/tcp.hpp>
#include<boost/asio/io_service.hpp>
#include<array>
#include<string>


using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::resolver resolv{ioservice};
tcp::socket tcp_socket{ioservice};

std::array<char,4096> bytes;

void read_handler(const boost::system::error_code& ec,std::size_t bytes_transferred){
  if(!ec){
    // write the received bytes on the console
    std::cout.write(bytes.data(),bytes_transferred);
    // then read again, continuously read from socket and write on the console 
    tcp_socket.async_read_some(buffer(bytes),read_handler);
  }
}

void connect_handler(const boost::system::error_code& ec){
  if(!ec){
    // string data that will be sent over the socket
    std::string r = "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";
    // write in the socket
    write(tcp_socket,buffer(r));
    // reading from the other side and give the handler that will handle when data from server arrives
    tcp_socket.async_read_some(buffer(bytes),read_handler);
  }
}

void resolv_handler(const boost::system::error_code& ec, tcp::resolver::iterator it){
  if(!ec){
    std::cout<<"connecting......"<<std::endl;
    tcp_socket.async_connect(*it,connect_handler);
  }
}

int main(){
  std::cout<<"starting client"<<std::endl;
  // connecting to the raspberry pi board with the socket and port

  char* my_name = "ishan";
  char* names = (char*) new char[10];
  names = my_name;
  std::cout<<names<<my_name<<std::endl;
  
  tcp::resolver::query q{"169.254.133.250","8080"};
  resolv.async_resolve(q,resolv_handler);
  ioservice.run();
}
