#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include <QLineEdit>


class Options : public QDialog
{
    Q_OBJECT

public:

    Options(QString host, int port, QWidget* parent = nullptr);

signals:

    void changed(QString, int);

private:

    void setupEdits();

    void setupLayout();

private slots:

    void change();

    void cancel();

private:

    QLineEdit m_ipEdit;

    QLineEdit m_portEdit;
};

#endif // OPTIONS_H
