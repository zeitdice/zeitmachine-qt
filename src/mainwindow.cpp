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

    videoWidget = new GLVideoWidget(this);
    this->setCentralWidget(videoWidget);

    progressbar = new QProgressBar;
    progressbar->hide();
    ui->statusbar->addPermanentWidget(progressbar, 100);

    EnableControls(false);
    this->ui->actionSettings->setVisible(false);
    this->ui->actionLoop->setChecked(true);
    this->ui->actionOpen->setEnabled(true);

    InitializeZeitdiceDirectory();

    zeitengine = new ZeitEngine(videoWidget);
    zeitengine->moveToThread(&engineThread);
    engineThread.start();

    connect(&engineThread, &QThread::finished, zeitengine, &QObject::deleteLater);

    connect(zeitengine, &ZeitEngine::VideoUpdated, videoWidget, &GLVideoWidget::DelegateUpdate);
    connect(zeitengine, &ZeitEngine::VideoConfigurationUpdated, videoWidget, &GLVideoWidget::ConfigureVideo);
    connect(zeitengine, &ZeitEngine::ControlsEnabled, this, &MainWindow::EnableControls);
    connect(zeitengine, &ZeitEngine::MessageUpdated, this, &MainWindow::UpdateMessage);
    connect(zeitengine, &ZeitEngine::ProgressUpdated, this, &MainWindow::UpdateProgress);

    connect(this, &MainWindow::LoadSignal, zeitengine, &ZeitEngine::Load);
    connect(this, &MainWindow::CacheSignal, zeitengine, &ZeitEngine::Cache);
    connect(this, &MainWindow::RefreshSignal, zeitengine, &ZeitEngine::Refresh);
    connect(this, &MainWindow::PlaySignal, zeitengine, &ZeitEngine::Play);
    connect(this, &MainWindow::ExportSignal, zeitengine, &ZeitEngine::Export);
}

MainWindow::~MainWindow()
{
    engineThread.quit();
    engineThread.wait();
    delete ui;
}

void MainWindow::InitializeZeitdiceDirectory()
{
    QFile file(".zeitdir");

    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        if(!file.atEnd()) {
            QByteArray path_bytes = file.readAll();
            QString path_string = QString(path_bytes);
            persistent_open_dir = QDir(path_string);
        }

        file.close();
    } else {

        persistent_open_dir = QDir(QCoreApplication::applicationDirPath());

        // Optional path correction for different operating systems
        #if defined(Q_OS_WIN)

        #elif defined(Q_OS_MAC)
            if (persistent_open_dir.dirName() == "MacOS") {
                persistent_open_dir.cdUp();
                persistent_open_dir.cdUp();
                persistent_open_dir.cdUp();
            }
        #endif

        PersistZeitdiceDirectory(persistent_open_dir);
    }
}

void MainWindow::PersistZeitdiceDirectory(const QDir dir) {
    QFile file(".zeitdir");

    if(file.open(QIODevice::WriteOnly)) {
        QByteArray dir_bytearray = dir.absolutePath().toUtf8();
        file.write(dir_bytearray);
    }

    file.close();
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
    zeitengine->control_mutex.unlock();

    emit PlaySignal();
}

void MainWindow::on_actionLoop_triggered()
{
    zeitengine->control_mutex.lock();
    zeitengine->loop_flag = !zeitengine->loop_flag;
    zeitengine->control_mutex.unlock();
}

void MainWindow::on_actionStop_triggered()
{
    zeitengine->control_mutex.lock();
    zeitengine->stop_flag = true;
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

    case ZEIT_RATE_23_976:  // Hidden but might be included in interface later
    case ZEIT_RATE_25p:
        new_rate = ZEIT_RATE_30p;
        new_rate_label = "30p (NTSC TV equivalent)";
        break;

    case ZEIT_RATE_29_97:   // Hidden but might be included in interface later
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
    default:
        new_rate = ZEIT_RATE_24p;
        new_rate_label = "24p (Cinema/Film equivalent)";
    break;

    }

    zeitengine->configured_framerate = new_rate;

    zeitengine->control_mutex.unlock();

    ui->statusbar->showMessage("Framerate set to " + new_rate_label);
}


void MainWindow::on_actionAbout_triggered()
{
    about.show();
}

