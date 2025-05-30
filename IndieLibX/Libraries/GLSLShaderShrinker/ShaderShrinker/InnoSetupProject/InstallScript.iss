#define MyAppName     "GLSL Shader Shrinker"
#define MyAppExeName  "Shrinker.WpfApp.exe"

[Setup]
AppId={{996B4B28-98DA-451F-ED15-8777E28DBDE4}
AppName={#MyAppName}
AppVersion=0.16
AppPublisher=Dean Edis
AppPublisherURL=https://github.com/deanthecoder/GLSLShaderShrinker
DefaultDirName={commonpf}\ShaderShrinker
DefaultGroupName={#MyAppName}
UninstallDisplayIcon={app}\Inno_Setup_Project.exe
Compression=lzma2
SolidCompression=yes
SourceDir=..\Shrinker.WpfApp\
OutputDir=..\InnoSetupProject\
OutputBaseFilename={#MyAppName} Installer

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "bin\Release\net5.0-windows\publish\win-x64\*.*"; DestDir: "{app}"; Excludes: "*.pdb"; Flags: ignoreversion
Source: "bin\Release\net5.0-windows\publish\win-x64\Presets\*.*"; DestDir: "{app}\Presets"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
