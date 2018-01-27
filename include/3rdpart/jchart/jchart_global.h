#ifndef JCHART_GLOBAL_H
#define JCHART_GLOBAL_H

#include <qglobal.h>
#include <QString>

// JCHART_VERSION is (major << 16) + (minor << 8) + patch.

#define JCHART_VERSION       0x000001
#define JCHART_VERSION_STR   "0.0.1"

#ifdef JCHART_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef JCHART_BUILD
#       define JCHART_EXPORT  Q_DECL_EXPORT
#   else
#       define JCHART_EXPORT  Q_DECL_IMPORT
#       if defined(_DEBUG) || defined(DEBUG)
#           pragma comment(lib, "jchartd.lib")
#       else
#           pragma comment(lib, "jchart.lib")
#       endif
#   endif // !JCHART_BUILD
#endif // _MSC_VER || ...
#endif // JCHART_LIB

#ifndef JCHART_EXPORT
#define JCHART_EXPORT
#endif

// - namespace JCHART -
#define JCHART_NAMESPACE JCHART
#define JCHART_BEGIN_NAMESPACE namespace JCHART_NAMESPACE {
#define JCHART_END_NAMESPACE }
#define JCHART_USE_NAMESPACE using namespace JCHART_NAMESPACE;

// - private pointer
#ifndef J_DECLARE_PRIVATE
#define J_DECLARE_PRIVATE(name) \
    name ## Private *d_ptr; \
    Q_DECLARE_PRIVATE(name)
#endif

#ifndef J_DECLARE_PUBLIC
#define J_DECLARE_PUBLIC(name) \
    name *q_ptr; \
    Q_DECLARE_PUBLIC(name)
#endif

#ifndef J_DECLARE_SINGLE_INSTANCE
#define J_DECLARE_SINGLE_INSTANCE(Class) \
public: \
    static Class *instance(); \
    static void releaseInstance(); \
private: \
    static Class *_instance;
#endif

#ifndef J_IMPLEMENT_SINGLE_INSTANCE
#define J_IMPLEMENT_SINGLE_INSTANCE(Class) \
Class *Class::_instance = 0; \
\
Class *Class::instance() { \
    if (Class::_instance == 0) { \
        Class::_instance = new Class; \
    } \
    return Class::_instance; \
} \
\
void Class::releaseInstance() { \
    if (Class::_instance != 0) { \
        delete Class::_instance; \
        Class::_instance = 0; \
    } \
}
#endif

namespace JChart {

/**
 * @brief 图表类型定义
 */
enum ChartType {
    ChartTypeUndefined = 0,     /**< 未定义图表类型 */
    ChartTypeCartesian,         /**< 笛卡尔坐标图表 */
    ChartTypePolar,             /**< 极坐标图表 */
    ChartTypeNumeric,           /**< 数值型数据图表 */
    ChartTypeBitMap,            /**< Bit映射数据图表 */
    ChartTypeFileNumeric,       /**< 文件数值型数据图表 */
    ChartTypeFileBitMap         /**< 文件Bit映射型数据图表 */
};

/**
 * @brief 图表主题类型定义
 */
enum ChartTheme {
    ChartThemeLight = 0,        /**< */
    ChartThemeBlueCerulean,     /**< */
    ChartThemeDark,             /**< */
    ChartThemeBrownSand,        /**< */
    ChartThemeBlueNcs,          /**< */
    ChartThemeHighContrast,     /**< */
    ChartThemeBlueIcy,          /**< */
    ChartThemeQt                /**< */
};

/**
 * @brief 曲线平移方式
 */
enum ShiftType {
    ShiftInvalid = -1,  /**< Invalid */
    ShiftByCount,       /**< 以数据个数方式平移 */
    ShiftByTime,        /**< 以时间带方式平移 */
};

//! \brief Axis index
enum Axis {
    //! Y axis left of the canvas
    yLeft,
    //! Y axis right of the canvas
    yRight,
    //! X axis below the canvas
    xBottom,
    //! X axis above the canvas
    xTop,
    //! Number of axes
    axisCnt
};

/*!
    Position of the legend, relative to the canvas.
    \sa insertLegend()
 */
enum LegendPosition {
    //! The legend will be left from the QwtPlot::yLeft axis.
    LeftLegend,
    //! The legend will be right from the QwtPlot::yRight axis.
    RightLegend,
    //! The legend will be below the footer
    BottomLegend,
    //! The legend will be above the title
    TopLegend
};

/**
 * @brief The ScaleDrawType enum
 */
enum ScaleDrawType
{
    ScaleDrawDefault,   /**<  */
    ScaleDrawTime       /**<  */
};

/**
 * @brief The NumericType enum
 */
enum NumericType {
    NumericInvalid = -1,
    NumericInt8,
    NumericUint8,
    NumericInt16,
    NumericUint16,
    NumericInt32,
    NumericUint32,
    NumericInt64,
    NumericUint64,
    NumericFloat64,
    NumericFloat32,
    NumericTotal
};

// - class JChartCore -

class ChartCorePrivate;

class JCHART_EXPORT JChartCore
{
public:
    static JChartCore *instance();
    static void releaseInstance();

    void init();

private:
    explicit JChartCore();
    ~JChartCore();

private:
    ChartCorePrivate *d;
};

} // end of namespace JChart

#endif // JCHART_GLOBAL_H
