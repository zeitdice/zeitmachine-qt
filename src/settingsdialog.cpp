#include "settingsdialog.h"
#include "ui_settingsdialog.h"

const char* SETTINGS_INITIAL = "42";

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QFile file(".zeitsettings");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if(!file.open(QIODevice::WriteOnly)) {
            // Trouble
            return;
        }

        file.write(SETTINGS_INITIAL);
        ui->plainTextEdit->setPlainText(SETTINGS_INITIAL);
    }

    if(!file.atEnd()) {
        QByteArray all = file.readAll();
        ui->plainTextEdit->setPlainText(all);
    }

    file.close();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{
    QFile file(".zeitsettings");

    if(!file.open(QIODevice::WriteOnly)) {
        // Trouble
        return;
    }

    file.write(ui->plainTextEdit->toPlainText().toLocal8Bit());
    file.close();
}

void SettingsDialog::on_buttonBox_rejected()
{
    QFile file(".zeitsettings");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if(!file.open(QIODevice::WriteOnly)) {
            // Trouble
            return;
        }

        file.write(SETTINGS_INITIAL);
        ui->plainTextEdit->setPlainText(SETTINGS_INITIAL);
    }

    if(!file.atEnd()) {
        QByteArray all = file.readAll();
        ui->plainTextEdit->setPlainText(all);
    }

    file.close();
}
