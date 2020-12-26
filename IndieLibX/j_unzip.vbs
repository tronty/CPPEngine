 Dim WSHShell : Set WSHShell = CreateObject("WScript.Shell")
 Dim sAPPDATA : sAPPDATA = WSHShell.Environment("Volatile").Item ("APPDATA")
 Dim Path : Path = sAPPDATA & "\Microsoft\foldername\foldername1\foldername2"

 '// a normal function
 'Call CreateDirectoryPath(Path)

 '// a recursion function
 'Call Recursion(Path)

 Function CreateDirectoryPath(DirPath)
 Dim FSO, aDirectories, sCreateDirectory, iDirectory

 Set FSO = CreateObject("Scripting.FileSystemObject")
 If FSO.FolderExists(DirPath) Then
 Exit Function
 End If

 aDirectories = Split(DirPath, "\")
 sCreateDirectory = aDirectories(0)
 For iDirectory = 1 To UBound(aDirectories)
 sCreateDirectory = sCreateDirectory & "\" & aDirectories(iDirectory)
 If Not FSO.FolderExists(sCreateDirectory) Then
 FSO.CreateFolder(sCreateDirectory)
 End If
 Next
 End Function

 Function Recursion(DirectoryPath)
 Dim FSO : Set FSO = CreateObject("Scripting.FileSystemObject")

 If FSO.FolderExists(DirectoryPath) Then Exit Function
 Call Recursion(FSO.GetParentFolderName(DirectoryPath))
 FSO.CreateFolder(DirectoryPath)
 End Function
 
 
 
' j_unzip.vbs
'
' UnZip a file script
'
' By Justin Godden 2010
'
' It's a mess, I know!!!
'

' Dim ArgObj, var1, var2

'The location of the zip file.
REM Set WshShell = CreateObject("Wscript.Shell")
REM CurDir = WshShell.ExpandEnvironmentStrings("%%cd%%")
Dim FSO
Set FSO = CreateObject("Scripting.FileSystemObject")
Dim sCurPath
sCurPath = FSO.GetAbsolutePathName(".")

WScript.Echo ( "sCurPath = " & sCurPath)

Set ArgObj = WScript.Arguments

If (Wscript.Arguments.Count > 0) Then
 var1 = ArgObj(0)
 var2 = ArgObj(1)
 WScript.Echo ( "var1 = " & var1)
 WScript.Echo ( "var2 = " & var2)
Else
 var1 = ""
 var2 = ""
End if

If var1 = "" then
 strFileZIP = "example.zip"
Else
 strFileZIP = var1
End if

If var2 = "" then
 strDirZIP = ""
Else
 strDirZIP = var2
End if

WScript.Echo ( "Extracting file " & strFileZIP & " To " & strDirZIP)

strZipFile = sCurPath & "\" & strFileZIP
'The folder the contents should be extracted to.
outFolder = sCurPath & "\" & strDirZIP & "\"



 WScript.Echo ( "strZipFile = " & strZipFile)
 
 WScript.Echo ( "outFolder = " & outFolder)

Call Recursion(outFolder)

Set objShell = CreateObject( "Shell.Application" )
Set objSource = objShell.NameSpace(strZipFile).Items()
Set objTarget = objShell.NameSpace(outFolder)
intOptions = 256
objTarget.CopyHere objSource, intOptions

 WScript.Echo ( "Extracted." )

' This bit is for testing purposes
REM Dim MyVar
REM MyVar = MsgBox ( strZipFile, 65, "MsgBox Example"
