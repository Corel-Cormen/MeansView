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
    ui->inputData->setText([] {
        QString textToInput = "";
        for(const auto &number : Filter::getFilter())
        {
            textToInput += QString::number(number) + " ";
        }
        return textToInput;
    }());
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
    }
    catch(std::invalid_argument exc)
    {
        QMessageBox::critical(this, "Error", "Bad data input");
        qDebug() << exc.what();
    }
}
