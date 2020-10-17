#include "Options.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDialogButtonBox>

Options::Options(QString host, int port, QWidget *parent) :
    QDialog(parent),
    m_ipEdit(host),
    m_portEdit(QString::number(port))
{
    setModal(true);
    setupEdits();
    setupLayout();
}

void Options::setupEdits()
{
    QString IpRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression IpRegex ("^" + IpRange
                                + "(\\." + IpRange + ")"
                                + "(\\." + IpRange + ")"
                                + "(\\." + IpRange + ")$");

    m_ipEdit.setValidator(new QRegularExpressionValidator(IpRegex, this));
    m_portEdit.setValidator(new QIntValidator(1, 65535, this));

    m_ipEdit.setPlaceholderText(QString::fromUtf8("IP հասցե"));
    m_portEdit.setPlaceholderText(QString::fromUtf8("Պորտ"));
}

void Options::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QWidget* edit_widget = new QWidget(this);
    QHBoxLayout* edit_layout = new QHBoxLayout(this);

    edit_layout->addWidget(&m_ipEdit, 3);
    edit_layout->addWidget(&m_portEdit, 1);

    edit_widget->setLayout(edit_layout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(change()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

    layout->addWidget(edit_widget);
    layout->addWidget(buttonBox);

    setLayout(layout);
}

void Options::change()
{
    changed(m_ipEdit.text(), m_portEdit.text().toInt());
    accept();
}

void Options::cancel()
{
    reject();
}
