#include "samplingthread.h"

SamplingThread::SamplingThread(QObject *parent) :
    QwtSamplingThread(parent),
    sourceDetector(nullptr)
{}

void SamplingThread::registerEyeDetector(EyeDetector &detector)
{
    sourceDetector = &detector;
}

void SamplingThread::sample(double elapsed) {

    // Makesure there is registered
    if(sourceDetector == nullptr || !sourceDetector->getIsEnableBlinkDetect()) {
        return;
    }

    sourceDetector->pending_mutex.lock();
    const QPointF s( elapsed, sourceDetector->minVal );
    sourceDetector->pending_mutex.unlock();

    sourceDetector->dataRecord_mutex.lock();
    sourceDetector->dataRecord.append(s);
    sourceDetector->dataRecord_mutex.unlock();


}
