function Component() {

}

Component.prototype.createOperations = function() {
    // call default implementation to actually install application!
    component.createOperations()

    //
    if (systemInfo.productType == "windows") {
        //
        component.addOperation(
            "CreateShortcut",
            "@TargetDir@/icdsystem/bin/simulator.exe",
            "@StartMenuDir@/ICD @Version@/ICD Simulator.lnk",
            "workingDirectory=@TargetDir@/icdsystem/bin/",
            "iconPath=@TargetDir@/icdsystem/bin/simulator.exe",
            "iconId=0")
    }
}

Component.prototype.createOperationsForArchive = function(archive) {
    //
    component.addOperation("Extract", archive, "@TargetDir@/icdsystem")
}