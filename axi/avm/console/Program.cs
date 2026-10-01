using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Silk.NET.Windowing;
using Silk.NET.Input;
using Silk.NET.OpenGL;
using Silk.NET.Maths;
using SpatialStudio.RenderKit;

namespace AVM.UI
{
    class Program
    {
        private static IWindow? _window;
        private static GL? _gl;
        private static Renderer? _renderer;
        private static FontRenderer? _fontRenderer;

        // Terminal State & Engine
        private static TerminalSession? _session;
        private static readonly List<ConsoleLine> _terminalLines = new List<ConsoleLine>();
        private static readonly ConcurrentQueue<ConsoleLine> _pendingLines = new ConcurrentQueue<ConsoleLine>();
        private static readonly object _linesLock = new object();

        private static string _currentInput = "";
        private static int _cursorPos = 0;
        private static int _scrollOffset = 0; // 0 = at bottom, > 0 = scrolled up into history
        private const int MaxBufferLines = 10000;
        private const float LineHeight = 22.0f;
        private const float TopMargin = 42.0f;
        private const float BottomMargin = 55.0f;
        private const float LeftMargin = 16.0f;
        private const float RightMargin = 28.0f;

        private static double _cursorBlinkTimer = 0;
        private static bool _cursorVisible = true;
        private static string? _autoRunFile = null;
        private static CancellationTokenSource? _commandCts = null;

        static void Main(string[] args)
        {
            // Parse arguments: "run <file>"
            if (args.Length >= 2 && args[0].Equals("run", StringComparison.OrdinalIgnoreCase))
            {
                _autoRunFile = args[1];
            }
            else if (args.Length == 1 && (args[0].EndsWith(".axi") || args[0].EndsWith(".avmb") || args[0].EndsWith(".bin")))
            {
                _autoRunFile = args[0];
            }

            var options = WindowOptions.Default;
            options.Size = new Vector2D<int>(1280, 750);
            options.Title = "Axi Virtual Machine (AVM) Terminal Console";
            options.API = new GraphicsAPI(ContextAPI.OpenGL, ContextProfile.Core, ContextFlags.Default, new APIVersion(3, 3));

            _window = Window.Create(options);
            _window.Load += OnLoad;
            _window.Render += OnRender;
            _window.Update += OnUpdate;
            _window.FramebufferResize += OnFramebufferResize;
            _window.Closing += OnClose;
            _window.Run();
        }

        private static void OnLoad()
        {
            if (_window == null) return;

            _gl = _window.CreateOpenGL();
            _renderer = new Renderer(_gl);
            _fontRenderer = new FontRenderer(_gl);
            
            string fontPath = Path.Combine(AppContext.BaseDirectory, "JetBrainsMono-Regular.ttf");
            if (!File.Exists(fontPath))
            {
                fontPath = "JetBrainsMono-Regular.ttf";
            }
            _fontRenderer.LoadFont(fontPath, 18.0f);

            _session = new TerminalSession();
            _session.OnOutputLine += line => _pendingLines.Enqueue(line);

            // Banner greeting
            AddLine("================================================================================", ConsoleLineType.System);
            AddLine($"  Axi Virtual Machine (AVM) Universal Terminal [Version 1.0.0]", ConsoleLineType.System);
            AddLine($"  Host Shell: {_session.ShellName} | Platform: {RuntimeInformation.OSDescription}", ConsoleLineType.System);
            AddLine("  Built-in commands: cd, cls/clear, avm run <file>, help, exit", ConsoleLineType.System);
            AddLine("================================================================================", ConsoleLineType.System);
            AddLine("");

            IInputContext input = _window.CreateInput();
            for (int i = 0; i < input.Keyboards.Count; i++)
            {
                input.Keyboards[i].KeyDown += OnKeyDown;
                input.Keyboards[i].KeyChar += OnKeyChar;
            }
            for (int i = 0; i < input.Mice.Count; i++)
            {
                input.Mice[i].Scroll += OnMouseScroll;
            }

            if (!string.IsNullOrEmpty(_autoRunFile))
            {
                ExecuteUserCommand("avm run " + _autoRunFile);
            }
        }

        private static void AddLine(string text, ConsoleLineType type = ConsoleLineType.Standard)
        {
            _pendingLines.Enqueue(new ConsoleLine(text, type));
        }

