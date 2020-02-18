//
// Created by Jonathan Hirsch on 2/17/20.
//
#include "FlirCamera.h"
#include <map>
#ifndef SKYPASTA_CONTINUOUSACQUISITION_H
#define SKYPASTA_CONTINUOUSACQUISITION_H

enum TriggerMode{
    CONTINUOUS,
    SINGLE_FRAME
    //MULTI_FRAME
};

class ImageRetriever {
public:
    ImageRetriever(CameraPtr cameraPtr);
    void startAcquisition();
    int stopAcquisition();
    void releaseCamera(){cameraPtr = nullptr;};
    void triggerCameraOnce();

    void setTriggerMode(TriggerMode triggerMode);
    void setContinuousRate(int continousRate){ this -> continousRate = continousRate;};
    int getContinuousRate(){return continousRate;}
    bool isRunning(){return running;};

private:
    void configureImageRetriever();
    void acquireImagesContinuous(INodeMap& nodeMap);
    void triggerImageRetrieval(INodeMap& nodeMap);
    void acquireImage(INodeMap& nodeMap);


    CameraPtr cameraPtr = nullptr;
    TriggerMode currentTriggerMode = CONTINUOUS;
    FILE* file;
    map<TriggerMode, string> triggerModeMap;

    int continousRate = 1;
    bool running = false;
    bool stopFlag = false;
};


#endif
