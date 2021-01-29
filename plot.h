#ifndef PLOT_H
#define PLOT_H
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#define MX 40
#define MY 20
class Plot : public QWidget
{
    Q_OBJECT
public:
    explicit Plot(QWidget *parent = nullptr);
    enum PlotStyle {
        LinearPlot,
        LogPlot,
        BarPlot
    };
    uint32_t plotMode=0;
    QVector<double> *dataPlot;
    QColor backgroundColor=Qt::black;
    QColor gridColor=Qt::gray;
    QColor textColor=Qt::white;
    QColor plotColor=Qt::red;
    volatile double markerX=0, markerY=0;
    int mosuePosX = 0, mosuePosY = 0;
    void setRange(double minX, double maxX, double minY, double maxY);
    void setAxes(int divX, double minX, double maxX, int divY, double minY, double maxY);
    void getMousePosition(int x, int y);

    inline double getMinValueX() { return minValueX; }
    inline double getMaxValueX() { return maxValueX; }
    inline double getMinAxisX() { return minAxisX; }
    inline double getMaxAxisX() { return maxAxisX; }
    inline double getMinValueY() { return minValueY; }
    inline double getMaxValueY() { return maxValueY; }
    inline double getMinAxisY() { return minAxisY; }
    inline double getMaxAxisY() { return maxAxisY; }

private:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void drawLinearGrid(QPainter &painter);
    void drawLinearData(QPainter &painter);
    void drawLogGrid(QPainter &painter);
    void drawLogData(QPainter &painter);
    void drawSemiGrid(QPainter &painter);
    void drawSemiData(QPainter &painter);
    void drawBarData(QPainter &painter);

    void drawMarkers(QPainter &painter);

    double dx, dy, dvx, dvy;
    int gx, gy, gw, gh, gmy;
    int gridNumX=10, gridNumY=10;
    double minValueX= 0, maxValueX=10;
    double minValueY=-1, maxValueY=1;
    double minAxisX= 0, maxAxisX=1;
    double minAxisY=-1, maxAxisY=1;
signals:
    void doubleClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *);
};
#endif // PLOT_H
