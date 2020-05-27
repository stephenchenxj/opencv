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

int main(int argc, const char * argv[]) {
    
    cv::VideoCapture stream1(0);
    if (!stream1.isOpened()) { //check if video device has been initialised
    std::cout << "cannot open camera";
    }
    //unconditional loop
    while (true) {
    cv::Mat cameraFrame;
    stream1.read(cameraFrame);
    imshow("cam", cameraFrame);
    if (cv::waitKey(30) >= 0)
    break;
    }
    
    return 0;
}
