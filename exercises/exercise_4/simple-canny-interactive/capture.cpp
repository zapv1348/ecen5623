/*
 *
 *  Example by Sam Siewert 
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define HRES 960
#define VRES 720

// Transform display window
char timg_window_name[] = "Edge Detector Transform";

struct timespec frame_time;
double curr_frame_time, prev_frame_time;


int lowThreshold=0;
int const max_lowThreshold = 100;
int kernel_size = 3;
int edgeThresh = 1;
int ratio = 3;
Mat canny_frame, cdst, timg_gray, timg_grad;

IplImage* frame;

void CannyThreshold(int, void*)
{
	Mat mat_frame(frame);

	cvtColor(mat_frame, timg_gray, CV_RGB2GRAY);

	/// Reduce noise with a kernel 3x3
	blur( timg_gray, canny_frame, Size(3,3) );

	/// Canny detector
	Canny( canny_frame, canny_frame, lowThreshold, lowThreshold*ratio, kernel_size );

	/// Using Canny's output as a mask, we display our result
	timg_grad = Scalar::all(0);

	mat_frame.copyTo( timg_grad, canny_frame);

	imshow( timg_window_name, timg_grad );

}


int main( int argc, char** argv )
{
	double ave_framedt=0.0, ave_frame_rate=0.0, fc=0.0, framedt=0.0;
	unsigned int frame_count=0;
	CvCapture* capture;
	int dev=0;
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

	namedWindow( timg_window_name, CV_WINDOW_AUTOSIZE );
	// Create a Trackbar for user to enter threshold
	createTrackbar( "Min Threshold:", timg_window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

	capture = (CvCapture *)cvCreateCameraCapture(dev);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);

	while(1)
	{
		clock_gettime(CLOCK_REALTIME,&frame_time);
		frame=cvQueryFrame(capture);
		if(!frame) break;
		else{

			CannyThreshold(0, 0);

			clock_gettime(CLOCK_REALTIME, &frame_time);
			curr_frame_time=((double)frame_time.tv_sec * 1000.0) + ((double)((double)frame_time.tv_nsec /1000000.0));
			frame_count++;

			if(frame_count > 2)
			{
				fc=(double)frame_count;
				ave_framedt=((fc-1.0)*ave_framedt + framedt)/fc;
				ave_frame_rate=1.0/(ave_framedt/1000.0);
			}
		}
		printf("Frame @ %u sec, %lu nsec, dt=%5.2lf msec, avedt=%5.2lf msec, rate=%5.2lf fps\n", 
				(unsigned)frame_time.tv_sec, 
				(unsigned long)frame_time.tv_nsec,
				framedt, ave_framedt, ave_frame_rate);

		char q = cvWaitKey(33);
		if( q == 'q' )
		{
			printf("got quit\n"); 
			break;
		}
		framedt=curr_frame_time - prev_frame_time;
		prev_frame_time=curr_frame_time;

	}

	cvReleaseCapture(&capture);

};
