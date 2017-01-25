#ifndef DETECTORPLOT_H
#define DETECTORPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>

class DetectorPlot : public QwtPlot
{

public:

    DetectorPlot( QWidget *parent=0, char *name=0 );

    ~DetectorPlot();

    QwtPlotCurve *mainCurve;
private:
    void getData(double *v1, double *v2, double *v3);



};

#endif // DETECTORPLOT_H
