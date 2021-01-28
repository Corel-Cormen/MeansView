#ifndef PLOTPROPERTIES_H
#define PLOTPROPERTIES_H

#include <QDialog>
#include <plot.h>
#include <QPainter>

namespace Ui {
class PlotProperties;
}

class PlotProperties : public QDialog
{
    Q_OBJECT

public:
    explicit PlotProperties(QWidget *parent = nullptr);
    PlotProperties(Plot *p, QWidget *parent = nullptr);
    ~PlotProperties();

private slots:
    void on_buttonBox_accepted();

private:
    QMap<QString, QColor> colors = {{"", 0},{"Red", Qt::red},{"Green", Qt::green},{"Blue", Qt::blue},{"Black", Qt::black},
        {"White", Qt::white},{"Yellow", Qt::yellow},{"Gray", Qt::gray},{"Cyan", Qt::cyan},{"Magneta", Qt::magenta}};
    Ui::PlotProperties *ui;
    Plot *plot;
    bool flagToClose = true;
};

#endif // PLOTPROPERTIES_H
