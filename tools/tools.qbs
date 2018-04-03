import qbs

Product {

    files: [
        'xcopy.py'
    ]

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
