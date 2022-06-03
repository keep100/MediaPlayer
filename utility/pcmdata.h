#ifndef PCMDATA_H
#define PCMDATA_H
const static size_t maxPcmSize = 1024*256;
struct PCMData{
    char data[maxPcmSize];
    size_t size;
    int64_t pts;
    PCMData(char* d, size_t s, int64_t p){
        memcpy(data, d, s);
        size = s;
        pts = p;
    }
};
#endif // PCMDATA_H
