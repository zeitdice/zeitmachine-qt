#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "version.h"

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void on_closeButton_clicked();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
