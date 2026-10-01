#define FileHandle FileOpen("..\ide\VERSION")
#define MyAppVersion Trim(FileRead(FileHandle))
#expr FileClose(FileHandle)

[Setup]
AppName=Axos IDE {{FOSS Edition}
AppVersion={#MyAppVersion}
AppPublisher=Ethos
AppPublisherURL=https://ethos.dev
DefaultDirName={autopf64}\Ethos\Axos IDE
DefaultGroupName=Axos IDE
UninstallDisplayIcon={app}\axosIDE.exe
OutputDir=C:\Ethos\RELEASES\FOSS\Installers
OutputBaseFilename=AxosSetup
SetupIconFile=
Compression=lzma
SolidCompression=yes
WizardStyle=modern
DisableProgramGroupPage=no
ChangesEnvironment=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"

[Files]
; IDE
Source: "C:\Ethos\Axos\ide\axosIDE.exe"; DestDir: "{app}"; DestName: "axosIDE.exe"; Flags: ignoreversion
Source: "C:\Ethos\Axos\ide\WebView2Loader.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Ethos\Axos\ide\axi.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Ethos\Axos\ide\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Ethos\Axos\ide\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Ethos\Axos\ide\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Ethos\Axos\ide\components\*"; DestDir: "{app}\components"; Flags: ignoreversion recursesubdirs createallsubdirs

; CLI Tools
Source: "C:\Ethos\Axos\bin\axos.exe"; DestDir: "{app}\cli"; Flags: ignoreversion
Source: "C:\Ethos\Axos\bin\avm_core.dll"; DestDir: "{app}\cli"; Flags: ignoreversion
Source: "C:\Ethos\Axi\bin\axi.exe"; DestDir: "{app}\cli"; Flags: ignoreversion

[Icons]
Name: "{group}\Axos IDE"; Filename: "{app}\axosIDE.exe"
Name: "{group}\Uninstall Axos IDE"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Axos IDE"; Filename: "{app}\axosIDE.exe"; Tasks: desktopicon

[Registry]
Root: HKLM; Subkey: "System\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\cli"; Check: NeedsAddPath(ExpandConstant('{app}\cli'))

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
  WorkspacePage.Values[0] := ExpandConstant('{userdocs}\AxosWorkspace');
end;

function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'System\CurrentControlSet\Control\Session Manager\Environment', 'Path', OrigPath)
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
    ConfigDir    := ExpandConstant('{userappdata}\Axos IDE');
    ConfigFile   := ConfigDir + '\workspace.cfg';

    ForceDirectories(ConfigDir);
    ForceDirectories(WorkspaceDir);
    SaveStringToFile(ConfigFile, WorkspaceDir + #13#10, False);

    AxiExe := ExpandConstant('"{app}\cli\axi.exe"');
    if not DirExists(WorkspaceDir + '\.axi') then
      Exec('cmd.exe', '/C ' + AxiExe + ' init', WorkspaceDir, SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;
end;
