#ifndef JUTRALEDIT_VIEW_H
#define JUTRALEDIT_VIEW_H

#include <QWidget>
#include "jutraledit_global.h"

namespace JUtralEdit {

class JViewPrivate;

/**
 * @brief The JView class
 */
class JUTRALEDIT_EXPORT JView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool asciiVisible READ isAsciiVisible WRITE setAsciiVisible NOTIFY asciiVisibleChanged)
public:
    /**
     * @brief JView
     * @param parent
     */
    explicit JView(QWidget *parent = 0);

    ~JView();

    /**
     * @brief title
     * @return
     */
    QString title() const;

    /**
     * @brief isAsciiVisible
     * @return
     */
    bool isAsciiVisible() const;

    /**
     * @brief updateUi
     */
    void updateUi();

    /**
     * @brief updateUi
     * @param data
     */
    void updateUi(const QByteArray &data);

    /**
     * @brief updateUi
     * @param data
     * @param size
     */
    void updateUi(const void *data, int size);

signals:
    void titleChanged(const QString &text);
    void asciiVisibleChanged(bool checked);

public slots:
    void setTitle(const QString &text);
    void setAsciiVisible(bool visible);

private:
    Q_DISABLE_COPY(JView)
    J_DECLARE_PRIVATE(JView)
};

} // end of namespace JUtralEdit

#endif // JUTRALEDIT_VIEW_H
