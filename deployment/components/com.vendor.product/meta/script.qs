function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/Word-finder.exe", "@StartMenuDir@/Word-finder.lnk",
            "workingDirectory=@TargetDir@");
    }
}