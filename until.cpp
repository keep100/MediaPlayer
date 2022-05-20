#include "until.h"
#include <QDebug>

Until::Until()
{
    thread = new XDemuxThread();
    thread->Start();
}

void Until::open()
{
    thread->Open("D:/Chrome_download/Video/Saiai.Ep03.Chi_Jap.HDTVrip.1280X720-ZhuixinFan.mp4",
                 nullptr);
}

void Until::setPause(bool isPause)
{
    thread->SetPause(isPause);
}

Until::~Until()
{

}
