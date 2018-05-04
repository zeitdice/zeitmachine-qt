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

    text.append("ZEITMachine renders images into videos. Copyright (C) 2018  ZEITDICE INC.<br/><br/>");
    text.append("This program is distributed in the hope that it will be useful,<br/>");
    text.append("but WITHOUT ANY WARRANTY; without even the implied warranty<br/>");
    text.append("of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br/>");
    text.append("See the GNU General Public License for more details.<br/><br/>");
    text.append("You should have received a copy of the GNU General Public License<br/>");
    text.append("along with this program. If not, see <a href=\"https://www.gnu.org/licenses/\">https://www.gnu.org/licenses/</a>.<br/><br/>");
    text.append("The full source code can be obtained <a href=\"https://github.com/zeitdice/zeitmachine-qt\">here</a><br/><br/>");

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
