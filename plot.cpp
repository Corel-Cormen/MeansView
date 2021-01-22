#include "plot.h"
#include "armadillo"
using namespace arma;

Plot::Plot(QWidget *parent) : QWidget(parent)
{
    this->setMinimumHeight(4*MY);
    this->setMinimumWidth(4*MX);

}

void Plot::setRange(double minX, double maxX, double minY, double maxY)
{
    minValueX = minX;
    maxValueX = maxX;
    minValueY = minY;
    maxValueY = maxY;

    minAxisX = minX;
    maxAxisX = maxX;
    minAxisY = minY;
    maxAxisY = maxY;
}

void Plot::setAxes(int divX, double minX, double maxX, int divY, double minY, double maxY)
{
    gridNumX = divX;
    gridNumY = divY;
    minAxisX = minX;
    maxAxisX = maxX;
    minAxisY = minY;
    maxAxisY = maxY;
}

void Plot::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    gx=MX;
    gy=MY;
    gw=this->width()-(2*MX);
    gh=this->height()-(2*MY);
}
void Plot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    switch (plotMode) {
    case LinearPlot:
        drawLinearGrid(painter);
        drawLinearData(painter);
        break;
    case LogPlot:
        drawLogGrid(painter);
        drawLogData(painter);
        break;
    case BarPlot:
        drawLinearGrid(painter);
        drawBarData(painter);
        break;

    }
}

void Plot::drawLinearGrid(QPainter &painter)
{
    QPen pen;

    dx=gw/static_cast<double>(gridNumX);
    dy=gh/static_cast<double>(gridNumY);

    pen.setWidth(1);
    pen.setColor(backgroundColor);
    painter.setPen(pen);
    painter.setBrush(backgroundColor);
    painter.drawRect(0, 0, this->width(), this->height());

    // -----  frame ------------
    pen.setStyle(Qt::SolidLine);
    pen.setColor(gridColor);
    pen.setWidth(2);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.drawRect(gx, gy, gw, gh);
    // ----- grid ------------
    pen.setStyle(Qt::DotLine);
    pen.setWidth(1);
    painter.setPen(pen);
    for(int px=1; px<gridNumX; px++)
        painter.drawLine(QLineF(gx+(px*dx), gy, gx+(px*dx), gy+gh));
    for(int py=1; py<gridNumY; py++)
        painter.drawLine(QLineF(gx, gy+(py*dy), gx+gw, gy+(py*dy)));
    // ------ desc ----------
    QFont font;
    font.setPointSize(8);
    painter.setFont(font);
    pen.setColor(textColor);
    painter.setPen(pen);
    dvx=maxAxisX-minAxisX;
    dvy=maxAxisY-minAxisY;
    for(int px=0; px<=gridNumX; px++)
        painter.drawText(QPoint(gx-(font.pointSize()/4)+static_cast<int>(px*dx), gy+gh+(font.pointSize()*2)), QString("%1").arg(minAxisX+(dvx*px)/static_cast<double>(gridNumX)));
    for(int py=0; py<=gridNumY; py++)
        painter.drawText(QPoint(gx-(font.pointSize()*4), gy+(font.pointSize()/2)+static_cast<int>(py*dy)), QString("%1").arg(maxAxisY-(dvy*py)/static_cast<double>(gridNumY)));
}

void Plot::drawLinearData(QPainter &painter)
{
    QPen pen;
    dx=gw/(maxValueX - minValueX);
    dy=gh/(maxValueY - minValueY);
    gmy=gy+gh+dy*minValueY;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(plotColor);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.save();
    painter.setClipRect(gx, gy, gw, gh);
    for(int i = minValueX; i < maxValueX; i++)
        painter.drawLine(QLineF(gx+(i-minValueX)*dx, gmy-((*dataPlot)[i]*dy), gx+(i-minValueX+1)*dx, gmy-((*dataPlot)[i+1]*dy)));
    painter.restore();
}

