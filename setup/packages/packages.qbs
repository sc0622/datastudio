import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

Project {
    readonly property path packagesPath: path

    references: probeDepends.references

    Probe {
        id: probeDepends
        property stringList references: []
        configure: {
            var dirs = File.directoryEntries(path, File.Dirs | File.NoDotAndDotDot);
            if (dirs) {
                references = [];
                dirs.forEach(function(item){ references.push(item + '/module.qbs'); });
                found = true;
            }
        }
    }
}
