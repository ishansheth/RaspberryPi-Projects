#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "L3G4200D.h"
#include "ADXL345.h"
#include <string.h>
#include <unistd.h>
#include <string>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>



#define MAXEVENTS 64
#define PORT 8080

const int CHUNKSIZE = 30;
const int NUMGYROAXIS = 3;

static int socket_fd, epoll_fd;

ADXL345 acclCtrl(true);

static void socket_create_bind_local(){

  struct sockaddr_in server_addr;
  int opt = 1;

  if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
    perror("Socket");
    exit(1);
  }

  if(setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)) == 1){
    perror("Setsockopt");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_addr.sin_zero),8);


  if(bind(socket_fd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr)) == -1){
    perror("bind");
    exit(1);
  }
}


static int make_socket_non_blocking(int sfd){
  int flags;

  flags = fcntl(sfd,F_GETFL,0);

  if(flags == -1){
    perror("fcntl");
    return -1;
  }

  flags |= O_NONBLOCK;

  if(fcntl(sfd,F_SETFL,flags) == -1){
    perror("fcntl");
    return -1;    
  }

  return 0;
}

void accept_and_add_new(){

  struct epoll_event event;
  struct sockaddr in_addr;

  socklen_t in_len = sizeof(in_addr);

  int intfd;

  char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

  while((intfd = accept(socket_fd,&in_addr,&in_len)) != -1){
    if(getnameinfo(&in_addr,in_len,hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV) == 0){
      printf("Accepted connection on descriptor %d (host %s) (port %s)\n",intfd,hbuf,sbuf);
    }

    if(make_socket_non_blocking(intfd) == -1){
      abort();
    }

    event.data.fd = intfd;
    event.events = EPOLLIN | EPOLLET;

    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,intfd,&event) == -1){
      perror("epoll_ctl");
      abort();
    }

    in_len = sizeof(in_addr);
  }

  if(errno != EAGAIN && errno != EWOULDBLOCK)
    perror("accept");
}

void modify_event(int epollfd,int fd,int state){

  struct epoll_event ev;
  ev.events = state;
  ev.data.fd = fd;

  epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}

void writeSensorData(int epollfd,int fd){

  std::cout<<"in  writeSensorData"<<std::endl;
  float* gyroData = new float[CHUNKSIZE];

  int nwrite;
    
    memset(gyroData,0,CHUNKSIZE*sizeof(float));

    for(int i = 0;i<CHUNKSIZE;i=i+3){
      acclCtrl.update();
      gyroData[i] = acclCtrl.getAccX();
      gyroData[i+1] = acclCtrl.getAccY();
      gyroData[i+2] = acclCtrl.getAccZ();
    }
    nwrite = write(fd,gyroData,CHUNKSIZE*sizeof(float));

    modify_event(epollfd,fd,EPOLLIN);
}

void process_new_data(int epollfd,int fd){

  const char* sensorstart = "GYRODATASEND";

  ssize_t count;
  char buf[512] = {0};

  std::cout<<"waiting for the client......"<<std::endl;

  int valrec = read(fd,buf,sizeof(buf)-1);

  if(valrec > 0){
    std::cout<<"reading successful"<<std::endl;

    std::string receivedData = buf;
    std::cout<<"Received data:---->"<<receivedData<<std::endl;
    
    if(!receivedData.compare(sensorstart)){
      std::cout<<"starting to write gyrodata....."<<std::endl;
      writeSensorData(epollfd,fd);
    }else{
      std::cout<<"Client string is not 'GYRODATASEND' "<<std::endl;
    }

  }else if(valrec <=0){
    std::cout<<"could not read successfully from client"<<std::endl;
    close(fd);
  }
  
}

int main(){

  /**
     for reference, 

typedef union epoll_data {
    void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;      // Epoll events 
    epoll_data_t data;        // User data variable 

};
  **/
  
  struct epoll_event event,*events;

  // first bind the server socket with the local address
  socket_create_bind_local();

  if(make_socket_non_blocking(socket_fd) == -1){
    exit(1);
  }

  /**
     listen system call allows the process to listen on the particular socket for connections. First argument is the socket fd, second 
     argument is the isze of backlog queue i.e number of connections that can be waiting while the process is handling the particular
     connetion
   **/

  // start listening on the socket

  if(listen(socket_fd,5) == -1){
    perror("Listen\n");
    exit(1);
  }

  printf("\n TCP server waiting for the connections.....\n");
  fflush(stdout);

  // epoll_create and epoll_create1 is to create and return a file descriptor referring to the created epoll instance
  epoll_fd = epoll_create1(0);
  if(epoll_fd == -1){
    perror("epoll_create1");
    exit(1);
  }

  event.data.fd = socket_fd;
  event.events = EPOLLIN | EPOLLET;

  /**
     for reference, syntax of the epoll_ctl. It is used to control epoll operations

     int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

     first argument is the fd of epoll instance. second argument is to specify epoll operations 
     EPOLL_CTL_ADD,EPOLL_CTL_MOD,EPOLL_CTL_DEL 
     
     third argument is fd to be worked on

   **/

  if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,socket_fd,&event) == -1){
    perror("epoll_ctl");
    exit(1);
  }

  events = (epoll_event*)calloc(MAXEVENTS,sizeof(event));

  while(1){
    int n,i;

    /**
       for reference

       int epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout);

       int epoll_pwait(int epfd, struct epoll_event *events,
               int maxevents, int timeout,
               const sigset_t *sigmask);
     **/
    
    n = epoll_wait(epoll_fd,events,MAXEVENTS,-1);

    // for each ready sockets, handle the io operation
    for(i = 0;i<n;i++){
      if(events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN)){
	
	perror("epoll error");
	close(events[i].data.fd);
	
      }else if(events[i].data.fd == socket_fd){
	accept_and_add_new();
      }else if(events[i].events & EPOLLOUT){
	std::cout<<"EPOLLOUT occurred"<<std::endl;
	writeSensorData(epoll_fd,events[i].data.fd);
      }else{
	std::cout<<"EPOLLIN occurred"<<std::endl;

	process_new_data(epoll_fd,events[i].data.fd);
      }
    }
  }

  free(events);
  close(socket_fd);
  return 0;
}
