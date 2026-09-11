import type { KeyboardEvent, RefObject } from "react";
import { Send } from "lucide-react";
import { motion, AnimatePresence } from "motion/react";
import { fmtClock } from "./format";
import { MessageRow } from "./MessageRow";
import { WindowTitleBar } from "./WindowTitleBar";
import { TabStrip } from "./TabStrip";
import type { Message, Session } from "./types";

export function WindowModePanel({
  visible,
  sessions,
  activeSessionId,
  onSwitchTab,
  onCloseTab,
  onAddTab,
  onRenameTab,
  messages,
  historyPaneRef,
  time,
  input,
  setInput,
  onKeyDown,
  onSubmit,
  windowInputRef,
  onMinimize,
  onClose,
  themeId,
  onThemeChange,
  label = "Console",
  prompt = "console ~",
}: {
  visible: boolean;
  sessions: Session[];
  activeSessionId: string;
  onSwitchTab: (id: string) => void;
  onCloseTab: (id: string) => void;
  onAddTab: () => void;
  onRenameTab: (id: string, label: string) => void;
  messages: Message[];
  historyPaneRef: RefObject<HTMLDivElement | null>;
  time: Date;
  input: string;
  setInput: (v: string) => void;
  onKeyDown: (e: KeyboardEvent<HTMLInputElement>) => void;
  onSubmit: () => void;
  windowInputRef: RefObject<HTMLInputElement | null>;
  onMinimize: () => void;
  onClose: () => void;
  themeId: string;
  onThemeChange: (id: string) => void;
  label?: string;
  prompt?: string;
}) {
  return (
    <AnimatePresence>
      {visible && (
        <motion.div
          initial={{ opacity: 0, scale: 0.97, y: 12 }}
          animate={{ opacity: 1, scale: 1, y: 0 }}
          exit={{ opacity: 0, scale: 0.97, y: 12 }}
          transition={{ duration: 0.22, ease: [0.25, 0.1, 0.25, 1] }}
          className="fixed inset-0 flex items-center justify-center z-50 pointer-events-none p-4"
        >
          <div className="pointer-events-auto w-[740px] h-[540px] max-w-[calc(100vw-2rem)] max-h-[calc(100vh-2rem)] rounded-[18px] border border-[rgba(120,80,40,0.14)] bg-[rgba(253,246,238,0.97)] backdrop-blur-3xl shadow-[0_24px_72px_rgba(120,80,40,0.18),0_0_0_0.5px_rgba(120,80,40,0.08)] flex flex-col overflow-hidden">
            <WindowTitleBar
              onMinimize={onMinimize}
              onClose={onClose}
              themeId={themeId}
              onThemeChange={onThemeChange}
              label={label}
            />

            <TabStrip
              sessions={sessions}
              activeId={activeSessionId}
              onSwitch={onSwitchTab}
              onClose={onCloseTab}
              onAdd={onAddTab}
              onRename={onRenameTab}
            />

            {/* Message history */}
            <div
              ref={historyPaneRef}
              className="flex-1 overflow-y-auto px-6 py-5 space-y-3.5"
              style={{ scrollbarWidth: "none" }}
            >
              {messages.length === 0 && (
                <p className="text-[11px] font-['JetBrains_Mono'] text-[#c8b098] italic">
                  Console cleared.
                </p>
              )}
              {messages.map((msg) => (
                <MessageRow key={msg.id} msg={msg} />
              ))}
            </div>

            {/* Status bar */}
            <div className="h-7 px-5 border-t border-[rgba(120,80,40,0.1)] bg-[rgba(240,232,220,0.6)] flex items-center gap-5 shrink-0">
              <span className="text-[9px] font-['JetBrains_Mono'] text-[#c0a880]">
                {messages.length} entries
              </span>
              <span className="text-[9px] font-['JetBrains_Mono'] text-[#c0a880]">
                {prompt}
              </span>
              <span className="flex-1" />
              <span className="text-[9px] font-['JetBrains_Mono'] text-[#a08868]">
                {fmtClock(time)}
              </span>
              <span className="flex items-center gap-1.5">
                <span className="w-1.5 h-1.5 rounded-full bg-[rgb(var(--console-accent))] opacity-80 animate-pulse" />
                <span className="text-[9px] font-['JetBrains_Mono'] text-[#b07060]">active</span>
              </span>
            </div>

            {/* Input row */}
            <div className="px-5 py-3 border-t border-[rgba(120,80,40,0.1)] bg-[rgba(247,239,228,0.7)] flex items-center gap-3 shrink-0">
              <span className="text-[12px] font-['JetBrains_Mono'] text-[rgb(var(--console-accent))] shrink-0 font-medium">
                {prompt}
              </span>
              <input
                ref={windowInputRef}
                className="flex-1 bg-transparent text-[13px] font-['JetBrains_Mono'] text-[#2c1a0e] placeholder:text-[#c8b098] outline-none caret-[rgb(var(--console-accent))]"
                placeholder="enter command..."
                value={input}
                onChange={(e) => setInput(e.target.value)}
                onKeyDown={onKeyDown}
                spellCheck={false}
                autoComplete="off"
              />
              <button
                onClick={onSubmit}
                className="shrink-0 px-3 h-7 rounded-md flex items-center gap-1.5 bg-[rgb(var(--console-accent)/10%)] border border-[rgb(var(--console-accent)/25%)] text-[rgb(var(--console-accent))] text-[11px] font-['Figtree'] font-semibold hover:bg-[rgb(var(--console-accent)/20%)] hover:border-[rgb(var(--console-accent)/40%)] transition-all"
              >
                <Send size={10} />
                Send
              </button>
            </div>
          </div>
        </motion.div>
      )}
    </AnimatePresence>
  );
}
