#include "precomp.h"
#include "progressdialog.h"

namespace Icd {

// class ProgressDialogPrivate

class ProgressDialogPrivate
{
public:
    ProgressDialogPrivate(ProgressDialog *q)
        : J_QPTR(q)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(ProgressDialog)
    QProgressBar *progressBar;
    QLabel *labelMsg;
    QPushButton *buttonAccept;
    QPushButton *buttonCancel;
    QFutureWatcher<bool> watcher;
};

void ProgressDialogPrivate::init()
{
    Q_Q(ProgressDialog);
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
    buttonAccept = new QPushButton(ProgressDialog::tr("Ok"), q);
    buttonCancel = new QPushButton(ProgressDialog::tr("Cancel"), q);
    buttonAccept->setVisible(false);
    buttonAccept->setMinimumWidth(80);
    buttonCancel->setMinimumWidth(80);

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();
    horiLayoutBottom->addWidget(buttonAccept);
    horiLayoutBottom->addWidget(buttonCancel);

    QObject::connect(progressBar, &QProgressBar::valueChanged,
                     q, &ProgressDialog::progressValueChanged);
    QObject::connect(buttonAccept, &QPushButton::clicked, q, &QDialog::accept);
    QObject::connect(buttonCancel, &QPushButton::clicked, q, &QDialog::reject);

    //
    QObject::connect(&watcher, &QFutureWatcher<bool>::canceled, q, [=](){
        watcher.cancel();
        watcher.waitForFinished();
        q->hide();
        q->reject();
    });
    QObject::connect(q, &ProgressDialog::rejected, q, [=](){
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

// class ProgressDialog

ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent)
    , J_DPTR(new ProgressDialogPrivate(this))
{
    Q_D(ProgressDialog);
    d->init();
}

Icd::ProgressDialog::~ProgressDialog()
{
    Q_D(ProgressDialog);
    delete d;
}

QString ProgressDialog::message() const
{
    Q_D(const ProgressDialog);
    return d->labelMsg->text();
}

int ProgressDialog::progressMinimum() const
{
    Q_D(const ProgressDialog);
    return d->progressBar->minimum();
}

int ProgressDialog::progressMaximum() const
{
    Q_D(const ProgressDialog);
    return d->progressBar->maximum();
}

int ProgressDialog::progressValue() const
{
    Q_D(const ProgressDialog);
    return d->progressBar->value();
}

bool ProgressDialog::progressVisible() const
{
    Q_D(const ProgressDialog);
    return d->progressBar->isVisible();
}

QString ProgressDialog::acceptText() const
{
    Q_D(const ProgressDialog);
    return d->buttonAccept->text();
}

bool ProgressDialog::acceptVisible() const
{
    Q_D(const ProgressDialog);
    return d->buttonAccept->isVisible();
}

QString ProgressDialog::cancelText() const
{
    Q_D(const ProgressDialog);
    return d->buttonCancel->text();
}

bool ProgressDialog::cancelVisible() const
{
    Q_D(const ProgressDialog);
    return d->buttonCancel->isVisible();
}

void ProgressDialog::waitForFinished()
{
    Q_D(ProgressDialog);
    d->watcher.waitForFinished();
}

void ProgressDialog::setFuture(const QFuture<bool> &future)
{
    Q_D(ProgressDialog);
    if (!d->watcher.isFinished()) {
        d->watcher.cancel();
        d->watcher.waitForFinished();
    }
    d->watcher.setFuture(future);
}

bool ProgressDialog::isCanceled() const
{
    Q_D(const ProgressDialog);
    return d->watcher.isCanceled();
}

bool ProgressDialog::futureResult() const
{
    Q_D(const ProgressDialog);
    return d->watcher.result();
}

void ProgressDialog::setInvertedAppearance(bool invert)
{
    Q_D(ProgressDialog);
    d->progressBar->setInvertedAppearance(invert);
}

bool ProgressDialog::invertedAppearance() const
{
    Q_D(const ProgressDialog);
    return d->progressBar->invertedAppearance();
}

void ProgressDialog::setMessage(const QString &message)
{
    Q_D(ProgressDialog);
    if (message != d->labelMsg->text()) {
        d->labelMsg->setText(message);
        emit messageChanged(message);
    }
}

void ProgressDialog::setProgressRange(int minimum, int maximum)
{
    Q_D(ProgressDialog);
    d->progressBar->setRange(minimum, maximum);
}

void ProgressDialog::setProgressValue(int value)
{
    Q_D(ProgressDialog);
    d->progressBar->setValue(value);
}

void ProgressDialog::setProgressVisible(bool visible)
{
    Q_D(ProgressDialog);
    d->progressBar->setVisible(visible);
    emit progressVisibleChanged(visible);
}

void ProgressDialog::setAcceptText(const QString &text)
{
    Q_D(ProgressDialog);
    if (text != d->buttonAccept->text()) {
        d->buttonAccept->setText(text);
        emit acceptTextChanged(text);
    }
}

void ProgressDialog::setAcceptVisible(bool visible)
{
    Q_D(ProgressDialog);
    d->buttonAccept->setVisible(visible);
    emit acceptVisibleChanged(visible);
}

void ProgressDialog::setCancelText(const QString &text)
{
    Q_D(ProgressDialog);
    if (text != d->buttonCancel->text()) {
        d->buttonCancel->setText(text);
        emit cancelTextChanged(text);
    }
}

void ProgressDialog::setCancelVisible(bool visible)
{
    Q_D(ProgressDialog);
    d->buttonCancel->setVisible(visible);
    emit cancelVisibleChanged(visible);
}

} // end of namespace Icd
