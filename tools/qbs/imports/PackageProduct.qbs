import qbs
import qbs.File
import qbs.FileInfo

PackProduct {
    Depends { name: 'cpp' }
    Depends { name: 'data' }
    Depends { name: 'meta' }

    type: base.concat([ 'package.in' ])
}
