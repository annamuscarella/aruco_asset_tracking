#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;

int main() {


    cv::VideoCapture stream1(0);
    if (!stream1.isOpened()) { //check if video device has been initialised
        std::cout << "cannot open camera";
    }


    cv::Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);


//unconditional loop
    while (true) {
        cv::Mat cameraFrame, frame_gray;
        stream1.read(cameraFrame);

            std::vector<int> ids;
        std::vector<std::vector<cv::Point2f> > corners;
        cv::aruco::detectMarkers(cameraFrame, dictionary, corners, ids);
        std::cout << ids.size() << std::endl;
        // if at least one marker detected
        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(cameraFrame, corners, ids);
        }

        imshow("cam", cameraFrame);

        if ((cv::waitKey(1) & 0xEFFFFF) == 27)
            break;
    }
    return 0;
}
