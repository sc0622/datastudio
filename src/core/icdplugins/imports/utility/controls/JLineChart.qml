import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import "../gauge/gauge.js" as GagueJs
import "../chart" as QCharts
import "../chart/QChart.js" as Charts
import "../chart/QChartGallery.js" as QChartGallery

Page {
    id: root
    padding: 0
    leftPadding: 0
    rightPadding: 0
    topPadding: 3
    bottomPadding: 3
    Layout.fillWidth: true
    Layout.fillHeight: true
    property string suffix: ""
    title: qsTr("--")
    property real value: 0
    property int decimals: 0
    property int maxCount: 50

    background: Rectangle {
        color: Material.listHighlightColor
        radius: 3
    }

    contentItem: ColumnLayout {
        Label {
            id: labelTitle
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: 10
                topMargin: 2
            }
            Layout.fillWidth: true
            font.weight: Font.Normal
            renderType: Text.QtRendering
            opacity: 0.8
            horizontalAlignment: Text.AlignLeft
            text: title
        }

        QCharts.QChart {
            id: canvas
            anchors {
                left: parent.left
                right: parent.right
                top: labelTitle.bottom
                bottom: parent.bottom
                leftMargin: 5
                rightMargin: 5
                topMargin: 6
                bottomMargin: 0
            }
            clip: true
            antialiasing: true
            chartType: Charts.ChartType.LINE
            chartData: QChartGallery.ChartLineData
            chartOptions: QChartGallery.ChartLineOptions
        }

        Label {
            id: labelValue
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 2
            }
            Layout.fillWidth: true
            font.pointSize: 18
            font.weight: Font.Normal
            //font.family: Flat.FlatStyle.fontFamily
            renderType: Text.QtRendering
            Material.foreground: Material.accent
            horizontalAlignment: Text.AlignHCenter
            text: value.toFixed(decimals) + suffix
        }
    }

    function shiftValue(value) {
        var labels = canvas.chartData.labels
        var data = canvas.chartData.datasets[0].data
        labels.push("")
        data.push(value)
        if (labels.length > maxCount) {
            labels.shift()
            data.shift()
        }
        canvas.requestPaint()
    }
    Component.onCompleted: {
        var labels = canvas.chartData.labels
        var data = canvas.chartData.datasets[0].data
        for (var i = 0; i < root.maxCount; ++i) {
            labels.push("")
            data.push(0)
        }
        canvas.requestPaint()
    }
}
