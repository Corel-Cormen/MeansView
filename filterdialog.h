#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = nullptr);
    ~FilterDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::FilterDialog *ui;
};

#endif // FILTERDIALOG_H
