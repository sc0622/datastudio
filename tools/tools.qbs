import qbs

Product {
    
    files: [
        'function.js',
        'xcopy.py'
    ]

    Group {
        name: 'qbs'
        prefix: 'qbs/'
        files: [
            // imports
            "imports/JCppDyLibrary.qbs",
            // modules
        ]
    }
}
