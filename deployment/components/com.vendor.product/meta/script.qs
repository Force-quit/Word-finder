function Component()
{
	
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    component.addOperation("CreateShortcut", "@TargetDir@/Word-finder.exe", "@StartMenuDir@/Word-finder.lnk", "workingDirectory=@TargetDir@");
    
    const documentsFolder = "@HomeDir@/Documents/Word-finder";
    component.addOperation("Mkdir", documentsFolder);

    const wordListsFolder = "@TargetDir@/resources/word-lists";
    component.addOperation("Copy", wordListsFolder + "/english.txt", documentsFolder);
    component.addOperation("Copy", wordListsFolder + "/francais.txt", documentsFolder);
}