import qbs
import 'tools/function.js' as Function

Project {

    qbs.enableDebugCode: true

    readonly property string dynamicSuffix: Function.dynamicSuffix(qbs)

    qbsSearchPaths: [
        'tools/qbs'
    ]

    references: [
        'src/src.qbs',
        'tools/tools.qbs'
    ]
}
