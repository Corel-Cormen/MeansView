#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filter.h"
#include "filterdialog.h"
#include "opendatadialog.h"
#include "QtMath"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    centralWidget()->layout()->setContentsMargins(0,0,0,0);
    centralWidget()->layout()->setSpacing(0);
    this->setStyleSheet("QToolBar {background: rgb(255, 255, 255)}");

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::changeStatusToWrite);
    connect(ui->actionRun, &QAction::triggered, this, &MainWindow::sendCommand);
    connect(&serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(&serial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(error(QSerialPort::SerialPortError)));
    connect(ui->actionFilter, &QAction::triggered, this, &MainWindow::openFilterWindow);

    data.resize(4);
    data[0].fill(0).resize(DSIZE2);                                 // normal read data ploting
    data[1].fill(0).resize(FFT_SIZE2);                              // fft data ploting
    data[2].fill(0).resize(data[1].size()/10);                      // fft as bars ploting
    data[3].fill(0).resize(DSIZE2);

    ui->plot->dataPlot = &data[0];
    ui->plot_2->dataPlot = &data[1];
    ui->plot_3->dataPlot = &data[2];
    ui->plot_4->dataPlot = &data[3];

    ui->plot->plotMode = Plot::LinearPlot;
    ui->plot->plotColor = Qt::red;
    ui->plot->setRange(0, DSIZE2-1, -1, 1);
    ui->plot->setAxes(14, 0, 1000*DSIZE2/FS, 10, -1, 1);

    Filter filter;
    filter.setFilter({0.2, 0.2, 0.2, 0.2, 0.2});
    ui->plot_4->plotMode = Plot::LinearPlot;
    ui->plot_4->plotColor = Qt::red;
    ui->plot_4->setRange(0, DSIZE2-1, -1, 1);
    ui->plot_4->setAxes(14, 0, 1000*DSIZE2/FS, 10, -1, 1);

    double f1 = 10;
    double f2 = 10000;
    double scl = FFT_SIZE/static_cast<double>(FS);
    Filter::setSize(FFT_SIZE);
    Filter::rectangularWindow(1);

    ui->plot_2->plotMode = Plot::LogPlot;
    ui->plot_2->plotColor = Qt::green;
    ui->plot_2->setRange((f1 * scl), f2 * scl, -200, 0);
    ui->plot_2->setAxes(0, f1, f2, 10, -100, 0);

    ui->plot_3->plotMode = Plot::BarPlot;
    ui->plot_3->plotColor = Qt::green;
    ui->plot_3->setRange(f1 * scl/10, f2 * scl/10, 0, 1);
    ui->plot_3->setAxes(20, f1, f2, 10, 0, 1);

    fftData.resize(FFT_SIZE);
    fftData.fill(0);
    fftWin.resize(FFT_SIZE);

    // Rectangular Window
    fftWin.fill(1);
    // Hann Window
    for(int i = 0; i < FFT_SIZE; i++)
        fftWin[i] = (0.5*(1-cos(2*M_PI*i/(FFT_SIZE-1))));
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
    static uint32_t cnt = 0;
    if (serial.size() >= PSIZE) {

        readdata = serial.read(PSIZE);

        uint16_t *sample = reinterpret_cast<uint16_t*>(readdata.data());
        for (int n = 0; n < PSIZE2; n++)
        {
            (*ui->plot->dataPlot)[cnt+n] = (sample[n]-32768)/32768.0;
        }

        cnt += PSIZE2;
        if(cnt >= (DSIZE2))
        {
            cnt = 0;
        }
        if(flagToWrite)
        {
            writeData(cnt, cnt+PSIZE2);
        }

        calculateFFT();
        *ui->plot_2->dataPlot = magnitudeData;
        calculateBarData();
        ui->plot->update();
        ui->plot_2->update();
        ui->plot_3->update();

        if(Filter::getStatusModule())
            ui->plot_4->setRange(0, DSIZE2-1, 0, 1);
        else
            ui->plot_4->setRange(0, DSIZE2-1, -1, 1);

        filtration();
        ui->plot_4->update();
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
    for(int i = 0; i < DSIZE2; i++)
    {
        fftData[static_cast<uint>(i)].real((*ui->plot->dataPlot)[i]*fftWin[i]);
        fftData[static_cast<uint>(i)].imag(0);
    }

    fftData=arma::fft(fftData);

    for(uint i = 0; i < fftData.size(); i++){
        fftData[i] *= (*Filter::getFilterFFT())[i];
    }

    for(int i = 0; i < FFT_SIZE2; i++)
    {
        magnitudeData[i]=abs(fftData[static_cast<uint>(i)])/(FFT_SIZE2);
        phaseData[i]=arg(fftData[static_cast<uint>(i)]);
    }

}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    int f = 100*value;
    for(int n = 0; n < (*ui->plot->dataPlot).size(); n++)
        (*ui->plot->dataPlot)[n] = (5.0*sin(2*M_PI*f*n/static_cast<double>(FS)));

    calculateFFT();

    (*ui->plot_2->dataPlot) = magnitudeData;

    calculateBarData();
    filtration();
    ui->plot->update();
    ui->plot_2->update();
    ui->plot_3->update();
    ui->plot_4->update();

    this->setWindowTitle(QString("freq: %1").arg(f));

}

void MainWindow::calculateBarData()
{
    for(int n = 0; n < (magnitudeData.size()/10) -1; n++)
    {
        (*ui->plot_3->dataPlot)[n] = std::accumulate(magnitudeData.begin() +(10 * n), magnitudeData.begin() + (10 * n) + 10, 0.0);
    }
}

void MainWindow::filtration()
{
    for(int i = 0; i < (*ui->plot->dataPlot).size() - Filter::getFilter().length(); i++)
    {
        (*ui->plot_4->dataPlot)[i] = 0;
        for(int j = 0; j < Filter::getFilter().length(); j++)
        {
            (*ui->plot_4->dataPlot)[i] += (*ui->plot->dataPlot)[i+j] * Filter::getFilter().at(j);
        }
    }
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

void MainWindow::writeData(int p1, int p2)
{
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream out(&file);
        for(int i = p1; i < p2; i++)
        {
            out << (*ui->plot->dataPlot)[i] << ' ';
        }
        out << '\n';
    }
    file.close();
}

void MainWindow::openFilterWindow()
{
    FilterDialog filterDialog;
    filterDialog.setModal(true);
    filterDialog.exec();
}

void MainWindow::on_actionOpenFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open saved data"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    tr("Text Files (*.txt)"));

    if(fileName != nullptr)
    {
        openDataDialog = new OpenDataDialog(this, fileName);
        openDataDialog->show();
    }
}

void MainWindow::calculateModule()
{
    for(int i = 0; i < ui->plot_4->dataPlot->size(); i++)
    {
        if(ui->plot_4->dataPlot->at(i) < 0)
            (*ui->plot_4->dataPlot)[i] *= -1;
    }
}
