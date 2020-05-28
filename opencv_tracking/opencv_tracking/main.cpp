//
//  main.cpp
//  opencv_tracking
//
//  Created by Xujian CHEN on 5/26/20.
//  Copyright © 2020 Xujian CHEN. All rights reserved.
//

#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/video.hpp>

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    cv::Rect* pTrack_window = (cv::Rect*)userdata;
    
     if  ( event == cv::EVENT_LBUTTONDOWN )
     {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
        pTrack_window->x = x;
        pTrack_window->y = y;
     }
    if  ( event == cv::EVENT_LBUTTONUP )
    {
        std::cout << "Left button of the mouse is released - position (" << x << ", " << y << ")" << std::endl;
        pTrack_window->width = std::max(std::abs(x - pTrack_window->x), 5);
        pTrack_window->height = std::max(std::abs(y - pTrack_window->y), 5);
        pTrack_window->x = std::min(x, pTrack_window->x);
        pTrack_window->y = std::min(y, pTrack_window->y);
        
    }
    /*
     else if  ( event == cv::EVENT_RBUTTONDOWN )
     {
          std::cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
     }
     else if  ( event == cv::EVENT_MBUTTONDOWN )
     {
          std::cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
     }
     else if ( event == cv::EVENT_MOUSEMOVE )
     {
          std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
     }
     */
}

/*
int main(int argc, const char * argv[]) {
    
    cv::VideoCapture stream1(0);
    if (!stream1.isOpened()) { //check if video device has been initialised
    std::cout << "cannot open camera";
    }
    //Create a window
    cv::namedWindow("Webcam", 1);
    //set the callback function for any mouse event
    cv::setMouseCallback("Webcam", CallBackFunc, NULL);
    
    
    //unconditional loop
    while (true) {
        cv::Mat cameraFrame;
        stream1.read(cameraFrame);
        
        
        
        
        
        
        imshow("Webcam", cameraFrame);
        if (cv::waitKey(30) >= 0)
        break;
    }
    
    cv::destroyAllWindows();
    stream1.release();

    
    return 0;
}
*/


using namespace cv;
using namespace std;
int main(int argc, char **argv)
{
    cv::Mat img = cv::imread("/Users/chen/github/opencv/tracking/opencv_tracking/bus.png");
    cv::imshow("Input Image", img);
    
    cv::Mat hsv_img, frame, roi, hsv_roi, mask, hsv_inRange;
    cvtColor(img, hsv_img, COLOR_BGR2HSV);
    cv::imshow("HSV Image", hsv_img);
    
    cv::Rect track_window(0, 0, 0, 0);
    
    //set the callback function for any mouse event
    cv::setMouseCallback("Input Image", CallBackFunc, &track_window);
    
    
    while(true){
        
        //if (track_window.width != 0)
        //    std::cout <<track_window.x<<","<<track_window.y<<","<<track_window.width<<","<<track_window.height<<std::endl;
        
        cv::Vec3b hsvPixel(hsv_img.at<Vec3b>(track_window.y + 0.5*track_window.height, track_window.x + 0.5*track_window.width)); // it seems x and y need to be swapped.
        int thresh = 40;

        cv::Scalar minHSV = cv::Scalar(hsvPixel.val[0] - thresh, hsvPixel.val[1] - thresh, hsvPixel.val[2] - thresh);
        cv::Scalar maxHSV = cv::Scalar(hsvPixel.val[0] + thresh, hsvPixel.val[1] + thresh, hsvPixel.val[2] + thresh);
        
        cv::inRange(hsv_img, minHSV, maxHSV, hsv_inRange);
        cv::imshow("HSV_inRange", hsv_inRange);
        //cv::bitwise_and(brightHSV, brightHSV, resultHSV, maskHSV);

        
        int keyboard = waitKey(300);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }

    
    /*
    const string about =
        "This sample demonstrates the camshift algorithm.\n"
        "The example file can be downloaded from:\n"
        "  https://www.bogotobogo.com/python/OpenCV_Python/images/mean_shift_tracking/slow_traffic_small.mp4";
    const string keys =
        "{ h help |      | print this help message }"
        "{ @image |<none>| path to image file }";
    CommandLineParser parser(argc, argv, keys);
    parser.about(about);
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    string filename = parser.get<string>("@image");
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }
    VideoCapture capture(filename);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }
    Mat frame, roi, hsv_roi, mask;
    // take first frame of the video
    capture >> frame;
    // setup initial location of window
    Rect track_window(300, 200, 100, 50); // simply hardcoded the values
    //Rect track_window(150, 180, 50, 100); // simply hardcoded the values
    // set up the ROI for tracking
    roi = frame(track_window);
    cvtColor(roi, hsv_roi, COLOR_BGR2HSV);
    inRange(hsv_roi, Scalar(0, 60, 32), Scalar(180, 255, 255), mask);
    imshow("hsv_roi", hsv_roi);
    imshow("mask", mask);
    
    float range_[] = {0, 180};
    const float* range[] = {range_};
    Mat roi_hist;
    int histSize[] = {180};
    int channels[] = {0};
    calcHist(&hsv_roi, 1, channels, mask, roi_hist, 1, histSize, range);
    normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);
    // Setup the termination criteria, either 10 iteration or move by atleast 1 pt
    TermCriteria term_crit(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
    while(true){
        Mat hsv, dst;
        capture >> frame;
        if (frame.empty())
            break;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        calcBackProject(&hsv, 1, channels, roi_hist, dst, range);
        imshow("dst", dst);
        // apply camshift to get the new location
        RotatedRect rot_rect = CamShift(dst, track_window, term_crit);
        // Draw it on image
        Point2f points[4];
        rot_rect.points(points);
        for (int i = 0; i < 4; i++)
            line(frame, points[i], points[(i+1)%4], 255, 2);
        imshow("img2", frame);
        int keyboard = waitKey(3);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }
     */
}
