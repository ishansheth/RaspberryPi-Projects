/*
 * gpio_class.cpp
 *
 *  Created on: Oct 28, 2016
 *      Author: ishan
 */

#include"gpio_class.h"

using namespace cv;

static const int CHARS= 76;     //Sending 54 chararcters at a time with \r , \n and \0 it becomes 57
static const int ADD_SIZE= 7;   // ADD_SIZE for TO,FROM,SUBJECT,CONTENT-TYPE,CONTENT-TRANSFER-ENCODING,CONETNT-DISPOSITION and \r\n
static const int SEND_BUF_SIZE= 54;
static char (*fileBuf)[CHARS] = NULL;
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


void encodeblock(unsigned char in[3], unsigned char out[4], int len)
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

void encode(FILE *infile, unsigned char *output_buf, int rowcount)
{
    unsigned char in[3], out[4];
        int i, len;
        *output_buf = 0;

    while(!feof(infile)) {
        len = 0;
        for(i = 0; i < 3; i++) {
            in[i] = (unsigned char) getc(infile);
            if(!feof(infile) ) {
                len++;
            }
            else {
                in[i] = 0;
            }
        }
        if(len) {
            encodeblock(in, out, len);
            strncat((char*)output_buf, (char*)out, 4);
        }
    }
}



size_t read_file(string TO,string filename)
{
        FILE* hFile=NULL;
        string to_string = "To: "+ TO +"\r\n";
        size_t fileSize(0),len(0),buffer_size(0);
        hFile = fopen(filename.c_str(),"rb");
        if(!hFile) {
                cout << "File not found!!!" << endl;
                exit(1);
                exit(EXIT_FAILURE);
        }
        fseek(hFile,0,SEEK_END);
        fileSize = ftell(hFile);
        fseek(hFile,0,SEEK_SET);
        int no_of_rows = fileSize/SEND_BUF_SIZE + 1;
        int charsize = (no_of_rows*72);
        unsigned char* b64encode = new unsigned char[charsize];
        *b64encode = 0;
        encode(hFile, b64encode, no_of_rows );
        string encoded_buf = (char*)b64encode;
        fileBuf = new char[ADD_SIZE + no_of_rows][CHARS];  //ADD_SIZE for TO,FROM,SUBJECT,CONTENT-TYPE,CONTENT-TRANSFER-
                                                           //ENCODING,CONETNT-DISPOSITION and \r\n
        strcpy(fileBuf[len++],to_string.c_str());
        buffer_size += strlen(fileBuf[len-1]);
        strcpy(fileBuf[len++],"From: " FROM "\r\n");
        buffer_size += strlen(fileBuf[len-1]);
        strcpy(fileBuf[len++],"Subject: SMTP TLS example message\r\n");
        buffer_size += strlen(fileBuf[len-1]);

        string temp = "Content-Type: application/x-msdownload; name="+ filename +"\r\n";
        strcpy(fileBuf[len++],temp.c_str());
        buffer_size += strlen(fileBuf[len-1]);
        strcpy(fileBuf[len++],"Content-Transfer-Encoding: base64\r\n");
        buffer_size += strlen(fileBuf[len-1]);

        temp = "Content-Disposition: attachment; filename="+ filename +"\r\n";
        strcpy(fileBuf[len++],temp.c_str());
        buffer_size += strlen(fileBuf[len-1]);
        strcpy(fileBuf[len++],"\r\n");
        buffer_size += strlen(fileBuf[len-1]);

        int pos = 0;
        string sub_encoded_buf;
        for(int i = 0; i <= no_of_rows-1; i++)
        {
                sub_encoded_buf = encoded_buf.substr(pos*72,72);  //Reads 72 characters at a time
                sub_encoded_buf += "\r\n";                        //and appends \r\n at the end
                strcpy(fileBuf[len++], sub_encoded_buf.c_str());  //copy the 72 characters & \r\n to email
                buffer_size += sub_encoded_buf.size();            //now increase the buffer_size
                pos++;                                            //finally increase pos by 1
        }

        delete[] b64encode;
        return buffer_size;
}

