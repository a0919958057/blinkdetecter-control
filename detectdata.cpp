#include "detectdata.h"


DetectData::DetectData() : QwtSeriesData<QPointF>(), m_boundingrect(1.0, 1.0, -1.0 , -1.0)
{

}

size_t DetectData::size() const
{
    return m_values.size();
}

QPointF DetectData::sample(size_t i) const
{
    return m_values[i];
}

QRectF DetectData::boundingRect() const
{
    return m_boundingrect;
}

void DetectData::append(const QPointF &data)
{
    // Add the new data to the vector
    m_values.append(data);

    // Compute and change the bounding rect
    if(m_boundingrect.width() < 0.0 || m_boundingrect.height() < 0.0) {
        // Setup the initial data
        m_boundingrect.setRect( data.x(), data.y(), 0.0, 0.0 );

    }
    else {
        m_boundingrect.setRight(data.x());

        if(data.y() < m_boundingrect.top()) {
            m_boundingrect.setTop(data.y());
        }

        if(data.y() > m_boundingrect.bottom()) {
            m_boundingrect.setBottom(data.y());
        }
    }
    if(m_values.size() > 500) {

        m_boundingrect.setRect(
                    data.x()-4.0 , 0, 4.0, m_boundingrect.height());
    }
}

void DetectData::clearStaleData()
{
    m_boundingrect.setRect(1.0, 1.0, -1.0 , -1.0);
    m_values.clear();
}
