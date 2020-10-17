#include "NetworkConroller.h"

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QProcess>
#include <QFileInfo>
#include <QCoreApplication>


NetworkConroller::NetworkConroller() : m_accessManager(this), m_process(this)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MapClient", "MapClient");
    m_host = settings.value("host", "127.0.0.1").toString();
    m_port = settings.value("port", 4000).toInt();

    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int code, QProcess::ExitStatus exitStatus) {

        QFile file (m_path);
        if (file.exists())
            file.remove();

        if(code == 0 && exitStatus == QProcess::NormalExit) {

            emit fileDownloaded(true, true);
        }
        else {

            emit fileDownloaded(true, false);
        }
    });
}

void NetworkConroller::setHostAndPort(QString host, int port)
{
    m_host = host;
    m_port = port;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MapClient", "MapClient");
    settings.setValue("host", host);
    settings.setValue("port", port);
}

QString NetworkConroller::host() const
{
    return m_host;
}

int NetworkConroller::port() const
{
    return m_port;
}

void NetworkConroller::getFilesList()
{
    QUrl url("http://" + m_host + ":" + QString::number(m_port)  + "/list");
    QNetworkRequest request(url);

    auto reply = m_accessManager.get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {

        if(reply->error() == QNetworkReply::NoError) {

            auto data = reply->readAll();
            auto filesList = parseJson(data);

            emit filesListRetrieved(filesList, true);
        }
        else {

            emit filesListRetrieved(QStringList(), false);
        }

        reply->deleteLater();
    });
}

void NetworkConroller::downloadFile(QString filename, QString downloadPath)
{
    QUrl url("http://" + m_host + ":" + QString::number(m_port)  + "/map");
    QUrlQuery query;

    query.addQueryItem("name", filename);
    url.setQuery(query.query());

    QNetworkRequest request(url);

    auto reply = m_accessManager.get(request);
    setupDownload(reply);
    saveDownloadedFile(reply, downloadPath, filename);
}

QStringList NetworkConroller::parseJson(const QByteArray &json)
{
    QStringList result;

    auto jsonDocument = QJsonDocument::fromJson(json);
    auto jsonArray = jsonDocument.array();

    foreach (const QJsonValue & value, jsonArray)
    {
        result.push_back(value.toString());
    }

    return result ;
}

void NetworkConroller::setupDownload(QNetworkReply *reply)
{
    connect(reply, &QNetworkReply::downloadProgress, this, &NetworkConroller::progressChanged);
}

void NetworkConroller::saveDownloadedFile(QNetworkReply *reply, QString savePath, QString filename)
{
    QByteArray* buffer = new QByteArray();

    connect(reply, &QNetworkReply::readyRead, [buffer, reply]() {

        buffer->append(reply->readAll());
    });

    connect(reply, &QNetworkReply::finished, [=]() {

        if(reply->error() != QNetworkReply::NoError) {

            emit fileDownloaded(false, false);
            return;
        }

        QFile* file = new QFile(savePath + "/" + filename, reply);
        file->open(QIODevice::WriteOnly);

        file->write(*buffer);

        file->flush();
        file->close();

        if (filename.size() > 4 && filename.mid(filename.size() - 4) == ".zip") {

            this->unzipAndDelete(savePath + "/" + filename);
        }
        else {

            if(reply->error() == QNetworkReply::NoError) {

                emit fileDownloaded(true, true);
            }
            else {

                emit fileDownloaded(false, false);
            }
        }

        reply->deleteLater();
    });
}

void NetworkConroller::unzipAndDelete(QString path)
{
    emit unzippingFile();

    QString program = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + "\\7-Zip\\7z.exe";
    if (!QFile::exists(program)) {
        emit fileDownloaded(true, false);
        return;
    }

    m_path = path;
    m_process.setWorkingDirectory(QDir::toNativeSeparators(QFileInfo(path).absoluteDir().absolutePath()));
    m_process.start(
                program,
                QStringList() << "x"
                << QDir::toNativeSeparators(path)
                );
}
