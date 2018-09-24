/*************************************************************************************************/
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/*************************************************************************************************/
/* 
 * File:   Vision.cpp
 * Author: Thong_Laptop
 * 
 * Created on September 19, 2018, 10:35 AM
 */
/*************************************************************************************************/
#include "Vision.h"
/*************************************************************************************************/
static void* THREADCAMERA(void* parameter) {
    Interface* p_interface = (Interface*)parameter;
    Camera* p_camera = new Camera(p_interface);
    if(p_camera->run()) {
        pthread_exit(0);
    } else {
        pthread_exit(0);
    }
}
/*************************************************************************************************/
static void* THREADVISION(void* parameter) {
    Interface* p_interface = (Interface*)parameter;
    Vision *p_vision = new Vision(p_interface);
    if(p_vision->run()) {
        pthread_exit(0);
    } else {
        pthread_exit(0);
    }
}
/*************************************************************************************************/
void initVision() {
    av_register_all();
    avcodec_register_all();
    avformat_network_init();
}
/*************************************************************************************************/
string getDayTime() {
    timeval tp;
    gettimeofday(&tp, 0);
    time_t nowtime;
    nowtime = tp.tv_sec;
    struct tm *nowtm;
    nowtm = localtime(&nowtime);
    string m_return = std::to_string(nowtm->tm_mday) + "-";
    m_return += std::to_string(nowtm->tm_mon) + "-";
    m_return += std::to_string(nowtm->tm_year + 1900) + "_";
    m_return += std::to_string(nowtm->tm_hour) + "-";
    m_return += std::to_string(nowtm->tm_min) + "-";
    m_return += std::to_string(nowtm->tm_sec);
    return m_return;
}
/*************************************************************************************************/
Interface::Interface() {
    this->flag_data_ready = false;
    this->flag_detect_face = false;
    this->flag_detect_person = false;
    this->flag_save_clip = false;
    this->link = "";
    this->name = "";
    this->path = "";
#ifdef M_DEBUG
    my_log.write("Vision->Interface", "created");
#endif
}
/*************************************************************************************************/
Interface::~Interface() {
    
}
/*************************************************************************************************/
void Interface::init(string t_name, string t_link, string t_path) {
    this->name = t_name;
    this->link = t_link;
    this->path = t_path;
#ifdef M_DEBUG
    string t_str = "Vision->Interface[" + this->name + "]";
    my_log.write(t_str, "inited");
#endif
}
/*************************************************************************************************/
void Interface::setDetectPerson(bool flag) {
    this->flag_detect_person = flag;
#ifdef M_DEBUG
    string t_str = "Vision->Interface[" + this->name + "]";
    if(this->flag_detect_person) {
        my_log.write(t_str, "detect person");
    } else {
        my_log.write(t_str, "don't detect person");
    }
#endif
}
/*************************************************************************************************/
void Interface::setDetectFace(bool flag) {
    this->flag_detect_face = flag;
#ifdef M_DEBUG
    string t_str = "Vision->Interface[" + this->name + "]";
    if(this->flag_detect_person) {
        my_log.write(t_str, "detect face");
    } else {
        my_log.write(t_str, "don't detect face");
    }
#endif
}
/*************************************************************************************************/
void Interface::setSaveClip(bool flag) {
    this->flag_save_clip = flag;
}
/*************************************************************************************************/
void Interface::setState(bool flag) {
    this->flag_run = true;
}
/*************************************************************************************************/
bool Interface::getState() {
    return this->flag_run;
}
/*************************************************************************************************/
bool Interface::getStateDetectPerson() {
    return this->flag_detect_person;
}
/*************************************************************************************************/
bool Interface::getStateDetectFace() {
    return this->flag_detect_face;
}
/*************************************************************************************************/
Mat Interface::getBuffer() {
    Mat buffer;
    pthread_mutex_lock( &m_mutex );
    if(this->flag_data_ready) {
        buffer = this->src_img.clone();
    } 
    pthread_mutex_unlock( &m_mutex );
    return buffer.clone();
}
/*************************************************************************************************/
void Interface::setBuffer(Mat& src) {
    pthread_mutex_lock( &m_mutex );
    this->flag_data_ready = true;
    this->src_img = src.clone();
    pthread_mutex_unlock( &m_mutex );
}
/*************************************************************************************************/
bool Interface::getSaveClip() {
   return this->flag_save_clip; 
}
/*************************************************************************************************/
void Interface::setNameFileClip(string name) {
    this->name_file_save_clip = name;
}
/*************************************************************************************************/
string Interface::getNameFileClip() {
    return this->name_file_save_clip;
}
/*************************************************************************************************/
void Interface::beginUpdateStatus() {
    pthread_mutex_lock( &m_mutex );
}
/*************************************************************************************************/
void Interface::endUpdateStatus() {
    pthread_mutex_unlock( &m_mutex );
}
/*************************************************************************************************/
void Interface::setrun() {
    //pthread_t camera
    int rc;
    rc = pthread_create(&camera, NULL, THREADCAMERA, (void *)this);
    rc = pthread_create(&vision, NULL, THREADVISION, (void *)this);
    this->flag_run = true;
}
/*************************************************************************************************/
void Interface::setstop() {
    this->flag_run = false;
    sleep(5);
}
/*************************************************************************************************/
string Interface::getName() {
    return this->name;
}
/*************************************************************************************************/
string Interface::getLink() {
    return this->link;
}
/*************************************************************************************************/
string Interface::getPath() {
    return this->path;
}
/*************************************************************************************************/
// End class Interface
/*************************************************************************************************/
// Begin class Camera
/*************************************************************************************************/
Camera::Camera(Interface* interface) {
    this->p_interface = interface;
}
/*************************************************************************************************/
void Camera::load() {
    
}
/*************************************************************************************************/
bool Camera::initCamera(AVFormatContext** inputFormatContext, int& index_stream, AVCodecContext** inputCodec) {
#ifdef M_DEBUG
    string t_str;
#endif   
    string link_camera = this->p_interface->getLink();
    AVDictionary *opts = 0;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    if (avformat_open_input(inputFormatContext, link_camera.c_str(), NULL, &opts) != 0) {
        printf("|Camera| : Couldn't open input!\n");
#ifdef M_DEBUG
        t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
        my_log.write(t_str, "couldn't open input");
#endif
        return false;
    }
#ifdef M_DEBUG
    t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "opened input");
