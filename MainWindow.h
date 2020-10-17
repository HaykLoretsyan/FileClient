#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QLabel>
#include <QStringListModel>
#include <QPushButton>
#include <QMovie>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);

private:

    void setupLayout();

    void setupController();

    void disableActions();

    void enableActions();

    void showSuccess();

    void showFailure();

    void showLoading();

private slots:

    void retrieveFilesList();

    void listRetrieved(QStringList list, bool success);

    void startDownload();

    void downloadProgress(qint64 downloaded, qint64 total);

    void unzipping();

    void downloadFinished(bool success, bool unzip_success);

    void openOptions();

private:

    QLabel m_statusIconLabel;

    QLabel m_statusTextLabel;

    QListView m_filesView;

    QStringListModel m_filesModel;

    QPushButton m_settingsButton;

    QPushButton m_refreshButton;

    QPushButton m_downloadButton;

    QMovie m_loadingGif;
};

#endif // MAINWINDOW_H
