#include "facerec.h"

using namespace facerec;

VideoCapture cap1;

// See the header file for function usage

// Callback function for slider in video
void onTrackbarSlide(int pos, void* userData=NULL){
  cap1.set(CV_CAP_PROP_POS_FRAMES,pos);
}

FaceDetect::FaceDetect(){
    FACE_CLASSIFIER_SCALE = 1.2;    // Reduce image size by this scale for faster detection
    EYE_CLASSIFIER_SCALE = 1.2;     // Enlarge face images to bigger size so that eyes are detected
    MIN_FACE_DETECT_SIZE = 40;      // Minimum bound for face detection
    MAX_FACE_DETECT_SIZE = 300;     // Maximum bound for face detection
    MIN_EYE_DETECT_SIZE = 1;        // Minimum bound for eye detection
    MAX_EYE_DETECT_SIZE = 60;       // Maximum bound for eye detection
    EYE_DETECT_FACE_SIZE = 200;     // Enlarge face images to this size for eye detection

    // Path to classifier xml files
    EYE_CLASSIFIER_NAME = "../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
    FACE_CLASSIFIER_NAME = "../data/haarcascades/haarcascade_frontalface_alt.xml";

    if (!face_classifier.load(face_classifier_name))
      cerr << "Face Detection Classifier cannot be loaded" << endl;
    if (!eye_classifier.load(eye_classifier_name))
      cerr << "Eye Detection Classifier cannot be loaded" << endl;
}

void Face::align_face(Mat &out, Mat &img, struct Eye eye, float offset_x, float offset_y, Size sz, string mode){
  // Find Face Parameters
  if (eye.left_radius <=0 && eye.right_radius <= 0){
    resize(img,out,sz, 0, 0, INTER_LINEAR);  
    return;
  }
  else if (eye.left_radius <= 0 || eye.right_radius <= 0){
    Rect crop;
    Point2f pt1,pt2;
    Mat tmp;
    if (eye.right_radius <= 0){
      pt1.x = max(eye.left_pos.x - (1-offset_x)*img.rows,0);
      pt1.y = max(eye.left_pos.y - offset_y*img.cols,0);
      pt2.x = min(eye.left_pos.x + offset_x*img.rows,img.rows-1);
      pt2.y = min(eye.left_pos.y + (1-offset_y)*img.cols,img.cols-1);
    }
    else{
      pt1.x = max(eye.right_pos.x - offset_x*img.rows,0);
      pt1.y = max(eye.right_pos.y - offset_y*img.cols,0);
      pt2.x = min(eye.right_pos.x + (1-offset_x)*img.rows,img.rows-1);
      pt2.y = min(eye.right_pos.y + (1-offset_y)*img.cols,img.cols-1);
    }
    crop.x = pt1.x;
    crop.y = pt1.y;
    crop.width = pt2.x - pt1.x; 
    crop.height = pt2.y - pt1.y;
    tmp = img(crop);
    resize(tmp,out,sz, 0, 0, INTER_LINEAR);    
  }
  else {
    float rotation_angle, scale, dist_eyes, dist_ref, cosine, sine;
    rotation_angle = -atan2(eye.left_pos.y - eye.right_pos.y, eye.left_pos.x - eye.right_pos.x);
    float diffx = (eye.right_pos.x - eye.left_pos.x), diffy = (eye.right_pos.y - eye.left_pos.y);
    dist_eyes = sqrt(diffx*diffx + diffy*diffy);
    dist_ref = sz.width - 2.0*offset_x;
    scale = dist_eyes/dist_ref;

    // Find Affine Parameters
    Point2f center = eye.right_pos;
    Mat rot_mat(2,3,CV_32FC1);
    Mat transformed_img,tmp;
    rot_mat = getRotationMatrix2D(center, rotation_angle, 1);
    warpAffine(img,transformed_img,rot_mat,img.size(),INTER_CUBIC);

    // Crop Transformed Image
    Rect crop;
    Point2f pt1,pt2;
    pt1.x = max(eye.right_pos.x - offset_x*img.rows,0);
    pt1.y = max(eye.right_pos.y - offset_y*img.cols,0);
    pt2.x = min(eye.left_pos.x + offset_x*img.rows,img.rows-1);
    pt2.y = min(eye.right_pos.y + (1-offset_y)*img.cols,img.cols-1);
    crop.x = pt1.x;
    crop.y = pt1.y;
    crop.width = pt2.x - pt1.x; 
    crop.height = pt2.y - pt1.y;
    tmp = transformed_img(crop);

    // Resize face to appropriate size
    resize(tmp,out,sz, 0, 0, INTER_LINEAR);
  }
}
  