#endif
        
    if (avformat_find_stream_info(*inputFormatContext, NULL)<0) {
        printf("|Camera| : Couldn't find stream information!\n");
#ifdef M_DEBUG
        string t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
        my_log.write(t_str, "couldn't find stream information");
#endif
        return false;
    }
#ifdef M_DEBUG
    t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "finded stream information");
#endif

    index_stream = -1;
    
    for (int i = 0; i < (*inputFormatContext)->nb_streams; i++) {
        if ((*inputFormatContext)->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            index_stream = i;
            break;
        }
    }

    if (index_stream == -1) {
        printf("|Camera| : Didn't find a video stream!\n");
#ifdef M_DEBUG
        string t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
        my_log.write(t_str, "couldn't find a video stream");
#endif
        return false;
    }
#ifdef M_DEBUG
    t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "finded a video stream");
#endif
    
    av_read_play((*inputFormatContext));
    av_dump_format(*inputFormatContext, index_stream, link_camera.c_str(), 0);
    
    *inputCodec = (*inputFormatContext)->streams[index_stream]->codec;
    
    AVCodec * pCodec = avcodec_find_decoder((*inputCodec)->codec_id);
    if (pCodec == NULL) {
        printf("|Camera| : Didn't find codec!\n");
#ifdef M_DEBUG
        t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
        my_log.write(t_str, "couldn't find codec");
#endif
        return false;
    }
#ifdef M_DEBUG
    t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "finded codec");
#endif
    
    if (avcodec_open2(*inputCodec, pCodec, NULL) < 0) {
        printf("|Camera| : Didn't load codec!\n");
#ifdef M_DEBUG
        string t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
        my_log.write(t_str, "couldn't load codec");
#endif
        return false;
    }
