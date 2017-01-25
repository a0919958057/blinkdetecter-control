#include "detectorplot.h"

DetectorPlot::DetectorPlot( QWidget *parent, char *name ) : QwtPlot(parent) {

    setAxisTitle( xBottom, "time(sec)");
    setAxisTitle( yLeft, "Compare Value");

    setLineWidth(2);

    mainCurve = new QwtPlotCurve("Curve 1");

    // Enable Antialiased
    mainCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

    mainCurve->attach(this);

    // Add the grid to the plot
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach(this);
    setAxisAutoScale(QwtPlot::yLeft);



    replot();
}

DetectorPlot::~DetectorPlot()
{
    delete mainCurve;
}
