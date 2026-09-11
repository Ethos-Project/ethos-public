[Setup]
AppName=Axi IDE {FOSS Edition}
AppVersion=1.0
DefaultDirName={autopf}\Axi IDE
DefaultGroupName=Axi IDE
OutputDir=C:\Ethos\ethos-logos\RELEASES\FOSS\Installers
OutputBaseFilename=AxiSetup
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes

[Files]
Source: "C:\Ethos\ethos-logos\RELEASES\FOSS\axi_ide\axi_ide.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Ethos\ethos-logos\RELEASES\FOSS\axi_ide\Scintilla.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Ethos\bin\axi.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "C:\Ethos\bin\allos.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "C:\Ethos\bin\*.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

[Icons]
Name: "{group}\Axi IDE"; Filename: "{app}\axi_ide.exe"
Name: "{autodesktop}\Axi IDE"; Filename: "{app}\axi_ide.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\bin"; Check: NeedsAddPath(ExpandConstant('{app}\bin'))

[Code]
var
  WorkspacePage: TInputDirWizardPage;

procedure InitializeWizard;
begin
  WorkspacePage := CreateInputDirPage(wpSelectDir,
    'Select Workspace Root', 'Where should Axi track your files?',
    'Select the folder where you want your DVCS (Data Version Control System) root to be initialized.',
    False, '');
  WorkspacePage.Add('Workspace Root Directory:');
  
  // Default to a folder in Documents
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
  ConfigFile: String;
  ConfigDir: String;
  WorkspaceDir: String;
  AxiCmd: String;
  ResultCode: Integer;
begin
  if CurStep = ssPostInstall then
  begin
    WorkspaceDir := WorkspacePage.Values[0];
    ConfigDir := ExpandConstant('{userappdata}\Axi IDE');
    ConfigFile := ConfigDir + '\workspace.cfg';
    
    // Create config dir
    ForceDirectories(ConfigDir);
    
    // Write the workspace to the config
    SaveStringToFile(ConfigFile, WorkspaceDir + #13#10, False);
    
    // Create workspace dir if it doesn't exist
    ForceDirectories(WorkspaceDir);
    
    // Run 'axi init' if .axi doesn't exist
    if not DirExists(WorkspaceDir + '\.axi') then
    begin
      AxiCmd := ExpandConstant('"{app}\bin\axi.exe" init');
      Exec('cmd.exe', '/C "' + AxiCmd + '"', WorkspaceDir, SW_HIDE, ewWaitUntilTerminated, ResultCode);
    end;
  end;
end;