#ifdef M_DEBUG
    t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "loaded codec");
#endif

    return true;
}
/*************************************************************************************************/
void Camera::initConvertToImage(SwsContext** img_convert, uint8_t** input, uint8_t** ouput, AVCodecContext* codec, AVFrame** frameYUV, AVFrame** frameBGR) {
#ifdef M_DEBUG
    string t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "frame size (" + to_string(codec->width) + "x" + to_string(codec->height));
#endif
    cout << codec->width << "x" << codec->height << endl;
    *img_convert = sws_getContext(codec->width, codec->height, codec->pix_fmt, codec->width, codec->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
    int size1 = avpicture_get_size(AV_PIX_FMT_YUV420P, codec->width, codec->height);
    int size2 = avpicture_get_size(AV_PIX_FMT_BGR24, codec->width, codec->height);
    *input = (uint8_t*)(av_malloc(size1));
    *ouput = (uint8_t*)(av_malloc(size2));
    *frameYUV = av_frame_alloc();
    *frameBGR = av_frame_alloc();
    avpicture_fill((AVPicture *)*frameYUV, *input, AV_PIX_FMT_YUV420P, codec->width, codec->height);
    avpicture_fill((AVPicture *)*frameBGR, *ouput, AV_PIX_FMT_BGR24, codec->width, codec->height);
}
/*************************************************************************************************/
Camera::~Camera() {
    if(this->p_interface->getState()) {
        this->p_interface->setState(false);
        sleep(1);
    }
#ifdef M_DEBUG
    string t_str = "Vision->Camera[" + this->p_interface->getName() + "]";
    my_log.write(t_str,"destroyed");
#endif
}
/*************************************************************************************************/
bool Camera::run() {
    load();
    AVFormatContext *pFormatCtx = NULL;
    int index_video_stream;
    AVCodecContext *pCodecCtx = NULL;
    
    if (initCamera(&pFormatCtx, index_video_stream, &pCodecCtx) == false) {
        printf("|Camera| : init input error!\n");
        return false;
    }
    else {
        printf("|Camera| : init input successful!\n");
    }
    
    SwsContext *img_convert_ctx = NULL;
    uint8_t* buffer_in = NULL;
    uint8_t* buffer_out = NULL;
    AVFrame* picture_yuv = NULL;
    AVFrame* picture_rgb = NULL;
    initConvertToImage(&img_convert_ctx, &buffer_in, &buffer_out, pCodecCtx, &picture_yuv, &picture_rgb);
    
    this->p_interface->setState(true);
    string tmp_str_name_file_store =  "store_" + this->p_interface->getName() + "_" + getDayTime();
    WriteClip* p_clip = new WriteClip(this->p_interface->getPath(), tmp_str_name_file_store, pFormatCtx);
#ifdef M_DEBUG
    string t_str = "Vision->Camera[" + this->p_interface->getName()+ "]";
    my_log.write(t_str, "started");
#endif // DEBUG    
    WriteClip* p_saving = nullptr; 
    int frameFinished;
    while(this->p_interface->getState()) {
        AVPacket *packet = (AVPacket*)malloc(sizeof(struct AVPacket));
        av_init_packet(packet);
        if (av_read_frame(pFormatCtx, packet) >= 0) {
            if (packet->stream_index == index_video_stream) {
                if(p_clip->getNumFrame() < LIMIT_FRAME_CAMERA) {
                    p_clip->writeFile(packet);
                } else {
                    p_clip->closeFile();
                    tmp_str_name_file_store =  "store_" + this->p_interface->getName() + "_" + getDayTime();
                    p_clip = new WriteClip(this->p_interface->getPath(), tmp_str_name_file_store, pFormatCtx);
                    p_clip->writeFile(packet);
                }
                if(this->p_interface->getSaveClip()) {
                    if(p_saving != nullptr) {
                        if(p_saving->getWritable()) {
                            p_saving->writeFile(packet);
                        }
                    } else {
                        this->p_interface->setNameFileClip("clip_" + getDayTime());
                        p_saving = new WriteClip(this->p_interface->getPath(), this->p_interface->getNameFileClip(), pFormatCtx);
                        if(p_saving->getWritable()) {
                            p_saving->writeFile(packet);
                        }
                    }
                } else {
                    if(p_saving != nullptr) {
                        if(p_saving->getWritable()) {
                            p_saving->stopWrite();
                            delete p_saving;
                            p_saving = nullptr;
                        }
                    }
                }
                
                avcodec_decode_video2(pCodecCtx, picture_yuv, &frameFinished, packet);
                sws_scale(img_convert_ctx, picture_yuv->data, picture_yuv->linesize, 0, pCodecCtx->height, picture_rgb->data, picture_rgb->linesize);
                Mat tmp(Size(pCodecCtx->width, pCodecCtx->height), CV_8UC3, picture_rgb->data[0], picture_rgb->linesize[0]);
                //imshow("Video",tmp);
                //waitKey(1);
                this->p_interface->setBuffer(tmp);
               
                av_free_packet(packet);
                usleep(1000);
            }
        }
    }
    p_clip->closeFile();
#ifdef M_DEBUG
    t_str = "Vision->Camera[" + this->p_interface->getName()+"]";
    my_log.write(t_str, "stoped");
#endif // DEBUG
    av_free(picture_yuv);
    av_free(picture_rgb);
    av_free(buffer_in);
    av_free(buffer_out);
    av_read_pause(pFormatCtx);
    return true;
}
/*************************************************************************************************/
WriteClip::WriteClip(string path, string name, AVFormatContext* inputFormatContext)
{
    this->path = path;
    this->name = name;
    initSystem();
    format_context = NULL;
    flag_writable = false;
    initSystem();
    if (createNewFile(inputFormatContext) == false) {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "error created");
#endif // DEBUG
    }
    else {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "created");
