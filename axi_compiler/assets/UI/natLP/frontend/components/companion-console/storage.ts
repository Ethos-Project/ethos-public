import type { Message, Session } from "./types";
import { DEFAULT_THEME_ID } from "./themes";
import { uid } from "./format";

/** Default localStorage key. Pass a different `storageKey` prop to <CompanionConsole /> to isolate multiple instances. */
export const DEFAULT_STORAGE_KEY = "companion-console-state-v1";

const MESSAGE_TYPES = new Set(["system", "user", "ai", "error"]);

export interface PersistedState {
  sessions: Session[];
  activeSessionId: string;
  themeId: string;
  nextSessionSeq: number;
}

function seedMessages(): Message[] {
  const now = Date.now();
  return [
    {
      id: "s1",
      type: "system",
      lines: ["Console — build 0.9.4", "Session opened"],
      timestamp: new Date(now - 1000 * 260).toISOString(),
    },
    {
      id: "s2",
      type: "ai",
      lines: ["Good evening. I'm here when you need me."],
      timestamp: new Date(now - 1000 * 200).toISOString(),
    },
    {
      id: "s3",
      type: "user",
      lines: ["status"],
      timestamp: new Date(now - 1000 * 140).toISOString(),
    },
    {
      id: "s4",
      type: "ai",
      lines: [
        "All systems nominal. 3 background tasks running.",
        "Memory   2.4 GB / 16 GB   ████████░░░░░░░░   15%",
        "CPU      12% avg across 8 cores",
        "Uptime   4h 22m 11s",
        "Tasks    compile:watch  ·  sync:remote  ·  log:monitor",
      ],
      timestamp: new Date(now - 1000 * 80).toISOString(),
    },
  ];
}

export function createSession(label: string, messages: Message[] = []): Session {
  return { id: uid(), label, messages, cmdHistory: [] };
}

function defaultState(): PersistedState {
  const first = createSession("Session 1", seedMessages());
  return { sessions: [first], activeSessionId: first.id, themeId: DEFAULT_THEME_ID, nextSessionSeq: 2 };
}

export function getActiveSession(state: PersistedState): Session {
  const found = state.sessions.find((s) => s.id === state.activeSessionId);
  if (found) return found;
  const fallback = state.sessions[0];
  if (fallback) return fallback;
  // Unreachable in practice: state always carries at least one session.
  return createSession("Session 1");
}

function isMessage(x: unknown): x is Message {
  if (typeof x !== "object" || x === null) return false;
  const m = x as Record<string, unknown>;
  return (
    typeof m["id"] === "string" &&
    typeof m["type"] === "string" &&
    MESSAGE_TYPES.has(m["type"]) &&
    Array.isArray(m["lines"]) &&
    typeof m["timestamp"] === "string"
  );
}

function isSession(x: unknown): x is Session {
  if (typeof x !== "object" || x === null) return false;
  const s = x as Record<string, unknown>;
  return (
    typeof s["id"] === "string" &&
    typeof s["label"] === "string" &&
    Array.isArray(s["messages"]) &&
    s["messages"].every(isMessage) &&
    Array.isArray(s["cmdHistory"])
  );
}

export function loadState(storageKey: string = DEFAULT_STORAGE_KEY): PersistedState {
  try {
    const raw = localStorage.getItem(storageKey);
    if (!raw) return defaultState();
    const parsed: unknown = JSON.parse(raw);
    if (typeof parsed !== "object" || parsed === null) return defaultState();
    const p = parsed as Record<string, unknown>;

    const rawSessions = p["sessions"];
    if (!Array.isArray(rawSessions)) return defaultState();
    const sessions: Session[] = [];
    for (const s of rawSessions) {
      if (isSession(s)) sessions.push(s);
    }
    const first = sessions[0];
    if (!first) return defaultState();

    const requestedActiveId = typeof p["activeSessionId"] === "string" ? p["activeSessionId"] : first.id;
    const activeSessionId = sessions.some((s) => s.id === requestedActiveId) ? requestedActiveId : first.id;
    const themeId = typeof p["themeId"] === "string" ? p["themeId"] : DEFAULT_THEME_ID;
    const nextSessionSeq = typeof p["nextSessionSeq"] === "number" ? p["nextSessionSeq"] : sessions.length + 1;

    return { sessions, activeSessionId, themeId, nextSessionSeq };
  } catch {
    return defaultState();
  }
}

export function saveState(state: PersistedState, storageKey: string = DEFAULT_STORAGE_KEY) {
  try {
    localStorage.setItem(storageKey, JSON.stringify(state));
  } catch {
    // ignore storage errors (e.g. private browsing quota)
  }
}
