#include "precomp.h"
#include "ComIcdSystemUi.h"
#include "jtinyxml.h"

ComIcdSystemUi::ComIcdSystemUi(IJAttempter &attempter, QWidget *parent)
    : QWidget(parent)
    , d_attempter(attempter)
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);

    QGroupBox *groupBox = new QGroupBox(QStringLiteral("关于ICD系统"), this);
    layoutMain->addWidget(groupBox);

    QVBoxLayout *layoutGroup = new QVBoxLayout(groupBox);

    QGroupBox *groupVersion = new QGroupBox(QStringLiteral("版本信息"), this);
    layoutGroup->addWidget(groupVersion);

    QFormLayout *formLayoutVersion = new QFormLayout(groupVersion);
    formLayoutVersion->setLabelAlignment(Qt::AlignRight);

    JXmlElement emApp = JXml::elementByPath(QString::fromStdString(jframeFacade()->configDirPath())
                                            .append("/jframe_global.xml"), "app");
    if (!emApp.isNull()) {
        std::string sVal;
        if (emApp->QueryStringAttribute("version", &sVal) == TIXML_SUCCESS) {
            formLayoutVersion->addRow(QStringLiteral("软件版本号："),
                                      new QLabel(QString::fromStdString(sVal), groupVersion));
        }
    }

    layoutGroup->addStretch();

    QGroupBox *groupBoxAuthor = new QGroupBox(QStringLiteral("系统维护人员"), this);
    layoutMain->addWidget(groupBoxAuthor);

    QFormLayout *formLayoutAuthor = new QFormLayout(groupBoxAuthor);

    formLayoutAuthor->addRow(QStringLiteral("【编辑系统】："),
                             new QLabel(QStringLiteral("huangjh，Tel：18108071563"), this));
    formLayoutAuthor->addRow(QStringLiteral("【偏好设置】："),
                             new QLabel(QStringLiteral("liaokh，Tel：18583834930"), this));
    formLayoutAuthor->addRow(QStringLiteral("【监控系统】："),
                             new QLabel(QStringLiteral("liaokh，Tel：18583834930"), this));
    formLayoutAuthor->addRow(QStringLiteral("【仿真系统】："),
                             new QLabel(QStringLiteral("liaokh，Tel：18583834930"), this));
    formLayoutAuthor->addRow(QStringLiteral("【数据分析】："),
                             new QLabel(QStringLiteral("liaokh，Tel：18583834930"), this));
}

ComIcdSystemUi::~ComIcdSystemUi()
{

}
