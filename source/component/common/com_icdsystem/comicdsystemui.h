#ifndef COMICDSYSTEMUI_H
#define COMICDSYSTEMUI_H

#include <QtWidgets>

class IJAttempter;

class ComIcdSystemUi : public QWidget
{
    Q_OBJECT
public:
    explicit ComIcdSystemUi(IJAttempter &attempter, QWidget *parent = 0);
    ~ComIcdSystemUi();

    /**
     * @brief sourceType
     * @return
     */
    QString sourceType() const;

    /**
     * @brief filePath
     * @return
     */
    QString filePath() const;

Q_SIGNALS:

public Q_SLOTS:

private:
    IJAttempter &d_attempter;
};

#endif // COMICDSYSTEMUI_H
