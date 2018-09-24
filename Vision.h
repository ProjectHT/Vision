/*************************************************************************************************/
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/*************************************************************************************************/
/* 
 * File:   Vision.h
 * Author: Thong_Laptop
 *
 * Created on September 19, 2018, 10:35 AM
 */
/*************************************************************************************************/
#ifndef VISION_H
#define VISION_H
/*************************************************************************************************/
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
/*************************************************************************************************/
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libswscale/swscale.h>
    #include <libavutil/time.h>
}
/*************************************************************************************************/
#include "../Log/Log.h"
#include "../Http/Http.h"
/*************************************************************************************************/
using namespace std;
using namespace cv;
/*************************************************************************************************/
#ifndef M_DEBUG
#define M_DEBUG
#endif
/*************************************************************************************************/
void initVision();
/*************************************************************************************************/
#ifndef FPS_CAMERA
    #define FPS_CAMERA  30
#endif
#ifndef LIMIT_FRAME_CAMERA
    //#define LIMIT_FRAME_CAMERA    54000
    #define LIMIT_FRAME_CAMERA  27000
#endif
#ifndef NUM_FRAME_BACKGROUND
    #define NUM_FRAME_BACKGROUND  20
#endif
/*************************************************************************************************/
string getDayTime();
/*************************************************************************************************/
class Interface {
public:
    /*********************************************************************************************/
    Interface();
    virtual ~Interface();
    /*********************************************************************************************/
    void init(string t_name, string t_link, string t_path);
    void setDetectPerson(bool flag);
    void setDetectFace(bool flag);
    void setSaveClip(bool flag);
    void setState(bool flag);
    void setNameFileClip(string name);
    bool getState();
    bool getStateDetectPerson();
    bool getStateDetectFace();
    bool getSaveClip();
    string getNameFileClip();
    /*********************************************************************************************/
    string getName();
    string getLink();
    string getPath();
    /*********************************************************************************************/
    void setrun();
    void setstop();
    /*********************************************************************************************/
    Mat getBuffer();
    void setBuffer(Mat& src);
    /*********************************************************************************************/
    void beginUpdateStatus();
    /*********************************************************************************************/
    void endUpdateStatus();
    /*********************************************************************************************/
private:
    /*********************************************************************************************/
    string name;
    string link;
    string path;
    /*********************************************************************************************/
    bool flag_run;
    /*********************************************************************************************/
    bool flag_data_ready;
    Mat src_img;
    /*********************************************************************************************/
    bool flag_detect_person;
    /*********************************************************************************************/
    bool flag_detect_face;
    /*********************************************************************************************/
    bool flag_save_clip;
    string name_file_save_clip;
    /*********************************************************************************************/
    pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
    /*********************************************************************************************/
    pthread_t camera;
    pthread_t vision;
    /*********************************************************************************************/
};
/*************************************************************************************************/
class Camera {
public:
    /*********************************************************************************************/
    Camera(Interface* interface);
    virtual ~Camera();
    /*********************************************************************************************/
    bool run();
    /*********************************************************************************************/
private:
    /*********************************************************************************************/
    Interface* p_interface;
    /*********************************************************************************************/
    void load();
    bool initCamera(AVFormatContext** inputFormatContext, int& index_stream, AVCodecContext** inputCodec);
    void initConvertToImage(SwsContext** img_convert, uint8_t** input, uint8_t** ouput, AVCodecContext* codec, AVFrame** frameYUV, AVFrame** frameBGR);
    /*********************************************************************************************/
};
/*************************************************************************************************/
class WriteClip {
public:
    /*********************************************************************************************/
    WriteClip(string path, string name, AVFormatContext* inputFormatContext);
    ~WriteClip();
    /*********************************************************************************************/
    bool writeFile(AVPacket* data);
    void closeFile();
    /*********************************************************************************************/
    bool getWritable();
    long getNumFrame();
    string getLink();
    string getName();
    void stopWrite();
    /*********************************************************************************************/
private:
    /*********************************************************************************************/
    string path;
    string name;
    bool flag_writable;
    long num_frame;
    AVFormatContext* format_context;
    /*********************************************************************************************/
    void initSystem();
    bool createNewFile(AVFormatContext* inputFormatContext);
    bool initSaveFile(AVFormatContext* inputFormatContext);
    bool tranferCodec(AVFormatContext* inputFormatContext);
    bool createFile();
    bool writeHeaderFile();
};
/*************************************************************************************************/
class Vision {
public:
    /*********************************************************************************************/
    Vision(Interface* interface);
    virtual ~Vision();
    /*********************************************************************************************/
    bool run();
    /*********************************************************************************************/
private:
    /*********************************************************************************************/
    Interface* p_interface;
    /*********************************************************************************************/
    string writePhoto(Mat& src);
    /*********************************************************************************************/
    bool detectFace(CascadeClassifier &detector, Mat &src, double scale);
};
/*************************************************************************************************/
#endif /* VISION_H */
/*************************************************************************************************/
