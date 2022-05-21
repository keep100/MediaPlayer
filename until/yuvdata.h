#ifndef YUVDATA_H
#define YUVDATA_H
const static size_t maxSize = 2560 * 1440;

// 最终发送到前端的原始视频帧数据结构
// 格式：AV_PIX_FMT_YUV420P
#ifndef __YUVData_LOCAL_DEFINED
#define __YUVData_LOCAL_DEFINED
struct YUVData {
    unsigned char Y[maxSize];
    unsigned char U[maxSize];
    unsigned char V[maxSize];
    int yHeight=0,yWidth=0;
    int uWidth=0,uHeight=0;
    int vHeight=0,vWidth=0;
};
#endif
#endif // YUVDATA_H
