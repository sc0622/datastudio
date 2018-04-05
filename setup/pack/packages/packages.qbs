import qbs
import qbs.FileInfo

Project {

    readonly property stringList modules: [
        '3rdpart', 'core', 'app'
    ]

    references: {
        var items = [];
        modules.forEach(function(module){
            var name = 'com.smartsoft.datastudio.' + module;
            items.push(FileInfo.joinPaths(name, name + '.qbs'));
        });
        return items;
    }

    Product {
        name: 'pack-build'
        Depends { name: 'cpp' }
    }
}
