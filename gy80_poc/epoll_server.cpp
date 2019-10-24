#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>


#define IPADDRESS "127.0.0.1"
#define PORT 8787
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

static int socket_bind(const char* ip,int port);

static void do_epoll(int listenfd);

static void handle_events(int epollfd,struct epoll_event *events,int num, int listenfd,char* buf);

static void handle_accept(int epollfd,int listenfd);

static void do_read(int epollfd,int fd,char* buf);

static void do_write(int epollfd,int fd,char* buf);

static void add_event(int epollfd,int fd,int state);

static void modify_event(int epollfd,int fd,int state);

static void delete_event(int epollfd,int fd,int state);

static int make_socket_non_blocking(int sfd)
{
  int flags;
  
  flags = fcntl(sfd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl");
    return -1;
  }
  
  flags |= O_NONBLOCK;
  if (fcntl(sfd, F_SETFL, flags) == -1) {
    perror("fcntl");
    return -1;
  }
  
  return 0;
}

int main(int argc,char* argv[]){
  int listenfd;

  // first create the main_socket and bind it to the localhost 
  listenfd = socket_bind(IPADDRESS,PORT);

  make_socket_non_blocking(listenfd);
  
  // start listening on the main_socket 
  listen(listenfd,LISTENQ);

  // create epoll interface handler/fd for the main_socket
  do_epoll(listenfd);
  return 0;  
}

static int socket_bind(const char* ip, int port){

  int listenfd;
  struct sockaddr_in servaddr;

  // creating the main_socket
  listenfd = socket(AF_INET,SOCK_STREAM,0);
  if(listenfd == -1){
    perror("socket error");
    exit(1);
  }
  bzero(&servaddr,sizeof(servaddr));
  
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  
  inet_pton(AF_INET,ip,&servaddr.sin_addr);

  // bind the socket to the localhost
  if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1){
    perror("bind error");
    exit(1);
  }

  return listenfd;
}

static void do_epoll(int listenfd){
  int epollfd;

  struct epoll_event events[EPOLLEVENTS];
  int ret;

  char buf[MAXSIZE];
  memset(buf,0,MAXSIZE);

  // create fd of the epoll interface
  epollfd = epoll_create(FDSIZE);

  // add EPOLLIN event to the epoll_event
  add_event(epollfd,listenfd,EPOLLIN);

  // continuous loop to monitor the file descriptors
  for(;;){
    printf("server do epoll>>>>\n");
    // wait for the event to happen
    ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);

    // handle event for the 'ret' number of sockets
    handle_events(epollfd,events,ret,listenfd,buf);
  }

  close(epollfd);
}

static void handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char* buf){
  int i;
  int fd;

  for(i = 0;i<num;i++){
    fd = events[i].data.fd;

    if((fd == listenfd) && (events[i].events & EPOLLIN)){
      // if fd == listenfd, accept new connection
      printf("accepting new connection>>>\n");
      handle_accept(epollfd,listenfd);
    }else if(events[i].events & EPOLLIN){
      do_read(epollfd,fd,buf);
    }else if(events[i].events & EPOLLOUT){
      // if event is EPOLLIN, then read from socket
      printf("writing the other side>>>\n");
      do_write(epollfd,fd,buf);
    }
  }
}

static void handle_accept(int epollfd,int listenfd){
  int clifd;

  struct sockaddr_in cliaddr;
  socklen_t cliaddrlen = sizeof(cliaddr);

  // get the socket of the new incomming client connection
  clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);

  make_socket_non_blocking(clifd);
  
  if(clifd == -1)
    perror("accept error");
  else{
       printf("accept new client: %s : %d",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
    // add that to the event
    add_event(epollfd,clifd,EPOLLIN | EPOLLET);
  }
}

static void do_read(int epollfd,int fd, char* buf){
  int nread;

  nread = read(fd,buf,MAXSIZE-1);
  buf[MAXSIZE] = '\0';

  if(nread == -1){
    perror("read error");
    close(fd);
    delete_event(epollfd,fd,EPOLLIN);
  }else if(nread == 0){
    fprintf(stderr,"client close\n");
    close(fd);
    delete_event(epollfd,fd,EPOLLIN);    
  }else{
    printf("read message : %s",buf);
    modify_event(epollfd,fd,EPOLLOUT);
  }
  
}

static void do_write(int epollfd,int fd, char* buf){
  int nwrite;

  nwrite = write(fd,buf,strlen(buf));

  printf("do_write message is: %s",buf);

  if(nwrite == -1){
    perror("write error");
    close(fd);
    delete_event(epollfd,fd,EPOLLOUT);
  }else{
    modify_event(epollfd,fd,EPOLLIN);    
  }

  memset(buf,0,MAXSIZE);
}

static void add_event(int epollfd,int fd, int state){
  struct epoll_event ev;

  ev.events = state;
  ev.data.fd = fd;

  epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

static void delete_event(int epollfd,int fd,int state){
  struct epoll_event ev;

  ev.events = state;
  ev.data.fd = fd;

  epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev); 
}


static void modify_event(int epollfd,int fd,int state){
  struct epoll_event ev;

  ev.events = state;
  ev.data.fd = fd;

  epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev); 
}
