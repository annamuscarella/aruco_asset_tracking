//
// Created by Anna Bendel on 03.04.18.
//
#include <iostream>
#include <opencv/cv.hpp>
#include <aruco/aruco.h>
using namespace cv;

#ifndef ASSETTRACKING_ARUCOTRACKER_H
#define ASSETTRACKING_ARUCOTRACKER_H



class ArucoTracker {

    aruco::Dictionary dictionary;
    int initialize();
};


#endif //ASSETTRACKING_ARUCOTRACKER_H
