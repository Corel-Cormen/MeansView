#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtMath"
#include <QDebug>
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    centralWidget()->layout()->setContentsMargins(0,0,0,0);
    centralWidget()->layout()->setSpacing(0);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::changeStatusToWrite);
    connect(ui->actionRun, &QAction::triggered, this, &MainWindow::sendCommand);
    connect(&serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(&serial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(error(QSerialPort::SerialPortError)));

    ui->plot->counterData = 0;
    ui->plot->plotData.resize(PSIZE);
    ui->plot->plotData.fill(0);
    ui->plot->dataSeries.resize(DSIZE2);
    ui->plot->dataSeries.fill(0);
    ui->plot->plotMode=Plot::LinearPlot;
    ui->plot->plotColor=Qt::red;
    ui->plot->setRange(0, DSIZE2-1, -5, 5);
    ui->plot->setAxes(14, 0, 1000*DSIZE2/FS, 10, -1, 1);

    ui->plot_2->dataSeries.resize(FFT_SIZE2);
    ui->plot_2->dataSeries.fill(0);
    ui->plot_2->plotMode=Plot::LogPlot;
    ui->plot_2->plotColor=Qt::green;

    double f1 = 10;
    double f2 = 10000;
    double scl = FFT_SIZE/static_cast<double>(FS);

    ui->plot_2->setRange((f1 * scl), f2 * scl, -100, 0);
    ui->plot_2->setAxes(0, f1, f2, 10, -100, 0);

    ui->plot_3->dataSeries.resize(ui->plot_2->dataSeries.size()/10);
    ui->plot_3->dataSeries.fill(0);
    ui->plot_3->plotMode=Plot::BarPlot;
    ui->plot_3->plotColor=Qt::green;

    ui->plot_3->setRange(f1 * scl/10, f2 * scl/10, 0, 1);
    ui->plot_3->setAxes(20, f1, f2, 10, 0, 1);

    fftData.resize(FFT_SIZE);
    fftData.fill(0);
    fftWin.resize(FFT_SIZE);

    // Rectangular Window
    fftWin.fill(1);
    // Hann Window
    for(int i=0; i<FFT_SIZE;i++)
        fftWin[i]=(0.5*(1-cos(2*M_PI*i/(FFT_SIZE-1))));
    magnitudeData.resize(FFT_SIZE2);
    phaseData.resize(FFT_SIZE2);


    ui->statusbar->showMessage("No device");
    QString portname;
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info:infos)
    {
        if(info.description() == "VCOM-MEAS")
        {
            portname=info.portName();
            serial.setPortName(portname);
            if(serial.open(QIODevice::ReadWrite))
            {
                ui->statusbar->showMessage(tr("Device: %1").arg(info.description()));
                serial.clear();
                ui->statusbar->setEnabled(true);
            }
            else
            {
                ui->statusbar->showMessage(tr("Can't open %1, error code %2").arg(serial.portName()).arg(serial.error()));
                return;
            }
            break;
        }
    }
}

MainWindow::~MainWindow()
{
    serial.close();
    delete ui;
}

void MainWindow::sendCommand()
{
    senddata.clear();
    senddata.resize(1);
    senddata[0]=static_cast<uint8_t>(ui->actionRun->isChecked() << 7); // Run/Stop
    serial.write(senddata);
}

void MainWindow::readData()
{
    if (serial.size() >= DSIZE) {

        readdata=serial.read(DSIZE);

        uint16_t *sample=reinterpret_cast<uint16_t*>(readdata.data());
        for (int n=0; n<ui->plot->plotData.size(); n++) {
            ui->plot->plotData[n]=(sample[n]-32768)/32768.0;
        }

        int i = 0;
        while(i < PSIZE)
        {
            ui->plot->dataSeries[ui->plot->counterData] = ui->plot->plotData[i];
            i++;
            ui->plot->counterData++;
        }

        if(ui->plot->counterData >= DSIZE2)
            ui->plot->counterData = 0;

        if(flagToWrite)
            writeData();

        calculateFFT();
        ui->plot_2->dataSeries=magnitudeData;
        ui->plot->update();
        ui->plot_2->update();
    }
}

void MainWindow::error(QSerialPort::SerialPortError error)
{
    qDebug()<<error;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    if(ui->actionRun->isChecked())
        ui->actionRun->trigger();
}

void MainWindow::calculateFFT()
{
    fftData.fill(0);
    for(int i=0;i<DSIZE2; i++) {

        fftData[static_cast<uint>(i)].real(ui->plot->dataSeries[i]*fftWin[i]);
        fftData[static_cast<uint>(i)].imag(0);
    }

    fftData=arma::fft(fftData);

    for(int i=0;i<FFT_SIZE2; i++) {

        magnitudeData[i]=abs(fftData[static_cast<uint>(i)])/(FFT_SIZE2);
        phaseData[i]=arg(fftData[static_cast<uint>(i)]);
    }
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    int f = value;
    for(int n=0; n<ui->plot->dataSeries.size(); n++)
        ui->plot->dataSeries[n]=(1.0*sin(2*M_PI*f*n/static_cast<double>(FS)));

    calculateFFT();

    ui->plot_2->dataSeries=magnitudeData;

    for(int n = 0; n < (magnitudeData.size()/10) -1; n++)
    {
        ui->plot_3->dataSeries[n] = std::accumulate(magnitudeData.begin() +(10 * n), magnitudeData.begin() + (10 * n) + 10, 0.0);
    }

    ui->plot->update();
    ui->plot_2->update();
    ui->plot_3->update();

    this->setWindowTitle(QString("f1: %1").arg(value));

}

void MainWindow::changeStatusToWrite()
{
    if(ui->actionRun->isChecked())
    {
        if(ui->actionSave->isChecked())
        {
            flagToWrite = !flagToWrite;

            QFile file(fileName);
            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                out << "";
            }
            file.close();
        }
    }
    else
    {
        ui->actionSave->setChecked(false);
        QMessageBox::information(this, "Alert", "You must start recording plot data");
    }

}

void MainWindow::writeData()
{
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream out(&file);
        for(auto sample : ui->plot->plotData)
        {
            out << sample << " ";
        }
        out << '\n';
    }
    file.close();
}
