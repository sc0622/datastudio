import qbs

Project {
    minimumQbsVersion: "1.7.1"
    //qbs.buildVariants: [ 'debug', 'release' ]

    CppApplication {
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }

        // Additional import path used to resolve QML modules in Qt Creator's code model
        property pathList qmlImportPaths: []

        cpp.cxxLanguageVersion: "c++11"

        destinationDirectory: sourceDirectory + "/bin"
        consoleApplication: false
        //targetName: "helloworld"
        targetName: {
            var name = 'helloworld'
            if (qbs.buildVariant == 'debug') {
                name += 'd'
            }
            return name
        }

        cpp.defines: [
            // The following define makes your compiler emit warnings if you use
            // any feature of Qt which as been marked deprecated (the exact warnings
            // depend on your compiler). Please consult the documentation of the
            // deprecated API in order to know how to port your code away from it.
            "QT_DEPRECATED_WARNINGS",

            // You can also make your code fail to compile if you use deprecated APIs.
            // In order to do so, uncomment the following line.
            // You can also select to disable deprecated APIs only up to a certain version of Qt.
            //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0
            "HELLO_WORLD= 234"
        ]

        files: [
            "main.cpp",
            "qml.qrc",
        ]

        Group {
            name: "resource"
            // We need the version in two separate formats for the .rc file
            //  RC_VERSION=4,3,82,0 (quadruple)
            //  RC_VERSION_STRING="4.4.0-beta1" (free text)
            // Also, we need to replace space with \x20 to be able to work with both rc and windres
            cpp.defines: outer.concat(["RC_VERSION=" + "2,0,1" + ",0",
                                       "RC_VERSION_STRING=" + "2.0.1",
                                       "RC_COPYRIGHT=2008-" + "2018"
                                       + " The Qt Company Ltd".replace(/ /g, "\\x20")])
            files: "untitled.rc"
        }

        Group {     // Properties for the produced executable
            fileTagsFilter: "application"
            qbs.install: true
        }
    }
}
