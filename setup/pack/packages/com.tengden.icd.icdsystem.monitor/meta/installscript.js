function Component() {

}

Component.prototype.createOperations = function() {
    // call default implementation to actually install application!
    component.createOperations()

    console.log("hello")

    //
    if (systemInfo.productType == "windows") {
        //
        component.addOperation(
            "CreateShortcut",
            "@TargetDir@/icdsystem/bin/monitor.exe",
            "@StartMenuDir@/ICD @Version@/ICD Monitor.lnk",
            "workingDirectory=@TargetDir@/icdsystem/bin/",
            "iconPath=@TargetDir@/icdsystem/bin/monitor.exe",
            "iconId=0")
    }
}

Component.prototype.createOperationsForArchive = function(archive) {
    //
    component.addOperation("Extract", archive, "@TargetDir@/icdsystem")
}