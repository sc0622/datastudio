import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

Project {
    property path packagesPath: null

    references: probeDepends.references

    Probe {
        id: probeDepends
        readonly property path packagesPath: project.packagesPath
        property stringList references: []
        configure: {
            if (!packagesPath) {
                return;
            }
            var dirs = File.directoryEntries(packagesPath, File.Dirs | File.NoDotAndDotDot);
            if (dirs) {
                references = [];
                dirs.forEach(function(item){
                    references.push(packagesPath + '/' + item + '/module.qbs');
                });
                found = true;
            }
        }
    }
}
