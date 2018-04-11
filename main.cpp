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

void processImageAndPublishResult(cv::VideoCapture stream1, Ptr<aruco::Dictionary> dictionary, mosquitto *client) {

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
                mosquitto_publish(client, nullptr, "telemetry/t429df05af75246fb971a493a22aa85a8/Android1", strlen(char_message), char_message, 0, false);
                std::cout << message << std::endl;
            }
        }

        imshow("cam", copyOfCameraFrame);

        if ((cv::waitKey(1) & 0xEFFFFF) == 27)
            break;
    }
}

int main(int argc, char *argv[]) {

    mosquitto_lib_init();

    cv::VideoCapture stream1(0);
    if (!stream1.isOpened()) { //check if video device has been initialised
        std::cout << "cannot open camera";
    }

    cv::Ptr<aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    mosquitto *client = mosquitto_new("Android1", false, 0);
    mosquitto_connect_callback_set(client, connect_callback);
    mosquitto_log_callback_set(client, logging);

    mosquitto_loop_start(client);
    int tlsSuccess = mosquitto_tls_set(client, "iothub.crt", nullptr, nullptr, nullptr, 0);
    //int tlsSuccess = mosquitto_tls_insecure_set(client, true);
    std::cout << tlsSuccess << std::endl;
    // mosquitto_tls_set(client, "iothub.crt",NULL, NULL, NULL, NULL);
    //mosquitto_tls_insecure_set(client, 1);
    //mosquitto_tls_opts_set(client, 0, NULL, NULL);


    mosquitto_username_pw_set(client, "Android1@t429df05af75246fb971a493a22aa85a8", "Password");
    mosquitto_connect(client, "mqtt.bosch-iot-hub.com", 8883, 5);
    mosquitto_disconnect(client);
    int connectSuccess = mosquitto_connect(client, "mqtt.bosch-iot-hub.com", 8883, 5);
    std::cout << "Connect: "<< connectSuccess << std::endl;

    //const char *char_message = "{ msg: \"test\" }";
    //SEND MQTT MESSAGE
    //int msgId;
    //int result = mosquitto_publish(client, &msgId, "telemetry", strlen(char_message), char_message, 0, false);
    //std::cout << "Publish: " << result << " MsgId: " <<  msgId << std::endl;


    processImageAndPublishResult(stream1, dictionary, client);
    return 0;
}



