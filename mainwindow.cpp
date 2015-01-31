#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qRegisterMetaType<QFileInfo>("QFileInfo");
    qRegisterMetaType<QFileInfoList>("QFileInfoList");
    qRegisterMetaType<QImage::Format>("QImage::Format");

    ui->setupUi(this);

    videoWidget = new GLVideoWidget;
    this->setCentralWidget(videoWidget);

    progressbar = new QProgressBar;
    progressbar->hide();
    ui->statusbar->addPermanentWidget(progressbar, 100);

    zeitengine = new ZeitEngine(videoWidget);
    zeitengine->moveToThread(&engineThread);
    engineThread.start();

    connect(&engineThread, &QThread::finished, zeitengine, &QObject::deleteLater);

    connect(zeitengine, &ZeitEngine::VideoUpdated, videoWidget, &GLVideoWidget::DelegateUpdate);
    connect(zeitengine, &ZeitEngine::VideoConfigurationUpdated, videoWidget, &GLVideoWidget::ConfigureVideo);
    connect(zeitengine, &ZeitEngine::MessageUpdated, this, &MainWindow::UpdateMessage);
    connect(zeitengine, &ZeitEngine::ProgressUpdated, this, &MainWindow::UpdateProgress);

    connect(this, &MainWindow::LoadSignal, zeitengine, &ZeitEngine::Load);
    connect(this, &MainWindow::CacheSignal, zeitengine, &ZeitEngine::Cache);
    connect(this, &MainWindow::PlaySignal, zeitengine, &ZeitEngine::Play);
    connect(this, &MainWindow::ExportSignal, zeitengine, &ZeitEngine::Export);
}

MainWindow::~MainWindow()
{
    engineThread.quit();
    engineThread.wait();
    delete ui;
}

void MainWindow::UpdateMessage(const QString text)
{
    ui->statusbar->showMessage(text);
}

void MainWindow::UpdateProgress(const QString text, const int current, const int total)
{
    if (current == total && total > 0) {
        progressbar->hide();
    } else {
        progressbar->setMaximum(total);
        progressbar->setValue(current);

        if(total > 0) {
            progressbar->setFormat(text + " %v/%m");
        } else {
            progressbar->setFormat(text);
        }

        if(progressbar->isHidden()) {
            progressbar->show();
        }
    }
}

void MainWindow::on_actionPlay_triggered()
{
    zeitengine->control_mutex.lock();
    zeitengine->stop_flag = false;
    zeitengine->loop_flag = false;
    zeitengine->control_mutex.unlock();

    emit PlaySignal();
}

void MainWindow::on_actionLoop_triggered()
{
    zeitengine->control_mutex.lock();
    zeitengine->stop_flag = false;
    zeitengine->loop_flag = true;
    zeitengine->control_mutex.unlock();

    emit PlaySignal();
}

void MainWindow::on_actionStop_triggered()
{
    zeitengine->control_mutex.lock();
    zeitengine->stop_flag = true;
    zeitengine->loop_flag = false;
    zeitengine->control_mutex.unlock();
}

void MainWindow::on_actionCycleFramerates_triggered()
{
    QString new_rate_label;
    ZeitRate new_rate;

    zeitengine->control_mutex.lock();

    switch(zeitengine->configured_framerate) {

        case ZEIT_RATE_24p:
            new_rate = ZEIT_RATE_25p;
            new_rate_label = "25p (PAL/SECAM TV equivalent)";
            break;

        case ZEIT_RATE_25p:
            new_rate = ZEIT_RATE_30p;
            new_rate_label = "30p (NTSC TV equivalent)";
            break;

        case ZEIT_RATE_30p:
            new_rate = ZEIT_RATE_48p;
            new_rate_label = "48p (Cinema HFR equivalent)";
            break;

        case ZEIT_RATE_48p:
            new_rate = ZEIT_RATE_50p;
            new_rate_label = "50p (Next-Gen HDTV equivalent)";
            break;

        case ZEIT_RATE_50p:
            new_rate = ZEIT_RATE_60p;
            new_rate_label = "60p (Next-Gen HDTV equivalent)";
            break;

        case ZEIT_RATE_60p:
            new_rate = ZEIT_RATE_24p;
            new_rate_label = "24p (Cinema/Film equivalent)";
            break;

        // Hide akward special rates in the interface for now
        // Can be considered for re-inclusion later
        case ZEIT_RATE_23_976:
        case ZEIT_RATE_29_97:
            break;

    }

    zeitengine->configured_framerate = new_rate;

    zeitengine->control_mutex.unlock();

    ui->statusbar->showMessage("Framerate set to " + new_rate_label);
}


