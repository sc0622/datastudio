#include "precomp.h"
#include "ComIcdSystemUi.h"
#include "jtinyxml.h"

ComIcdSystemUi::ComIcdSystemUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);

    QGroupBox *groupBox = new QGroupBox(QStringLiteral("����ICDϵͳ"), this);
    layoutMain->addWidget(groupBox);

    QVBoxLayout *layoutGroup = new QVBoxLayout(groupBox);

    QGroupBox *groupVersion = new QGroupBox(QStringLiteral("�汾��Ϣ"), this);
    layoutGroup->addWidget(groupVersion);

    QFormLayout *formLayoutVersion = new QFormLayout(groupVersion);
    formLayoutVersion->setLabelAlignment(Qt::AlignRight);

    JXmlElement emApp = JXml::elementByPath(QString::fromStdString(jframeFacade()->configDirPath())
                                            .append("/jframe_global.xml"), "app");
    if (!emApp.isNull()) {
        std::string sVal;
        if (emApp->QueryStringAttribute("version", &sVal) == TIXML_SUCCESS) {
            formLayoutVersion->addRow(QStringLiteral("����汾�ţ�"),
                                      new QLabel(QString::fromStdString(sVal), groupVersion));
        }
    }

    layoutGroup->addStretch();

    QGroupBox *groupBoxAuthor = new QGroupBox(QStringLiteral("ϵͳά����Ա"), this);
    layoutMain->addWidget(groupBoxAuthor);

    QFormLayout *formLayoutAuthor = new QFormLayout(groupBoxAuthor);

    formLayoutAuthor->addRow(QStringLiteral("���༭ϵͳ����"),
                             new QLabel(QStringLiteral("huangjh��Tel��18108071563"), this));
    formLayoutAuthor->addRow(QStringLiteral("��ƫ�����á���"),
                             new QLabel(QStringLiteral("liaokh��Tel��18583834930"), this));
    formLayoutAuthor->addRow(QStringLiteral("�����ϵͳ����"),
                             new QLabel(QStringLiteral("liaokh��Tel��18583834930"), this));
    formLayoutAuthor->addRow(QStringLiteral("������ϵͳ����"),
                             new QLabel(QStringLiteral("liaokh��Tel��18583834930"), this));
    formLayoutAuthor->addRow(QStringLiteral("�����ݷ�������"),
                             new QLabel(QStringLiteral("liaokh��Tel��18583834930"), this));
}

ComIcdSystemUi::~ComIcdSystemUi()
{

}
