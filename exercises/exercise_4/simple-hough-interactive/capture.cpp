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

using namespace cv;
using namespace std;

#define HRES 160
#define VRES 120

double prev_frame_time, prev_frame_time_l, prev_frame_time_r;
double curr_frame_time, curr_frame_time_l, curr_frame_time_r;
struct timespec frame_time, frame_time_l, frame_time_r;

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
	Mat gray, canny_frame, cdst;
	vector<Vec4i> lines;

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
		Canny(mat_frame, canny_frame, 50, 200, 3);

		//cvtColor(canny_frame, cdst, CV_GRAY2BGR);
		//cvtColor(mat_frame, gray, CV_BGR2GRAY);

		HoughLinesP(canny_frame, lines, 1, CV_PI/180, 50, 50, 10);

		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];
			line(mat_frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
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
