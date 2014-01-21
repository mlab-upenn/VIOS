#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

int main(int argc,char **argv){
  string input_file,output_file,data_file;
  vector <Rect> out;
  Mat frame;
  for (int i=1;i<argc;i++){  
    if (strcmp(argv[i],"-i")==0)
      input_file = string(argv[++i]);
    else if (strcmp(argv[i],"-o")==0)
      output_file = string(argv[++i]);
    else if (strcmp(argv[i],"-d")==0)
      data_file = string(argv[++i]);   
    else
      cerr << "Wrong Usage" << endl;
  }
  ofstream fout;
  ifstream fin;
  fin.open(data_file.c_str(),ofstream::in);
  fout.open(output_file.c_str(),ofstream::out);
  string team1_name, team2_name;
  int team1_goal[50], team2_goal[50],ii =0,ts[50],rtime,tot;
  string goal_team_name[50];
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
  
  //namedWindow("Sports Detection",CV_WINDOW_NORMAL);
  int frame_count = 0;
  fout << "<?xml version=\"1.0\"?>" << endl;
  fout << "<data xmlns:mac=\"http://www.tvworks.com/tva/xml/ns/max/data-types\"" << endl;
  fout << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << endl;
  fout<<" xsi:noNamespaceSchemaLocation=\"sports_detection.xsd\">" << endl;
  fout << "<sports_detection_data>" << endl;
  
  ii =0;
  while (1){
    cap >> frame;
    if (frame.empty())
      break;
    rtime = (int)cap.get(CV_CAP_PROP_POS_MSEC);
    if (ii<tot-1 && rtime > ts[ii+1])
      ii++;
    fout << "<row time=\"" << rtime << "\">" << endl;
    fout << " <team1_name>" << team1_name << "</team1_name>" << endl;
    fout << " <team2_name>" << team2_name << "</team2_name>" << endl;
    fout << " <team1_goal>" << team1_goal[ii] << "</team1_goal>" << endl;
    fout << " <team2_goal>" << team2_goal[ii] << "</team2_goal>" << endl;
    fout << " <celeb_pattern>" << goal_team_name[ii] << "</celeb_pattern>" << endl;
    fout << "</row>" << endl;
    //imshow("Sports Detection",frame);
    //if(waitKey(30) == 27)
    //  break;
  }
  fout << "</sports_detection_data>"<< endl;
  fout << "</data>" << endl;
  return 0;
}
