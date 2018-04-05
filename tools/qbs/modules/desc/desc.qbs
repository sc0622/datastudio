import qbs

Module {
    property string version: '0.0.1'
    property string versionSuffix
    property string fileDesc: '?'
    property string productName: '?'
    property string copyrightYear: '?'
    property string copyrightDesc: '?'
    property string companyName: 'The Smartsoft Company Ltd'
    property string iconName: ''
    readonly property int versionMajor: parseInt(version.split('.')[0])
    readonly property int versionMinor: parseInt(version.split('.')[1])
    readonly property int versionPatch: parseInt(version.split('.')[2])
    readonly property string displayVersion: version + (!!versionSuffix ? '-' + versionSuffix : '')
}
