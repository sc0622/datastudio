#ifndef JCHART_TOOLBAR_H
#define JCHART_TOOLBAR_H

#include <QToolBar>
#include "jchart_global.h"

namespace JChart {

class ToolBarPrivate;

class JCHART_EXPORT ToolBar : public QToolBar
{
    Q_OBJECT
    Q_PROPERTY(bool zoomInVisible READ isZoomInVisible WRITE setZoomInVisible NOTIFY zoomInVisibleChanged)
    Q_PROPERTY(bool zoomOutVisible READ isZoomOutVisible WRITE setZoomOutVisible NOTIFY zoomOutVisibleChanged)
    Q_PROPERTY(bool zoomResetVisible READ isZoomResetVisible WRITE setZoomResetVisible NOTIFY zoomResetVisibleChanged)
    Q_PROPERTY(bool clearMarkerVisible READ isClearMarkerVisible WRITE setClearMarkerVisible NOTIFY clearMarkerVisibleChanged)
    Q_PROPERTY(bool selectVisible READ isSelectVisible WRITE setSelectVisible NOTIFY selectVisibleChanged)
    Q_PROPERTY(bool fullScreenVisible READ isFullScreenVisible WRITE setFullScreenVisible NOTIFY fullScreenVisibleChanged)
    Q_PROPERTY(bool playVisible READ isPlayVisible WRITE setPlayVisible NOTIFY playVisibleChanged)
    Q_PROPERTY(bool settingsVisible READ isSettingsVisible WRITE setSettingsVisible NOTIFY settingsVisibleChanged)
    Q_PROPERTY(bool closeVisible READ isCloseVisible WRITE setCloseVisible NOTIFY closeVisibleChanged)
public:
    explicit ToolBar(QWidget *parent = 0);
    ~ToolBar();

    bool isZoomInVisible() const;
    bool isZoomOutVisible() const;
    bool isZoomResetVisible() const;
    bool isClearMarkerVisible() const;
    bool isSelectVisible() const;
    bool isFullScreenVisible() const;
    bool isPlayVisible() const;
    bool isSettingsVisible() const;
    bool isCloseVisible() const;
    bool isSelected() const;
    bool isFullScreen() const;
    bool isPlay() const;

signals:
    void zoomInVisibleChanged(bool value);
    void zoomOutVisibleChanged(bool value);
    void zoomResetVisibleChanged(bool value);
    void clearMarkerVisibleChanged(bool value);
    void selectVisibleChanged(bool value);
    void fullScreenVisibleChanged(bool value);
    void playVisibleChanged(bool value);
    void settingsVisibleChanged(bool value);
    void closeVisibleChanged(bool value);
    void triggledZoomIn();
    void triggledZoomOut();
    void triggledZoomReset();
    void triggledClearMarker();
    void triggledSelect(bool checked);
    void triggledFullScreen(bool checked);
    void triggledPlay(bool checked);
    void triggledSettings();
    void triggledExport();
    void triggledClose();

public slots:
    void setZoomInVisible(bool value);
    void setZoomOutVisible(bool value);
    void setZoomResetVisible(bool value);
    void setClearMarkerVisible(bool value);
    void setSelectVisible(bool value);
    void setFullScreenVisible(bool value);
    void setPlayVisible(bool value);
    void setSettingsVisible(bool value);
    void setCloseVisible(bool value);
    void toggleSelect(bool checked);
    void toggleFullScreen(bool checked);
    void togglePlay(bool checked);

private:
    Q_DISABLE_COPY(ToolBar)
    J_DECLARE_PRIVATE(ToolBar)
};

} // end of namespace JChart

#endif // JCHART_TOOLBAR_H