double FaceDetect::detect_face(vector <Rect> &face_pos, vector <struct Eye> &eye, Mat &img, double scale, string mode){
  double tt, dt, aspect_ratio;
  struct Eye eye_pos;
  face_pos.clear();
  eye.clear();
  Mat scaled_img(round(img.rows/scale),round(img.cols/scale),CV_8UC1), face_ROI, gray_img;
  if (img.channels() > 1)
    cvtColor(img, gray_img, CV_RGB2GRAY);
  else
    gray_img = img;
  
  resize(gray_img, scaled_img, scaled_img.size(), 0, 0, INTER_LINEAR );
  equalizeHist(scaled_img, scaled_img);
  
  tt = (double)getTickCount();
  face_classifier.detectMultiScale(scaled_img, face_pos, FACE_CLASSIFIER_SCALE,
                                  3, // Number of neighbours
                                  (0 | CV_HAAR_SCALE_IMAGE), 
                                  Size(MIN_FACE_DETECT_SIZE, MIN_FACE_DETECT_SIZE),
                                  Size(MAX_FACE_DETECT_SIZE, MAX_FACE_DETECT_SIZE));
                                  
  int cnt = 0;
  int i=0;

  // Prune detected face based on eye level, aspect ratio, size, etc.
  for(vector <Rect>::const_iterator r = face_pos.begin(); r != face_pos.end(); r++,i++){
    aspect_ratio = (double)(r->width)/r->height;
    if (aspect_ratio > 0.75 && aspect_ratio < 1.3){
      Point2f cen;
      cen.x = r->x + (r->width + r->height)/2;
      cen.y = r->y + (r->width + r->height)/2;
      face_pos[i].width = max(r->width,r->height);
      face_pos[i].height = r->width;
      face_pos[i].x = cen.x - r->width;
      face_pos[i].y = cen.y - r->height;
      face_ROI = scaled_img(*r);
      FaceDetect::detect_eyes(eye_pos, face_ROI);
      eye_pos.left_pos.x = round((r->x + eye_pos.left_pos.x)*scale);
      eye_pos.left_pos.y = round((r->y + eye_pos.left_pos.y)*scale);
      eye_pos.right_pos.x = round((r->x + eye_pos.right_pos.x)*scale);
      eye_pos.right_pos.y = round((r->y + eye_pos.right_pos.y)*scale);
      eye_pos.left_radius = round(eye_pos.left_radius * scale);
      eye_pos.right_radius = round(eye_pos.right_radius * scale);
      face_pos[cnt].x *= scale;
      face_pos[cnt].y *= scale;
      face_pos[cnt].width *=scale;
      face_pos[cnt++].height *=scale;
      eye.push_back(eye_pos);
    }
  }
  
  // Get frame rate of detection
  tt = (double)getTickCount() - tt;
  dt = tt/((double)getTickFrequency());
  //cout << "Face Detection time = " << tt/((double)getTickFrequency()*1000)<< " ms" << endl;
  return dt;
}

