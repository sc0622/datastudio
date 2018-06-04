#include "precomp.h"
#include "JEncryptModel.h"

namespace icdmeta {

// class JEncryptModelPrivate

class JEncryptModelPrivate
{
public:
    JEncryptModelPrivate(JEncryptModel *q)
        : J_QPTR(q)
        , active(false)
    {

    }

    void init();
    QString querySN();
    QString generateSN();
    static QString generateSN(const QString &input);

private:
    J_DECLARE_PUBLIC(JEncryptModel)
    QString cpuId;
    QString hdSeries;
    QString macAddress;
    QString serialNumber;
    bool active;
};

void JEncryptModelPrivate::init()
{
    Q_Q(JEncryptModel);

    // CPU ID
    jencrypt::JCpuId _cpuId;
    while (_cpuId.hasNext()) {
        const std::string id = _cpuId.next();
        if (!id.empty()) {
            cpuId = QString::fromStdString(id);
            break;
        }
    }

    // HD serial number
    jencrypt::JDiskSerial diskSerial;
    while (diskSerial.hasNext()) {
        const std::string serial = diskSerial.next();
        if (!serial.empty()) {
            hdSeries = QString::fromStdString(serial);
            break;
        }
    }

    // MAC address
    jencrypt::JMacAddress mac;
    while (mac.hasNext()) {
        const std::string addr = mac.next();
        if (!addr.empty()) {
            macAddress = QString::fromStdString(addr);
            break;
        }
    }

    //
    if (!cpuId.isEmpty()/* && !hdSeries.isEmpty()*/ && !macAddress.isEmpty()) {
        //
        QSettings settings;
        settings.beginGroup(QCoreApplication::applicationVersion() + "/Settings/License");
        const QString serial = settings.value("SerialNumber").toString();
        settings.endGroup();
        //
        const QString output = querySN();
        if (output.isEmpty()) {
            //
            q->setSerialNumber(serial);
            //
            if (!serial.isEmpty()) {
                const QString sn = generateSN();
                if (sn == serial) {
                    active = true;
                }
            }
        } else {
            //
            q->setSerialNumber(output);
            //
            if (!serial.isEmpty()) {
                if (serial == output) {
                    active = true;
                }
            }
        }
    }
}

QString JEncryptModelPrivate::querySN()
{
    if (cpuId.isEmpty()/* || hdSeries.isEmpty()*/ || macAddress.isEmpty()) {
        return QString();
    }

    //
    const QString input = cpuId + hdSeries + macAddress;

    //TEST
    const QString output = generateSN(input);

    return output;
}

QString JEncryptModelPrivate::generateSN()
{
    const QString input = cpuId + hdSeries + macAddress;
    if (input.isEmpty()) {
        return QString();
    }

    jencrypt::JMd5 md5;
    md5.update(input.toStdString());

    std::string _output = md5.toString();
    if (_output.empty()) {
        return QString();
    }

    // »°≈º–Ú¡–
    std::string output;
    for (size_t i = 0; i < _output.size() / 2; ++i) {
        output.append(1, _output[i * 2 + 1]);
    }

    //
    for (size_t i = 4; i < output.size(); i += 5) {
        output.insert(i, 1, '-');
    }

    return QString::fromStdString(output);
}

QString JEncryptModelPrivate::generateSN(const QString &input)
{
    if (input.isEmpty()) {
        return QString();
    }

    jencrypt::JMd5 md5;
    md5.update(input.toStdString());

    std::string _output = md5.toString();
    if (_output.empty()) {
        return QString();
    }

    // o
    std::string output;
    for (size_t i = 0; i < _output.size() / 2; ++i) {
        output.append(1, _output[i * 2 + 1]);
    }

    //
    for (size_t i = 4; i < output.size(); i += 5) {
        output.insert(i, 1, '-');
    }

    return QString::fromStdString(output);
}

// class JEncryptModel

J_QML_IMPLEMENT_SINGLE_INSTANCE(JEncryptModel, QQmlEngine::CppOwnership, IcdCore)

JEncryptModel::JEncryptModel(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JEncryptModelPrivate(this))
{
    Q_D(JEncryptModel);
    d->init();
}

JEncryptModel::~JEncryptModel()
{
    Q_D(JEncryptModel);
    delete d;
}

void JEncryptModel::registerQmlType()
{
    IcdMetaRegisterSingletonType3(JEncryptModel);
}

QString JEncryptModel::cpuId() const
{
    Q_D(const JEncryptModel);
    return d->cpuId;
}

QString JEncryptModel::hdSeries() const
{
    Q_D(const JEncryptModel);
    return d->hdSeries;
}

QString JEncryptModel::macAddress() const
{
    Q_D(const JEncryptModel);
    return d->macAddress;
}

QString JEncryptModel::serialNumber() const
{
    Q_D(const JEncryptModel);
    return d->serialNumber;
}

bool JEncryptModel::isActive() const
{
    Q_D(const JEncryptModel);
    return d->active;
}

bool JEncryptModel::activate()
{
    Q_D(JEncryptModel);
    const QString sn = d->generateSN();
    if (sn != d->serialNumber) {
        return false;
    }

    //
    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion() + "/Settings/License");
    settings.setValue("SerialNumber", d->serialNumber);
    settings.endGroup();

    setActive(true);

    return true;
}

bool JEncryptModel::deactivate()
{
    //
    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion() + "/Settings/License");
    settings.remove("SerialNumber");
    settings.endGroup();

    setActive(false);

    return true;
}

void JEncryptModel::setSerialNumber(const QString &value)
{
    Q_D(JEncryptModel);
    if (value != d->serialNumber) {
        d->serialNumber = value;
        emit serialNumberChanged(value);
    }
}

void JEncryptModel::setActive(bool value)
{
    Q_D(JEncryptModel);
    if (value != d->active) {
        d->active = value;
        emit activeChanged(value);
    }
}

void JEncryptModel::reset()
{

}

}
