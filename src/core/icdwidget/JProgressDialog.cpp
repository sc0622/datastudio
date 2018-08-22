#include "precomp.h"
#include "JProgressDialog.h"

namespace Icd {

// class JProgressDialogPrivate

class JProgressDialogPrivate
{
public:
    JProgressDialogPrivate(JProgressDialog *q)
        : J_QPTR(q)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JProgressDialog)
    QProgressBar *progressBar;
    QLabel *labelMsg;
    QPushButton *buttonAccept;
    QPushButton *buttonCancel;
    QFutureWatcher<bool> watcher;
};

void JProgressDialogPrivate::init()
{
    Q_Q(JProgressDialog);
    //
    q->resize(450, 150);
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(q);
    //
    progressBar = new QProgressBar(q);
    progressBar->setRange(0, 100);
    progressBar->setTextVisible(false);
    progressBar->setInvertedAppearance(true);
    vertLayoutMain->addWidget(progressBar);
    //
    labelMsg = new QLabel(q);
    vertLayoutMain->addWidget(labelMsg);
    //
    buttonAccept = new QPushButton(JProgressDialog::tr("Ok"), q);
    buttonCancel = new QPushButton(JProgressDialog::tr("Cancel"), q);
    buttonAccept->setVisible(false);
    buttonAccept->setMinimumWidth(80);
    buttonCancel->setMinimumWidth(80);

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();
    horiLayoutBottom->addWidget(buttonAccept);
    horiLayoutBottom->addWidget(buttonCancel);

    QObject::connect(progressBar, &QProgressBar::valueChanged,
                     q, &JProgressDialog::progressValueChanged);
    QObject::connect(buttonAccept, &QPushButton::clicked, q, &QDialog::accept);
    QObject::connect(buttonCancel, &QPushButton::clicked, q, &QDialog::reject);

    //
    QObject::connect(&watcher, &QFutureWatcher<bool>::canceled, q, [=](){
        watcher.cancel();
        watcher.waitForFinished();
        q->hide();
        q->reject();
    });
    QObject::connect(q, &JProgressDialog::rejected, q, [=](){
        watcher.cancel();
        watcher.waitForFinished();
    });
    QObject::connect(&watcher, &QFutureWatcher<bool>::canceled, q, [=](){
        emit q->canceled();
    });
    QObject::connect(&watcher, &QFutureWatcher<bool>::finished, q, [=](){
        emit q->finished();
    });
}

// class JProgressDialog

JProgressDialog::JProgressDialog(QWidget *parent)
    : QDialog(parent)
    , J_DPTR(new JProgressDialogPrivate(this))
{
    Q_D(JProgressDialog);
    d->init();
}

Icd::JProgressDialog::~JProgressDialog()
{
    Q_D(JProgressDialog);
    delete d;
}

QString JProgressDialog::message() const
{
    Q_D(const JProgressDialog);
    return d->labelMsg->text();
}

int JProgressDialog::progressMinimum() const
{
    Q_D(const JProgressDialog);
    return d->progressBar->minimum();
}

int JProgressDialog::progressMaximum() const
{
    Q_D(const JProgressDialog);
    return d->progressBar->maximum();
}

int JProgressDialog::progressValue() const
{
    Q_D(const JProgressDialog);
    return d->progressBar->value();
}

bool JProgressDialog::progressVisible() const
{
    Q_D(const JProgressDialog);
    return d->progressBar->isVisible();
}

QString JProgressDialog::acceptText() const
{
    Q_D(const JProgressDialog);
    return d->buttonAccept->text();
}

bool JProgressDialog::acceptVisible() const
{
    Q_D(const JProgressDialog);
    return d->buttonAccept->isVisible();
}

QString JProgressDialog::cancelText() const
{
    Q_D(const JProgressDialog);
    return d->buttonCancel->text();
}

bool JProgressDialog::cancelVisible() const
{
    Q_D(const JProgressDialog);
    return d->buttonCancel->isVisible();
}

void JProgressDialog::waitForFinished()
{
    Q_D(JProgressDialog);
    d->watcher.waitForFinished();
}

void JProgressDialog::setFuture(const QFuture<bool> &future)
{
    Q_D(JProgressDialog);
    if (!d->watcher.isFinished()) {
        d->watcher.cancel();
        d->watcher.waitForFinished();
    }
    d->watcher.setFuture(future);
}

bool JProgressDialog::isCanceled() const
{
    Q_D(const JProgressDialog);
    return d->watcher.isCanceled();
}

bool JProgressDialog::futureResult() const
{
    Q_D(const JProgressDialog);
    return d->watcher.result();
}

void JProgressDialog::setInvertedAppearance(bool invert)
{
    Q_D(JProgressDialog);
    d->progressBar->setInvertedAppearance(invert);
}

bool JProgressDialog::invertedAppearance() const
{
    Q_D(const JProgressDialog);
    return d->progressBar->invertedAppearance();
}

void JProgressDialog::setMessage(const QString &message)
{
    Q_D(JProgressDialog);
    if (message != d->labelMsg->text()) {
        d->labelMsg->setText(message);
        emit messageChanged(message);
    }
}

void JProgressDialog::setProgressRange(int minimum, int maximum)
{
    Q_D(JProgressDialog);
    d->progressBar->setRange(minimum, maximum);
}

void JProgressDialog::setProgressValue(int value)
{
    Q_D(JProgressDialog);
    d->progressBar->setValue(value);
}

void JProgressDialog::setProgressVisible(bool visible)
{
    Q_D(JProgressDialog);
    d->progressBar->setVisible(visible);
    emit progressVisibleChanged(visible);
}

void JProgressDialog::setAcceptText(const QString &text)
{
    Q_D(JProgressDialog);
    if (text != d->buttonAccept->text()) {
        d->buttonAccept->setText(text);
        emit acceptTextChanged(text);
    }
}

void JProgressDialog::setAcceptVisible(bool visible)
{
    Q_D(JProgressDialog);
    d->buttonAccept->setVisible(visible);
    emit acceptVisibleChanged(visible);
}

void JProgressDialog::setCancelText(const QString &text)
{
    Q_D(JProgressDialog);
    if (text != d->buttonCancel->text()) {
        d->buttonCancel->setText(text);
        emit cancelTextChanged(text);
    }
}

void JProgressDialog::setCancelVisible(bool visible)
{
    Q_D(JProgressDialog);
    d->buttonCancel->setVisible(visible);
    emit cancelVisibleChanged(visible);
}

} // end of namespace Icd
