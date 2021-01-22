#include "opendatadialog.h"
#include "ui_opendatadialog.h"
#include "QtMath"
#include <QFile>
#include <QMessageBox>

#define FS 96000

#define DSIZE 20000

OpenDataDialog::OpenDataDialog(QWidget *parent, QString fileName) :
    QMainWindow(parent),
    ui(new Ui::OpenDataDialog)
{
    ui->setupUi(this);

    openDataFromFile(fileName);
    int sPlot = (showData.size() < DSIZE) ? 1 : 100;
    if(sPlot == 1)
    {
        ui->horizontalSlider->hide();
        ui->plot->dataPlot = &showData;
    }
    else
    {
        ui->plot->dataPlot = new QVector<double>;
        ui->plot->dataPlot->resize(showData.size()/sPlot);
        for(int i = 0; i < showData.size()/sPlot; i++) {
            (*ui->plot->dataPlot)[i] = showData[i];
        }
    }

    ui->plot->plotMode = Plot::LinearPlot;
    ui->plot->plotColor = Qt::red;
    ui->plot->setRange(0, ((*ui->plot->dataPlot).size())-1, -1, 1);
    ui->plot->setAxes(14, 0, ((1000*(*ui->plot->dataPlot).size()))/FS, 10, -1, 1);
}

OpenDataDialog::~OpenDataDialog()
{
    delete ui;
}

void OpenDataDialog::openDataFromFile(QString fileName)
{
    QFile file(fileName);

    file.open(QIODevice::ReadOnly);
    QTextStream inStream(&file);

    //QVector<double> dataFromFile;
    std::function<void(QString)> dataFile = [&](QString line) {
        line.resize(line.length()-1);
        QStringList slist = line.split(' ');
        for(const auto &sample : slist)
            showData.push_back(sample.toDouble());
    };

    while(!file.atEnd())
    {
        dataFile(inStream.readLine());
    }

    file.close();
}

void OpenDataDialog::on_horizontalSlider_valueChanged(int value)
{
    this->setWindowTitle(QString("scrap: %1/100").arg(value+1));
    int j = 0;
    for(int i = (showData.size()/100)*value; i < (showData.size()/100)*value+(*ui->plot->dataPlot).size(); i++)
    {
        (*ui->plot->dataPlot)[j] = showData[i];
        j++;
    }
    ui->plot->update();
}
