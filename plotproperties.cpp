#include "plotproperties.h"
#include "ui_plotproperties.h"
#include <QMessageBox>

#define RANGE 500

PlotProperties::PlotProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotProperties)
{
    ui->setupUi(this);
}

PlotProperties::PlotProperties(Plot *p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotProperties),
    plot(p)
{
    ui->setupUi(this);
    ui->minSpinBox->setRange(-RANGE, RANGE);
    ui->maxSpinBox->setRange(-RANGE, RANGE);
    ui->minSpinBox->setValue(plot->getMinAxisY());
    ui->maxSpinBox->setValue(plot->getMaxAxisY());
    for(int i = 0; i < colors.size(); i++)
    {
        auto color = colors.keys()[i];
        ui->plotColorWidget->addItem(color);
        ui->lineColorWidget->addItem(color);
        ui->gridColorWidget->addItem(color);
        ui->textColorWidget->addItem(color);
    }
    ui->plotColorWidget->setCurrentRow(0);
    ui->lineColorWidget->setCurrentRow(0);
    ui->gridColorWidget->setCurrentRow(0);
    ui->textColorWidget->setCurrentRow(0);
}

PlotProperties::~PlotProperties()
{
    delete ui;
}

void PlotProperties::on_buttonBox_accepted()
{
    try
    {
        int min = ui->minSpinBox->text().toInt();
        int max = ui->maxSpinBox->text().toInt();
        if(min >= max)
            throw std::invalid_argument("invalid argument");
        plot->setRange(plot->getMinValueX(), plot->getMaxValueX(), min, max);
        if(ui->plotColorWidget->currentRow() != 0)
            plot->backgroundColor = colors.values()[ui->plotColorWidget->currentRow()];
        if(ui->lineColorWidget->currentRow() != 0)
            plot->plotColor = colors.values()[ui->lineColorWidget->currentRow()];
        if(ui->gridColorWidget->currentRow() != 0)
            plot->gridColor = colors.values()[ui->gridColorWidget->currentRow()];
        if(ui->textColorWidget->currentRow() != 0)
            plot->textColor = colors.values()[ui->textColorWidget->currentRow()];

        plot->repaint();
    }
    catch(std::invalid_argument exc)
    {
        QMessageBox::critical(this, "Critical", "Bad input valuse");
    }
}
