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
        spacing: 3
        RowLayout {
            Label {
                id: labelTitle
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                leftPadding: 10
                topPadding: 0
                font.weight: Font.Normal
                renderType: Text.QtRendering
                opacity: 0.8
                text: title
                Label {
                    id: labelValue
                    Layout.alignment: Qt.AlignTop
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    font.pointSize: 18
                    font.weight: Font.Normal
                    //font.family: Flat.FlatStyle.fontFamily
                    renderType: Text.QtRendering
                    Material.foreground: Material.accent
                    horizontalAlignment: Text.AlignHCenter
                    text: value.toFixed(decimals) + suffix
                }
            }
        }
        Pane {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 0
            background: null
            contentItem: QCharts.QChart {
                id: canvas
                anchors.fill: parent
                anchors.leftMargin: 3
                anchors.rightMargin: 3
                clip: true
                antialiasing: true
                chartType: Charts.ChartType.LINE
                chartData: QChartGallery.ChartLineData
                chartOptions: QChartGallery.ChartLineOptions
            }
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
