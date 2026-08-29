using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace AxiCLI
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("Usage: axi <command>");
                Console.WriteLine("Commands: init, track, wrap, ship, status");
                return;
            }

            string command = args[0].ToLower();
            string currentDir = Directory.GetCurrentDirectory();
            string AxiDir = Path.Combine(currentDir, ".axi");
            
            string objectsDir = Path.Combine(AxiDir, "objects");
            string refsDir = Path.Combine(AxiDir, "refs");
            string headFile = Path.Combine(AxiDir, "HEAD");
            string configFile = Path.Combine(AxiDir, "axi_config.toon");

            switch (command)
            {
                case "init":
                    if (!Directory.Exists(AxiDir))
                    {
                        Directory.CreateDirectory(AxiDir);
                        Directory.CreateDirectory(objectsDir);
                        Directory.CreateDirectory(Path.Combine(refsDir, "heads"));
                        File.WriteAllText(headFile, "ref: refs/heads/main\n");
                        File.WriteAllText(Path.Combine(refsDir, "heads", "main"), "");
                        
                        File.WriteAllText(configFile, "tracked_directories[0]:\n");
                        
                        Console.WriteLine($"[Axi DVCS] Initialized local DAG ledger in {AxiDir}");
                        Console.WriteLine("[Axi DVCS] Created axi_config.toon (Run 'axi track <path>' to add folders)");
                    }
                    else
                    {
                        Console.WriteLine($"[Axi DVCS] Ledger already initialized in {AxiDir}.");
                    }
                    break;

                case "track":
                    if (!Directory.Exists(AxiDir))
                    {
                        Console.WriteLine("[Error] Not an Axi repository. Run 'axi init' first.");
                        return;
                    }
                    if (args.Length < 2)
                    {
                        Console.WriteLine("Usage: axi track <absolute_path>");
                        return;
                    }
                    
                    string targetPath = Path.GetFullPath(args[1]);
                    if (!Directory.Exists(targetPath))
                    {
                        Console.WriteLine($"[Error] Directory does not exist: {targetPath}");
                        return;
                    }

                    string[] configLines = File.ReadAllLines(configFile);
                    List<string> newConfig = new List<string>();
                    bool pathExists = false;

                    foreach (var line in configLines)
                    {
                        if (line.Trim() == targetPath) pathExists = true;
                        
                        if (line.StartsWith("tracked_directories["))
                        {
                            Match m = Regex.Match(line, @"\[(\d+)\]");
                            if (m.Success && !pathExists)
                            {
                                int currentCount = int.Parse(m.Groups[1].Value);
                                newConfig.Add($"tracked_directories[{currentCount + 1}]:");
                                continue;
                            }
                        }
                        newConfig.Add(line);
                    }

                    if (pathExists)
                    {
                        Console.WriteLine($"[Axi DVCS] '{targetPath}' is already being tracked.");
                        return;
                    }

                    newConfig.Add($"  {targetPath}");
                    File.WriteAllLines(configFile, newConfig);
                    Console.WriteLine($"[Axi DVCS] Now tracking: {targetPath}");
                    break;

                case "wrap":
                    if (!Directory.Exists(AxiDir))
                    {
                        Console.WriteLine("[Error] Not an Axi repository. Run 'axi init' first.");
                        return;
                    }

                    List<string> trackedDirs = new List<string>();
                    if (File.Exists(configFile))
                    {
                        string[] lines = File.ReadAllLines(configFile);
                        foreach (var line in lines)
                        {
                            if (!line.StartsWith("tracked_directories") && !string.IsNullOrWhiteSpace(line))
                            {
                                trackedDirs.Add(line.Trim());
                            }
                        }
                    }

                    if (trackedDirs.Count == 0)
                    {
                        Console.WriteLine("[Warning] No directories are being tracked. Run 'axi track <path>' before wrapping.");
                        return;
                    }

                    long timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
                    string salt = GetRandomSalt();

                    StringBuilder payloadBuilder = new StringBuilder();
                    foreach (var dir in trackedDirs)
                    {
                        if (Directory.Exists(dir))
                        {
                            string[] files = Directory.GetFileSystemEntries(dir);
                            for (int i = 0; i < files.Length; i++) files[i] = Path.GetFileName(files[i]);
                            payloadBuilder.Append($"[{Path.GetFileName(dir)}:{string.Join("|", files)}]");
                        }
                    }

                    string payload = payloadBuilder.ToString();
                    
                    using (SHA256 sha256 = SHA256.Create())
                    {
                        byte[] hashBytes = sha256.ComputeHash(Encoding.UTF8.GetBytes(payload + salt + timestamp.ToString()));
                        string digest = BitConverter.ToString(hashBytes).Replace("-", "").ToLower();

                        string wipPath = Path.Combine(objectsDir, $"wip-{digest.Substring(0, 40)}");
                        
                        StringBuilder toonBuilder = new StringBuilder();
                        toonBuilder.AppendLine("commit_node:");
                        toonBuilder.AppendLine($"  type: wip");
                        toonBuilder.AppendLine($"  hash: \"{digest}\"");
                        toonBuilder.AppendLine($"  timestamp: {timestamp}");
                        toonBuilder.AppendLine($"  salt: \"{salt}\"");
                        toonBuilder.AppendLine("semantic_layer[1]:");
                        toonBuilder.AppendLine($"  {payload}");
                        
                        File.WriteAllText(wipPath, toonBuilder.ToString());

                        Console.WriteLine($"[Axi DVCS] Snapshotting {trackedDirs.Count} tracked directories...");
                        Console.WriteLine($"[Axi DVCS] Created WIP State Node: wip-{digest.Substring(0, 10)} (TOON Format)");
                    }
                    break;

                case "status":
                    if (!Directory.Exists(AxiDir))
                    {
                        Console.WriteLine("[Error] Not an Axi repository. Run 'axi init' first.");
                        return;
                    }
                    string currentHead = File.ReadAllText(Path.Combine(refsDir, "heads", "main")).Trim();
                    if (string.IsNullOrEmpty(currentHead)) currentHead = "genesis";

                    Console.WriteLine($"[Axi DVCS] Workspace: {currentDir}");
                    Console.WriteLine($"Current Branch: main (HEAD -> {(currentHead.Length > 10 ? currentHead.Substring(0, 10) : currentHead)})");
                    Console.WriteLine("Status: Ready for 'axi wrap' (Snapshot) or 'axi ship' (Release).");
                    break;

                default:
                    Console.WriteLine($"Unknown Axi command: {command}");
                    break;
            }
        }

        static string GetRandomSalt()
        {
            using (SHA256 sha256 = SHA256.Create())
            {
                byte[] hashBytes = sha256.ComputeHash(Encoding.UTF8.GetBytes(DateTimeOffset.UtcNow.ToUnixTimeMilliseconds().ToString() + new Random().Next().ToString()));
                return BitConverter.ToString(hashBytes).Replace("-", "").ToLower().Substring(0, 16);
            }
        }
    }
}
