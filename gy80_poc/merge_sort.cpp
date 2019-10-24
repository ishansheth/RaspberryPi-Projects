#include<iostream>
#include<string>
#include<array>
#include<iomanip>

#include<boost/asio/io_service.hpp>
#include<boost/asio/steady_timer.hpp>
#include<chrono>


void mergeSort(int* numbers,int size){  
}




struct mystruct{

  int x;
  int y;
  char c;
  bool b;
};

int main(){
  int numbers1[] = {1,23,43,2323,21,6765,455,456,935,587};
  int numbers2[] = {2,234,42,236,217,765,55,56,935,87};

  int arraysize = sizeof(numbers1)/sizeof(numbers1[0]);
  mergeSort(numbers1,sizeof(numbers1)/sizeof(numbers1[0]));

  double d  = {7.9};

  mystruct s;

  s.x = 10;
  s.y = 100;
  s.c = 'a';
  s.b = true;

  int* p = reinterpret_cast<int*>(&s);

  std::cout<<*p<<std::endl;
  p++;
  std::cout<<*p<<std::endl;

  p++;
  char* ch = reinterpret_cast<char*>(p);

  std::cout<<*ch<<std::endl;

  ch++;
  bool* n = reinterpret_cast<bool*>(ch);

  std::cout<<*n<<std::endl;

  using namespace boost::asio;

  io_service io;

  steady_timer timer(io,std::chrono::seconds(3));
  timer.async_wait([](const boost::system::error_code& ec){std::cout<<"3 sec\n"<<std::endl;});

  io.run();
}
