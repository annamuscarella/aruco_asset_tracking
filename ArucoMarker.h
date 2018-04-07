//
// Created by Anna Bendel on 03.04.18.
//
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;

#ifndef ASSETTRACKING_ARUCOMARKER_H
#define ASSETTRACKING_ARUCOMARKER_H


class ArucoMarker {
int markerId;
std::vector<Point2f> markerCorners;
};


#endif //ASSETTRACKING_ARUCOMARKER_H
