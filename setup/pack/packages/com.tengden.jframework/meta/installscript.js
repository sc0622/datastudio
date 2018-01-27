function Component() {
    installer.installationFinished.connect(this, Component.prototype.installationFinished)
}

Component.prototype.createOperations = function() {
    // Call default implementation to actually install application.
    component.createOperations()

    // try {
    //     // 
    //     if (systemInfo.productType === "windows") {
    //         //
    //         component.addOperation(
    //             "GlobalConfig",
    //             "HKEY_CURRENT_USER\\Software\\Smartsoft\\JFramework\\icd.@Version@",
    //             "InstallPath",
    //             "@TargetDir@\\jframework"
    //         )
    //     } else if (systemInfo.productType === "neokylin") {
    //         //
    //     }
    // } catch (e) {
    //     console.log(e)
    // }
}

Component.prototype.createOperationsForArchive = function(archive) {
    //
    component.addOperation("Extract", archive, "@TargetDir@/jframework")
}

Component.prototype.installationFinished = function() {
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
            if (systemInfo.productType === "windows") {
                //
                console.log("***** !!!!!!")
            } else if (systemInfo.productType === "neokylin") {
                console.log(systemInfo.productType);
            }
        }
    } catch (e) {
        console.log(e)
    }
}