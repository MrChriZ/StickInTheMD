; StickInTheMD Windows installer (Inno Setup 6+)
; Single-file setup.exe embedding stickinthemd.exe (static MSVC; uses OS WebView2).
; Build: scripts/build-windows-installer.ps1

#ifndef MyAppVersion
  #define MyAppVersion "1.0.0"
#endif

#ifndef MyAppSourceDir
  #define MyAppSourceDir "..\..\build\Release"
#endif

#define MyAppName "StickInTheMD"
#define MyAppPublisher "StickInTheMD"
#define MyAppExeName "stickinthemd.exe"
#define MyAppProgId "StickInTheMDMarkdown"

[Setup]
AppId={{E8C4A21D-9F6B-4D2E-A1C3-5B7D8E9F0A1B}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=LICENSE.txt
OutputDir=..\..\dist
OutputBaseFilename=StickInTheMD-{#MyAppVersion}-setup
SetupIconFile=..\..\assets\icon\stickinthemd.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
Compression=lzma2/max
SolidCompression=yes
DiskSpanning=no
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
WizardStyle=modern
MinVersion=10.0

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional shortcuts:"; Flags: unchecked
Name: "assocmd"; Description: "Make {#MyAppName} the default app for &Markdown (.md, .markdown) files"; GroupDescription: "File associations:"; Flags: checkedonce

[Files]
Source: "{#MyAppSourceDir}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "THIRD_PARTY_NOTICES.txt"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Registry]
; ProgId
Root: HKA; Subkey: "Software\Classes\{#MyAppProgId}"; ValueType: string; ValueData: "Markdown Document"; Tasks: assocmd; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\{#MyAppProgId}\DefaultIcon"; ValueType: string; ValueData: "{app}\{#MyAppExeName},0"; Tasks: assocmd; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\{#MyAppProgId}\shell\open\command"; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: assocmd; Flags: uninsdeletevalue

; Extensions
Root: HKA; Subkey: "Software\Classes\.md"; ValueType: string; ValueData: "{#MyAppProgId}"; Tasks: assocmd; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\.markdown"; ValueType: string; ValueData: "{#MyAppProgId}"; Tasks: assocmd; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\.md\OpenWithProgids\{#MyAppProgId}"; Flags: uninsdeletevalue; Tasks: assocmd
Root: HKA; Subkey: "Software\Classes\.markdown\OpenWithProgids\{#MyAppProgId}"; Flags: uninsdeletevalue; Tasks: assocmd

; Settings > Default apps
Root: HKA; Subkey: "Software\{#MyAppName}\Capabilities"; ValueType: string; ValueName: "ApplicationName"; ValueData: "{#MyAppName}"; Tasks: assocmd; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\{#MyAppName}\Capabilities"; ValueType: string; ValueName: "ApplicationDescription"; ValueData: "Markdown editor with live preview"; Tasks: assocmd
Root: HKA; Subkey: "Software\{#MyAppName}\Capabilities\FileAssociations"; ValueType: string; ValueName: ".md"; ValueData: "{#MyAppProgId}"; Tasks: assocmd
Root: HKA; Subkey: "Software\{#MyAppName}\Capabilities\FileAssociations"; ValueType: string; ValueName: ".markdown"; ValueData: "{#MyAppProgId}"; Tasks: assocmd
Root: HKA; Subkey: "Software\RegisteredApplications"; ValueType: string; ValueName: "{#MyAppName}"; ValueData: "Software\{#MyAppName}\Capabilities"; Tasks: assocmd; Flags: uninsdeletevalue

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent
