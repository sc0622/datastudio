#include "precomp.h"
#include "JWindowFrame.h"
#include <QQuickWindow>
#include <QQuickItem>

namespace icdmeta {

enum RegionType {
    RegionLeft = 0x01,
    RegionRight = 0x02,
    RegionTop = 0x04,
    RegionBottom = 0x08,
    RegionTopLeft = RegionTop | RegionLeft,
    RegionTopRight = RegionTop | RegionRight,
    RegionBottomLeft = RegionBottom | RegionLeft,
    RegionBottomRight = RegionBottom | RegionRight,
    RegionCaption = 0x80
};
Q_DECLARE_FLAGS(RegionTypes, RegionType)

// class JWindowFrmePrivate

class JWindowFramePrivate
{
public:
    JWindowFramePrivate(JWindowFrame *q)
        : J_QPTR(q)
        , shadowSize(8)
        , resizable(true)
        , window(nullptr)
        , header(nullptr)
    {

    }

    void init();

    RegionTypes testHit(const QPoint &pos) const;
    int hitFromRegion(RegionTypes regionTypes) const;
#ifdef _MSC_VER
    void doNCPaint(const MSG *msg);
#else
    //
#endif

private:
    J_DECLARE_PUBLIC(JWindowFrame)
    int shadowSize;
    bool resizable;
    QQuickWindow *window;
    QQuickItem *header;
};

void JWindowFramePrivate::init()
{

}

RegionTypes JWindowFramePrivate::testHit(const QPoint &pos) const
{
    const int width = window->width();
    const int height = window->height();
    int captionHeight = header ? header->height() : 0;
    const int shadowSize2 = shadowSize;// * 2
    RegionTypes regionTypes = 0;
    // caption
    if (shadowSize2 < pos.x() && pos.x() < width - shadowSize2
            && shadowSize < pos.y() && pos.y() < captionHeight) {
        regionTypes = RegionCaption;
    } else {
        // left
        if (0 <= pos.x() && pos.x() < shadowSize2) {
            if (pos.x() < shadowSize || pos.y() < shadowSize) {
                regionTypes |= RegionLeft;
            }
        }
        // right
        if (width - shadowSize2 < pos.x() && pos.x() <= width) {
            if (pos.x() > width - shadowSize || pos.y() < shadowSize) {
                regionTypes |= RegionRight;
            }
        }
        // top
        if (0 <= pos.y() && pos.y() < shadowSize2) {
            if (pos.y() < shadowSize || pos.x() < shadowSize2) {
                regionTypes |= RegionTop;
            }
        }
        // bottom
        if (shadowSize2 < pos.y() && pos.y() <= height) {
            if (pos.y() > height - shadowSize || pos.y() < shadowSize) {
                regionTypes |= RegionBottom;
            }
        }
    }

    return regionTypes;
}

int JWindowFramePrivate::hitFromRegion(RegionTypes regionTypes) const
{
#ifdef _WIN32
    switch (regionTypes) {
    case RegionLeft: return HTLEFT;
    case RegionRight: return HTRIGHT;
    case RegionTop: return HTTOP;
    case RegionBottom: return HTBOTTOM;
    case RegionTopLeft: return HTTOPLEFT;
    case RegionTopRight: return HTTOPRIGHT;
    case RegionBottomLeft: return HTBOTTOMLEFT;
    case RegionBottomRight: return HTBOTTOMRIGHT;
    case RegionCaption: return HTCAPTION;
    default: return HTCLIENT;
    }
#else
    Q_UNUSED(regionTypes);
    return 0;
#endif
}

void JWindowFramePrivate::doNCPaint(const MSG *msg)
{
    Q_UNUSED(msg);
}

// class JWindowFrame

JWindowFrame::JWindowFrame(QQuickWindow *window, QObject *parent)
    : QObject(parent)
    , J_DPTR(new JWindowFramePrivate(this))
{
    Q_D(JWindowFrame);
    d->init();
    d->window = window;
    if (d->window) {
        // header
        QVariant header = d->window->property("header");
        if (header.isValid()) {
            d->header = header.value<QQuickItem *>();
        }
        //
        if (qApp) {
            qApp->installNativeEventFilter(this);
        }
    }
}

JWindowFrame::~JWindowFrame()
{
    if (qApp) {
        qApp->removeNativeEventFilter(this);
    }
}

bool JWindowFrame::resizable() const
{
    Q_D(const JWindowFrame);
    return d->resizable;
}

void JWindowFrame::setResizable(bool value)
{
    Q_D(JWindowFrame);
    if (value != d->resizable) {
        d->resizable = value;
        emit resizableChanged(value);
    }
}

bool JWindowFrame::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG") {
        return winEventFilter(message, result);
    }

    return false;
}

bool JWindowFrame::winEventFilter(void *message, long *result)
{
#ifdef _WIN32
    Q_D(JWindowFrame);
    if (!d->window) {
        return false;
    }
    const MSG *msg = reinterpret_cast<MSG *>(message);
    if (msg->hwnd != (HWND)d->window->winId()) {
        return false;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
#else
#endif
    qDebug() << msg->message << msg->wParam << msg->lParam;

    switch (msg->message) {
#if 0
    case WM_GETMINMAXINFO:
    {
        const QPoint pos = QPoint(msg->pt.x, msg->pt.y) / d->window->effectiveDevicePixelRatio();
        QRect rect = QApplication::desktop()->availableGeometry(pos);
        //qDebug() << "WM_GETMINMAXINFO:" << pos << rect;
        MINMAXINFO *info = reinterpret_cast<MINMAXINFO *>(msg->lParam);
        info->ptMaxPosition.x = rect.x() - d->shadowSize;
        info->ptMaxPosition.y = rect.y() - d->shadowSize;
        info->ptMaxSize.x = rect.width() + d->shadowSize * 2;
        info->ptMaxSize.y = rect.height() + d->shadowSize * 2;
        return true;
    }
#endif
    case WM_NCHITTEST:
    {
        if (d->window->windowState() & Qt::WindowFullScreen) {
            break;
        }

        const QPoint pos = QPoint(msg->pt.x, msg->pt.y) / d->window->effectiveDevicePixelRatio()
                - d->window->position();
        int _result = d->hitFromRegion(d->testHit(pos));
        if (result) {
            *result = _result;
        }
        //qDebug() << "WM_NCHITTEST:" << pos;
        switch (_result) {
        case HTCAPTION:
        {
            QQuickItem *focusItem = d->window->activeFocusItem();
            if (focusItem) {
                //QObject *parentO = focusItem->parent();
                //QQuickItem *parentIt = focusItem->parentItem();
                //qDebug() << "parentO:" << parentO << ", " << "parentItem:" << parentIt;
                //const QString className = focusItem->metaObject()->className();
                if (focusItem->inherits("QQuickPopupItem")) {
                    break;
                }
            }

            QVariant acceptCaption = true;
            if (QMetaObject::invokeMethod(d->window, "caption",
                                          Q_RETURN_ARG(QVariant, acceptCaption),
                                          Q_ARG(QVariant, pos))) {
                if (acceptCaption.toBool()) {
                    return true;
                }
            } else {
                return true;
            }
            break;
        }
        case HTCLIENT:
        {
            return true;
        }
        default:
        {
            if (!(d->window->windowState() & Qt::WindowMaximized)) {
                return true;
            }
            break;
        }
        case HTNOWHERE:
            break;
        }
        break;
    }
#if 0
    case WM_NCPAINT:
    {
        //qDebug() << "WM_NCPAINT";
        break;
    }
#endif
#if 0
    case WM_NCCALCSIZE:
    {
        int xFrame = 1;
        int yFrame = 1;
        int nTHeight = 1;
        RECT *rc;
#if 0
        RECT ar, br, bcr;
        if (msg->wParam == TRUE) {
            NCCALCSIZE_PARAMS *params = reinterpret_cast<NCCALCSIZE_PARAMS *>(msg->lParam);

            CopyRect(&ar, &params->rgrc[1]);
            CopyRect(&br, &params->rgrc[0]);

            bcr.left = br.left + xFrame;
            bcr.top = br.top + nTHeight;
            bcr.right = br.right - xFrame;
            bcr.bottom = br.bottom - yFrame;

            CopyRect(&params->rgrc[0], &bcr);
            CopyRect(&params->rgrc[1], &ar);
            CopyRect(&params->rgrc[2], &br);
        } else {
            rc = reinterpret_cast<RECT *>(msg->lParam);
            rc->left = rc->left + xFrame;
            rc->top = rc->top + nTHeight;
            rc->right = rc->right - xFrame;
            rc->bottom = rc->bottom - yFrame;
        }
#else
        rc = reinterpret_cast<RECT *>(msg->lParam);
        rc->left = rc->left + xFrame;
        rc->top = rc->top + nTHeight;
        rc->right = rc->right - xFrame;
        rc->bottom = rc->bottom - yFrame;
        if (msg->wParam == FALSE) {
            RECT *rc2 = reinterpret_cast<RECT *>(result);
            rc2->left = rc->left - 6;
            rc2->top = rc->top - 30;
            rc2->right = rc->right + 6;
            rc2->bottom = rc->bottom + 6;
        } else {
            *result = ::GetLastError();
        }
#endif
        return true;
    }
#endif
    case WM_KEYDOWN:
    {
        switch (msg->wParam) {
        case VK_ESCAPE:
        {
            if (d->window->windowState() & Qt::WindowFullScreen) {
                ::ShowWindow(msg->hwnd, SW_RESTORE);
                return true;
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case WM_NCLBUTTONDBLCLK:
    {
        if (::IsZoomed(msg->hwnd)) {
            ::ShowWindow(msg->hwnd, SW_RESTORE);
        } else {
            d->window->showMaximized();
            //::ShowWindow(msg->hwnd, SW_MAXIMIZE);
        }
        return true;
    }
    case WM_DESTROY:
    {
        if (qApp) {
            qApp->removeNativeEventFilter(this);
        }
        break;
    }
    default:
        break;
    }
#else
    //
#endif

    return false;
}

}
