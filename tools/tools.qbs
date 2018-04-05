import qbs

Product {

    files: []

    Group {
        name: 'qbs'
        prefix: 'qbs/'
        files: [
            // imports
            "imports/**/*.qbs",
            // modules
            "modules/**/*.qbs",
        ]
    }
}
