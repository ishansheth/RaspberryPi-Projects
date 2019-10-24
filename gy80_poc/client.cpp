#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

int main(int argc,char* argv[]){

  if(argc != 3){
    printf("please provide address and port\n");
    exit(1);
  }

  int sock;

  char send_data[1024];

  /**
     structure definition for reference

  struct  hostent {
        char    *h_name;        
        char    **h_aliases;    
        int     h_addrtype;     
        int     h_length;       
        char    **h_addr_list;  
        #define h_addr  h_addr_list[0]  
	};

  **/
		      
  struct hostent* host;

  /**
     structure definition for reference

  struct sockaddr_in {
     short   sin_family;
     u_short sin_port;
     struct  in_addr sin_addr;
     char    sin_zero[8];
  };

  **/
  
  struct sockaddr_in server_addr;

  host = gethostbyname(argv[1]);

  unsigned short port;
  
  sscanf(argv[2],"%d",&port);
  
  if((sock = socket(AF_INET,SOCK_STREAM,0)) == -1){
    perror("Socket");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  server_addr.sin_addr = *((struct in_addr*) host->h_addr);

  bzero(&(server_addr.sin_zero),8);

  if(connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1){
    perror("connect");
    exit(1);
  }

  const char* cmd = "GYRODATASEND";
  float sensorData[3];

  std::cout<<"sending command to server"<<std::endl;
  send(sock,cmd,strlen(cmd),0);

  
  while(1){

    std::cout<<"reading-----"<<std::endl;
    int readbytes = read(sock,sensorData,3*sizeof(float));

    if(readbytes > 0){
      std::cout<<sensorData[0]<<std::endl;
      std::cout<<sensorData[1]<<std::endl;
      std::cout<<sensorData[2]<<std::endl;
    }else if(readbytes == 0){
      std::cout<<"server closed the connection"<<std::endl;
      break;
    }      
 
  }

  close(sock);
  
  return 0;
}