void FaceDetect::detect_eyes(struct Eye &eye_pos, Mat &img){
  double radius, radius1 = -1, radius2 = -1, scale = (double)EYE_DETECT_FACE_SIZE/img.cols;
  Point2f center, center1, center2;
  vector <Rect> eye_objects;
  Mat scaled_img;
  resize(img,scaled_img, Size(round(scale*img.rows), EYE_DETECT_FACE_SIZE), 0, 0, INTER_LINEAR);
  eye_classifier.detectMultiScale(scaled_img, eye_objects, EYE_CLASSIFIER_SCALE,
                                  3, // Number of neighbours
                                  (0 | CV_HAAR_SCALE_IMAGE), 
                                  Size(MIN_EYE_DETECT_SIZE, MIN_EYE_DETECT_SIZE),
                                  Size(MAX_EYE_DETECT_SIZE, MAX_EYE_DETECT_SIZE));
  int img_half_level = scaled_img.rows/2;
  for(vector <Rect>::const_iterator r = eye_objects.begin(); r != eye_objects.end(); r++){
    radius = 0.25*(r->width + r->height);
    center.x = r->x + r->width*0.5;
    center.y = r->y + r->height*0.5;
    if (center.y > img_half_level)
        continue;
    if (radius > radius1){
      radius2 = radius1;
      center2.x = center1.x;
      center2.y = center1.y;
      radius1 = radius;
      center1.x = center.x;
      center1.y = center.y;
    }
    else if (radius > radius2){
      radius2 = radius;
      center2.x = center.x;
      center2.y = center.y;
    }
  }

  // Assign eye to left or right appropriately
  int image_width_half = scaled_img.cols/2;
  if (radius2 <= 0){
      if (center1.x < image_width_half){
        eye_pos.left_pos.x = center2.x/scale;
        eye_pos.left_pos.y = center2.y/scale;
        eye_pos.left_radius = radius2/scale;
        eye_pos.right_pos.x = center1.x/scale;
        eye_pos.right_pos.y = center1.y/scale;
        eye_pos.right_radius = radius1/scale;
      }
      else {
        eye_pos.left_pos.x = center1.x/scale;
        eye_pos.left_pos.y = center1.y/scale;
        eye_pos.left_radius = radius1/scale;
        eye_pos.right_pos.x = center2.x/scale;
        eye_pos.right_pos.y = center2.y/scale;
        eye_pos.right_radius = radius2/scale;
      }
  }
  else{
      if (center1.x < center2.x){
        eye_pos.left_pos.x = center2.x/scale;
        eye_pos.left_pos.y = center2.y/scale;
        eye_pos.left_radius = radius2/scale;
        eye_pos.right_pos.x = center1.x/scale;
        eye_pos.right_pos.y = center1.y/scale;
        eye_pos.right_radius = radius1/scale;
      }
      else {
        eye_pos.left_pos.x = center1.x/scale;
        eye_pos.left_pos.y = center1.y/scale;
        eye_pos.left_radius = radius1/scale;
        eye_pos.right_pos.x = center2.x/scale;
        eye_pos.right_pos.y = center2.y/scale;
        eye_pos.right_radius = radius2/scale;
      }
  }
  return;
}

void FaceDetect::draw_face(vector <Rect> &face_pos, vector <struct Eye> & eye_pos, Mat &img, bool eye_flag){
  Point pt1,pt2;
  for(int i=0; i<face_pos.size(); i++){
    pt1.x = face_pos[i].x;
    pt1.y = face_pos[i].y;
    pt2.x = pt1.x + face_pos[i].width;
    pt2.y = pt1.y + face_pos[i].height;
    rectangle(img,pt1,pt2,CV_RGB(0,0,255),1,8,0);
    if (eye_flag){
      if (eye_pos[i].left_radius > 0){
        circle(img, eye_pos[i].left_pos, eye_pos[i].left_radius, CV_RGB(0,0,255), 1, 8, 0);
      }
      if (eye_pos[i].right_radius > 0){
        circle(img, eye_pos[i].right_pos, eye_pos[i].right_radius, CV_RGB(0,0,255), 1, 8, 0);
      }
    }
  }
  return;
}

