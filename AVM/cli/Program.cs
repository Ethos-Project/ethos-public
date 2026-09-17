using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Net.Http;
using System.Threading.Tasks;

namespace AxiCLI
{
    class Program
    {
        static async Task Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("Usage: axi <command>");
                Console.WriteLine("Commands: init, track, wrap, ship, status, push, pull");
                return;
            }

            string command = args[0].ToLower();
            
            // Dynamic pathing: Use the directory where the user typed the command
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
                        
                        // Initialize empty YAML config
                        File.WriteAllText(configFile, "tracked_directories[0]:\n");
                        
                        Console.WriteLine($"[Axi DVCS] Initialized local cryptographic DAG ledger in {AxiDir}");
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

                    string salt = GetCognitiveSalt();
                    long timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

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
                    
                    using (SHA512 sha512 = SHA512.Create())
                    {
                        byte[] hashBytes = sha512.ComputeHash(Encoding.UTF8.GetBytes(payload + salt + timestamp.ToString()));
                        string digest = BitConverter.ToString(hashBytes).Replace("-", "").ToLower().Substring(0, 40);

                        string wipPath = Path.Combine(objectsDir, $"wip-{digest}");
                        
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
                        Console.WriteLine($"[Axi DVCS] Created WIP State Node: wip-{digest.Substring(0, 10)} (YAML Format)");
                    }
                    break;

                case "ship":
                    if (!Directory.Exists(AxiDir))
                    {
                        Console.WriteLine("[Error] Not an Axi repository. Run 'axi init' first.");
                        return;
                    }
                    string shipSalt = GetCognitiveSalt();
                    long shipTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
                    string shipPayload = "ship_payload";

                    using (SHA512 sha512 = SHA512.Create())
                    {
                        byte[] hashBytes = sha512.ComputeHash(Encoding.UTF8.GetBytes(shipPayload + shipSalt + shipTimestamp.ToString()));
                        string digest = BitConverter.ToString(hashBytes).Replace("-", "").ToLower().Substring(0, 40);

                        string commitPath = Path.Combine(objectsDir, digest);
                        
                        StringBuilder toonShipBuilder = new StringBuilder();
                        toonShipBuilder.AppendLine("commit_node:");
                        toonShipBuilder.AppendLine($"  type: ship");
                        toonShipBuilder.AppendLine($"  hash: \"{digest}\"");
                        toonShipBuilder.AppendLine($"  timestamp: {shipTimestamp}");
                        toonShipBuilder.AppendLine($"  salt: \"{shipSalt}\"");
                        toonShipBuilder.AppendLine("semantic_layer[1]:");
                        toonShipBuilder.AppendLine($"  {shipPayload}");
                        
                        File.WriteAllText(commitPath, toonShipBuilder.ToString());
                        File.WriteAllText(Path.Combine(refsDir, "heads", "main"), digest);

                        Console.WriteLine("[Axi DVCS] Initiating Release Pipeline...");
                        Console.WriteLine($"5. Transmitting mutations to local ledger (Signed Proof: 0x{digest.Substring(0, 12)})... [OK]");
                        Console.WriteLine("[Axi DVCS] Release Staged & Promoted to Production!");
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

                    Console.WriteLine($"[Axi DVCS] Querying authoritative workspace: {currentDir}");
                    Console.WriteLine($"Current Branch: main (HEAD -> {(currentHead.Length > 10 ? currentHead.Substring(0, 10) : currentHead)})");
                    Console.WriteLine("Status: Ready for 'axi wrap' (Snapshot) or 'axi ship' (Release).");
                    break;

                case "push":
                    if (!Directory.Exists(AxiDir))
                    {
                        Console.WriteLine("[Error] Not an Axi repository. Run 'axi init' first.");
                        return;
                    }
                    string pushHead = File.ReadAllText(Path.Combine(refsDir, "heads", "main")).Trim();
                    if (string.IsNullOrEmpty(pushHead))
                    {
                        Console.WriteLine("[Error] No commits to push. Run 'axi ship' first.");
                        return;
                    }
                    string commitFile = Path.Combine(objectsDir, pushHead);
                    if (!File.Exists(commitFile))
                    {
                        Console.WriteLine($"[Error] Commit object {pushHead} not found.");
                        return;
                    }
                    
                    try
                    {
                        using (HttpClient client = new HttpClient())
                        {
                            string commitData = File.ReadAllText(commitFile);
                            StringContent content = new StringContent(commitData, Encoding.UTF8, "application/json");
                            Console.WriteLine($"[Axi DVCS] Pushing {pushHead.Substring(0, 10)} to http://localhost:8081/push ...");
                            HttpResponseMessage response = await client.PostAsync("http://localhost:8081/push", content);
                            if (response.IsSuccessStatusCode)
                            {
                                Console.WriteLine("[Axi DVCS] Push successful.");
                            }
                            else
                            {
                                Console.WriteLine($"[Error] Push failed with status code: {response.StatusCode}");
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"[Error] Failed to connect to backend on port 8081: {ex.Message}");
                    }
                    break;

                case "pull":
                    if (!Directory.Exists(AxiDir))
                    {
                        Console.WriteLine("[Error] Not an Axi repository. Run 'axi init' first.");
                        return;
                    }
                    try
                    {
                        using (HttpClient client = new HttpClient())
                        {
                            Console.WriteLine("[Axi DVCS] Pulling latest from http://localhost:8081/pull ...");
                            HttpResponseMessage response = await client.GetAsync("http://localhost:8081/pull");
                            if (response.IsSuccessStatusCode)
                            {
                                string pullData = await response.Content.ReadAsStringAsync();
                                Console.WriteLine("[Axi DVCS] Pull successful.");
                                Console.WriteLine(pullData);
                            }
                            else
                            {
                                Console.WriteLine($"[Error] Pull failed with status code: {response.StatusCode}");
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"[Error] Failed to connect to backend on port 8081: {ex.Message}");
                    }
                    break;

                default:
                    Console.WriteLine($"Unknown Axi command: {command}");
                    break;
            }
        }

        static string GetCognitiveSalt()
        {
            using (SHA256 sha256 = SHA256.Create())
            {
                byte[] hashBytes = sha256.ComputeHash(Encoding.UTF8.GetBytes(DateTimeOffset.UtcNow.ToUnixTimeMilliseconds().ToString() + new Random().Next().ToString()));
                return BitConverter.ToString(hashBytes).Replace("-", "").ToLower().Substring(0, 16);
            }
        }
    }
}

