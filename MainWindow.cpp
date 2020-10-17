#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QIcon>
#include <QPixmap>

#include "Network/NetworkConroller.h"
#include "Options.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_statusIconLabel(""),
    m_statusTextLabel(""),
    m_settingsButton(QIcon(":/Images/settings.png"), "", this),
    m_refreshButton(QIcon(":/Images/refresh.png"), "", this),
    m_downloadButton(QIcon(":/Images/download.png"), "", this),
    m_loadingGif(":/Images/loading.gif")
{
    setupLayout();
    setupController();
    retrieveFilesList();

    setStyleSheet("font-size: 20px");
    setMinimumSize(750, 600);
}

void MainWindow::setupLayout()
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_settingsButton.setIconSize(QSize(64, 64));
    m_settingsButton.setFixedSize(70, 70);
    m_settingsButton.setToolTip(QString::fromUtf8("Կարգավորումներ"));
    connect(&m_settingsButton, &QPushButton::pressed, this, &MainWindow::openOptions);

    m_refreshButton.setIconSize(QSize(64, 64));
    m_refreshButton.setFixedSize(70, 70);
    m_refreshButton.setToolTip(QString::fromUtf8("Թարմացնել"));
    connect(&m_refreshButton, &QPushButton::pressed, this, &MainWindow::retrieveFilesList);

    m_downloadButton.setIconSize(QSize(64, 64));
    m_downloadButton.setFixedSize(70, 70);
    m_downloadButton.setToolTip(QString::fromUtf8("Ներբեռնել"));
    connect(&m_downloadButton, &QPushButton::pressed, this, &MainWindow::startDownload);

    m_statusIconLabel.setFixedSize(70, 70);
    m_filesView.setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_filesView.setStyleSheet("font-size: 24px");

    QWidget* topWidget = new QWidget(this);
    QHBoxLayout* topWidgetLayout = new QHBoxLayout(this);

    topWidgetLayout->addWidget(&m_statusIconLabel, 1);
    topWidgetLayout->addWidget(&m_statusTextLabel, 3);
    topWidgetLayout->addWidget(&m_settingsButton, 1);
    topWidgetLayout->addWidget(&m_refreshButton, 1);
    topWidgetLayout->addWidget(&m_downloadButton, 1);
    topWidget->setLayout(topWidgetLayout);

    layout->addWidget(topWidget);
    layout->addWidget(&m_filesView);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::setupController()
{
    connect( &NetworkConroller::Instance(),
             &NetworkConroller::filesListRetrieved,
             this,
             &MainWindow::listRetrieved );

    connect( &NetworkConroller::Instance(),
             &NetworkConroller::progressChanged,
             this,
             &MainWindow::downloadProgress );

    connect( &NetworkConroller::Instance(),
             &NetworkConroller::unzippingFile,
             this,
             &MainWindow::unzipping );

    connect( &NetworkConroller::Instance(),
             &NetworkConroller::fileDownloaded,
             this,
             &MainWindow::downloadFinished );
}

void MainWindow::disableActions()
{
    m_settingsButton.setEnabled(false);
    m_refreshButton.setEnabled(false);
    m_downloadButton.setEnabled(false);
}

void MainWindow::enableActions()
{
    m_settingsButton.setEnabled(true);
    m_refreshButton.setEnabled(true);
    m_downloadButton.setEnabled(true);
}

void MainWindow::showSuccess()
{
    m_loadingGif.stop();

    m_statusIconLabel.setPixmap(QPixmap(":/Images/success.png").scaled(64, 64, Qt::KeepAspectRatio));
}

void MainWindow::showFailure()
{
    m_loadingGif.stop();

    m_statusIconLabel.setPixmap(QPixmap(":/Images/failure.png").scaled(64, 64, Qt::KeepAspectRatio));
}

void MainWindow::showLoading()
{
    m_loadingGif.setScaledSize(QSize(64, 64));

    m_statusIconLabel.setAttribute(Qt::WA_NoSystemBackground);
    m_statusIconLabel.setMovie(&m_loadingGif);
    m_loadingGif.start();
}

void MainWindow::retrieveFilesList()
{
    disableActions();
    m_statusTextLabel.setText(QString::fromUtf8("Ֆայլերի ցանկի ներբեռնում"));
    showLoading();
    NetworkConroller::Instance().getFilesList();
}

void MainWindow::listRetrieved(QStringList list, bool success)
{
    m_filesModel.setStringList(list);
    m_filesView.setModel(&m_filesModel);

    if (success) {
        showSuccess();
        m_statusTextLabel.setText(QString::fromUtf8("Ֆայլերի ցանկը"));
    }
    else {
        showFailure();
        m_statusTextLabel.setText(QString::fromUtf8("Չհաջոցվեց ներբեռնել ֆայլերի ցանկը"));
    }

    enableActions();
}

void MainWindow::startDownload()
{
    QString filename = m_filesView.currentIndex().data().toString();
    if (filename.isEmpty()) {
        m_statusTextLabel.setText(QString::fromUtf8("Ընտրեք ֆայլ ներբեռնելու համար"));
        return;
    }

    QString dir = QFileDialog::getExistingDirectory(this, QString::fromUtf8("Ներբեռնման դիրեկտորիայի ընտրություն"));
    if (!dir.isEmpty()) {
        disableActions();
        showLoading();
        m_statusTextLabel.setText("Starting download process");
        NetworkConroller::Instance().downloadFile(filename, dir);
    }
}

void MainWindow::downloadProgress(qint64 downloaded, qint64 total)
{
    qint64 percent = downloaded * 100 / total;
    m_statusTextLabel.setText(QString::fromUtf8("Պրոգրես ") + QString::number(percent) + "%");
}

void MainWindow::unzipping()
{
    m_statusTextLabel.setText(QString::fromUtf8("Զիփ ֆայլի բացում․․․"));
}

void MainWindow::downloadFinished(bool success, bool unzip_success)
{
    if (success && unzip_success) {
        showSuccess();
        m_statusTextLabel.setText(QString::fromUtf8(" ֆայլը հաջողությամբ ներբեռնվեց"));
    }
    else if(success && !unzip_success) {
        showFailure();
        m_statusTextLabel.setText(QString::fromUtf8("Չհաջողվեց բացել զիփ ֆայլը"));
    }
    else {
        showFailure();
        m_statusTextLabel.setText(QString::fromUtf8("Չհաջողվեց ներբեռնել ֆայլը"));
    }

    enableActions();
}

void MainWindow::openOptions()
{
    Options* options = new Options(
                NetworkConroller::Instance().host(),
                NetworkConroller::Instance().port(),
                this);

    connect(options, &Options::changed, this, [this](QString host, int port) {

        NetworkConroller::Instance().setHostAndPort(host, port);
        this->retrieveFilesList();
    });

    options->exec();
}
