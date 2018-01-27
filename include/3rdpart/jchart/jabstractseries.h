#ifndef JABSTRACTSERIES_H
#define JABSTRACTSERIES_H

#include "jchart_global.h"
#include <QObject>

class QwtPlotCurve;

namespace JChart {

class Chart;
class AbstractSeriesPrivate;

/**
 * @brief 曲线系列抽象类
 */
class JCHART_EXPORT AbstractSeries
{
public:
    /**
     * @brief Curve styles
     */
    enum CurveStyle {
        NoCurve = -1,   /**< Don't draw a curve. Note: This doesn't affect the symbols. */
        Line,           /**< Connect the points with straight lines. The lines might be interpolated on the 'Fitted' attribute.
                             Curve fitting can be configured using setCurveFitter(). */
        Sticks,         /**< Draw vertical or horizontal sticks ( depending on the orientation() ) from a baseline which is defined by setBaseLine(). */
        Steps,          /**< Connect the points with a step function. The step function is draw from the left to the right or vice versa, depending on the JAbstractSeries::Inverted attribute. */
        Dots,           /**< Draw dots at the locations of the data points. Note:
                             This is different from a dotted line (see setPen()), and faster as a curve in JAbstractSeries::NoCurve style and a symbol painting a point. */
        UserCurve = 100 /**< Styles >= JAbstractSeries::UserCurve are reserved for derived
                             classes of JAbstractSeries that overland drawCurve() with additional application specific curve types. */
    };

    /**
     * @brief Attribute for drawing the curve
     * @sa setCurveAttribute(), testCurveAttribute(), curveFitter()
     */
    enum CurveAttribute {
        Inverted = 0x01,    /**< For JAbstractSeries::Steps only. Draw a step function from the right to the left. */
        Fitted = 0x02       /**< Only in combination with JAbstractSeries::Fitted mode is slow it might be better
                                 to fit the points, before they are passed to JAbstractSeries. */
    };

    /**
     * @brief Curve attributes
     */
    typedef QFlags<CurveAttribute> CurveAttributes;

    /**
     * @brief The LegendAttribute enum
     */
    enum LegendAttribute {
        NoAttrinute = 0x00,     /**< JAbstractSeries tries to find a color representing the curve and points a rectangle with it. */
        ShowLine = 0x01,        /**< If the style() is not JAbstractSeries::NoCurve a line is painted with the curve pen() */
        ShowSymbol = 0x02,      /**< If the curve has a valid symbol it is painted. */
        ShowBrush = 0x04        /**< If the curve has a brush a rectangle filled with the curve brush() is painted. */
    };

    /**
     * @brief Legend attributes
     */
    typedef QFlags<LegendAttribute> LegendAttributes;

    /**
     * @brief \Attribute to modify the drawing algorithm. The default setting enables ClipPolygons | FilterPoints
     */
    enum PaintAttribute {
        ClipPolygons = 0x01,    /**< Clip polygons before painting them. In situations, where points are for outside the visible area (f.e when zooming deep) this
                                     might be a substantial improvement for the painting performance. */
        FilterPoints = 0x02,    /**< Tries to reduce the data that has to be painted, by sorting out duplicates, or paintings outside the visible area. Might have a
                                     notable impact on curves with many close points. Only a couple of very filtering algorithms area implemented. */
        MinimizeMemory = 0x04,  /**< Minimize memory useage that is temporarily needed forthe translated points, before they get painted.
                                     This might slow down the performance of painting. */
        ImageBuffer = 0x08      /**< Render the points to a temporary image and paint the image. This is a very special optimization for Dots style, when having a
                                     huge amount of points. With a reasonable number of points QPainter::drawPoints() will be faster. */
    };
    /*!
      Symbol Style
      \sa setStyle(), style()
     */
    enum SymbolStyle
    {
        //! No Style. The symbol cannot be drawn.
        NoSymbol = -1,

        //! Ellipse or circle
        Ellipse,

        //! Rectangle
        Rect,

        //!  Diamond
        Diamond,

        //! Triangle pointing upwards
        Triangle,

        //! Triangle pointing downwards
        DTriangle,

        //! Triangle pointing upwards
        UTriangle,

        //! Triangle pointing left
        LTriangle,

        //! Triangle pointing right
        RTriangle,

        //! Cross (+)
        Cross,

        //! Diagonal cross (X)
        XCross,

        //! Horizontal line
        HLine,

        //! Vertical line
        VLine,

        //! X combined with +
        Star1,

        //! Six-pointed star
        Star2,

        //! Hexagon
        Hexagon,

        /*!
          The symbol is represented by a painter path, where the
          origin ( 0, 0 ) of the path coordinate system is mapped to
          the position of the symbol.

          \sa setPath(), path()
         */
        Path,