size_t fileBuf_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
        struct fileBuf_upload_status *upload_ctx = (struct fileBuf_upload_status *)userp;
        const char *fdata;

        if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1))
        {
                return 0;
        }

        fdata = fileBuf[upload_ctx->lines_read];

        if(strcmp(fdata,""))
        {
                size_t len = strlen(fdata);
                memcpy(ptr, fdata, len);
                upload_ctx->lines_read++;
                return len;
        }
        return 0;
}


CURLcode send_mail(char* TO,string filename){
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct fileBuf_upload_status file_upload_ctx;
    size_t file_size(0);

    file_upload_ctx.lines_read = 0;

    curl = curl_easy_init();
    file_size = read_file(TO,filename);
    if(curl)
    {
      curl_easy_setopt(curl, CURLOPT_USERNAME, "ishansheth@gmail.com");
      curl_easy_setopt(curl, CURLOPT_PASSWORD, "doingthesis");
      curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com");
      curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
      curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
      recipients = curl_slist_append(recipients, TO);
      curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
      curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size);
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, fileBuf_source);
      curl_easy_setopt(curl, CURLOPT_READDATA, &file_upload_ctx);
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Dont display Curl Connection data Change 1L to 0

      res = curl_easy_perform(curl);

//      if(res != CURLE_OK)
//        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      curl_slist_free_all(recipients);
      curl_easy_cleanup(curl);
    }
    delete[] fileBuf;
    return res;

}

void* grab_video(void* flag){
	cout<<"in thread function:"<<(bool*)flag<<endl;
    VideoCapture capture(0);
    double elapsed;
    time_t start, end;
    double dwidth=capture.get(CV_CAP_PROP_FRAME_WIDTH);
    double dheight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    Size framesize(static_cast<int>(dwidth),static_cast<int>(dheight));
    VideoWriter owriter("MyVideo.avi",CV_FOURCC('P','I','M','1'),20,framesize,true);

    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    if(!capture.isOpened()){
	    cout << "Failed to connect to the camera." << endl;
	    return 0;
    }
//    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE);

    if(!owriter.isOpened()){
    	cout<<"can not open videowriter class"<<endl;
    	return 0;
    }
    start = time(NULL);
    while(1){
    	Mat frame;
    	bool bsuccess = capture.read(frame);
    	if(!bsuccess){
    		cout<<"can not read a frame from camera";
    		return 0;
    	}
    	owriter.write(frame);
//    	imshow("MyVideo",frame);
/*
    	if(waitKey(10) == 27){
    		cout<<"esc key is pressed"<<endl;
    		break;
    	}
*/
    	end = time(NULL);
    	elapsed = difftime(end,start);
    	cout<<"\nin loop:"<<elapsed;
    	if(elapsed >= 20.0)
    		break;
    }

    pthread_exit(NULL);
}


void* capture_image(void* flag){
    VideoCapture capture(0);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);

    if(!capture.isOpened()){
	    cout << "Failed to connect to the camera." << endl;
	    return 0;
    }
/*
    while(1)
        {
            Mat image;
            capture>>image;
            if (!image.data) break;
            if (waitKey(30) >= 0) break;

            imshow("test",image);
            waitKey(1);
        }
*/

    Mat frame;
    struct timespec start, end;
    clock_gettime( CLOCK_REALTIME, &start );
    capture >> frame;

    clock_gettime( CLOCK_REALTIME, &end );
    double difference = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/1000000000.0d;
    cout << "It took " << difference << " seconds to process "<< endl;

    string filename;
    string dir_name="/home/ishan/";
    filename += "capture";
    stringstream ss;
    ss<<FILE_COUNT;
    filename += ss.str();
    filename += ".png";
    cout<<filename<<endl;
    imwrite(filename, frame);
//    string command = "mv "+filename+" "+dir_name;
//    system(command.c_str());
    FILE_COUNT++;
    pthread_exit(NULL);
}




