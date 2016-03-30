/* Zachary Vogel and Priyanka Pashte
 * REal time embedded Exercise 4
 * Problem 5
 *   
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#define frames 240



using namespace cv;
using namespace std;

#define HRES 160
#define VRES 120
#define arrin 0

// Transform display window
char timg_window_name[] = "Edge Detector Transform";

struct timespec frame_time1, frame_time2, frame_time3;
double curr_frame_time1, prev_frame_time1;
double curr_frame_time2, prev_frame_time2;
double curr_frame_time3, prev_frame_time3;


double sup[5][3]={{38,36,36},{41,37,38},{50,39,43},{58,43,60},{74,67,80}};


int lowThreshold=0;
int const max_lowThreshold = 100;
int kernel_size = 3;
int edgeThresh = 1;
int ratio = 3;
Mat canny_frame, cdst, timg_gray, timg_grad, canny_frame1;

pthread_mutex_t capturelock, windowlock;

unsigned int frame_count1=0,frame_count2=0,frame_count3=0;
CvCapture* capture;


double standard_dev(double avg, double * values, int length)
{
    double sum;
    for(int i=0;i<length;i++)
    {
        sum+=pow(values[i]-avg,2.0);
    }
    sum/=length;
    return sqrt(sum);
}


void* CannyThreshold(void* b)
{
    double ave_framedt1=0.0, ave_frame_rate1=0.0, fc1=0.0, framedt1=0.0;
    IplImage* frame;
    pthread_mutex_lock(&windowlock);
    cvNamedWindow( timg_window_name, CV_WINDOW_AUTOSIZE );
    pthread_mutex_unlock(&windowlock);
    // Create a Trackbar for user to enter threshold
    //createTrackbar( "Min Threshold:", timg_window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

    double a[frames];
    
    int i=0;
    while(i<frames)
    {
        clock_gettime(CLOCK_REALTIME,&frame_time1);
        curr_frame_time1=((double)frame_time1.tv_sec * 1000.0) + ((double)((double)frame_time1.tv_nsec /1000000.0));
        frame_count1++;
        if(frame_count1 > 2)
        {
            fc1=(double)frame_count1;
            ave_framedt1=((fc1-1.0)*ave_framedt1 + framedt1)/fc1;
            ave_frame_rate1=1.0/(ave_framedt1/1000.0);
        }

        pthread_mutex_lock(&capturelock);
        frame=cvQueryFrame(capture);
        pthread_mutex_unlock(&capturelock);

        Mat mat_frame(frame);

        cvtColor(mat_frame, timg_gray, CV_RGB2GRAY);

        /// Reduce noise with a kernel 3x3
        blur( timg_gray, canny_frame, Size(3,3) );

        /// Canny detector
        Canny( canny_frame, canny_frame, lowThreshold, lowThreshold*ratio, kernel_size );

        /// Using Canny's output as a mask, we display our result
        timg_grad = Scalar::all(0);

        mat_frame.copyTo( timg_grad, canny_frame);
        
        pthread_mutex_lock(&windowlock);
        imshow( timg_window_name, timg_grad );
        pthread_mutex_unlock(&windowlock);
        framedt1=curr_frame_time1 - prev_frame_time1;
        prev_frame_time1=curr_frame_time1;
        if(framedt1>sup[arrin][1]*1000.0/3.0&frame_count1>2)
        {
            printf("canny failed deadline\n");
        }
        a[i]=framedt1;
        i++;
    }
    cvDestroyWindow(timg_window_name);
    if(ave_framedt1>sup[arrin][1]*1000.0/3.0)
    {
        printf("average deadline was failed deadline for canny\n");
    }
    printf("The average execution time in ms for canny was: %f \n",ave_framedt1);
    printf("The standard deviation for canny in ms was: %f\nThis was for %d frames\n",standard_dev(ave_framedt1,&a[2],frames-2),frames);
    printf("fps for canny was %f\n", ave_frame_rate1);
    return NULL;
}


void* Eliptical(void * b)
{
    Mat gray;

    vector<Vec3f> circles;

    double a[frames];
   
    double ave_framedt2=0.0, ave_frame_rate2=0.0, fc2=0.0, framedt2=0.0;
    IplImage* frame;

    pthread_mutex_lock(&windowlock);
    cvMoveWindow("Capture Example",HRES,0);
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    pthread_mutex_unlock(&windowlock);
    //printf("elip got here\n");
    int i;
    while(i<frames)
    {
        clock_gettime(CLOCK_REALTIME,&frame_time2);
        curr_frame_time2=((double)frame_time2.tv_sec * 1000.0) + ((double)((double)frame_time2.tv_nsec /1000000.0));
        frame_count2++;
        if(frame_count2 > 2)
        {
            fc2=(double)frame_count2;
            ave_framedt2=((fc2-1.0)*ave_framedt2 + framedt2)/fc2;
            ave_frame_rate2=1.0/(ave_framedt2/1000.0);
        }
        pthread_mutex_lock(&capturelock);
        frame=cvQueryFrame(capture); 
        pthread_mutex_unlock(&capturelock);
        Mat mat_frame(frame);
        cvtColor(mat_frame,gray, CV_BGR2GRAY);
        GaussianBlur(gray,gray,Size(9,9),2,2);

        HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1 , gray.rows/8 , 100 ,50 ,0 , 0);


        for(size_t j=0; j<circles.size();j++)
        {
            Point center(cvRound(circles[j][0]), cvRound(circles[j][1]));
            int radius = cvRound(circles[j][2]);
            //circle center
            circle(mat_frame,center,3,Scalar(0,255,0), -1,8,0);
            //circle outline
            circle(mat_frame,center,radius,Scalar(0,0,255),3,8,0);
        }
        if(!frame) break;
        
        pthread_mutex_lock(&windowlock);
        cvShowImage("Capture Example",frame);
        pthread_mutex_unlock(&windowlock);
        char c=cvWaitKey(10);
        if (c==27) break;

        
        framedt2=curr_frame_time2 - prev_frame_time2;
        prev_frame_time2=curr_frame_time2;
        if(framedt2>sup[arrin][2]*1000.0/3.0&frame_count2>2)
        {
            printf("eliptical failed a deadline\n");
        }
        a[i]=framedt2;
        i++;
    }
    cvDestroyWindow("Capture Example\n");
    if(ave_framedt2>sup[arrin][2]*1000.0/3.0)
    {
        printf("average deadline was failed deadline for elliptical Hough\n");
    }
    printf("The average execution time in ms for eliptical was:%f\nthe standard deviation in ms was: %f\nThis was for %d frames\n",ave_framedt2,standard_dev(ave_framedt2,&a[2],frames-2),frames);
    printf("fps for elliptical was %f\n", ave_frame_rate2);
    return NULL;
}

void* Lines_onlines(void * b)
{
    IplImage* frame;
    vector<Vec4i> lines;
    double ave_framedt3=0.0, ave_frame_rate3=0.0, fc3=0.0, framedt3=0.0;
    
    double a[frames];


    pthread_mutex_lock(&windowlock);
    cvNamedWindow("Capture lines", CV_WINDOW_AUTOSIZE);
    pthread_mutex_unlock(&windowlock);
    
    int i;

    while(i<frames)
    {
        clock_gettime(CLOCK_REALTIME,&frame_time3);
        curr_frame_time3=((double)frame_time3.tv_sec * 1000.0) + ((double)((double)frame_time3.tv_nsec /1000000.0));
        frame_count3++;
        if(frame_count3 > 2)
        {
            fc3=(double)frame_count3;
            ave_framedt3=((fc3-1.0)*ave_framedt3 + framedt3)/fc3;
            ave_frame_rate3=1.0/(ave_framedt3/1000.0);
        }
        pthread_mutex_lock(&capturelock);
        frame=cvQueryFrame(capture); 
        pthread_mutex_unlock(&capturelock); 

        Mat mat_frame(frame);
        Canny(mat_frame, canny_frame, 50, 200, 3);

        HoughLinesP(canny_frame, lines, 1, CV_PI/180, 50, 50, 10);

        for(size_t j=0; j<lines.size();j++)
        {
            Vec4i l=lines[j];
            line(mat_frame,Point(l[0], l[1]),Point(l[2], l[3]),Scalar(0,0,255),3,CV_AA);
        }
        
        if(!frame) break;
        pthread_mutex_lock(&windowlock);
        cvShowImage("Capture lines",frame);
        pthread_mutex_unlock(&windowlock);
        char c=cvWaitKey(10);
        if (c==27) break;
        framedt3=curr_frame_time3 - prev_frame_time3;
        prev_frame_time3=curr_frame_time3;
        if(framedt3>sup[arrin][3]*1000.0/3.0&frame_count3>2)
        {
            printf("hough line failed deadline\n");
        }
        a[i]=framedt3;
        i++;
    }
    cvDestroyWindow("Capture lines\n"); 
    if(ave_framedt3>sup[arrin][3]*1000.0/3.0)
    {
        printf("average deadline was failed deadline for line Hough\n");
    }
    printf("The average execution time for line Hough in ms was: %f\nthe standard deviation in ms was: %f\nThis was for %d frames\n",ave_framedt3,standard_dev(ave_framedt3,&a[2],frames-2),frames);
    printf("fps for lines was %f\n", ave_frame_rate3);
    return NULL;
}

int main( int argc, char** argv )
{
    //cvStartWindowThread();
    //spawn threads
    pthread_attr_t attr;
    pthread_attr_t * attrp;
    
    pthread_t pth[3];
    struct sched_param param1;
    attrp=&attr;

    param1.sched_priority=1;

    pthread_attr_init(attrp);
    pthread_attr_setinheritsched(attrp,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(attrp,SCHED_FIFO);
    pthread_attr_setschedparam(attrp,&param1);


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

    capture = (CvCapture *)cvCreateCameraCapture(dev);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);
    
    //cvNamedWindow("Capture lines", CV_WINDOW_AUTOSIZE); 
    //cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    //cvMoveWindow("Capture lines", HRES,VRES);

    pthread_create(&pth[0],attrp,Lines_onlines,NULL);
    pthread_create(&pth[1],attrp,Eliptical,NULL);
    pthread_create(&pth[2],attrp,CannyThreshold,NULL);


    pthread_join(pth[0],NULL);
    pthread_join(pth[1],NULL);
    pthread_join(pth[2],NULL);

    cvReleaseCapture(&capture);
};
