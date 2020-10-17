#ifndef NETWORKCONROLLER_H
#define NETWORKCONROLLER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QProcess>

class NetworkConroller : public QObject
{
    Q_OBJECT

private:

    NetworkConroller();

public:

    static NetworkConroller& Instance() {

        static NetworkConroller instance;
        return instance;
    }

    void setHostAndPort(QString host, int port);

    QString host() const;

    int port() const;

    void getFilesList();

    void downloadFile(QString filename, QString downloadPath);

signals:

    void filesListRetrieved(QStringList, bool);

    void fileDownloaded(bool, bool);

    void unzippingFile();

    void progressChanged(qint64, qint64);

private:

    QStringList parseJson(const QByteArray& json);

    void setupDownload(QNetworkReply* reply);

    void saveDownloadedFile(QNetworkReply* reply, QString savePath, QString filename);

    void unzipAndDelete(QString path);

private:

    QNetworkAccessManager m_accessManager;

    QString m_host;

    int m_port;

    QProcess m_process;

    QString m_path;
};

#endif // NETWORKCONROLLER_H
