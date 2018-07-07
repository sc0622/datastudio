import qbs
import 'tools/qbs/imports' as I

I.JSolution {
    projectName: 'datastudio'
    projectDisplayName: 'Data Studio'

    references: base.concat([
                                //'node/node.qbs'
                            ])
}
