#include <iostream>
#include <unistd.h>
#include <future>
#include "ImageRetriever.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "Telemetry.h"


constexpr int PORT = 5000;
constexpr auto ADDRESS  = "127.0.0.1";
constexpr int RATE = 2; // delay between each image acquisition trigger


using namespace std;

bool stopFlag = false;

void acquireImagesFixedRate(int rate, ImageRetriever *imageRetriever){
    imageRetriever->startAcquisition();

    while (!stopFlag){
        cout << "acquiring........." << endl;
        auto triggerCameraOnceFuture(async(launch::async, &ImageRetriever::acquireImage, imageRetriever));
        if (stopFlag){
            break;
        }
        sleep(rate);

        cout << endl;
    }

    imageRetriever->stopAcquisition();
}

// Reads Telemetry data from socket, exits on error from telemetry.
void readFromSocket(Telemetry *telemetry){
    int reconnectAttempts = 0;
//    if (!telemetry->isConnected()){
//        cout << "Telemetry error: Not connected" << endl;
//        return;
//    }
    cout << "Here:" << endl;
    while (!stopFlag){
        if (telemetry->readData() == -1 || !telemetry->isConnected()){
            cout << "Error with server, attempting to reconnect..." << endl;

            telemetry->connectServer();

            if (telemetry->isConnected()){
                reconnectAttempts = 0;
                continue;
            }
            //reconnectAttempts ++;
            if (reconnectAttempts > 5 || stopFlag){
                return;
            }
            sleep(3);
        }
        if (stopFlag){
            return;
        }
        cout << endl;
    }
}

void tagImages(ImageTag *imageTag){
    while(!stopFlag) {
       imageTag->processNextImage();
    }
}

// In milliseconds
void sleepWrapper(int milliseconds)
{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    Sleep(milliseconds);
#else
    usleep(1000 * milliseconds);
#endif
}


int main() {

    try {
        cout << "SkyPasta is booting up..." << endl;

        ImageTag imageTag;

        cout << "Camera setup starting" << endl;
        FlirCamera flirCamera;
        flirCamera.setDefaultSettings();
        cout << "Camera setup complete" << endl;

        cout << "ImageRetriever setup starting" << endl;
        ImageRetriever imageRetriever(&imageTag, CameraType::FLIR, &flirCamera);
        cout << "ImageRetriever setup complete" << endl;

        cout << "Telemetry setup starting" << endl;
        Telemetry telemetry(ADDRESS,PORT, &imageTag);
        telemetry.connectServer();
        cout << "Telemetry setup complete" << endl;

        auto acquireImagesFixedRateFuture(async(launch::async, acquireImagesFixedRate, RATE, &imageRetriever));
        auto readFromSocketFuture(async(launch::async, readFromSocket, &telemetry));
        auto processNextImageFuture(async(launch::async, tagImages, &imageTag));

        cout << "Boot up complete..." << endl;

        sleepWrapper(50000);
        stopFlag = true;

        acquireImagesFixedRateFuture.get();

        imageRetriever.releaseCamera();
    }
    catch (const Exception& e){
        cout << "Error in main:  ";
        cout << e.what() << endl;
    }
}

