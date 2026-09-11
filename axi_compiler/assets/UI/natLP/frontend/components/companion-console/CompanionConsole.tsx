import { useState, useRef, useEffect, useCallback, type KeyboardEvent } from "react";
import "./theme.css";
import type { Message, Session, CSSVars } from "./types";
import { THEMES, getTheme } from "./themes";
import { COMMANDS, respond } from "./commands";
import { loadState, saveState, createSession, getActiveSession, DEFAULT_STORAGE_KEY, type PersistedState } from "./storage";
import { uid } from "./format";
import { BarModePanel } from "./BarModePanel";
import { WindowModePanel } from "./WindowModePanel";

type ViewMode = "bar" | "window";

export interface CompanionConsoleProps {
  /**
   * localStorage key used to persist sessions/history/theme. Give each mounted
   * instance its own key if more than one console can be open in the same app.
   * Defaults to a shared key so a single console's history survives reloads.
   */
  storageKey?: string;
  /** Text shown in the expanded window's title bar. Defaults to "Console". */
  label?: string;
  /** Single character shown in the floating bar's monogram badge. Defaults to "C". */
  monogram?: string;
  /** Prompt string shown in the expanded window's status bar and input row. Defaults to "console ~". */
  prompt?: string;
  /** Extra classes applied to the (non-visual, zero-footprint) wrapper element. */
  className?: string;
}

/**
 * A floating, self-contained command console widget. Drop it once near the root
 * of any app — it renders as a fixed-position overlay (a compact bar by default,
 * expandable into a full window) and manages its own state and persistence.
 * It has no layout footprint of its own, so it is safe to mount anywhere in the tree.
 */