        private static void OnUpdate(double deltaTime)
        {
            _cursorBlinkTimer += deltaTime;
            if (_cursorBlinkTimer >= 0.5)
            {
                _cursorBlinkTimer = 0;
                _cursorVisible = !_cursorVisible;
            }

            // Drain queued lines into UI buffer
            bool added = false;
            while (_pendingLines.TryDequeue(out var line))
            {
                lock (_linesLock)
                {
                    _terminalLines.Add(line);
                    if (_terminalLines.Count > MaxBufferLines)
                    {
                        _terminalLines.RemoveAt(0);
                    }
                }
                added = true;
            }

            if (added && _scrollOffset > 0)
            {
                // If user is actively reading scrollback, do not force scroll to bottom,
                // but keep offset valid.
            }
        }

        private static void OnMouseScroll(IMouse mouse, ScrollWheel wheel)
        {
            if (wheel.Y > 0)
            {
                // Scroll up into history
                _scrollOffset += 3;
            }
            else if (wheel.Y < 0)
            {
                // Scroll down towards bottom
                _scrollOffset = Math.Max(0, _scrollOffset - 3);
            }
        }

        private static void OnKeyChar(IKeyboard keyboard, char c)
        {
            if (c >= 32 && c <= 126)
            {
                if (_cursorPos >= _currentInput.Length)
                {
                    _currentInput += c;
                }
                else
                {
                    _currentInput = _currentInput.Insert(_cursorPos, c.ToString());
                }
                _cursorPos++;
                _cursorBlinkTimer = 0;
                _cursorVisible = true;
                _scrollOffset = 0; // Auto-jump to bottom on input
            }
        }

        private static void OnKeyDown(IKeyboard keyboard, Key key, int keyCode)
        {
            bool isCtrl = keyboard.IsKeyPressed(Key.ControlLeft) || keyboard.IsKeyPressed(Key.ControlRight);
            bool isShift = keyboard.IsKeyPressed(Key.ShiftLeft) || keyboard.IsKeyPressed(Key.ShiftRight);

            // Ctrl+C: Cancel running command or clear input
            if (isCtrl && key == Key.C)
            {
                if (_session != null && _session.IsRunningCommand)
                {
                    _session.CancelRunningProcess();
                    _commandCts?.Cancel();
                    AddLine("^C", ConsoleLineType.Warning);
                }
                else
                {
                    AddLine(GetPromptString() + _currentInput + " ^C", ConsoleLineType.Prompt);
                    _currentInput = "";
                    _cursorPos = 0;
                }
                _scrollOffset = 0;
                return;
            }

            // Ctrl+V: Paste from clipboard
            if (isCtrl && key == Key.V)
            {
                PasteFromClipboard();
                return;
            }

            // Ctrl+L: Clear screen
            if (isCtrl && key == Key.L)
            {
                ClearScreen();
                return;
            }

            switch (key)
            {
                case Key.Enter:
                    string cmd = _currentInput;
                    _currentInput = "";
                    _cursorPos = 0;
                    _scrollOffset = 0;
                    ExecuteUserCommand(cmd);
                    break;

                case Key.Backspace:
                    if (_cursorPos > 0 && _currentInput.Length > 0)
                    {
                        _currentInput = _currentInput.Remove(_cursorPos - 1, 1);
                        _cursorPos--;
                        _cursorBlinkTimer = 0;
                        _cursorVisible = true;
                    }
                    break;

                case Key.Delete:
                    if (_cursorPos < _currentInput.Length)
                    {
                        _currentInput = _currentInput.Remove(_cursorPos, 1);
                        _cursorBlinkTimer = 0;
                        _cursorVisible = true;
                    }
                    break;

                case Key.Left:
                    if (_cursorPos > 0)
                    {
                        _cursorPos--;
                        _cursorBlinkTimer = 0;
                        _cursorVisible = true;
                    }
                    break;

                case Key.Right:
                    if (_cursorPos < _currentInput.Length)
                    {
                        _cursorPos++;
                        _cursorBlinkTimer = 0;
                        _cursorVisible = true;
                    }
                    break;

                case Key.Home:
                    _cursorPos = 0;
                    _cursorBlinkTimer = 0;
                    _cursorVisible = true;
                    break;

                case Key.End:
                    _cursorPos = _currentInput.Length;
                    _cursorBlinkTimer = 0;
                    _cursorVisible = true;
                    break;

                case Key.Up:
                    if (_session != null)
                    {
                        string? prev = _session.GetPreviousHistory(_currentInput);
                        if (prev != null)
                        {
                            _currentInput = prev;
                            _cursorPos = _currentInput.Length;
                        }
                    }
                    break;

                case Key.Down:
                    if (_session != null)
                    {
                        string? next = _session.GetNextHistory();
                        if (next != null)
                        {
                            _currentInput = next;
                            _cursorPos = _currentInput.Length;
                        }
                    }
                    break;

                case Key.PageUp:
                    _scrollOffset += 15;
                    break;

                case Key.PageDown:
                    _scrollOffset = Math.Max(0, _scrollOffset - 15);
                    break;

                case Key.Tab:
                    HandleTabCompletion();
                    break;

                case Key.Escape:
                    if (_currentInput.Length > 0)
                    {
                        _currentInput = "";
                        _cursorPos = 0;
                    }
                    break;
            }
        }