void FaceDetect::draw_eyes(vector <struct Eye> & eye_pos, Mat &img){
  for(int i=0; i<eye_pos.size(); i++){
    if (eye_pos[i].left_radius > 0)
        circle(img, eye_pos[i].left_pos, eye_pos[i].left_radius, CV_RGB(0,0,255), 1, 8, 0);
    if (eye_pos[i].right_radius > 0)
        circle(img, eye_pos[i].right_pos, eye_pos[i].right_radius, CV_RGB(0,0,255), 1, 8, 0);
  }
  return;
}

void FaceRecognize::load(string file_recognizer, string file_class){

  // Load Saved Face Recognition Classifiers
  eigen_recognizer = createEigenFaceRecognizer();
  fisher_recognizer = createFisherFaceRecognizer();
  lbph_recognizer = createLBPHFaceRecognizer();
  eigen_recognizer->load(file_recognizer + "_eigen.xml");
  fisher_recognizer->load(file_recognizer + "_fisher.xml");
  lbph_recognizer->load(file_recognizer + "_lbph.xml");
  
  // Load corresponding face labels
  ifstream file(file_class.c_str(), ifstream::in);
  if (!file)
    throw std::exception();
  char separator = ';';
  string line, name, class_label;
  while (getline(file, line)) {
    stringstream liness(line);
    getline(liness, name, separator);
    getline(liness, class_label);  
    face_name.push_back(name);
  }
  return;
}

void FaceRecognize::recognize_face(vector <string> &name, vector <double> &confidence, vector <int> &label, vector <Rect> &face_pos, Mat &img, string classifier){
  Mat scaled_img, gray_img;
  int eigen_prediction, fisher_prediction, lbph_prediction, N = face_pos.size();
  double eigen_confidence, fisher_confidence, lbph_confidence; 
  
  name.clear();
  confidence.clear();
  label.clear();
  if (img.channels() > 1) 
    cvtColor(img, gray_img, CV_RGB2GRAY);
  else
    gray_img = img;
    
  confidence.resize(N);
  label.resize(N);
  name.resize(N);
  for (int i=0; i<N; i++){
    resize(gray_img(face_pos[i]), scaled_img, Size(FACE_RECOGNIZE_SIZE, FACE_RECOGNIZE_SIZE), 0, 0, INTER_LINEAR );   
    eigen_prediction = -1; fisher_prediction = -1; lbph_prediction = -1;
    eigen_confidence = -1; fisher_confidence = -1; lbph_confidence = -1; 
    label.push_back(-1);
    confidence.push_back(-1);
    
    // Predict using different classifiers
    if (classifier == "eigen" || classifier == "majority" || classifier == "all")
      eigen_recognizer->predict(scaled_img,eigen_prediction,eigen_confidence);
    if (classifier == "fisher" || classifier == "majority" || classifier == "all")
      fisher_recognizer->predict(scaled_img,fisher_prediction,fisher_confidence);
    if (classifier == "lbph" || classifier == "majority" || classifier == "all")
      lbph_recognizer->predict(gray_img(face_pos[i]),lbph_prediction,lbph_confidence);
    
    // Decide which classifiers to use
    if (classifier == "eigen"){
      label[i] = eigen_prediction;
      confidence[i] = eigen_confidence;
    }
    else if (classifier == "fisher"){
      label[i] = fisher_prediction;
      confidence[i] = fisher_confidence;
    }
    else if (classifier == "lbph"){
      label[i] = lbph_prediction;
      confidence[i] = lbph_confidence;
    }
    else if (classifier == "majority"){
      if (eigen_prediction == fisher_prediction || eigen_prediction == lbph_prediction)
        label[i] = eigen_prediction;
      else if (fisher_prediction == lbph_prediction)
        label[i] = fisher_prediction;
      else
        label[i] = -1;
      confidence[i] = -1;
    }
    else { //"all" 
      if (eigen_prediction == fisher_prediction && eigen_prediction == lbph_prediction)
        label[i] = eigen_prediction;
      else
        label[i] = -1;
      confidence[i] = fisher_confidence;
    }
    
    if (label[i] == -1)
      name[i] = "Unknown";
    else
      name[i] = face_name[label[i]];
  }   
}