export default function CompanionConsole({
  storageKey = DEFAULT_STORAGE_KEY,
  label = "Console",
  monogram = "C",
  prompt = "console ~",
  className,
}: CompanionConsoleProps) {
  const [state, setState] = useState<PersistedState>(() => loadState(storageKey));
  const [viewMode, setViewMode] = useState<ViewMode>("bar");
  const [outputExpanded, setOutputExpanded] = useState(false);
  const [input, setInput] = useState("");
  const [histIdx, setHistIdx] = useState(-1);
  const [time, setTime] = useState(new Date());

  const barInputRef = useRef<HTMLInputElement>(null);
  const windowInputRef = useRef<HTMLInputElement>(null);
  const historyPaneRef = useRef<HTMLDivElement>(null);

  const { sessions, activeSessionId, themeId } = state;
  const activeSession = getActiveSession(state);
  const theme = getTheme(themeId);

  useEffect(() => {
    const id = setInterval(() => setTime(new Date()), 1000);
    return () => clearInterval(id);
  }, []);

  useEffect(() => {
    const ref = viewMode === "bar" ? barInputRef : windowInputRef;
    setTimeout(() => ref.current?.focus(), 80);
  }, [viewMode]);

  useEffect(() => {
    if (historyPaneRef.current) {
      historyPaneRef.current.scrollTop = historyPaneRef.current.scrollHeight;
    }
  }, [activeSession.messages, viewMode]);

  useEffect(() => {
    saveState(state, storageKey);
  }, [state, storageKey]);

  // Escape minimizes the window back to bar mode even when focus isn't on the input.
  useEffect(() => {
    if (viewMode !== "window") return;
    const onKeyDown = (e: globalThis.KeyboardEvent) => {
      if (e.key === "Escape") setViewMode("bar");
    };
    window.addEventListener("keydown", onKeyDown);
    return () => window.removeEventListener("keydown", onKeyDown);
  }, [viewMode]);

  const updateSession = useCallback((id: string, updater: (s: Session) => Session) => {
    setState((prev) => ({ ...prev, sessions: prev.sessions.map((s) => (s.id === id ? updater(s) : s)) }));
  }, []);

  const pushMessages = useCallback(
    (id: string, msgs: Message[]) => {
      updateSession(id, (s) => ({ ...s, messages: [...s.messages, ...msgs] }));
    },
    [updateSession]
  );

  const clearConsole = useCallback(() => {
    updateSession(activeSessionId, (s) => ({ ...s, messages: [] }));
    setOutputExpanded(false);
  }, [activeSessionId, updateSession]);

  const resetInputState = useCallback(() => {
    setInput("");
    setHistIdx(-1);
    setOutputExpanded(false);
  }, []);

  const addTab = useCallback(() => {
    setState((prev) => {
      const label2 = `Session ${prev.nextSessionSeq}`;
      const next = createSession(label2);
      return {
        ...prev,
        sessions: [...prev.sessions, next],
        activeSessionId: next.id,
        nextSessionSeq: prev.nextSessionSeq + 1,
      };
    });
    resetInputState();
  }, [resetInputState]);

  const closeTab = useCallback(
    (id: string) => {
      setState((prev) => {
        if (prev.sessions.length <= 1) return prev;
        const idx = prev.sessions.findIndex((s) => s.id === id);
        const remaining = prev.sessions.filter((s) => s.id !== id);
        let nextActiveId = prev.activeSessionId;
        if (id === prev.activeSessionId) {
          const fallback = remaining[Math.max(0, idx - 1)] ?? remaining[0];
          nextActiveId = fallback ? fallback.id : prev.activeSessionId;
        }
        return { ...prev, sessions: remaining, activeSessionId: nextActiveId };
      });
      resetInputState();
    },
    [resetInputState]
  );

  const switchTab = useCallback(
    (id: string) => {
      setState((prev) => ({ ...prev, activeSessionId: id }));
      resetInputState();
    },
    [resetInputState]
  );

  const cycleTab = useCallback(
    (direction: 1 | -1) => {
      setState((prev) => {
        const idx = prev.sessions.findIndex((s) => s.id === prev.activeSessionId);
        if (idx === -1) return prev;
        const nextIdx = (idx + direction + prev.sessions.length) % prev.sessions.length;
        const next = prev.sessions[nextIdx];
        return next ? { ...prev, activeSessionId: next.id } : prev;
      });
      resetInputState();
    },
    [resetInputState]
  );

  const renameTab = useCallback(
    (id: string, newLabel: string) => {
      const trimmedLabel = newLabel.trim();
      if (!trimmedLabel) return;
      updateSession(id, (s) => ({ ...s, label: trimmedLabel }));
    },
    [updateSession]
  );

  const setThemeId = useCallback((id: string) => {
    setState((prev) => ({ ...prev, themeId: id }));
  }, []);

  const handleSubmit = useCallback(() => {
    const trimmed = input.trim();
    setInput("");
    setHistIdx(-1);
    if (!trimmed) return;

    const activeId = activeSessionId;
    const nowIso = () => new Date().toISOString();
    const userMsg: Message = { id: uid(), type: "user", lines: [trimmed], timestamp: nowIso() };
    updateSession(activeId, (s) => ({
      ...s,
      messages: [...s.messages, userMsg],
      cmdHistory: [trimmed, ...s.cmdHistory.slice(0, 49)],
    }));

    const c = trimmed.toLowerCase();
    const tabRenameMatch = /^tab rename\s+(.+)$/i.exec(trimmed);
    const themeSetMatch = /^theme\s+(\S+)$/i.exec(trimmed);

    if (c === "tab new") {
      addTab();
      return;
    }
    if (c === "tab close") {
      if (sessions.length <= 1) {
        pushMessages(activeId, [
          { id: uid(), type: "error", lines: ["Cannot close the only open session."], timestamp: nowIso() },
        ]);
        return;
      }
      closeTab(activeId);
      return;
    }
    if (c === "tab next") {
      cycleTab(1);
      return;
    }
    if (c === "tab prev" || c === "tab previous") {
      cycleTab(-1);
      return;
    }
    if (tabRenameMatch) {
      const name = (tabRenameMatch[1] ?? "").trim();
      renameTab(activeId, name);
      pushMessages(activeId, [
        { id: uid(), type: "system", lines: [`Renamed session to "${name}".`], timestamp: nowIso() },
      ]);
      return;
    }
    if (c === "tabs") {
      const lines = sessions.map((s, i) => `  ${i + 1}. ${s.label}${s.id === activeId ? "  (active)" : ""}`);
      pushMessages(activeId, [{ id: uid(), type: "system", lines: ["Open sessions:", ...lines], timestamp: nowIso() }]);
      if (viewMode === "bar") setOutputExpanded(true);
      return;
    }
    if (c === "theme") {
      const lines = THEMES.map((t) => `  ${t.id.padEnd(8)} ${t.name}${t.id === themeId ? "  (current)" : ""}`);
      pushMessages(activeId, [
        { id: uid(), type: "system", lines: ["Available themes:", ...lines, "", "Usage: theme <name>"], timestamp: nowIso() },
      ]);
      if (viewMode === "bar") setOutputExpanded(true);
      return;
    }
    if (themeSetMatch) {
      const requested = (themeSetMatch[1] ?? "").toLowerCase();
      const found = THEMES.find((t) => t.id === requested);
      if (found) {
        setThemeId(found.id);
        pushMessages(activeId, [{ id: uid(), type: "system", lines: [`Theme set to ${found.name}.`], timestamp: nowIso() }]);
      } else {
        pushMessages(activeId, [
          {
            id: uid(),
            type: "error",
            lines: [`Unknown theme: "${requested}"`, "Type 'theme' to see available themes."],
            timestamp: nowIso(),
          },
        ]);
      }
      if (viewMode === "bar") setOutputExpanded(true);
      return;
    }

    const result = respond(trimmed);
    if (result === null) return;

    if (result[0] === "__CLEAR__") {
      clearConsole();
      return;
    }
    if (result[0] === "__CLOSE__") {
      setViewMode("bar");
      setOutputExpanded(false);
      return;
    }

    if (result[0] === "__HISTORY__") {
      const session = sessions.find((s) => s.id === activeId);
      const cmdHistory = session ? session.cmdHistory : [];
      const histLines =
        cmdHistory.length === 0
          ? ["No command history yet."]
          : cmdHistory.map((cmd, i) => `  ${String(i + 1).padStart(2, "0")}  ${cmd}`);
      pushMessages(activeId, [{ id: uid(), type: "ai", lines: ["Command history:", ...histLines], timestamp: nowIso() }]);
      if (viewMode === "bar") setOutputExpanded(true);
      return;
    }

    pushMessages(activeId, [{ id: uid(), type: "ai", lines: result, timestamp: nowIso() }]);
    if (viewMode === "bar") setOutputExpanded(true);
  }, [
    input,
    activeSessionId,
    sessions,
    themeId,
    viewMode,
    updateSession,
    addTab,
    closeTab,
    cycleTab,
    renameTab,
    pushMessages,
    clearConsole,
    setThemeId,
  ]);

  const handleTabComplete = useCallback(() => {
    if (input.includes(" ")) return; // only complete the command word itself, not arguments
    const prefix = input.toLowerCase();
    if (!prefix) return;

    const matches = COMMANDS.filter((cmd) => cmd.startsWith(prefix));
    if (matches.length === 1) {
      const match = matches[0];
      if (match) setInput(match + " ");
      return;
    }
    if (matches.length > 1) {
      pushMessages(activeSessionId, [
        { id: uid(), type: "system", lines: [matches.join("   ")], timestamp: new Date().toISOString() },
      ]);
      if (viewMode === "bar") setOutputExpanded(true);
    }
  }, [input, viewMode, activeSessionId, pushMessages]);

  const handleInterrupt = useCallback(() => {
    setHistIdx(-1);
    setInput((current) => {
      const trimmedCurrent = current.trim();
      if (trimmedCurrent) {
        pushMessages(activeSessionId, [
          { id: uid(), type: "user", lines: [`${current} ^C`], timestamp: new Date().toISOString() },
        ]);
      }
      return "";
    });
  }, [activeSessionId, pushMessages]);

  const handleKeyDown = (e: KeyboardEvent<HTMLInputElement>) => {
    if (e.key === "Enter") {
      handleSubmit();
      return;
    }

    if (e.key === "Tab") {
      e.preventDefault();
      handleTabComplete();
      return;
    }

    if (e.ctrlKey && e.key.toLowerCase() === "l") {
      e.preventDefault();
      clearConsole();
      return;
    }

    if (e.ctrlKey && e.key.toLowerCase() === "c") {
      e.preventDefault();
      handleInterrupt();
      return;
    }

    if (e.ctrlKey && e.key.toLowerCase() === "u") {
      e.preventDefault();
      setInput("");
      setHistIdx(-1);
      return;
    }

    if (e.key === "ArrowUp") {
      e.preventDefault();
      const hist = activeSession.cmdHistory;
      const next = Math.min(histIdx + 1, hist.length - 1);
      setHistIdx(next);
      setInput(hist[next] ?? "");
      return;
    }
    if (e.key === "ArrowDown") {
      e.preventDefault();
      const next = Math.max(histIdx - 1, -1);
      const hist = activeSession.cmdHistory;
      setHistIdx(next);
      setInput(next === -1 ? "" : hist[next] ?? "");
      return;
    }
    if (e.key === "Escape") {
      if (viewMode === "window") setViewMode("bar");
      else setOutputExpanded(false);
    }
  };

  const outputMsgs = activeSession.messages.filter((m) => m.type !== "user");
  const lastOutput = outputMsgs[outputMsgs.length - 1];

  const rootStyle: CSSVars = { "--console-accent": theme.accentRgb };

  return (
    <div className={className} style={rootStyle}>
      <BarModePanel
        visible={viewMode === "bar"}
        lastOutput={lastOutput}
        outputMsgs={outputMsgs}
        outputExpanded={outputExpanded}
        setOutputExpanded={setOutputExpanded}
        input={input}
        setInput={setInput}
        onKeyDown={handleKeyDown}
        onSubmit={handleSubmit}
        onOpenWindow={() => setViewMode("window")}
        inputRef={barInputRef}
        themeId={themeId}
        onThemeChange={setThemeId}
        monogram={monogram}
      />

      <WindowModePanel
        visible={viewMode === "window"}
        sessions={sessions}
        activeSessionId={activeSessionId}
        onSwitchTab={switchTab}
        onCloseTab={closeTab}
        onAddTab={addTab}
        onRenameTab={renameTab}
        messages={activeSession.messages}
        historyPaneRef={historyPaneRef}
        time={time}
        input={input}
        setInput={setInput}
        onKeyDown={handleKeyDown}
        onSubmit={handleSubmit}
        windowInputRef={windowInputRef}
        onMinimize={() => setViewMode("bar")}
        onClose={() => setViewMode("bar")}
        themeId={themeId}
        onThemeChange={setThemeId}
        label={label}
        prompt={prompt}
      />
    </div>
  );
}
