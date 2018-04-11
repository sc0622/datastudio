import qbs

Product {
    name: 'setenv'
    qbsSearchPaths: ['.']

    Depends { name: 'qtinstall' }
    Depends { name: 'jframework' }
    Depends { name: 'datastudio' }
}
