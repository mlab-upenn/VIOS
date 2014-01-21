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
/**********************************************************************************************/
#define BAUD_RATE B57600
/**********************************************************************************************/

using namespace cv;
using namespace std;

int debug = 0;

/******************************   Serial Port Functions ****************************************/
int open_port(string dev){//'open_port()' - Open serial port 1.
	int fd;//File descriptor for the port 
    fd = open(dev.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1){//Could not open the port.
		perror("open_port: Unable to open serialport - ");
    }
    else fcntl(fd, F_SETFL, 0);
    debug && printf ("Port file Descriptor = %d\n",fd);
    return (fd);
}

int write_port(int fd,char * a){//fd is port descriptor & a contains the msg
	int n;
    n = write(fd, a,sizeof(a)/sizeof(char));
    if (n < 0){
    	debug && fputs("write() failed!\n", stderr); 
    	return 0;
    }
    //printf ("Data Written\n");
    return 1;
}

char * read_port(int fd){//fd is port descriptor & a is the buffer which receives the msg
	char *a;
	a=(char *)malloc(sizeof(char)*100);
	fcntl(fd, F_SETFL, 0);//finite wait time for read;
	read(fd,a,100);
	debug && printf ("Read Data = %s\n",a);
	return a;
}

void set_bd_rate(int fd){
	struct termios options;
    tcgetattr(fd, &options); //Get the current options for the port...
    cfsetispeed(&options, BAUD_RATE);//Set the baud rates to 19200...
    cfsetospeed(&options, BAUD_RATE);//Set the baud rates to 19200...
	options.c_cflag |= (CLOCAL | CREAD);//Enable the receiver and set local mode...
    tcsetattr(fd, TCSANOW, &options);//Set the new options for the port...
    debug && printf ("Baud Rate set...\n");
    return;
}
/******************************************************************************************/

int main(int argc,char **argv){
  string input_file,type,data_file,dev;
  vector <Rect> out;
  Mat frame, frame_rsz(480,640,CV_8UC3);
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
      cout << "Usage: sudo ./immersive_exp -i <input-video-file> -d <input-data-file> -t <detection-type> -dev <serial-device-port>" << endl << endl;
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
  int team1_goal[20], team2_goal[20],ts[20],rtime,tot;
  int ii = 0;
  string goal_team_name[20];
  if (!fin)
    throw std::exception();
  
  string line;
  getline(fin,line);
  istringstream liness;
  liness.clear();
  liness.str(line);
  liness >> team1_name >> team2_name;
  while (getline(fin, line)) {
    liness.clear();
    liness.str(line);
    liness >> ts[ii] >> team1_goal[ii] >> team2_goal[ii] >> goal_team_name[ii];
    ii++;
  }
  
  VideoCapture cap(input_file);
  int numFrames = (int) cap.get(CV_CAP_PROP_FRAME_COUNT);
  int fps = (int) cap.get(CV_CAP_PROP_FPS);
  ts[ii] = numFrames*1000 / fps;
  tot = ii;
  
  namedWindow("Sports Detection",CV_WINDOW_NORMAL);
  int frame_count = 0, fd;
  if (!dev.empty()){
    fd=open_port(dev);
	  if(fd<0)
		  cerr<<"Could not open serial port" << endl;
	  set_bd_rate(fd);
	}
	char sstr[50];
	strcpy(sstr,type.c_str());
	/*if (!dev.empty())
	  write_port(fd,sstr);
	else
	  cout << sstr << endl;*/
  ii =0;

  while (1){
    cap >> frame;
    if (frame.empty())
      break;
    resize(frame,frame_rsz,frame_rsz.size());
    rtime = (int)cap.get(CV_CAP_PROP_POS_MSEC);
    if ( ii<tot && rtime >= ts[ii+1])
      ii++;
    sprintf(sstr,"%s %s %s %d %d %s",type.c_str(),team1_name.c_str(),team2_name.c_str(),team1_goal[ii],team2_goal[ii],goal_team_name[ii].c_str());
    if (!dev.empty())
      write_port(fd,sstr);
    else
      cout << sstr << endl;
    imshow("Sports Detection",frame_rsz);
    if(waitKey(25) == ' ')
      break;
  }
  return 0;
}