#endif // DEBUG
    }
    num_frame = 0;
}
/*************************************************************************************************/
WriteClip::~WriteClip()
{
    stopWrite();
#ifdef M_DEBUG
    string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
    my_log.write(t_str, "destroyed");
#endif // DEBUG
}
/*************************************************************************************************/
void WriteClip::initSystem() {
#ifdef M_DEBUG
    string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
    my_log.write(t_str, "inited");
#endif // DEBUG
}
/*************************************************************************************************/
bool WriteClip::createNewFile(AVFormatContext* inputFormatContext) {
    AVOutputFormat* pOutputFormat;
    if (initSaveFile(inputFormatContext) == false) {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "error create new file");
#endif // DEBUG
        return false;
    }
    if (tranferCodec(inputFormatContext) == false) {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "error create new file");
#endif // DEBUG
        return false;
    }
    if (createFile() == false) {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "error create new file");
#endif // DEBUG
        return false;
    }
    if (writeHeaderFile() == false) {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "error create new file");
#endif // DEBUG
        return false;
    }
#ifdef M_DEBUG
    string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
    my_log.write(t_str, "create new file");
#endif // DEBUG
    flag_writable = true;
    return true;
}
/*************************************************************************************************/
bool WriteClip::initSaveFile(AVFormatContext* inputFormatContext) {
    string m_file = this->path + "/" + this->name + ".mp4";
    avformat_alloc_output_context2(&format_context, NULL, "mp4", m_file.c_str());
    if (format_context == NULL) {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "error init saved file");
#endif // DEBUG
        return false;
    }
#ifdef M_DEBUG
    string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
    my_log.write(t_str, "init saved file");