void MainWindow::on_actionAbout_triggered()
{
    QPixmap *pixmap = new QPixmap(":/about/splash.png");
    QSplashScreen *splash = new QSplashScreen(*pixmap);
    splash->show();
}

void MainWindow::on_actionVignette_triggered(bool checked)
{
    zeitengine->control_mutex.lock();
    zeitengine->filter_flag = checked ? ZEIT_FILTER_VIGNETTE : ZEIT_FILTER_NONE;
    zeitengine->control_mutex.unlock();

    UncheckOtherFilters(this->ui->actionVignette);
}

void MainWindow::on_actionBlackWhite_triggered(bool checked)
{
    zeitengine->control_mutex.lock();
    zeitengine->filter_flag = checked ? ZEIT_FILTER_BLACKWHITE : ZEIT_FILTER_NONE;
    zeitengine->control_mutex.unlock();

    UncheckOtherFilters(this->ui->actionBlackWhite);
}

void MainWindow::on_actionSepia_triggered(bool checked)
{
    zeitengine->control_mutex.lock();
    zeitengine->filter_flag = checked ? ZEIT_FILTER_SEPIA : ZEIT_FILTER_NONE;
    zeitengine->control_mutex.unlock();

    UncheckOtherFilters(this->ui->actionSepia);
}

void MainWindow::on_actionHipstagram_triggered(bool checked)
{
    zeitengine->control_mutex.lock();
    zeitengine->filter_flag = checked ? ZEIT_FILTER_HIPSTAGRAM : ZEIT_FILTER_NONE;
    zeitengine->control_mutex.unlock();

    UncheckOtherFilters(this->ui->actionHipstagram);
}

void MainWindow::on_actionMovie_triggered()
{
    QString footage_folder = QFileDialog::getExistingDirectory(this,
                                                               "Select a folder to output to",
                                                               "",
                                                               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    if(!footage_folder.isEmpty()) {
        qDebug() << QFileInfo(footage_folder, "zeitdice-export.h264").absoluteFilePath();
        emit ExportSignal(QFileInfo(footage_folder, "zeitdice-export.h264"));
    }
}

void MainWindow::on_actionSettings_triggered()
{
    settings.show();
}

void MainWindow::UncheckOtherFilters(QAction *action)
{
    if(action != this->ui->actionVignette) { this->ui->actionVignette->setChecked(false); }
    if(action != this->ui->actionBlackWhite) { this->ui->actionBlackWhite->setChecked(false); }
    if(action != this->ui->actionSepia) { this->ui->actionSepia->setChecked(false); }
    if(action != this->ui->actionHipstagram) { this->ui->actionHipstagram->setChecked(false); }
}


void MainWindow::on_actionOpen_triggered()
{
    QDir footage_root(QCoreApplication::applicationDirPath());

    // Optional path correction for different operating systems
    #if defined(Q_OS_WIN)

    #elif defined(Q_OS_MAC)

    //    if (footage_root.dirName() == "MacOS")
    //    {
    //        footage_root.cdUp();
    //        footage_root.cdUp();
    //        footage_root.cdUp();
    //    }

    #endif

    QString footage_folder = QFileDialog::getExistingDirectory(this,
                                                               "Open Footage Folder",
                                                               footage_root.absolutePath(),
                                                               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    if(!footage_folder.isEmpty()) {
        // Select footage files in most recent footage folder
        QDir footage_files = QDir(footage_folder, "*", QDir::Name, QDir::Files | QDir::NoDotAndDotDot);

        // Filter for allowed image types
        QStringList image_types;
        image_types << "*.jpg" << "*.jpeg" << "*.png";
        footage_files.setNameFilters(image_types);

        emit LoadSignal(footage_files.entryInfoList());

        //emit CacheSignal();
    }
}

void MainWindow::on_actionFlipVertically_triggered()
{
    zeitengine->control_mutex.lock();
    zeitengine->vertical_flip_flag = !zeitengine->vertical_flip_flag;
    zeitengine->control_mutex.unlock();
}
