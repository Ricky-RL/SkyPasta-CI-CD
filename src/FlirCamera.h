#ifndef FLIR_FLIRCAMERA_H
#define FLIR_FLIRCAMERA_H

#include <iostream>
#include "Spinnaker.h"

#pragma once

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

/*
 * Currently this class works only if there is one object of this class in memory.
 * If we are going to go through we this, we might want to make this into a singleton class.
 */

/*
 * The FlirCamera class should handle most, if not all the communication with the camera.
 * cleanExit() should be called when done with the camera
 */
class FlirCamera
{
public:
    FlirCamera();
    bool initialize(int retryCountMax = 5);
    void resetCamera(bool resetSettings = false);
    int cleanExit();

    // Camera setup and status
    int getNumCameras() const;
    CameraPtr& getCamera() {return this->cameraPtr;}
    bool getStatus() {return this->status;}

    // User set (setting saving/loading)
    bool saveUserSet(std::string userSet);
    bool loadUserSet(std::string userSet);

    // Camera setting setting
    void setDefaultSettings(std::string acqMode = "Continous",
        std::string trigType = "FrameStart",
        std::string trigSrc = "Software",
        std::string trigMode = "On");
    void setAcquisitionMode(std::string selectedMode);
    void setTriggerType(std::string triggerTypeToSet);
    void setTriggerSource(std::string triggerSourceToSet);
    void setTriggerMode(std::string triggerModeToSet);

    // Image Capture
    void startCapture();
    void stopCapture();
    bool getImage(ImagePtr *imagePtr, int *timestamp);

private:
    void findEpochOffset();
    bool selectUserSet(std::string userSet);
    void sleepWrapper(int milliseconds);

    // The system pointer that we grab the camera list from
    SystemPtr system;

    // The list of cameras attached to the system
    CameraList cameraList;

    // Contains the pointer to the camera
    CameraPtr cameraPtr;

    // Number of cameras attached to the system
    int numberOfCameras = 0;

    // Offset we apply to image timestamps to convert them to epoch
    int cameraTimestampEpochOffset_ms = 0;

    // Whether or not the camera is connected
    bool status = false;
};
#endif //FLIR_FLIRCAMERA_H