void Plot::drawLogData(QPainter &painter)
{
    dx=gw/(log10(maxValueX)-log10(minValueX));
    dy=gh/(maxValueY-minValueY);
    gmy=gy+gh+dy*minValueY;
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(plotColor);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.save();
    painter.setClipRect(gx, gy, gw, gh);
    for(int i=minValueX; i<maxValueX-1; i++)
        painter.drawLine(QLineF(gx+(log10(i)-log10(minValueX))*dx, gmy+(20*log10(1/((*dataPlot)[i]))*dy),
                                gx+(log10(i+1)-log10(minValueX))*dx, gmy+(20*log10(1/((*dataPlot)[i+1]))*dy)));
    painter.restore();
}

void Plot::drawLogGrid(QPainter &painter)
{
    QPen pen;
    dx=gw/static_cast<double>(log10(maxAxisX)- log10(minAxisX));
    dy=gh/static_cast<double>(gridNumY);
    pen.setWidth(1);
    pen.setColor(backgroundColor);
    painter.setPen(pen);
    painter.setBrush(backgroundColor);
    painter.drawRect(0, 0, this->width(), this->height());

    // ----- frame ------------
    pen.setStyle(Qt::SolidLine);
    pen.setColor(gridColor);
    pen.setWidth(2);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.drawRect(gx, gy, gw, gh);

    // ----- grid ------------
    pen.setStyle(Qt::DotLine);
    pen.setWidth(1);
    painter.setPen(pen);

    int dl = 1;
    for(int px = minAxisX; px <= maxAxisX; px+=dl)
    {
        painter.drawLine(QLineF(gx+((log10(px) - log10(minAxisX)) * dx), gy,gx + ((log10(px)-log10(minAxisX)) * dx), gy+gh));
        if((px%(dl*10)) == 0){
            dl*=10;
        }
    }

    for(int py = 1; py<gridNumY; py++)
    {
        painter.drawLine(QLineF(gx,gy+(py * dy), gx + gw, gy + (py * dy)));
    }


    // ------ desc ----------
    QFont font;
    font.setPointSize(8);
    painter.setFont(font);
    pen.setColor(textColor);
    painter.setPen(pen);
    dvx=maxAxisX-minAxisX;
    dvy=maxAxisY-minAxisY;

    painter.drawText(QPointF(gx -(font.pointSize()), gy+gh+(font.pointSize()*2)), QString().asprintf("%4d",(int)minAxisX));
    for(int px = 10 * pow(10,(int)log10(minAxisX));px<maxAxisX; px *=10){
        painter.drawText(QPointF(gx+(log10(px) - log10(minAxisX)) * dx-(font.pointSize()), gy+gh + (font.pointSize() * 2)), QString().asprintf("%4d",px));
    }
    painter.drawText(QPointF(gx+(log10(maxAxisX) - log10(minAxisX)) * dx-(font.pointSize()), gy+gh + (font.pointSize() * 2)), QString().asprintf("%4d",(int)maxAxisX));

    for(int py = 0; py < gridNumY; py++){
        painter.drawText(QPointF(gx - (font.pointSize() * 4), gy +(font.pointSize()/2) + py * dy), QString().asprintf("%4d", static_cast<int>(maxAxisY - (dvy * py) / gridNumY)));
    }
}

void Plot::drawBarData(QPainter &painter){

    QPen pen;

    dx = gw/(maxValueX - minValueX);
    dy = gh/(maxValueY - minValueY);
    gmy = gy + gh + dy *minValueY;

    pen.setStyle(Qt::SolidLine);
    pen.setColor(plotColor);
    pen.setWidth(1);

    painter.setPen(pen);
    plotColor.setAlpha(128);
    painter.setBrush(plotColor);
    QFont font;

    font.setPointSize(10);
    painter.setFont(font);
    painter.save();

    painter.setClipRect(gx-1, gy-1, gw +1, gh+1);

    for(int i = minValueX; i < maxValueX; i++)
    {
        painter.drawRect(QRectF(gx + (i - minValueX) * dx, gmy, 0.8 * dx, -(*dataPlot)[i] * dy));
    }

    painter.restore();

}


