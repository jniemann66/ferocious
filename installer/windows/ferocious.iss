[Setup]
AppName=ferocious
AppVersion={#AppVersion}
AppPublisher=jniemann66
AppPublisherURL=https://github.com/jniemann66/ferocious
DefaultDirName={autopf}\ferocious
DefaultGroupName=ferocious
OutputDir=.
OutputBaseFilename=ferocious-{#AppVersion}-windows-setup
SetupIconFile=..\..\ferocious\ferocious.ico
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "deploy\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\ferocious"; Filename: "{app}\ferocious.exe"
Name: "{group}\{cm:UninstallProgram,ferocious}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\ferocious"; Filename: "{app}\ferocious.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\ferocious.exe"; Description: "{cm:LaunchProgram,ferocious}"; Flags: nowait postinstall skipifsilent