void MainWindow::on_actionVignette_triggered(bool checked)
{
    zeitengine->control_mutex.lock();
    zeitengine->filter_flag = checked ? ZEIT_FILTER_VIGNETTE : ZEIT_FILTER_NONE;
    zeitengine->control_mutex.unlock();

    UncheckOtherFilters(this->ui->actionVignette);

    RefreshSignal();
}

void MainWindow::on_actionBlackWhite_triggered(bool checked)
{
    zeitengine->control_mutex.lock();
    zeitengine->filter_flag = checked ? ZEIT_FILTER_BLACKWHITE : ZEIT_FILTER_NONE;
    zeitengine->control_mutex.unlock();

    UncheckOtherFilters(this->ui->actionBlackWhite);

    RefreshSignal();
}

void MainWindow::on_actionSepia_triggered(bool checked)
{
    zeitengine->control_mutex.lock();
    zeitengine->filter_flag = checked ? ZEIT_FILTER_SEPIA : ZEIT_FILTER_NONE;
    zeitengine->control_mutex.unlock();

    UncheckOtherFilters(this->ui->actionSepia);

    RefreshSignal();
}

void MainWindow::on_actionHipstagram_triggered(bool checked)
{
    zeitengine->control_mutex.lock();
    zeitengine->filter_flag = checked ? ZEIT_FILTER_HIPSTAGRAM : ZEIT_FILTER_NONE;
    zeitengine->control_mutex.unlock();

    UncheckOtherFilters(this->ui->actionHipstagram);

    RefreshSignal();
}

void MainWindow::EnableControls(const bool lock)
{
    this->ui->actionOpen->setEnabled(lock);
    this->ui->actionPlay->setEnabled(lock);
    this->ui->actionLoop->setEnabled(lock);
    this->ui->actionStop->setEnabled(lock);
    this->ui->actionCycleFramerates->setEnabled(lock);
    this->ui->actionFlipX->setEnabled(lock);
    this->ui->actionFlipY->setEnabled(lock);
    this->ui->actionRotateCCW->setEnabled(lock);
    this->ui->actionRotateCW->setEnabled(lock);
    this->ui->actionVignette->setEnabled(lock);
    this->ui->actionBlackWhite->setEnabled(lock);
    this->ui->actionSepia->setEnabled(lock);
    this->ui->actionHipstagram->setEnabled(lock);
    this->ui->actionMovie->setEnabled(lock);
}

