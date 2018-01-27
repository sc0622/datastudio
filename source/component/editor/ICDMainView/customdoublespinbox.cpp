#include "customdoublespinbox.h"

CustomDoubleSpinbox::CustomDoubleSpinbox(QWidget *parent)
    : QDoubleSpinBox(parent)
{
}

CustomDoubleSpinbox::~CustomDoubleSpinbox()
{
}

QString CustomDoubleSpinbox::textFromValue(double val) const
{
    std::string content = QDoubleSpinBox::textFromValue(val).toStdString();
    std::string::size_type pos = content.find_last_not_of("0");
    if (std::string::npos != pos) {
        content = content.substr(0, ('.' == content[pos]) ? pos : pos + 1);
    }

    return QString::fromStdString(content);
}

QValidator::State CustomDoubleSpinbox::validate(QString &input, int &pos) const
{
    QValidator::State state = QDoubleSpinBox::validate(input, pos);
    if (QValidator::Invalid != state) {
        return state;   // ����QDoubleSpinBoxҪ��
    }
    QRegExp regExp("\\d{1,}.\\d{1,}");
    QRegExpValidator validator(regExp);
    // ����Ӧ��Ϊ������
    state = validator.validate(input, pos);
    if (QValidator::Acceptable != state) {
        return state;   // �������ݲ�Ϊ��ֵ
    }
    double _dValue = input.toDouble();
    if (_dValue > this->maximum() || _dValue < this->minimum()) {
        return (state = QValidator::Invalid);   // ����
    }
    int _dotPos = input.indexOf(".");
    Q_ASSERT(-1 != _dotPos);
    QString _integer = input.left(_dotPos);
    QString _decimals = input.mid(_dotPos + strlen("."));
    int _bound = this->decimals();
    if (_decimals.length() > _bound) {  // С���������ݳ��ȳ���
        /*
        // �����ݽ����޶�λ����������
        if (_decimals[_bound] > '4') {
            _decimals.truncate(_bound + 1);
            unsigned long long _value = _decimals.toULongLong() + 5;
            QString _tmp = QString::number(_value);
            if (_tmp.length() > _decimals.length()) {
                // ��Ҫ���������ֽ�λ
                _value = _integer.toULongLong() + 1;
                _integer = QString::number(_value);
                _decimals = _tmp.mid(1);
            } else {
                _decimals = _tmp;
            }
        }*/
        _decimals.truncate(_bound);
        input = _integer + "." + _decimals;
        /*
        // ���ݿ��ܴ��ڽ�λ�����ٴν��г����ж�
        _dValue = input.toDouble();
        if (_dValue > this->maximum() || _dValue < this->minimum()) {
            state = QValidator::Invalid;
        } else {
            state = QValidator::Acceptable;
            pos = input.length();
        }*/
    }

    return state;
}
