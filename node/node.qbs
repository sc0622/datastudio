import qbs

Project {

    references: [
        "src/src.qbs"
    ]

    Product {
        name: 'a-test'
        files: 'test/main.js'
    }
}
