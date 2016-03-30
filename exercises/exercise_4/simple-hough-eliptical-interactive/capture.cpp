/*
 *
 *  Example by Sam Siewert 
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>

using namespace cv;
using namespace std;

#define HRES 960
#define VRES 720

double prev_frame_time;
double curr_frame_time;
struct timespec frame_time;

int main( int argc, char** argv )
{
	double ave_framedt=0.0, ave_frame_rate=0.0, fc=0.0, framedt=0.0;
	unsigned int frame_count=0;

	cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
	//CvCapture* capture = (CvCapture *)cvCreateCameraCapture(0);
	//CvCapture* capture = (CvCapture *)cvCreateCameraCapture(argv[1]);
	CvCapture* capture;
	IplImage* frame;
	int dev=0;
	Mat gray;
	vector<Vec3f> circles;

	if(argc > 1)
	{
		sscanf(argv[1], "%d", &dev);
		printf("using %s\n", argv[1]);
	}
	else if(argc == 1)
		printf("using default\n");

	else
	{
		printf("usage: capture [dev]\n");
		exit(-1);
	}

	capture = (CvCapture *)cvCreateCameraCapture(dev);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);

	while(1)
	{
		frame=cvQueryFrame(capture);

		clock_gettime(CLOCK_REALTIME, &frame_time);
		curr_frame_time=((double)frame_time.tv_sec * 1000.0) + 
			((double)((double)frame_time.tv_nsec / 1000000.0));
		frame_count++;

		if(frame_count > 2)
		{
			fc=(double)frame_count;
			ave_framedt=((fc-1.0)*ave_framedt + framedt)/fc;
			ave_frame_rate=1.0/(ave_framedt/1000.0);
		}		
		printf("Frame @ %u sec, %lu nsec, dt=%5.2lf msec, avedt=%5.2lf msec, rate=%5.2lf fps\n", 
				(unsigned)frame_time.tv_sec, 
				(unsigned long)frame_time.tv_nsec,
				framedt, ave_framedt, ave_frame_rate);
		Mat mat_frame(frame);
		cvtColor(mat_frame, gray, CV_BGR2GRAY);
		GaussianBlur(gray, gray, Size(9,9), 2, 2);

		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 100, 50, 0, 0);

		//printf("circles.size = %d\n", circles.size());

		for( size_t i = 0; i < circles.size(); i++ )
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			// circle center
			circle( mat_frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
			// circle outline
			circle( mat_frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
		}


		if(!frame) break;

		cvShowImage("Capture Example", frame);

		char c = cvWaitKey(10);
		if( c == 27 ) break;

		framedt=curr_frame_time - prev_frame_time;
		prev_frame_time=curr_frame_time;

	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("Capture Example");

};