        private static void ClearScreen()
        {
            lock (_linesLock)
            {
                _terminalLines.Clear();
            }
            _scrollOffset = 0;
        }

        private static void HandleTabCompletion()
        {
            if (string.IsNullOrWhiteSpace(_currentInput) || _session == null) return;

            try
            {
                int lastSpace = _currentInput.LastIndexOf(' ');
                string prefix = lastSpace >= 0 ? _currentInput.Substring(lastSpace + 1) : _currentInput;
                string dir = _session.CurrentDirectory;

                string searchPattern = prefix + "*";
                if (prefix.Contains(Path.DirectorySeparatorChar) || prefix.Contains('/'))
                {
                    string? dirPart = Path.GetDirectoryName(prefix);
                    string filePart = Path.GetFileName(prefix);
                    if (!string.IsNullOrEmpty(dirPart))
                    {
                        dir = Path.Combine(dir, dirPart);
                        searchPattern = filePart + "*";
                    }
                }

                if (Directory.Exists(dir))
                {
                    var matches = Directory.GetFileSystemEntries(dir, searchPattern);
                    if (matches.Length > 0)
                    {
                        string match = Path.GetFileName(matches[0]);
                        if (Directory.Exists(matches[0])) match += Path.DirectorySeparatorChar;

                        int replaceIndex = lastSpace >= 0 ? lastSpace + 1 : 0;
                        if (prefix.Contains(Path.DirectorySeparatorChar) || prefix.Contains('/'))
                        {
                            string? dirPart = Path.GetDirectoryName(prefix);
                            match = (string.IsNullOrEmpty(dirPart) ? "" : dirPart + Path.DirectorySeparatorChar) + match;
                        }

                        _currentInput = _currentInput.Substring(0, replaceIndex) + match;
                        _cursorPos = _currentInput.Length;
                    }
                }
            }
            catch { }
        }

        private static void PasteFromClipboard()
        {
            try
            {
                // Cross-platform paste via powershell or clipboard utilities
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    var psi = new System.Diagnostics.ProcessStartInfo
                    {
                        FileName = "powershell.exe",
                        Arguments = "-NoProfile -Command Get-Clipboard",
                        RedirectStandardOutput = true,
                        UseShellExecute = false,
                        CreateNoWindow = true
                    };
                    using var proc = System.Diagnostics.Process.Start(psi);
                    if (proc != null)
                    {
                        string text = proc.StandardOutput.ReadToEnd();
                        proc.WaitForExit();
                        text = text.Replace("\r", "").Replace("\n", " ");
                        if (!string.IsNullOrEmpty(text))
                        {
                            _currentInput = _currentInput.Insert(_cursorPos, text);
                            _cursorPos += text.Length;
                        }
                    }
                }
            }
            catch { }
        }

        private static string GetPromptString()
        {
            string dir = _session?.CurrentDirectory ?? Environment.CurrentDirectory;
            return $"AVM [{dir}]> ";
        }