#endif // DEBUG
    return true;
}
/*************************************************************************************************/
bool WriteClip::tranferCodec(AVFormatContext* inputFormatContext) {
    int index_stream = av_find_best_stream(inputFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    //int index_stream = 0;
    AVStream* inAVStream = inputFormatContext->streams[index_stream];
    AVStream* outAVStream = NULL;
    outAVStream = avformat_new_stream(format_context, NULL);
    if (outAVStream == NULL) {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "error init encode file");
#endif // DEBUG
        return false;
    }
    AVCodecContext *icodec = inAVStream->codec;
    AVCodecContext *ocodec = outAVStream->codec;
    outAVStream->id = inAVStream->id;
    ocodec->codec_id = icodec->codec_id;
    ocodec->codec_type = icodec->codec_type;
    ocodec->bit_rate = icodec->bit_rate;
    int extra_size = (uint64_t)icodec->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE;
    ocodec->extradata = (uint8_t *)av_mallocz(extra_size);
    memcpy(ocodec->extradata, icodec->extradata, icodec->extradata_size);
    ocodec->extradata_size = icodec->extradata_size;
    if (format_context->oformat->flags & AVFMT_GLOBALHEADER) {
        outAVStream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    ocodec->width = icodec->width;
    ocodec->height = icodec->height;
    ocodec->time_base = icodec->time_base;
    ocodec->gop_size = icodec->gop_size;
    ocodec->pix_fmt = icodec->pix_fmt;
    AVDictionaryEntry *tag = NULL;
    while ((tag = av_dict_get(inputFormatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        //cout << tag->key << " = " << tag->value << endl;
        av_dict_set(&(format_context->metadata), tag->key, tag->value, 0);
    }
#ifdef M_DEBUG
    string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
    my_log.write(t_str, "init encode file");
#endif // DEBUG
    return true;
}
/*************************************************************************************************/
bool WriteClip::createFile() {
    string m_file = this->path + "/" + this->name + ".mp4";
    if (!(format_context->flags & AVFMT_NOFILE)) {
        if (avio_open(&format_context->pb, m_file.c_str(), AVIO_FLAG_WRITE) < 0) {
#ifdef M_DEBUG
            string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
            my_log.write(t_str, "error create file");
#endif // DEBUG
            return false;
        }
    }
#ifdef M_DEBUG
    string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
    my_log.write(t_str, "create file");
#endif // DEBUG
    return true;
}
/*************************************************************************************************/
bool WriteClip::writeHeaderFile() {
    if (avformat_write_header(format_context, NULL) < 0) {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "error write header file");
#endif // DEBUG
        return false;
    }
    else {
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "write header file");
#endif // DEBUG
        return true;
    }
}
/*************************************************************************************************/
bool WriteClip::writeFile(AVPacket* data) {
    if (flag_writable) {
        AVPacket* temp_data = (AVPacket*)malloc(sizeof(struct AVPacket));
        if (av_copy_packet(temp_data, data) == 0) {
            int64_t frameTime;
            int64_t frameDuration;
            frameDuration = format_context->streams[0]->time_base.den / 30;
            frameTime = num_frame * frameDuration;
            temp_data->pts = frameTime;
            temp_data->dts = frameTime;
            temp_data->duration = frameDuration;
            if (av_interleaved_write_frame(format_context, temp_data) >= 0) {
                num_frame++;
                av_free_packet(temp_data);
                return true;
            }
            else {
                return false;
            }
        }
        else {
            av_free_packet(temp_data);
            return false;
        }
    }
    else {
        return false;
    }
}
/*************************************************************************************************/
void WriteClip::closeFile() {
    if (flag_writable) {
        av_write_trailer(format_context);
        avio_close(format_context->pb);
        flag_writable = false;
    }
#ifdef M_DEBUG
    string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
    my_log.write(t_str, "close file");
#endif // DEBUG
}
/*************************************************************************************************/
bool WriteClip::getWritable() {
    return flag_writable;
}
/*************************************************************************************************/
long WriteClip::getNumFrame() {
    return num_frame;
}
/*************************************************************************************************/
string WriteClip::getLink() {
    return this->path + "/" + this->name + ".mp4";
}
/*************************************************************************************************/
string WriteClip::getName() {
    return this->name;
}
/*************************************************************************************************/
void WriteClip::stopWrite() {
    if(this->flag_writable) {
        closeFile();
        if (format_context != NULL) {
            avformat_free_context(format_context);
            format_context = NULL;
        }
#ifdef M_DEBUG
        string t_str = "Vision->WriteClip[" + this->path + "/" + this->name + ".mp4]";
        my_log.write(t_str, "exit");
#endif // DEBUG
    }
}
/*************************************************************************************************/
Vision::Vision(Interface* interface) {
    this->p_interface = interface;
#ifdef M_DEBUG
    string t_str = "Vision->Vision[" + this->p_interface->getName()+ "]";
    my_log.write(t_str, "created");
#endif // DEBUG
}
/*************************************************************************************************/
Vision::~Vision() {
#ifdef M_DEBUG
    string t_str = "Vision->Vision[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "destroyed");
#endif // DEBUG
}
/*************************************************************************************************/
bool Vision::run() {
#ifdef M_DEBUG
    string t_str = "Vision->Vision[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "started");
#endif // DEBUG
    HOGDescriptor hog_detect_person;
    CascadeClassifier cascade;
    
    hog_detect_person.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    cascade.load("/root/haarcascade_frontalface_default.xml");
    long tmp_index = 0;
    while(this->p_interface->getState()) {
        timeval begin_tp, end_tp;
        gettimeofday(&begin_tp, 0);
        Mat src = this->p_interface->getBuffer();
        if(!src.empty()) {
            if(this->p_interface->getStateDetectPerson()) {
                if(tmp_index < NUM_FRAME_BACKGROUND) {
                    tmp_index++;
                } else {
                    
                }
            } else {
                tmp_index  = 0;
            }
            if(this->p_interface->getStateDetectFace()) {
                detectFace(cascade, src, 1.0);
            }
        }
        gettimeofday(&end_tp, 0);
        long time = end_tp.tv_usec - begin_tp.tv_usec + 1000000*(end_tp.tv_sec - begin_tp.tv_sec);
        if(time < 5000000) {
            usleep(5000000 - time);
        }
    }
#ifdef M_DEBUG
    t_str = "Vision->Vision[" + this->p_interface->getName() + "]";
    my_log.write(t_str, "stoped");
#endif // DEBUG
    return true;
}
/*************************************************************************************************/
bool Vision::detectFace(CascadeClassifier &detector, Mat &src, double scale) {
    vector<Rect> faces;
    Mat gray, smallImg, display;
    display = src.clone();
    cvtColor( src, gray, COLOR_BGR2GRAY );
    double fx = 1.0 / scale; 
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );
    detector.detectMultiScale( smallImg, faces, 1.2,  2, 0|CASCADE_SCALE_IMAGE, Size(20, 20) );
    if( faces.size() <= 0 ) {
        return false;
    } 
    for ( size_t i = 0; i < faces.size(); i++ ) {
        Rect r = faces[i]; 
        Point center; 
        Scalar color = Scalar(255, 0, 0);
        
        int radius; 
  
        double aspect_ratio = (double)r.width/r.height; 
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 ) 
        { 
            center.x = cvRound((r.x + r.width*0.5)*scale); 
            center.y = cvRound((r.y + r.height*0.5)*scale); 
            radius = cvRound((r.width + r.height)*0.25*scale); 
            circle( display, center, radius, color, 3, 8, 0 ); 
        } 
        else {
            rectangle( display, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)), 
                    cvPoint(cvRound((r.x + r.width-1)*scale),  
                    cvRound((r.y + r.height-1)*scale)), color, 3, 8, 0); 
        }
    }
    cout << writePhoto(display) << endl;
    return true;
}
/*************************************************************************************************/
string Vision::writePhoto(Mat& src) {
    vector<int> compression_params;
    compression_params.push_back( cv::IMWRITE_JPEG_QUALITY );
    compression_params.push_back( 100 );
    string name_data = "image-" + getDayTime() + ".jpg";
    string path = "/root/Pictures/" + name_data;
    imwrite(path.c_str(), src, compression_params); 
#ifdef M_DEBUG
    string t_str = "Vision->Vision[" + this->p_interface->getName()+ "]";
    my_log.write(t_str, "save image " + path);
#endif // DEBUG
    return name_data;
}
/*************************************************************************************************/