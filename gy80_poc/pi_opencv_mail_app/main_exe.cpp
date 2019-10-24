/*
 * main_exe.cpp
 *
 *  Created on: Sep 24, 2016
 *      Author: ishan
 */


// the program to capture the image from the camera

#include<iostream>
#include<opencv2/highgui/highgui.hpp>
#include<time.h>
#include<stdlib.h>
#include<cstring>
#include<signal.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<stdio.h>
#include"gpio_class.h"


bool ctrl_c_pressed = false;
using namespace std;
using namespace cv;

void sig_handler(int sig){
	write(0,"ctrl c pressed in sig handler",32);
	ctrl_c_pressed = true;
}



int main(int argc,char *argv[])
{
	string inputstate;
	struct sigaction sig_struct;
	sig_struct.sa_handler = sig_handler;
	sig_struct.sa_flags = 0;
	sigemptyset(&sig_struct.sa_mask);
	if(sigaction(SIGINT,&sig_struct,NULL) == -1){
		cout<<"Problem with sigaction";
		exit(1);
	}

	string camera_switch;
	string video_switch;
	pthread_t thread1,thread2;

	bool video_flag = false;


	GPIOClass *gpio17 = new GPIOClass("17");
	GPIOClass *gpio27 = new GPIOClass("27");

	gpio17->export_gpio();
	gpio27->export_gpio();

	gpio17->setdir_gpio("in");
	gpio27->setdir_gpio("in");

	while(1){
//		usleep(500000);
		gpio17->getval_gpio(camera_switch);
		gpio27->getval_gpio(video_switch);
		if(camera_switch == "0"){
			usleep(5000);
			gpio17->getval_gpio(camera_switch);
			if(camera_switch == "0"){
				cout<<"calling capture_image:"<<video_flag<<endl;
				int rc = pthread_create(&thread1,NULL,&capture_image,(void*)video_flag);
				if(rc){
					cout<<"Error: Unable to create thread for image capture:"<<video_flag<<endl;
					exit(1);
				}
			}
		}

		if(video_switch == "0"){
			usleep(5000);
			gpio27->getval_gpio(video_switch);
			if(video_switch == "0"){
				if(!video_flag){
					video_flag = true;
				}
				cout<<"calling grab_video:"<<video_flag<<endl;
				grab_video(&video_flag);
				int rc = pthread_create(&thread2,NULL,&grab_video,(void*)video_flag);
				if(rc){
					cout<<"Error: Unable to create thread for video capture:"<<video_flag<<endl;
					exit(1);
				}
			}
		}

		if(ctrl_c_pressed){
			cout<<"ctrl c is pressed"<<endl;
			gpio17->unexport_gpio();
			gpio27->unexport_gpio();
			delete gpio17;
			delete gpio27;
			break;
		}


	}


	if(argc < 2){
		cout<<"Please specify the TO address for the mail";
		exit(1);
	}
	

	
//	GPIOClass *gpio17 = new GPIOClass("17");
	GPIOClass *gpio4 = new GPIOClass("4");

	gpio17->export_gpio();
	gpio4->export_gpio();
	gpio17->setdir_gpio("in");
	gpio4->setdir_gpio("out");
	gpio4->setval_gpio("1");

	while(1){
		usleep(500000);

		gpio17->getval_gpio(inputstate);
		if(inputstate == "0"){
			usleep(20000);
			gpio17->getval_gpio(inputstate);
			if(inputstate == "0"){
				cout<<"switch is pressed"<<"  capturing image"<<endl;
				cout<<" capturing image"<<endl;
				string filename = capture_image();
				cout<<"send a mail "<<filename<<endl;
				CURLcode res = send_mail(argv[1],filename);
				if(res == CURLE_OK){
//					gpio4->setval_gpio("0");
					cout<<"main sent"<<endl;

				}else{
//					gpio4->setval_gpio("1");
					cout<<"mail failed, check connection";
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				}
			}
		}
		
	}

    return 0;
}