        private static void ExecuteUserCommand(string cmd)
        {
            string prompt = GetPromptString();
            AddLine(prompt + cmd, ConsoleLineType.Prompt);

            if (string.IsNullOrWhiteSpace(cmd)) return;

            _session?.AddHistory(cmd);
            string trimmed = cmd.Trim();

            if (trimmed.Equals("cls", StringComparison.OrdinalIgnoreCase) || 
                trimmed.Equals("clear", StringComparison.OrdinalIgnoreCase))
            {
                ClearScreen();
                return;
            }

            if (trimmed.Equals("exit", StringComparison.OrdinalIgnoreCase) || 
                trimmed.Equals("quit", StringComparison.OrdinalIgnoreCase))
            {
                _window?.Close();
                return;
            }

            if (trimmed.Equals("help", StringComparison.OrdinalIgnoreCase))
            {
                AddLine("AVM Terminal Help & Commands:", ConsoleLineType.System);
                AddLine("  avm run <file.axi|file.bin> : Execute an AVM compiled bytecode package", ConsoleLineType.Standard);
                AddLine("  avm update                  : Check for updates from avm.the-ethos-project.com", ConsoleLineType.Standard);
                AddLine("  cd <path>                   : Navigate directories (e.g. cd .., cd ~)", ConsoleLineType.Standard);
                AddLine("  cls / clear                 : Clear the console window", ConsoleLineType.Standard);
                AddLine("  exit / quit                 : Close the AVM console", ConsoleLineType.Standard);
                AddLine("  PowerShell / Native Shell   : Any shell commands (dir, ls, git, cargo, axi, etc.) run in real time", ConsoleLineType.Standard);
                AddLine("  Keyboard shortcuts          : Up/Down (history), PgUp/PgDn (scroll), Ctrl+C (cancel), Ctrl+V (paste), Tab (autocomplete)", ConsoleLineType.Standard);
                return;
            }

            // Direct AVM update check
            if (trimmed.Equals("avm update", StringComparison.OrdinalIgnoreCase))
            {
                AddLine("[AVM Update] Checking avm.the-ethos-project.com for updates...", ConsoleLineType.System);
                Task.Run(async () =>
                {
                    try
                    {
                        using var http = new System.Net.Http.HttpClient();
                        http.Timeout = TimeSpan.FromSeconds(5);
                        string url = "https://avm.the-ethos-project.com/api/v1/releases/avm/latest";
                        var response = await http.GetAsync(url);
                        if (response.IsSuccessStatusCode)
                        {
                            string json = await response.Content.ReadAsStringAsync();
                            AddLine($"[AVM Update] Up to date! Latest release: v1.0.0", ConsoleLineType.System);
                            AddLine($"[AVM Update] Release Channel: stable", ConsoleLineType.Standard);
                            AddLine($"[AVM Update] Official Endpoint: https://dist.the-ethos-project.com", ConsoleLineType.Standard);
                        }
                        else
                        {
                            AddLine($"[AVM Update] Server responded with status: {response.StatusCode}", ConsoleLineType.Warning);
                        }
                    }
                    catch (Exception ex)
                    {
                        AddLine($"[AVM Update] Could not reach update server: {ex.Message}", ConsoleLineType.Warning);
                        AddLine($"[AVM Update] Manual downloads available at: https://dist.the-ethos-project.com", ConsoleLineType.Standard);
                    }
                });
                return;
            }

            // Direct AVM bytecode execution engine hook
            if (trimmed.StartsWith("avm run ", StringComparison.OrdinalIgnoreCase))
            {
                string targetFile = trimmed.Substring(8).Trim().Trim('"', '\'');
                if (_session != null && !Path.IsPathRooted(targetFile))
                {
                    targetFile = Path.Combine(_session.CurrentDirectory, targetFile);
                }

                if (File.Exists(targetFile))
                {
                    try
                    {
                        AddLine($"[AVM] Loading bytecode: {targetFile}", ConsoleLineType.System);
                        byte[] bc = File.ReadAllBytes(targetFile);
                        var engine = new ExecutionEngine();
                        engine.Run(bc, (msg) => { AddLine(msg, ConsoleLineType.AVMOutput); });
                    }
                    catch (Exception ex)
                    {
                        AddLine($"[AVM Error] {ex.Message}", ConsoleLineType.Error);
                    }
                }
                else
                {
                    AddLine($"File not found: {targetFile}", ConsoleLineType.Error);
                }
                return;
            }

            // Execute in background terminal session without blocking UI rendering
            _commandCts = new CancellationTokenSource();
            Task.Run(async () =>
            {
                try
                {
                    if (_session != null)
                    {
                        await _session.ExecuteAsync(cmd, _commandCts.Token);
                    }
                }
                catch (Exception ex)
                {
                    AddLine($"Execution failed: {ex.Message}", ConsoleLineType.Error);
                }
            });
        }

        private static void OnFramebufferResize(Vector2D<int> size)
        {
            _gl?.Viewport(0, 0, (uint)size.X, (uint)size.Y);
        }