void FaceRecognize::label_face(Mat &img, vector <string> &label, vector <Rect> &face_pos){
	Point2f txtpt, pt1, pt2;
	int N = label.size();
	for (int i=0; i<N; i++){
	  txtpt.x = face_pos[i].x;
	  txtpt.y = face_pos[i].y - 7;
	  pt1.x = face_pos[i].x;
	  pt1.y = face_pos[i].y;
	  pt2.x = face_pos[i].x + face_pos[i].width;
	  pt2.y = face_pos[i].y + face_pos[i].height;
	  rectangle(img, pt1, pt2, CV_RGB(0,255,0), 2, 8, 0);
	  putText(img, label[i], txtpt, CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1);
	}
	return;
}

void FaceRecognize::train(string train_file, string class_file, string recognizer_file){
  Mat img,scaled_img,recognize_img,nrecognize_img;
  vector <Rect> face_pos;
  vector <struct Eye> eye;
  vector <string> face_name;
  vector <int> face_count;
  
  double scale = 1 ;
  ifstream filec(class_file.c_str(), ifstream::in);
  string tmp,textn,tmp1;
  tmp1=train_file;
  tmp1[0]='o';
  tmp1[1]='o';
  stringstream ss(tmp1);
  
  
  getline(ss,tmp,'.');
  getline(ss,textn);
  
  ifstream chkfile_orig((recognizer_file + "_orig.csv").c_str(), ifstream::in);
  ifstream chkfile_resized((recognizer_file + "_resized.csv").c_str(), ifstream::in);
  
  // Create directories for putting train images
  if (!filec)
    throw std::exception();
  char separator = ';';  
  string line, path, name, class_label;
  string cmd,cmd_prefix = "mkdir ../train/";
  while (getline(filec, line)){
    stringstream liness(line);
    getline(liness, name, separator);
    getline(liness, class_label);
    face_name.push_back(name);
    face_count.push_back(0);
    cmd = cmd_prefix + name;
    cmd = cmd_prefix + name + "_resized";
    cout << "Creating directory : " + name << endl;
    system(cmd.c_str());
  }
  
  // Load saved cropped images for building a classifier
  string label;
  vector <int> labels;
  vector <Mat> images,orig_images;
  if (0 && chkfile_orig && chkfile_resized){
    while (getline(chkfile_orig, line)){
      stringstream liness(line);
      getline(liness, path, separator);
      getline(liness, label);
      img = imread(path, 0);
      orig_images.push_back(img);
      labels.push_back(atoi(label.c_str()));
    }
    while (getline(chkfile_resized, line)){
      stringstream liness(line);
      getline(liness, path, separator);
      getline(liness, label);
      img = imread(path, 0);
      images.push_back(img);
    }
  }
  else{
    // Train from a set of images
    if (textn == "csv"){
      ifstream file(train_file.c_str(), ifstream::in);
      ofstream fout_orig((recognizer_file + "_orig.csv").c_str(), ofstream::out);
      ofstream fout_resized((recognizer_file + "_resized.csv").c_str(), ofstream::out);
      if (!file)
        cerr << "File Not Found" << endl;
      while (getline(file, line)){
        stringstream liness(line);
        getline(liness, path, ';');
        getline(liness, label);
        img = imread(path, 0);
        int nlabel = atoi(label.c_str());
        if (img.rows > 600 || img.cols > 800){
          scale = img.rows/480;
          scaled_img.create(cvRound(img.rows/scale),cvRound(img.cols/scale),CV_8UC1);
          resize(img, scaled_img, scaled_img.size(), 0, 0, INTER_LINEAR );
        }
        else{
          scaled_img = img;
        }    
        string num_label = "00000";
        
        num_label[4] = face_count[nlabel] % 10 + '0';
        num_label[3] = (face_count[nlabel]/10) % 10 + '0';
        num_label[2] = (face_count[nlabel]/100) % 10 + '0';
        num_label[1] = (face_count[nlabel]/1000) % 10 + '0';
        num_label[0] = (face_count[nlabel]/10000) % 10 + '0';
        
        detect_face(face_pos, eye, scaled_img, 1);
        draw_face(face_pos,eye,scaled_img,1);
        if (face_pos.size() <= 0)
          continue;
        //if (eye[0].left_radius == -1 || eye[0].right_radius == -1)
        //  continue;
        recognize_img = scaled_img(face_pos[0]);
        struct Eye teye;
        teye.left_pos.x = eye[0].left_pos.x - face_pos[0].x;
        teye.left_pos.x = eye[0].left_pos.y - face_pos[0].y;
        teye.right_pos.x = eye[0].right_pos.x - face_pos[0].x;
        teye.right_pos.x = eye[0].right_pos.y - face_pos[0].y;
        align_face(nrecognize_img, recognize_img, teye, 0.3, 0.3, Size(FACE_RECOGNIZE_SIZE, FACE_RECOGNIZE_SIZE));
        imshow("detected_img",recognize_img);
        imshow("cropped_img",nrecognize_img);
        if (waitKey(0) == 'y'){
          string filepath = "../train/" + face_name[nlabel] + "_orig/" + face_name[nlabel] + num_label + ".jpg";
          fout_orig << filepath << ";"<<nlabel << endl;
          cout << "Saving original sample to" + filepath << endl;
          imwrite(filepath,recognize_img);
          orig_images.push_back(recognize_img);      
          filepath = "../train/" + face_name[nlabel] + "_resized/" + face_name[nlabel] + num_label + ".jpg";
          fout_resized << filepath <<";"<<nlabel<<endl;
          cout << "Saving resized sample to" + filepath << endl;
          imwrite(filepath,nrecognize_img);
          images.push_back(nrecognize_img);
          labels.push_back(nlabel);
          face_count[nlabel]++;
        }
      }
    }
    // Train from video
    else{
      cap1.open(train_file);
      Mat frame,frame_gray;
      int totalNumFrames = cap1.get(CV_CAP_PROP_FRAME_COUNT), pos = 0, skip_count = 0;
      bool skip_flag;
      namedWindow("frame",1);
			createTrackbar("pos","frame",&pos,totalNumFrames,onTrackbarSlide);
      while (1){
        cap1 >> frame;
        if (frame.empty())
          break;
        cvtColor(frame,frame_gray,CV_BGR2GRAY);
        detect_face(face_pos, eye, frame_gray, 1);
        skip_flag = 1;
        
        for (int m=0; m<face_pos.size(); m++){
          if ((eye[m].left_radius > 0 && eye[m].right_radius > 0) || skip_count >= 20){
            skip_flag = 0;
            skip_count = 0;
            recognize_img = frame_gray(face_pos[m]);
            align_face(nrecognize_img, recognize_img, eye[m], 0.3, 0.3, Size(FACE_RECOGNIZE_SIZE, FACE_RECOGNIZE_SIZE));
            imshow("detected_img",recognize_img);
            imshow("cropped_img",nrecognize_img);
            /*
            string num_label = "00000";
            num_label[4] = face_count[nlabel] % 10 + '0';
            num_label[3] = (face_count[nlabel]/10) % 10 + '0';
            num_label[2] = (face_count[nlabel]/100) % 10 + '0';
            num_label[1] = (face_count[nlabel]/1000) % 10 + '0';
            num_label[0] = (face_count[nlabel]/10000) % 10 + '0';
            */
            waitKey(0);
          }
        }
        imshow("frame",frame_gray);
        if (skip_flag){
          pos++;
          skip_count++;
        }
        else{
          pos += 5;
        }
        setTrackbarPos("pos","frame",pos);
        if(skip_flag==0 && (waitKey(0) == 27 || pos >= totalNumFrames))
          break;
        cap1.set(CV_CAP_PROP_POS_FRAMES,pos);
      }
    }

    // Create eigen, fisher and lbph classifiers
    eigen_recognizer = createEigenFaceRecognizer(80,EIGEN_THRESH);	  
    fisher_recognizer = createFisherFaceRecognizer(0,FISHER_THRESH);	  
    lbph_recognizer = createLBPHFaceRecognizer(1,8,8,8,LBPH_THRESH);	 
    eigen_recognizer->train(images, labels);
    fisher_recognizer->train(images, labels);
    lbph_recognizer->train(orig_images, labels); 

    // Save classifiers as xml files
    eigen_recognizer->save(recognizer_file + "_eigen.xml");
    fisher_recognizer->save(recognizer_file + "_fisher.xml");
    lbph_recognizer->save(recognizer_file + "_lbph.xml");
  
    cout << "Storing Eigen, Fisher and LBPH classifiers for the sample image at " + recognizer_file << endl;
  }
  return;
}
void FaceTracker::klt_track_face(vector <string> &name, vector <int> &label, vector <Rect> &face_pos, Mat &img, Mat &prev_img, vector < vector <Point2f> > &features, string classifier, string mode){
  name.clear();
  label.clear();
  face_pos.clear();
  int N = face_pos.size(), Ni, num_features, displacement_x, displacement_y;
  static int frame_count=0;
  vector <double> confidence;
  vector <float> err;
  vector <uchar> status;
  vector <Point2f> nfeatures;
  vector <struct Eye> eye;
  Rect roi;
  Mat gray_img,prev_gray_img;
  
  if (img.channels() > 1) 
    cvtColor(img, gray_img, CV_RGB2GRAY);
  else
    gray_img = img;
  
  if (prev_img.channels() > 1) 
    cvtColor(prev_img, prev_gray_img, CV_RGB2GRAY);
  else
    prev_gray_img = prev_img;
  
  // Find features every set number of frames
  if (frame_count == 0){
    detect_face(face_pos, eye, gray_img, 1, mode);
    recognize_face(name, confidence, label, face_pos, gray_img, classifier);
    N = face_pos.size();
    features.clear();
    features.resize(N);
  }
  for (int i=0; i<N; i++){
    roi.x = max(face_pos[i].x - ROI_ALLOWANCE,0);
    roi.y = max(face_pos[i].y - ROI_ALLOWANCE,0);
    roi.width = min(face_pos[i].width + 2*ROI_ALLOWANCE,gray_img.cols-roi.x-1);
    roi.height = min(face_pos[i].height + 2*ROI_ALLOWANCE,gray_img.rows-roi.y-1);

    // Track features using optical flow
    if (frame_count != 0){
      calcOpticalFlowPyrLK(prev_gray_img(roi),gray_img(roi),features[i],nfeatures,status,err,Size(10,10));
      Ni = status.size();
      displacement_x = 0;
      displacement_y = 0;
      for (int j=0; j<Ni; j++){
        if (status[j]){
          displacement_x += nfeatures[j].x - features[i][j].x;
          displacement_y += nfeatures[j].y - features[i][j].y;
        }
      }
      features[i].clear();
      num_features = 0;
      for (int j=0; j<Ni; j++){
        if (status[j]){
          features[i].push_back(nfeatures[j]);
          nfeatures[num_features++] = nfeatures[j];
        }
      }
      nfeatures.resize(num_features);
      displacement_x /= num_features;
      displacement_y /= num_features;
      face_pos[i].x += displacement_x;
      face_pos[i].y += displacement_y;
    }
    num_features = features[i].size();
    if (num_features < KLT_MIN_FEATURES_PER_FACE){
      features[i].clear();
      goodFeaturesToTrack(gray_img(roi), features[i], KLT_MAX_FEATURES_PER_FACE, KLT_FEATURE_QUALITY, KLT_MIN_FEATURE_DIST);
    }
  }
  frame_count++;
  if (frame_count == NUM_FRAMES_WITHOUT_DETECTION)
    frame_count = 0;
  return;
}
