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
                "@TargetDir@/modelsimu/bin/ModelSimu.exe",
                "@StartMenuDir@/ModelSimu @Version@/Model Simulator.lnk",
                "workingDirectory=@TargetDir@/modelsimu/bin/",
                "iconPath=@TargetDir@/modelsimu/bin/ModelSimu.exe",
                "iconId=0")
            //
        component.addOperation(
            "CreateShortcut",
            "@TargetDir@/modelsimu/bin/ModelSimu.exe",
            "@HomeDir@/Desktop/Model Simulator @Version@.lnk",
            "workingDirectory=@TargetDir@/modelsimu/bin/",
            "iconPath=@TargetDir@/modelsimu/bin/ModelSimu.exe",
            "iconId=0")
    }
}

Component.prototype.createOperationsForArchive = function(archive) {
    //
    component.addOperation("Extract", archive, "@TargetDir@/datastudio")
}
