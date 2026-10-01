using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace AVM.UI
{
    public enum ConsoleLineType
    {
        Standard,
        Prompt,
        Error,
        Warning,
        System,
        AVMOutput
    }

    public struct ConsoleLine
    {
        public string Text;
        public ConsoleLineType Type;
        public float R, G, B, A;

        public ConsoleLine(string text, ConsoleLineType type = ConsoleLineType.Standard)
        {
            Text = text;
            Type = type;
            switch (type)
            {
                case ConsoleLineType.Prompt:
                    R = 0.35f; G = 0.75f; B = 1.0f; A = 1.0f; // Cyan-blue
                    break;
                case ConsoleLineType.Error:
                    R = 0.95f; G = 0.3f; B = 0.3f; A = 1.0f; // Red
                    break;
                case ConsoleLineType.Warning:
                    R = 1.0f; G = 0.8f; B = 0.2f; A = 1.0f; // Amber
                    break;
                case ConsoleLineType.System:
                    R = 0.4f; G = 0.9f; B = 0.6f; A = 1.0f; // Emerald
                    break;
                case ConsoleLineType.AVMOutput:
                    R = 0.85f; G = 0.65f; B = 1.0f; A = 1.0f; // Lavender/Purple
                    break;
                case ConsoleLineType.Standard:
                default:
                    R = 0.88f; G = 0.90f; B = 0.92f; A = 1.0f; // Bright soft white
                    break;
            }
        }
    }

    /// <summary>
    /// Cross-platform Terminal Session supporting Windows (pwsh/powershell/cmd),
    /// Linux (pwsh/bash/sh), and macOS (pwsh/zsh/bash).
    /// </summary>
    public class TerminalSession : IDisposable
    {
        private string _currentDirectory;
        private readonly List<string> _commandHistory = new List<string>();
        private int _historyIndex = -1;
        private Process? _activeProcess = null;
        private readonly object _processLock = new object();

        public string CurrentDirectory => _currentDirectory;
        public IReadOnlyList<string> History => _commandHistory;
        public bool IsRunningCommand => _activeProcess != null && !_activeProcess.HasExited;

        public string ShellName { get; private set; }
        public string ShellExecutable { get; private set; }

        public event Action<ConsoleLine>? OnOutputLine;

        public TerminalSession(string? initialDirectory = null)
        {
            _currentDirectory = Directory.Exists(initialDirectory) 
                ? Path.GetFullPath(initialDirectory) 
                : Environment.CurrentDirectory;

            (ShellName, ShellExecutable) = DetectBestShell();
        }

        private static (string name, string executable) DetectBestShell()
        {
            bool isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
            bool isLinux = RuntimeInformation.IsOSPlatform(OSPlatform.Linux);
            bool isMac = RuntimeInformation.IsOSPlatform(OSPlatform.OSX);

            // Cross-platform pwsh priority
            string? pwshPath = FindExecutableOnPath(isWindows ? "pwsh.exe" : "pwsh");
            if (!string.IsNullOrEmpty(pwshPath))
            {
                return ("PowerShell 7 (Core)", pwshPath);
            }

            if (isWindows)
            {
                // Fallback to Windows PowerShell 5.1
                string winPS = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System), 
                    @"WindowsPowerShell\v1.0\powershell.exe");
                if (File.Exists(winPS))
                {
                    return ("Windows PowerShell", winPS);
                }
                return ("Command Prompt", "cmd.exe");
            }

            if (isMac)
            {
                string? zshPath = FindExecutableOnPath("zsh");
                if (!string.IsNullOrEmpty(zshPath)) return ("Zsh", zshPath);
                string? bashPath = FindExecutableOnPath("bash");
                if (!string.IsNullOrEmpty(bashPath)) return ("Bash", bashPath);
                return ("Sh", "/bin/sh");
            }

            if (isLinux)
            {
                string? bashPath = FindExecutableOnPath("bash");
                if (!string.IsNullOrEmpty(bashPath)) return ("Bash", bashPath);
                return ("Sh", "/bin/sh");
            }

            return ("Shell", "cmd.exe");
        }

        private static string? FindExecutableOnPath(string executable)
        {
            if (File.Exists(executable)) return Path.GetFullPath(executable);

            string? pathEnv = Environment.GetEnvironmentVariable("PATH");
            if (string.IsNullOrEmpty(pathEnv)) return null;

            char sep = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? ';' : ':';
            string[] paths = pathEnv.Split(sep, StringSplitOptions.RemoveEmptyEntries);

            foreach (var path in paths)
            {
                try
                {
                    string fullPath = Path.Combine(path.Trim('\"', ' '), executable);
                    if (File.Exists(fullPath)) return fullPath;
                }
                catch { }
            }
            return null;
        }

        public void AddHistory(string command)
        {
            if (string.IsNullOrWhiteSpace(command)) return;
            if (_commandHistory.Count == 0 || _commandHistory[^1] != command)
            {
                _commandHistory.Add(command);
            }
            _historyIndex = _commandHistory.Count;
        }

        public string? GetPreviousHistory(string currentDraft)
        {
            if (_commandHistory.Count == 0) return null;
            if (_historyIndex > 0)
            {
                _historyIndex--;
                return _commandHistory[_historyIndex];
            }
            return _commandHistory[0];
        }

        public string? GetNextHistory()
        {
            if (_commandHistory.Count == 0) return null;
            if (_historyIndex < _commandHistory.Count - 1)
            {
                _historyIndex++;
                return _commandHistory[_historyIndex];
            }
            _historyIndex = _commandHistory.Count;
            return string.Empty;
        }

        public void ChangeDirectory(string newPath)
        {
            try
            {
                string target;
                if (string.IsNullOrWhiteSpace(newPath) || newPath == "~")
                {
                    target = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
                }
                else
                {
                    target = Path.GetFullPath(Path.Combine(_currentDirectory, newPath));
                }

                if (Directory.Exists(target))
                {
                    _currentDirectory = target;
                    Environment.CurrentDirectory = target;
                }
                else
                {
                    Emit(new ConsoleLine($"Directory not found: {newPath}", ConsoleLineType.Error));
                }
            }
            catch (Exception ex)
            {
                Emit(new ConsoleLine($"Error changing directory: {ex.Message}", ConsoleLineType.Error));
            }
        }

        public async Task ExecuteAsync(string commandLine, CancellationToken ct = default)
        {
            if (string.IsNullOrWhiteSpace(commandLine)) return;

            string trimmed = commandLine.Trim();

            // Native builtin: cd
            if (trimmed.Equals("cd", StringComparison.OrdinalIgnoreCase))
            {
                ChangeDirectory("~");
                return;
            }
            if (trimmed.StartsWith("cd ", StringComparison.OrdinalIgnoreCase))
            {
                string path = trimmed.Substring(3).Trim().Trim('"', '\'');
                ChangeDirectory(path);
                return;
            }

            // Execute in native shell
            ProcessStartInfo psi = new ProcessStartInfo
            {
                FileName = ShellExecutable,
                WorkingDirectory = _currentDirectory,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                RedirectStandardInput = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                StandardOutputEncoding = Encoding.UTF8,
                StandardErrorEncoding = Encoding.UTF8
            };

            if (ShellName.Contains("PowerShell", StringComparison.OrdinalIgnoreCase))
            {
                // Set working directory inside PowerShell context and run command
                string escapedDir = _currentDirectory.Replace("'", "''");
                string psCommand = $"Set-Location -LiteralPath '{escapedDir}'; {commandLine}";
                psi.Arguments = $"-NoProfile -NonInteractive -ExecutionPolicy Bypass -Command \"{psCommand}\"";
            }
            else if (ShellExecutable.EndsWith("cmd.exe", StringComparison.OrdinalIgnoreCase))
            {
                psi.Arguments = $"/c \"cd /d \"{_currentDirectory}\" && {commandLine}\"";
            }
            else
            {
                // Linux/macOS bash/zsh/sh
                psi.Arguments = $"-c \"cd '{_currentDirectory.Replace("'", "'\\''")}' && {commandLine.Replace("\"", "\\\"")}\"";
            }

            try
            {
                Process? process = null;
                lock (_processLock)
                {
                    process = Process.Start(psi);
                    _activeProcess = process;
                }

                if (process == null)
                {
                    Emit(new ConsoleLine($"Failed to start shell: {ShellExecutable}", ConsoleLineType.Error));
                    return;
                }

                Task outputTask = Task.Run(() =>
                {
                    try
                    {
                        using var reader = process.StandardOutput;
                        string? line;
                        while ((line = reader.ReadLine()) != null)
                        {
                            Emit(new ConsoleLine(line, ConsoleLineType.Standard));
                        }
                    }
                    catch { }
                }, ct);

                Task errorTask = Task.Run(() =>
                {
                    try
                    {
                        using var reader = process.StandardError;
                        string? line;
                        while ((line = reader.ReadLine()) != null)
                        {
                            Emit(new ConsoleLine(line, ConsoleLineType.Error));
                        }
                    }
                    catch { }
                }, ct);

                await Task.WhenAll(outputTask, errorTask);
                await process.WaitForExitAsync(ct);
            }
            catch (OperationCanceledException)
            {
                Emit(new ConsoleLine("[Command Cancelled / Interrupted]", ConsoleLineType.Warning));
            }
            catch (Exception ex)
            {
                Emit(new ConsoleLine($"Shell Error: {ex.Message}", ConsoleLineType.Error));
            }
            finally
            {
                lock (_processLock)
                {
                    _activeProcess = null;
                }
            }
        }

        public void CancelRunningProcess()
        {
            lock (_processLock)
            {
                if (_activeProcess != null && !_activeProcess.HasExited)
                {
                    try
                    {
                        _activeProcess.Kill(entireProcessTree: true);
                    }
                    catch { }
                }
            }
        }

        private void Emit(ConsoleLine line)
        {
            OnOutputLine?.Invoke(line);
        }

        public void Dispose()
        {
            CancelRunningProcess();
        }
    }
}