        /*!
          The symbol is represented by a pixmap. The pixmap is centered
          or aligned to its pin point.

          \sa setPinPoint()
         */
        Pixmap,

        /*!
          The symbol is represented by a graphic. The graphic is centered
          or aligned to its pin point.

          \sa setPinPoint()
         */
        Graphic,

        /*!
          The symbol is represented by a SVG graphic. The graphic is centered
          or aligned to its pin point.

          \sa setPinPoint()
         */
        SvgDocument,

        /*!
         Styles >= QwtSymbol::UserSymbol are reserved for derived
         classes of QwtSymbol that overload drawSymbols() with
         additional application specific symbol types.
         */
        UserStyle = 1000
    };

    /**
     * @brief Paint attributes
     */
    typedef QFlags<PaintAttribute> PaintAttributes;

    /**
     * @brief JAbstractSeries
     * @param title
     */
    explicit AbstractSeries(const QString &title = QString::null);

    virtual ~AbstractSeries();

    /**
     * @brief identity
     * @return
     */
    QString identity() const;

    /**
     * @brief setIdentity
     * @param identity
     */
    void setIdentity(const QString &identity);

    /**
     * @brief setCurveStyle
     * @param style
     */
    void setCurveStyle(CurveStyle style);

    /**
     * @brief curveStyle
     * @return
     */
    CurveStyle curveStyle() const;

    /**
     * @brief setCurveAttribute
     * @param attr
     * @param on
     */
    void setCurveAttribute(CurveAttribute attr, bool on = true);

    /**
     * @brief testCurveAttribute
     * @param attr
     * @return
     */
    bool testCurveAttribute(CurveAttribute attr) const;

    /**
     * @brief setLegendAttribute
     * @param attr
     * @param on
     */
    void setLegendAttribute(LegendAttribute attr, bool on = true);

    /**
     * @brief testLegendAttribute
     * @param attr
     * @return
     */
    bool testLegendAttribute(LegendAttribute attr) const;

    /**
     * @brief setPaintAttribute
     * @param attr
     * @param on
     */
    void setPaintAttribute(PaintAttribute attr, bool on = true);

    /**
     * @brief tesPaintAttribute
     * @param attr
     * @return
     */
    bool tesPaintAttribute(PaintAttribute attr) const;

    /**
     * @brief pen
     * @return
     */
    virtual QPen pen() const;

    /**
     * @brief setPen
     * @param pen
     */
    virtual void setPen(const QPen &pen);

    /**
     * @brief brush
     * @return
     */
    virtual QBrush brush() const;

    /**
     * @brief setBrush
     * @param brush
     */
    virtual void setBrush(const QBrush &brush);

    /**
     * @brief title
     * @return
     */
    virtual QString title() const;

    /**
     * @brief setTitle
     * @param text
     */
    virtual void setTitle(const QString &text);

    /**
     * @brief setSymbol
     * @param style
     * @param brush
     * @param pen
     * @param size
     */
    void setSymbol(SymbolStyle style, const QBrush &brush,
                   const QPen &pen, const QSize &size);

    /**
     * @brief setSymbolBrush
     * @param brush
     */
    void setSymbolBrush(const QBrush &brush);

    /**
     * @brief setSymbolPen
     * @param pen
     */
    void setSymbolPen(const QPen &pen);

    /**
     * @brief setSymbolSize
     * @param size
     */
    void setSymbolSize(const QSize &size);

    /**
     * @brief attach
     * @param chart
     */
    virtual void attach(Chart *chart);

    /**
     * @brief detach
     */
    virtual void detach();

    /**
     * @brief chart
     * @return
     */
    Chart *chart() const;

    /**
     * @brief curve
     * @return
     */
    QwtPlotCurve *curve() const;

    /**
     * @brief samples
     * @return
     */
    virtual const QVector<QPointF> &samples() const;

    /**
     * @brief setSamples
     * @param data
     */
    virtual void setSamples(const QVector<QPointF> &data);

    /**
     * @brief appendSample
     * @param point
     */
    virtual void appendSample(const QPointF &point);

    /**
     * @brief leftShiftSample
     * @param point
     */
    virtual void shiftSample(const QPointF &point);

    /**
     * @brief update
     */
    void update();

    /**
     * @brief userData
     * @return
     */
    QObjectUserData *userData() const;

    /**
     * @brief setUserData
     * @param data
     */
    void setUserData(QObjectUserData *data);

private:
    Q_DISABLE_COPY(AbstractSeries)
    J_DECLARE_PRIVATE(AbstractSeries)
};

} // end of namespace JChart

#endif // JABSTRACTSERIES_H
