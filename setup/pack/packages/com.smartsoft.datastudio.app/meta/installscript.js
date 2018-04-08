function Component() {

}

Component.prototype.createOperations = function() {
    // call default implementation to actually install application!
    component.createOperations()

    //
    if (systemInfo.productType === "windows") {
        // Start Menu
        component.addOperation(
                    "CreateShortcut",
                    "@TargetDir@/@PROJECT@/bin/@APPNAME@.exe",
                    "@StartMenuDir@/Data Studio @Version@/Data Studio.lnk",
                    "workingDirectory=@TargetDir@/@PROJECT@/bin/",
                    "iconPath=@TargetDir@/@PROJECT@/bin/@APPNAME@.exe",
                    "iconId=0")
        // Home
        component.addOperation(
                    "CreateShortcut",
                    "@TargetDir@/@PROJECT@/bin/@APPNAME@.exe",
                    "@HomeDir@/Desktop/Data Studio @Version@.lnk",
                    "workingDirectory=@TargetDir@/@PROJECT@/bin/",
                    "iconPath=@TargetDir@/@PROJECT@/bin/@APPNAME@.exe",
                    "iconId=0")
    }
}

Component.prototype.createOperationsForArchive = function(archive) {
    //
    component.addOperation("Extract", archive, "@TargetDir@/datastudio")
}
