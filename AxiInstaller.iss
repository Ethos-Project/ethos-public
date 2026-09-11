[Setup]
AppName=Axi IDE {FOSS Edition}
AppVersion=1.0
AppPublisher=Ethos
AppPublisherURL=https://ethos.dev
DefaultDirName={autopf}\Axi IDE
DefaultGroupName=Axi IDE
UninstallDisplayIcon={app}\axi_ide.exe
OutputDir=C:\Ethos\ethos-logos\RELEASES\FOSS\Installers
OutputBaseFilename=AxiSetup
SetupIconFile=
Compression=lzma
SolidCompression=yes
WizardStyle=modern
DisableProgramGroupPage=no
ChangesEnvironment=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"; Flags: checked

[Files]
; IDE
Source: "C:\Ethos\ethos-logos\RELEASES\FOSS\axi_ide\axi_ide.exe"; DestDir: "{app}"; DestName: "Axi IDE.exe"; Flags: ignoreversion
Source: "C:\Ethos\ethos-logos\RELEASES\FOSS\axi_ide\Scintilla.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Ethos\ethos-logos\RELEASES\FOSS\axi_ide\build.ps1"; DestDir: "{app}\src"; Flags: ignoreversion
Source: "C:\Ethos\ethos-logos\RELEASES\FOSS\axi_ide\src\main.cpp"; DestDir: "{app}\src"; Flags: ignoreversion

; CLI Tools
Source: "C:\Ethos\bin\axi.exe"; DestDir: "{app}\cli"; Flags: ignoreversion
Source: "C:\Ethos\bin\allos.exe"; DestDir: "{app}\cli"; Flags: ignoreversion
Source: "C:\Ethos\bin\libgcc_s_seh-1.dll"; DestDir: "{app}\cli"; Flags: ignoreversion
Source: "C:\Ethos\bin\libstdc++-6.dll"; DestDir: "{app}\cli"; Flags: ignoreversion
Source: "C:\Ethos\bin\libwinpthread-1.dll"; DestDir: "{app}\cli"; Flags: ignoreversion

[Icons]
Name: "{group}\Axi IDE"; Filename: "{app}\Axi IDE.exe"
Name: "{group}\Uninstall Axi IDE"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Axi IDE"; Filename: "{app}\Axi IDE.exe"; Tasks: desktopicon

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\cli"; Check: NeedsAddPath(ExpandConstant('{app}\cli'))

[Code]
var
  WorkspacePage: TInputDirWizardPage;

procedure InitializeWizard;
begin
  WorkspacePage := CreateInputDirPage(wpSelectDir,
    'Set Up Your Workspace',
    'Where would you like Axi to track your project files?',
    'Axi DVCS will initialize a repository in this folder. You can change this later by typing "axi root <path>" in the IDE console.',
    False, 'Select Folder');
  WorkspacePage.Add('');
  WorkspacePage.Values[0] := ExpandConstant('{userdocs}\AxiWorkspace');
end;

function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  Result := Pos(';' + Param + ';', ';' + OrigPath + ';') = 0;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  WorkspaceDir, ConfigDir, ConfigFile, AxiExe: String;
  ResultCode: Integer;
begin
  if CurStep = ssPostInstall then
  begin
    WorkspaceDir := WorkspacePage.Values[0];
    ConfigDir    := ExpandConstant('{userappdata}\Axi IDE');
    ConfigFile   := ConfigDir + '\workspace.cfg';

    ForceDirectories(ConfigDir);
    ForceDirectories(WorkspaceDir);
    SaveStringToFile(ConfigFile, WorkspaceDir + #13#10, False);

    AxiExe := ExpandConstant('"{app}\cli\axi.exe"');
    if not DirExists(WorkspaceDir + '\.axi') then
      Exec('cmd.exe', '/C ' + AxiExe + ' init', WorkspaceDir, SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;
end;
