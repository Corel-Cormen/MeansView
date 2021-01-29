#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <armadillo>
#include <opendatadialog.h>
#include <QStandardPaths>

#define FS 96000

#define DSIZE 8192
#define DSIZE2 (DSIZE/2)
#define PSIZE 512
#define PSIZE2 (PSIZE/2)

#define FFT_SIZE 9600
#define FFT_SIZE2 (FFT_SIZE/2)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendCommand();
    void readData();
    void error(QSerialPort::SerialPortError error);
    void closeEvent(QCloseEvent *event);
    void changeStatusToWrite();
    void openFilterWindow();

    void on_verticalSlider_valueChanged(int value);

    void on_actionOpenFile_triggered();

private:
    QVector<QVector<double>> data;

    Ui::MainWindow *ui;
    OpenDataDialog *openDataDialog;
    void calculateFFT();

    QSerialPort serial;
    QByteArray senddata;
    QByteArray readdata;
    bool flagToWrite = false;
    QString fileName = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QString("/data.txt");

    QVector<double> magnitudeData;
    QVector<double> phaseData;
    QVector<double> fftWin;

    arma::cx_vec fftData;

    void calculateBarData();
    void writeData(int p1, int p2);
    void filtration();
    void calculateModule();

    void updateDataMark();
};
#endif // MAINWINDOW_H
