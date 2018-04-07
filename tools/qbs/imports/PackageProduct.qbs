import qbs
import qbs.File
import qbs.FileInfo

PackProduct {
    Depends { name: 'meta' }

    type: base.concat([ 'package.in' ])
}
