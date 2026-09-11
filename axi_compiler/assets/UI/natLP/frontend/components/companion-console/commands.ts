import { fmtClock, fmtDate } from "./format";

// Base command words used for Tab-completion. Multi-word commands (e.g. "tab new")
// are intercepted upstream before reaching respond(), but their root word still
// completes here.
export const COMMANDS = [
  "status", "help", "tasks", "history", "echo", "whoami",
  "date", "time", "version", "tabs", "tab", "theme",
  "clear", "close", "exit",
];

/**
 * Stateless command responses. Tab/theme management commands are handled
 * upstream (they need access to session/theme state) and never reach here.
 */
export function respond(raw: string): string[] | null {
  const trimmed = raw.trim();
  const c = trimmed.toLowerCase();
  if (!c) return null;

  if (c === "status" || c === "st") return [
    "All systems nominal. 3 background tasks running.",
    "Memory   2.4 GB / 16 GB   ████████░░░░░░░░   15%",
    "CPU      12% avg across 8 cores",
    "Uptime   4h 22m 11s",
    "Tasks    compile:watch  ·  sync:remote  ·  log:monitor",
  ];

  if (c === "help" || c === "?") return [
    "Available commands:",
    "  status          System resource overview",
    "  tasks           List active background tasks",
    "  history         Show command history for this session",
    "  echo <msg>      Print <msg> back to the console",
    "  whoami          Show current operator identity",
    "  date            Show current date",
    "  time            Show current time",
    "  version         Show console build version",
    "  tabs            List open session tabs",
    "  tab new         Open a new session tab",
    "  tab close       Close the current session tab",
    "  tab next/prev   Switch to the next/previous tab",
    "  tab rename <n>  Rename the current tab",
    "  theme           Show current theme and options",
    "  theme <name>    Switch console accent theme",
    "  clear           Clear console output",
    "  close           Minimize to bar",
    "  exit            Close the console",
    "",
    "Shortcuts: ↑/↓ history · Tab complete · Ctrl+L clear · Ctrl+U kill line · Ctrl+C cancel",
    "Tip: double-click a tab to rename it.",
  ];

  if (c === "tasks") return [
    "Active tasks (3):",
    "  [●] compile:watch    PID 4821   running   2h 14m",
    "  [●] sync:remote      PID 5102   running   1h 08m",
    "  [●] log:monitor      PID 5340   running   0h 44m",
  ];

  if (c === "echo" || c.startsWith("echo ")) {
    const text = trimmed.slice(4).trim();
    return [text];
  }

  if (c === "whoami") return ["console // operator: you"];
  if (c === "date") return [fmtDate(new Date())];
  if (c === "time") return [fmtClock(new Date())];
  if (c === "version" || c === "ver") return ["Companion Console — build 0.9.4"];

  if (c === "history") return ["__HISTORY__"];
  if (c === "clear")   return ["__CLEAR__"];
  if (c === "close")   return ["__CLOSE__"];
  if (c === "exit")    return ["Session closed. Goodbye."];

  return [`Unknown command: "${trimmed}"`, "Type 'help' to see available commands."];
}