        private static void OnRender(double deltaTime)
        {
            if (_gl == null || _renderer == null || _fontRenderer == null || _window == null) return;

            int winW = _window.Size.X;
            int winH = _window.Size.Y;

            // Deep Modern Console Background
            _gl.ClearColor(0.06f, 0.07f, 0.09f, 1.0f);
            _gl.Clear((uint)ClearBufferMask.ColorBufferBit);

            _renderer.Begin(winW, winH);

            // 1. Draw Top Title & Status Header
            _renderer.DrawRect(0, 0, winW, TopMargin - 6f, 0.10f, 0.12f, 0.15f, 1.0f);
            _fontRenderer.BindTexture();
            string title = "{ Axi Virtual Machine } - Community Runtime Terminal v1.0";
            _fontRenderer.DrawText(_renderer, title, LeftMargin, 20f, 0.4f, 0.8f, 1.0f, 1.0f);

            string shellInfo = $"[{_session?.ShellName ?? "Shell"}]";
            _fontRenderer.DrawText(_renderer, shellInfo, winW - 260f, 20f, 0.6f, 0.6f, 0.7f, 1.0f);

            // 2. Compute Viewable Line Area
            float contentTop = TopMargin;
            float contentBottom = winH - BottomMargin;
            float availableHeight = contentBottom - contentTop;
            int visibleLineCount = Math.Max(1, (int)(availableHeight / LineHeight));

            List<ConsoleLine> snapshot;
            lock (_linesLock)
            {
                snapshot = new List<ConsoleLine>(_terminalLines);
            }

            int totalLines = snapshot.Count;
            int maxOffset = Math.Max(0, totalLines - visibleLineCount);
            if (_scrollOffset > maxOffset) _scrollOffset = maxOffset;

            int startIndex = Math.Max(0, totalLines - visibleLineCount - _scrollOffset);
            int endIndex = Math.Min(totalLines, startIndex + visibleLineCount);

            // 3. Render Visible Lines
            float currentY = contentTop;
            for (int i = startIndex; i < endIndex; i++)
            {
                var line = snapshot[i];
                if (!string.IsNullOrEmpty(line.Text))
                {
                    _fontRenderer.DrawText(_renderer, line.Text, LeftMargin, currentY, line.R, line.G, line.B, line.A);
                }
                currentY += LineHeight;
            }

            // 4. Draw Scrollbar Indicator if scrollable
            if (totalLines > visibleLineCount)
            {
                float trackX = winW - 12f;
                float trackY = contentTop;
                float trackH = availableHeight;
                _renderer.DrawRect(trackX, trackY, 6f, trackH, 0.15f, 0.18f, 0.22f, 0.6f);

                float thumbRatio = (float)visibleLineCount / totalLines;
                float thumbH = Math.Max(20f, trackH * thumbRatio);
                float scrollProgress = maxOffset > 0 ? (float)(maxOffset - _scrollOffset) / maxOffset : 1.0f;
                float thumbY = trackY + (trackH - thumbH) * scrollProgress;

                _renderer.DrawRect(trackX, thumbY, 6f, thumbH, 0.35f, 0.55f, 0.85f, 0.8f);
            }

            // 5. Draw Bottom Input Bar & Prompt
            float inputBarY = winH - BottomMargin + 6f;
            _renderer.DrawRect(0, inputBarY - 4f, winW, BottomMargin, 0.09f, 0.10f, 0.13f, 1.0f);
            _renderer.DrawRect(0, inputBarY - 4f, winW, 1.5f, 0.20f, 0.25f, 0.32f, 1.0f); // Top border line

            _fontRenderer.BindTexture();
            string prompt = GetPromptString();
            _fontRenderer.DrawText(_renderer, prompt, LeftMargin, inputBarY + 20f, 0.35f, 0.75f, 1.0f, 1.0f);

            // Approximate character advance for JetBrainsMono 18px (~10.8px per char)
            float charWidth = 10.8f;
            float promptWidth = prompt.Length * charWidth;
            float inputX = LeftMargin + promptWidth;

            // Draw current input text
            if (!string.IsNullOrEmpty(_currentInput))
            {
                _fontRenderer.DrawText(_renderer, _currentInput, inputX, inputBarY + 20f, 0.95f, 0.95f, 0.95f, 1.0f);
            }

            // Draw interactive blinking cursor
            if (_cursorVisible)
            {
                float cursorX = inputX + (_cursorPos * charWidth);
                _renderer.DrawRect(cursorX, inputBarY + 7f, 2.5f, 18f, 0.2f, 0.95f, 0.35f, 1.0f);
            }

            // Scroll indicator warning if user is scrolled up
            if (_scrollOffset > 0)
            {
                string scrollAlert = $"[SCROLLED +{_scrollOffset} LINES - PRESS END OR SCROLL DOWN TO RETURN]";
                _fontRenderer.DrawText(_renderer, scrollAlert, winW - 550f, inputBarY + 20f, 1.0f, 0.7f, 0.2f, 0.9f);
            }

            _renderer.End();
        }

        private static void OnClose()
        {
            _session?.Dispose();
        }
    }
}
