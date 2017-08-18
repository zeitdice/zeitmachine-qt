#include "aboutdialog.h"
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

    text.append("This application is free software, licensed under the");
    text.append(" <a href=\"http://www.gnu.org/licenses/gpl-3.0.txt\">GNU General Public License, Version 3</a><br/>");
    text.append("The full source code can be obtained as a downloadable package <a href=\"http://www.zeitdice.com/SOURCE_DISTRIBUTION_PACKAGE_GOES_HERE\">here TODO</a><br/><br/>");

    text.append("<b>The following third party technologies form part of this application:</b><br/><br/>");
    text.append("Qt 5.9.1 Community/Open Source Edition");
    text.append(" - <a href=\"http://www.gnu.org/licenses/gpl-3.0.txt\">GPLv3</a><br/>");
    text.append("FFmpeg 3.3.3");
    text.append(" - <a href=\"http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt\">GPLv2</a><br/><br/>");

    text.append("Visit or contact us at <a href=\"http://www.zeitdice.com\">zeitdice.com</a>");

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
