#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;


int createBoard(int ids[], cv::aruco::PREDEFINED_DICTIONARY_NAME dictionaryType, cv::Ptr<aruco::GridBoard> outputBoard) {

    cv::Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(dictionaryType);

    cv::Ptr<aruco::GridBoard> board = aruco::GridBoard::create(4, 1, 0.04, 0.01, dictionary);

    Mat boardImage;

    std::vector<int> idsVector = std::vector<int>();
    ostringstream os;

    for (int i = 0; i < 4 ; i++){
        idsVector.push_back(ids[i]);
        os << ids[i];
    }
    board.operator*().ids = idsVector;
    board.operator*().draw( cv::Size(600, 500), boardImage, 10, 1 );

    imwrite("arucoBoard" + os.str() + ".png", boardImage);
    outputBoard = board;
    return 0;
}

int main() {


    cv::VideoCapture stream1(0);
    if (!stream1.isOpened()) { //check if video device has been initialised
        std::cout << "cannot open camera";
    }
    cv::Ptr<aruco::GridBoard> board;
    int idsArray[] = {21,10,5,3};
    createBoard(idsArray, cv::aruco::DICT_4X4_50, board);

    cv::Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    cv::Mat cameraMatrix, distCoeffs;
    FileStorage fs;
    fs.open("calibrate_camera.yml", FileStorage::READ);

    cameraMatrix = fs["camera_matrix"].mat();
    distCoeffs = fs["distortion_coefficients"].mat();


//unconditional loop
    while (true) {
        cv::Mat cameraFrame, copyOfCameraFrame;
        stream1.read(cameraFrame);
        cameraFrame.copyTo(copyOfCameraFrame);



        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f> > corners;

        cv::aruco::detectMarkers(cameraFrame, dictionary, corners, ids);
        std::cout << ids.size() << std::endl;
        // if at least one marker detected
        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(copyOfCameraFrame, corners, ids);
            cv::Vec3d rvec, tvec;
            int valid = estimatePoseBoard(corners, ids, board, cameraMatrix, distCoeffs, rvec, tvec);
            // if at least one board marker detected
            if(valid > 0)
                cv::aruco::drawAxis(copyOfCameraFrame, cameraMatrix, distCoeffs, rvec, tvec, 0.1);
        }



        imshow("cam", copyOfCameraFrame);

        if ((cv::waitKey(1) & 0xEFFFFF) == 27)
            break;
    }
    return 0;
}



