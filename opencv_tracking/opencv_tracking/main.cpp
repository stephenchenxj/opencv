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



using namespace cv;
using namespace std;
int main(int argc, char **argv)
{
    string filename = "/Users/chen/github/opencv/tracking/opencv_tracking/slow_traffic_small.mp4";
    //cv::VideoCapture capture(filename);
    cv::VideoCapture capture(0);
    cv::Mat img;
    capture >> img;
    cv::imshow("Input Image", img);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }
    //cv::Mat frame, roi, hsv_roi, mask;
    cv::Mat hsv_img, frame, roi, hsv_roi, mask, hsv_inRange;
    cv::Mat hsv, dst;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    //cv::imshow("HSV Image", hsv);
    
    cv::Rect track_window(300, 200, 100, 50);
    cv::Rect init_track(0, 0, 0, 0);
    
    //set the callback function for any mouse event
    cv::setMouseCallback("Input Image", CallBackFunc, &init_track);
    
    //roi = frame(track_window);
    roi = img(track_window);
    cv::cvtColor(roi, hsv_roi, COLOR_BGR2HSV);
    inRange(hsv_roi, Scalar(0, 60, 32), Scalar(180, 255, 255), mask);
    //imshow("hsv_roi", hsv_roi);
    //imshow("mask", mask);
    float range_[] = {0, 180};
    const float* range[] = {range_};
    Mat roi_hist;
    int histSize[] = {180};
    int channels[] = {0};
    calcHist(&hsv_roi, 1, channels, mask, roi_hist, 1, histSize, range);
    normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);
    // Setup the termination criteria, either 10 iteration or move by atleast 1 pt
    TermCriteria term_crit(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
    Point2f points[4];
    cv::Scalar minHSV;
    cv::Scalar maxHSV;
    while(true){
        capture >> img;
        if(init_track.width != 0) { // init track_window only when mouse left button is clicked and released.
            track_window = init_track;
            init_track = {0,0,0,0}; //clear init_track after copy it to track_window.
            roi = img(track_window);
            cv::cvtColor(roi, hsv_roi, COLOR_BGR2HSV);
            int thresh = 40;
            cv::Vec3b hsvPixel(hsv.at<Vec3b>(track_window.y + 0.5*track_window.height, track_window.x + 0.5*track_window.width)); // it seems x and y need to be swapped.
            minHSV = cv::Scalar(hsvPixel.val[0] - thresh, hsvPixel.val[1] - thresh, hsvPixel.val[2] - thresh);
            maxHSV = cv::Scalar(hsvPixel.val[0] + thresh, hsvPixel.val[1] + thresh, hsvPixel.val[2] + thresh);
            std::cout<< minHSV[0];
        }
        if(track_window.width != 0){
            roi = img(track_window);
            cv::cvtColor(roi, hsv_roi, COLOR_BGR2HSV);
            //inRange(hsv_roi, Scalar(0, 60, 32), Scalar(180, 255, 255), mask);
            inRange(hsv_roi, minHSV, maxHSV, mask);
            //imshow("hsv_roi", hsv_roi);
            //imshow("mask", mask);
            calcHist(&hsv_roi, 1, channels, mask, roi_hist, 1, histSize, range);
            normalize(roi_hist, roi_hist, 0, 255, NORM_MINMAX);
            // Setup the termination criteria, either 10 iteration or move by atleast 1 pt
            TermCriteria term_crit(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
            cv::cvtColor(img, hsv, COLOR_BGR2HSV);
            cv::calcBackProject(&hsv, 1, channels, roi_hist, dst, range);
            
            
            // option 1: apply camshift to get the new location
            cv::RotatedRect rot_rect = CamShift(dst, track_window, term_crit);
            // Draw it on image
            cv::Point2f points[4];
            rot_rect.points(points);
            for (int i = 0; i < 4; i++)
                line(img, points[i], points[(i+1)%4], 255, 2);
            // option2: meanshift
            //cv::meanShift(dst, track_window, term_crit);
            //cv::rectangle(img, track_window, 255, 2);
            
            imshow("Input Image", img);
            //cv::inRange(hsv, minHSV, maxHSV, hsv_inRange);
            //cv::imshow("HSV_inRange", hsv_inRange);
        //cv::bitwise_and(brightHSV, brightHSV, resultHSV, maskHSV);
        }

        
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }
    
    cv::destroyAllWindows();
    capture.release();
    return 0;

}
