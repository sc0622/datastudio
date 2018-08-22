#ifndef JPROGRESSDILOG_H
#define JPROGRESSDILOG_H

#include <QDialog>
#include "icdwidget_global.h"

template<typename T> class QFuture;

namespace Icd {

class JProgressDialogPrivate;

class ICDWIDGET_EXPORT JProgressDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(int progressValue READ progressValue WRITE setProgressValue NOTIFY progressValueChanged)
    Q_PROPERTY(bool progressVisible READ progressVisible WRITE setProgressVisible NOTIFY progressVisibleChanged)
    Q_PROPERTY(QString acceptText READ acceptText WRITE setAcceptText NOTIFY acceptTextChanged)
    Q_PROPERTY(bool acceptVisible READ acceptVisible WRITE setAcceptVisible NOTIFY acceptVisibleChanged)
    Q_PROPERTY(QString cancelText READ cancelText WRITE setCancelText NOTIFY cancelTextChanged)
    Q_PROPERTY(bool cancelVisible READ cancelVisible WRITE setCancelVisible NOTIFY cancelVisibleChanged)
public:
    explicit JProgressDialog(QWidget *parent = 0);
    ~JProgressDialog();

    QString message() const;
    int progressMinimum() const;
    int progressMaximum() const;
    int progressValue() const;
    bool progressVisible() const;
    QString acceptText() const;
    bool acceptVisible() const;
    QString cancelText() const;
    bool cancelVisible() const;

    void waitForFinished();
    void setFuture(const QFuture<bool> &future);
    bool isCanceled() const;
    bool futureResult() const;

    void setInvertedAppearance(bool invert);
    bool invertedAppearance() const;

signals:
    void messageChanged(const QString &message);
    void progressValueChanged(int value);
    void progressVisibleChanged(bool visible);
    void acceptTextChanged(const QString &text);
    void acceptVisibleChanged(bool visible);
    void cancelTextChanged(const QString &text);
    void cancelVisibleChanged(bool visible);
    void canceled();
    void finished();

public slots:
    void setMessage(const QString &message);
    void setProgressRange(int minimum, int maximum);
    void setProgressValue(int value);
    void setProgressVisible(bool visible);
    void setAcceptText(const QString &text);
    void setAcceptVisible(bool visible);
    void setCancelText(const QString &text);
    void setCancelVisible(bool visible);

private:
    Q_DISABLE_COPY(JProgressDialog)
    J_DECLARE_PRIVATE(JProgressDialog)
};

} // end of namespace Icd

#endif // JPROGRESSDILOG_H