void MainWindow::on_actionMovie_triggered()
{
    QHash<QString, QString> video_extension_filters;

    video_extension_filters["mp4"] = "MPEG-4 [.mp4] - Recommended Format (*.mp4)";
    video_extension_filters["mkv"] = "Matroska [.mkv] (*.mkv)";
    video_extension_filters["mov"] = "Quicktime [.mov] (*.mov)";

    QFileDialog dialog(this);

    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(persistent_open_dir);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setLabelText(QFileDialog::Accept, "Export");
    dialog.setNameFilters(video_extension_filters.values());
    dialog.setNameFilterDetailsVisible(true);
    dialog.setOption(QFileDialog::HideNameFilterDetails, false);
    dialog.setOption(QFileDialog::DontConfirmOverwrite, true);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setWindowTitle("Choose an export filename and location");

    dialog.selectFile("export.mp4");
    dialog.selectNameFilter(video_extension_filters["mp4"]);

    if(dialog.exec()) {
        QString export_file = dialog.selectedFiles()[0];
        QString export_format = dialog.selectedNameFilter();

        if(!export_file.isEmpty()) {
            QStringList extensions(video_extension_filters.keys());
            QString ext_str = "\\.(?:" + extensions.join("|") + ")";
            QRegularExpression rx("(" + ext_str + "$|(?<!" + ext_str + ")$)");

            export_file.replace(rx, "." + video_extension_filters.key(export_format));

            QFileInfo export_fileinfo(export_file);

            if(export_fileinfo.exists()) {
                QMessageBox confirmation(this);
                QPushButton* abort = confirmation.addButton("Don't overwrite",
                                                            QMessageBox::RejectRole);

                confirmation.addButton("Overwrite anyway", QMessageBox::AcceptRole);
                confirmation.setDefaultButton(abort);
                confirmation.setWindowTitle("Warning");
                confirmation.setText("A file with this name already exists!");

                confirmation.exec();

                if((QPushButton*)confirmation.clickedButton() == abort) {
                    return;
                }
            }

            EnableControls(false);

            zeitengine->control_mutex.lock();
            zeitengine->stop_flag = true;
            zeitengine->control_mutex.unlock();

            emit ExportSignal(QFileInfo(export_file));
        }
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
    QString dir_name = QFileDialog::getExistingDirectory(this,
                                                         "Open Footage Folder",
                                                         persistent_open_dir.absolutePath(),
                                                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    if(!dir_name.isEmpty()) {

        persistent_open_dir = QDir(dir_name);
        persistent_open_dir.cdUp();
        PersistZeitdiceDirectory(persistent_open_dir);

        QDir dir = QDir(dir_name);

        // Filter all supported footage from the opened folder

        dir.setFilter(QDir::Files);

        QStringList image_extension_filters{"*.jpg","*.jpeg","*.png"};
        dir.setNameFilters(image_extension_filters);

        QFileInfoList files = dir.entryInfoList();

        // Remove 0 byte images
        for(auto files_iterator = files.begin(); files_iterator != files.end(); ++files_iterator) {
            if((*files_iterator).size() == 0) {
                files.removeAt(files_iterator - files.begin());
            }
        }

        // Break and gracefully stop if there is no supported footage available now
        if(files.length() < 1) {
            QMessageBox::information(this,
                                     "No supported footage found",
                                     "Your chosen folder does not contain images, or does not contain any of a format that is supported (.jpg/.jpeg/.png).");
            return;
        }

        // Naturally sort the images by filename

        QCollator collator;
        collator.setNumericMode(true);
        collator.setCaseSensitivity(Qt::CaseInsensitive);

        std::sort(files.begin(), files.end(), [&](const QFileInfo& a, const QFileInfo& b) {

            return collator.compare(a.baseName(), b.baseName()) < 0;
        });

        // Stop all zeitengine activity and reset all flags
        zeitengine->control_mutex.lock();
        zeitengine->flip_x_flag = true;
        zeitengine->flip_y_flag = true;
        zeitengine->rotate_90d_cw_flag = false;
        zeitengine->filter_flag = ZEIT_FILTER_NONE;
        zeitengine->configured_framerate = ZEIT_RATE_24p;
        zeitengine->stop_flag = true;
        zeitengine->control_mutex.unlock();

        // Reflect the reset in the UI as well
        EnableControls(true);
        UncheckOtherFilters(NULL);

        // Send the list of files to the zeitengine
        emit LoadSignal(files);
    }
}

void MainWindow::on_actionFlipX_triggered()
{
    zeitengine->control_mutex.lock();
    if(zeitengine->rotate_90d_cw_flag) {
        zeitengine->flip_y_flag = !zeitengine->flip_y_flag;
    } else {
        zeitengine->flip_x_flag = !zeitengine->flip_x_flag;
    }
    zeitengine->control_mutex.unlock();

    RefreshSignal();
}

void MainWindow::on_actionFlipY_triggered()
{
    zeitengine->control_mutex.lock();
    if(zeitengine->rotate_90d_cw_flag) {
        zeitengine->flip_x_flag = !zeitengine->flip_x_flag;
    } else {
        zeitengine->flip_y_flag = !zeitengine->flip_y_flag;
    }
    zeitengine->control_mutex.unlock();

    RefreshSignal();
}

void MainWindow::on_actionRotateCCW_triggered()
{
    zeitengine->control_mutex.lock();
    if(!zeitengine->rotate_90d_cw_flag) {
        zeitengine->flip_x_flag = !zeitengine->flip_x_flag;
        zeitengine->flip_y_flag = !zeitengine->flip_y_flag;
    }
    zeitengine->rotate_90d_cw_flag = !zeitengine->rotate_90d_cw_flag;
    zeitengine->control_mutex.unlock();

    RefreshSignal();
}

void MainWindow::on_actionRotateCW_triggered()
{
    zeitengine->control_mutex.lock();
    if(zeitengine->rotate_90d_cw_flag) {
        zeitengine->flip_x_flag = !zeitengine->flip_x_flag;
        zeitengine->flip_y_flag = !zeitengine->flip_y_flag;
    }
    zeitengine->rotate_90d_cw_flag = !zeitengine->rotate_90d_cw_flag;
    zeitengine->control_mutex.unlock();

    RefreshSignal();
}
