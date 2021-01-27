#include "filterdialog.h"
#include "ui_filterdialog.h"
#include "filter.h"
#include <QMessageBox>
#include <QDebug>

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Change Filter");
    ui->params->setText("1");
    ui->inputData->setText([] {
        QString textToInput = "";
        for(const auto &number : Filter::getFilter())
        {
            textToInput += QString::number(number) + " ";
        }
        return textToInput;
    }());
    if(Filter::getStatusModule())
    {
        ui->moduleButton->setChecked(true);
    }
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

bool regexData(QString data)
{
    for(int i = 0; i < data.length(); i++)
    {
        if(!((data[i] >= '0' && data[i] <= '9') || data[i] == '.'))
            return false;
    }
    return true;
}


void FilterDialog::on_buttonBox_accepted()
{
    QString data = ui->inputData->text();
    try
    {
        QStringList dataList = data.split(' ');
        QVector<double> tempData;
        for(int i = 0; i < dataList.size(); i++)
        {
            if(!regexData(dataList[i]))
                throw std::invalid_argument("Bad data input");
            tempData.push_back(dataList.at(i).toDouble());
        }

        Filter::setFilter(tempData);

        switch(ui->filterWidged->currentIndex().row())
        {
        case 0: {
            double param = ui->params->text().toDouble();
            if(param <= 0)
                throw std::invalid_argument("invalid argument");
            Filter::rectangularWindow(param);
            break;
        }
        case 1: {
            double param = ui->params->text().toDouble();
            if(param > 0.5 || param < 0)
                throw std::invalid_argument("invalid argument");
            Filter::gaussWindow(param);
            break;
        }
        case 2: {
            QStringList params = ui->params->text().split(' ');
            if(params.size() != 2)
                throw std::invalid_argument("invalid argument");
            double alfa = params[0].toDouble();
            double beta = params[1].toDouble();
            Filter::hammingWindow(alfa, beta);
            break;
        }
        case 3: {
            double param = ui->params->text().toDouble();
            Filter::blackmanWindow(param);
            break;
        }
        }
    }
    catch(std::invalid_argument exc)
    {
        QMessageBox::critical(this, "Error", "Bad data input");
        qDebug() << exc.what();
    }
}

void FilterDialog::on_moduleButton_clicked()
{
    Filter::setStatusModule();
}
