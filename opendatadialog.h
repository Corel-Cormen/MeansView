#ifndef OPENDATADIALOG_H
#define OPENDATADIALOG_H

#include <QMainWindow>
#include "QVector"

namespace Ui {
class OpenDataDialog;
}

class OpenDataDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit OpenDataDialog(QWidget *parent = nullptr, QString fileName = "data.txt");
    ~OpenDataDialog();

private slots:
    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::OpenDataDialog *ui;
    QVector<double> showData;

    void openDataFromFile(QString fileName);
};

#endif // OPENDATADIALOG_H
