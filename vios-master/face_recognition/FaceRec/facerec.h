#ifndef FACEREC_H
#define FACEREC_H

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

struct Eye{
  Point2f left_pos,right_pos;  
  float left_radius,right_radius;
};

namespace facerec{
	class Face{
		public:
          Point2f leftEyePos, rightEyePos, facePos;
          float leftEyeRadius, rightEyeRadius, faceWidth, faceHeight;
          Face();
          Face(float pF, float fW, float fH);
          Face(float pF, float fW, float fH, float pLE = , float pRE, float rLE, float rRE);
          ~Face();

          // Crops and aligns face images for recognition
          void cropAlignedFace(Mat &out, Mat & img, float offset_x, float offset_y, Size sz);
	};

    class FaceDetect{
        public:
            // Classifier Parameters
            double FACE_CLASSIFIER_SCALE, EYE_CLASSIFIER_SCALE;
            int MIN_FACE_DETECT_SIZE, MAX_FACE_DETECT_SIZE, MIN_EYE_DETECT_SIZE, MAX_EYE_DETECT_SIZE, EYE_DETECT_FACE_SIZE;
            string EYE_CLASSIFIER_NAME, FACE_CLASSIFIER_NAME;
            CascadeClassifier eyeClassifier, faceClassifier;

        public:
            // Constructor initializing all classifier parameters
            FaceDetect();
            ~FaceDetect();

            // Returns a vector of face bounding boxes and eye circle pairs given the input image
            double detectFace(vector <Face> &faces, Mat &img, double scale = 1);
            // Returns a vector of eye circle pairs given face image
            void detectEyes(struct Eye &eyePos, Mat &img);
	};

	class FaceRecognize: public FaceDetect{
        public:
            // Face Recognition parameters
            int FACE_RECOGNIZE_SIZE, EYE_LEVEL, EIGEN_THRESH, FISHER_THRESH, LBPH_THRESH;
            vector <string> face_name;
            Ptr<FaceRecognizer> eigen_recognizer, fisher_recognizer, lbph_recognizer;
        public:
            // Constructor initializing Face Recognition parameters
            FaceRecognize():FaceDetect(){
                FACE_RECOGNIZE_SIZE = 70;
                EYE_LEVEL = FACE_RECOGNIZE_SIZE/3;
                EIGEN_THRESH = 1900;
                FISHER_THRESH = 85;
                LBPH_THRESH = 70;
            }
			//~FaceRecognize();

            // Command line way for training a classifier for recognition
			void train(string train_file, string class_file, string recognizer_file);

            // Load a pre-saved classifier for Face Recognition
			void load(string file_recognizer, string file_class);

            // Recognizes and returns labels of detected faces
			void recognize_face(vector <string> &name, vector <double> &confidence, vector <int> &label, vector <Rect> &face_pos, Mat &img, string classifier = "fisher");

            // Labels recognized faces given position, label and frame
			void label_face(Mat &img, vector <string> &label, vector <Rect> &face_pos);
	};
	
	class FaceTracker: public FaceRecognize{
        public:
            int NUM_FRAMES_WITHOUT_DETECTION, KLT_MAX_FEATURES_PER_FACE, KLT_MIN_FEATURES_PER_FACE, ROI_ALLOWANCE;
            double KLT_FEATURE_QUALITY, KLT_MIN_FEATURE_DIST;
            public:
                FaceTracker():FaceRecognize(){
                  NUM_FRAMES_WITHOUT_DETECTION = 10;
                  KLT_MAX_FEATURES_PER_FACE = 20;
                  KLT_MIN_FEATURES_PER_FACE = 10;
                  KLT_FEATURE_QUALITY = 0.01;
                  KLT_MIN_FEATURE_DIST = 4;
                  ROI_ALLOWANCE = 10;
                }
                //~FaceTracker();
                // Tracks face based on optical flow
                void klt_track_face(vector <string> &name, vector <int> &label, vector <Rect> &face_pos, Mat &img, Mat &prev_img, vector < vector <Point2f> > &features, string classifier = "fisher", string mode = "front");
	};
}

#endif
