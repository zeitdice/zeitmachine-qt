#include "source/aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString text;

    text.append("<b>Version ");
    text.append(ZEITDICE_APPLICATION_VERSION);
    text.append("</b><br/><br/>");
    text.append("<a href=\"http://www.zeitdice.com\">zeitdice.com</a>");

    this->ui->aboutLabel->setText(text);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_closeButton_clicked()
{
    this->hide();
}
