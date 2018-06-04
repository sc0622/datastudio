import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

PackageQt {
    name: 'com.smartsoft.datastudio.qt'

    moduleGeneral: base.concat(['SerialPort', 'Sql'])
}
