#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <mosquittopp.h>

using namespace std;
using namespace cv;

void connect_callback(struct mosquitto *client, void *obj, int result)
{
    printf("connect callback, rc=%d\n", result);
}

void logging(struct mosquitto *client, void *obj, int result, const char *logline)
{
    std::cout << logline << std::endl;
}

void processImageAndPublishResult(cv::VideoCapture stream1, Ptr<aruco::Dictionary> dictionary, mosquitto *client, string cameraId) {


    //unconditional loop
    while (true) {


        cv::Mat cameraFrame, copyOfCameraFrame;
        stream1.read(cameraFrame);
        cameraFrame.copyTo(copyOfCameraFrame);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f> > corners;

        cv::aruco::detectMarkers(cameraFrame, dictionary, corners, ids);
        //std::cout << ids.size() << std::endl;
        cv::aruco::drawDetectedMarkers(copyOfCameraFrame, corners, ids);

        if (ids.size() > 0) {
            for (int i = 0; i < ids.size(); i++) {
                //std::cout<< "[Info] [Marker] Position: ("<< ids[i]<<")"<<std::endl;
                std::ostringstream messageStream;
                messageStream << "{ \"markerId\": "<< ids[i] <<"}";
                std::string message(messageStream.str());
                const char *char_message = message.c_str();
                //SEND MQTT MESSAGE
                ostringstream path;
                path << "telemetry/t429df05af75246fb971a493a22aa85a8/" << cameraId;
                mosquitto_publish(client, nullptr, path.str().c_str(), strlen(char_message), char_message, 0, false);
                std::cout << message << std::endl;
            }
        }

        imshow("cam", copyOfCameraFrame);

        if ((cv::waitKey(1) & 0xEFFFFF) == 27)
            break;
    }
}

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

void readCameraId(cv::VideoCapture stream1, Ptr<aruco::Dictionary> dictionary, string &outputCameraId){

    cv::Ptr<aruco::GridBoard> board = aruco::GridBoard::create(4, 1, 0.04, 0.01, dictionary);

    cv::Mat cameraMatrix, distCoeffs;
    FileStorage fs;
    fs.open("calibrate_camera.yml", FileStorage::READ);

    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;

    distCoeffs = (Mat) distCoeffs;
    cameraMatrix = (Mat) cameraMatrix;

    fs.release();

    int loopCounter = 0;
    string returnString = "";

    while(loopCounter < 100 && returnString == ""){

        cv::Mat cameraFrame, copyOfCameraFrame;

        cv::Vec3d rvec, tvec;
        stream1.read(cameraFrame);
        cameraFrame.copyTo(copyOfCameraFrame);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f> > corners;

        cv::aruco::detectMarkers(cameraFrame, dictionary, corners, ids);
        cv::aruco::drawDetectedMarkers(copyOfCameraFrame, corners, ids);
        int valid = cv::aruco::estimatePoseBoard(corners, ids, board, cameraMatrix, distCoeffs, rvec, tvec);
        if(valid && board.operator*().ids.size() > 3){
            cv::aruco::drawAxis(copyOfCameraFrame, cameraMatrix, distCoeffs, rvec, tvec, 0.3);
            stringstream ss;
            ss << ids[0] << ids[1] << ids[2] << ids[3];
            returnString = ss.str().c_str();
            outputCameraId = returnString;
        }
        loopCounter++;
    }



}

int main(int argc, char *argv[]) {

    mosquitto_lib_init();

    cv::VideoCapture stream1(0);
    if (!stream1.isOpened()) { //check if video device has been initialised
        std::cout << "cannot open camera";
    }

    cv::Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    string cameraId = "Android1";

    readCameraId(stream1, dictionary, cameraId);
    std::cout << "camera id: " << cameraId << std::endl;

    mosquitto *client = mosquitto_new(cameraId.c_str(), false, 0);
    mosquitto_connect_callback_set(client, connect_callback);
    mosquitto_log_callback_set(client, logging);

    mosquitto_loop_start(client);
    mosquitto_tls_set(client, "iothub.crt", nullptr, nullptr, nullptr, 0);

    stringstream username;
    username << cameraId << "@t429df05af75246fb971a493a22aa85a8";

    mosquitto_username_pw_set(client, username.str().c_str(), "Password");
    mosquitto_connect(client, "mqtt.bosch-iot-hub.com", 8883, 5);
    mosquitto_disconnect(client);
    int connectSuccess = mosquitto_connect(client, "mqtt.bosch-iot-hub.com", 8883, 5);
    std::cout << "Connect: "<< connectSuccess << std::endl;


    cv::Ptr<aruco::GridBoard> board;
    int idsArray[] = {21,10,5,3};
    createBoard(idsArray, cv::aruco::DICT_4X4_50, board);

    processImageAndPublishResult(stream1, dictionary, client, cameraId);
    return 0;
}



