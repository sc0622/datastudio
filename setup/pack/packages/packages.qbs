import qbs
import qbs.FileInfo

Project {

    readonly property stringList modules: [
        '3rdpart', 'core', 'app'
    ]

    references: {
        var items = [];
        modules.forEach(function(module){
            items.push('com.smartsoft.datastudio.' + module + '/module.qbs');
        });
        return items;
    }
}
