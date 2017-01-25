#ifndef EYEDETECTOR_DETECTDATA_H
#define EYEDETECTOR_DETECTDATA_H

#include<qwt_series_data.h>
#include <qvector.h>
#include <qmutex.h>
#include <qreadwritelock.h>


class DetectData : public QwtSeriesData<QPointF>
{
public:
    DetectData();


    // QwtSeriesData interface
public:
    virtual size_t size() const;
    virtual QPointF sample(size_t i) const;
    virtual QRectF boundingRect() const;

    void append(const QPointF& data);

    void clearStaleData();

    QVector<QPointF> m_values;

    QRectF m_boundingrect;
};


#endif // EYEDETECTOR_DETECTDATA_H
