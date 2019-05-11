#include <iostream>
#include <functional>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>

int main(){

  /**
     Run command on Rpi board:
     ./capture -o -c0|ffmpeg -re -i - -vcodec mpeg4 -f rtp rtp://169.254.133.251:1234/


     ***** Useful information from internet blog******

     RTP streaming with ffmpeg
     Since I often receive private emails asking details about RTP streaming with ffmpeg, I decided to write down some notes about it.
     So, first of all, yes, ffmpeg can stream audio and video over RTP. And, as far as I know, there are no major issues with this feature... You just need to know how to do it.

     Let's see... The simplest command line you can use to generate an RTP session composed by an audio stream and a video stream is:


     ffmpeg -re -i input.mpg -vcodec copy -an -f rtp rtp://224.10.20.30:20000 -vn -acodec copy -f rtp rtp://224.10.20.30:30000 -newaudio

     Analysing this command line:

     "-re" is needed to stream the audio and the video at the correct rate (instead of streaming as fast as possible)
     "input.mpg" is the input file, to be streamed
     this example just streams the audio and the video tracks without re-encoding them... Hence, "-vcodec copy" and "-acodec copy"
     we need one output for the video, and one for the audio. Hence, the first output has "-an" (no audio), the second output has "-vn" (no video), and there is a "-newaudio" at the end (add the audio track to the second output)
     the output format has to be RTP. Hence, "-f rtp"
     the output protocol, has to be RPT, hence, the output file names are "rtp://:"

     If you want to re-encode the audio or the video, you can change "-vcodec copy" and "-acodec copy" with whatever you prefer (for example, "-vcodec mpeg4", or similar).

     After you start the ffmpeg program, it will print something like


     [...]
     Stream #0.1 -> #1.0
     SDP:
     v=0
     o=- 0 0 IN IP4 127.0.0.1
     s=No Name
     t=0 0
     a=tool:libavformat 52.61.0
     m=video 20000 RTP/AVP 32
     c=IN IP4 224.10.20.30
     b=AS:104857
     m=audio 30000 RTP/AVP 14
     c=IN IP4 224.10.20.30
     b=AS:64
     [...]



     You need to copy the SDP description (starting with "v=0") in a .sdp file, that you will use to play the stream with vlc, ffplay, mplayer, or your favorite video player.

     That's it!!!
     
     ***************************************************************************************************************

     opencv uses ffmpeg as a backend. In ffmpeg, one can enable or disable certain protocols to be used for the video capturing
     For this purpose, first file,rtp and udp protocols must be enabled or put it in a "whitelist"
     This can be done via environment variable
     More info --> https://ffmpeg.org/ffmpeg-protocols.html
   **/
  setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS","protocol_whitelist;file,rtp,udp",1);
  
  cv::VideoCapture* stream = new cv::VideoCapture("rpi_streaming.sdp");

  if(!stream->isOpened()){
    std::cout<<"cound not open video stream"<<std::endl;
    return -1;
  }

  cv::namedWindow("rtp_stream",CV_WINDOW_AUTOSIZE);
  cv::Mat frame;

  // once esc key is pressed, leave the loop
  while(cv::waitKey(30) != 27){
    if(!stream->read(frame))
      return -1;

    cv::imshow("rtp_stream",frame);
  }

  return 1;
}
