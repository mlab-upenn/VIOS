/*********************************   Header Files  *******************************************/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>  /* For malloc dynamic memory allocation */

using namespace cv;
using namespace std;


int main(int argc,char **argv){
  string input_file,type,data_file,dev;
  vector <Rect> out;
  Mat frame;
  for (int i=1;i<argc;i++){  
    if (strcmp(argv[i],"-i")==0)
      input_file = string(argv[++i]);
    else if (strcmp(argv[i],"-t")==0)
      type = string(argv[++i]);
    else if (strcmp(argv[i],"-d")==0)
      data_file = string(argv[++i]);   
    else if (strcmp(argv[i],"-dev")==0)
      dev = string(argv[++i]);
    else if (strcmp(argv[i],"-h")==0){
      cout << "**********************************************************************************************************************" << endl;
      cout << "Usage: sudo ./movie_exp -i <input-video-file> -d <input-data-file> -t <detection-type> -dev <serial-device-port>" << endl << endl;
      cout << "-i   : input video file name (eg Manchester vs .....ts)" << endl;
      cout << "-d   : the corresponding data file (eg manu_vs_chelsea.txt)" << endl;
      cout << "-t   : type of video sports/movie" << endl;
      cout << "-dev : serial device port in ubuntu (eg /dev/usb/....)" << endl;
      cout << "without -dev option will output the commands to std::output ie terminal" << endl;
      cout << "***********************************************************************************************************************" << endl;
      return 0;
    } 
    else
      cerr << "Wrong Usage: "<< string(argv[i]) << endl;
  }
  ifstream fin;
  fin.open(data_file.c_str(),ofstream::in);
  string team1_name, team2_name;
  int ts[50],tse[50],rtime,tot;
  int ii = 0;
  string movie_name;
  vector <string> ambience,vibrator,solenoid;
  ambience.resize(50);
  vibrator.resize(50);
  solenoid.resize(50);
  if (!fin)
    throw std::exception();
  
  string line;
  getline(fin,line);
  istringstream liness;
  liness.clear();
  liness.str(line);
  liness >> movie_name;
  while (getline(fin, line)) {
    liness.clear();
    liness.str(line);
    liness >> ts[ii] >> tse[ii] >> ambience[ii] >> vibrator[ii] >> solenoid[ii];
    ii++;
  }
  
  VideoCapture cap(input_file);
  int numFrames = (int) cap.get(CV_CAP_PROP_FRAME_COUNT);
  int fps = (int) cap.get(CV_CAP_PROP_FPS);
  ts[ii] = numFrames*1000 / fps;
  tot = ii;
  
  namedWindow("Movie Detection",CV_WINDOW_NORMAL);
  int frame_count = 0, fd;
  ii = 0;
  while (1){
    cap >> frame;
    if (frame.empty())
      break;
    rtime = (int)cap.get(CV_CAP_PROP_POS_MSEC);
    if (rtime >= ts[ii+1] && ii<tot)
      ii++;
    cout << type.c_str() << " " << ambience[ii].c_str() << " " << vibrator[ii].c_str() << " " << solenoid[ii].c_str() << endl;
    
    imshow("Movie",frame);
    if(waitKey(1000/fps) == ' ')
      break;
  }
  return 0;
}
