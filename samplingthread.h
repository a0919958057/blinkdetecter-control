#ifndef SAMPLINGTHREAD_H
#define SAMPLINGTHREAD_H

#include <qwt_sampling_thread.h>
#include "EyeDetector.h"


class SamplingThread : public QwtSamplingThread
{
    //Q_OBJECT

public:
    SamplingThread( QObject *parent = NULL );

    void registerEyeDetector(EyeDetector& sourceDetector);

signals:

public slots:

    // QwtSamplingThread interface
protected:
    virtual void sample(double elapsed) override;

    EyeDetector* sourceDetector;

};

#endif // SAMPLINGTHREAD_H
