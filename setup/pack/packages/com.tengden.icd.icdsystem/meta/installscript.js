function Component() {

}

Component.prototype.createOperations = function() {
    console.log("------------------")
        // call default implementation to actually install application!
    component.createOperations()

    console.log("hello")

    //
    if (systemInfo.productType == "windows") {
        //
        component.addOperation(
                "CreateShortcut",
                "@TargetDir@/icdsystem/bin/icdsystem.exe",
                "@StartMenuDir@/ICD @Version@/ICD System.lnk",
                "workingDirectory=@TargetDir@/icdsystem/bin/",
                "iconPath=@TargetDir@/icdsystem/bin/icdsystem.exe",
                "iconId=0")
            //
        component.addOperation(
            "CreateShortcut",
            "@TargetDir@/icdsystem/bin/icdsystem.exe",
            "@HomeDir@/Desktop/ICD @Version@.lnk",
            "workingDirectory=@TargetDir@/icdsystem/bin/",
            "iconPath=@TargetDir@/icdsystem/bin/icdsystem.exe",
            "iconId=0")
    }
}

Component.prototype.createOperationsForArchive = function(archive) {
    //
    component.addOperation("Extract", archive, "@TargetDir@/icdsystem")
}