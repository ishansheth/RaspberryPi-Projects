/*
 * gpio_class.h
 *
 *  Created on: Oct 28, 2016
 *      Author: ishan
 */

#ifndef GPIO_CLASS_H_
#define GPIO_CLASS_H_

#include<string>
#include<iostream>
#include<fstream>
#include<curl/curl.h>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<time.h>
#include<cstdlib>
#include<signal.h>
#include<unistd.h>
#include<errno.h>

using namespace std;

#define FROM "<ishansheth@gmail.com>"
static int FILE_COUNT = 0;
class GPIOClass{
private:
	string gpionum;

public:
	GPIOClass(){};

	GPIOClass(string x){gpionum = x;}

	int export_gpio(){
		string export_str = "/sys/class/gpio/export";
		ofstream exportgpio(export_str.c_str());
		if(exportgpio < 0){
			cout<<"\n Operation failed to export gpio"<<this->gpionum<<"."<<endl;
			return -1;
		}

		exportgpio<<this->gpionum;
		exportgpio.close();
		return 0;
	}

	int unexport_gpio(){
		string unexport_str = "/sys/class/gpio/unexport";
		ofstream unexportgpio(unexport_str.c_str());
		if(unexportgpio < 0){
			cout<<"\n Operation failed to export gpio"<<this->gpionum<<"."<<endl;
			return -1;
		}

		unexportgpio<<this->gpionum;
		unexportgpio.close();
		return 0;
	}

	int setdir_gpio(string dir){
		string setdir_str = "/sys/class/gpio/gpio"+this->gpionum+"/direction";
		ofstream setdir_gpio(setdir_str.c_str());
		if(setdir_gpio < 0){
			cout<<"\n Operation failed to set direction for gpio"<<this->gpionum<<"."<<endl;
			return -1;

		}

		setdir_gpio<<dir;
		setdir_gpio.close();

		return 0;
	}

	int setval_gpio(string val){
		string setval_str = "/sys/class/gpio/gpio"+this->gpionum+"/value";
		ofstream setval_gpio(setval_str.c_str());
		if(setval_gpio < 0){
			cout<<"\n Operation failed to set value"<<val<<"direction for gpio"<<this->gpionum<<"."<<endl;
			return -1;
		}

		setval_gpio<<val;
		setval_gpio.close();
		return 0;
	}

	int getval_gpio(string &val){
		string getval_str = "/sys/class/gpio/gpio"+this->gpionum+"/value";
		ifstream getval_gpio(getval_str.c_str());
		if(getval_gpio < 0){
			cout<<"\n Operation failed to get value"<<val<<"direction for gpio"<<this->gpionum<<"."<<endl;
			return -1;
		}

		getval_gpio>>val;
		if(val != "0")
			val = "1";
		else
			val = "0";

		getval_gpio.close();
		return 0;

	}



};

struct fileBuf_upload_status
{
  int lines_read;
};

CURLcode send_mail(char*,string);
size_t fileBuf_source(void *ptr, size_t size, size_t nmemb, void *userp);
size_t read_file();
void encode(FILE *infile, unsigned char *output_buf, int rowcount);
void encodeblock(unsigned char in[3], unsigned char out[4], int len);
void* capture_image(void*);
void* grab_video(void*);









#endif /* GPIO_CLASS_H_ */
