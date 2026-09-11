import type { KeyboardEvent, RefObject } from "react";
import { ChevronUp, ChevronDown, Maximize2, Send } from "lucide-react";
import { motion, AnimatePresence } from "motion/react";
import { MessageRow } from "./MessageRow";
import { ThemeMenu } from "./ThemeMenu";
import type { Message } from "./types";

export function BarModePanel({
  visible,
  lastOutput,
  outputMsgs,
  outputExpanded,
  setOutputExpanded,
  input,
  setInput,
  onKeyDown,
  onSubmit,
  onOpenWindow,
  inputRef,
  themeId,
  onThemeChange,
  monogram = "C",
}: {
  visible: boolean;
  lastOutput: Message | undefined;
  outputMsgs: Message[];
  outputExpanded: boolean;
  setOutputExpanded: (v: boolean) => void;
  input: string;
  setInput: (v: string) => void;
  onKeyDown: (e: KeyboardEvent<HTMLInputElement>) => void;
  onSubmit: () => void;
  onOpenWindow: () => void;
  inputRef: RefObject<HTMLInputElement | null>;
  themeId: string;
  onThemeChange: (id: string) => void;
  monogram?: string;
}) {
  if (!visible) return null;

  return (
    <div className="fixed bottom-5 left-1/2 -translate-x-1/2 flex flex-col items-center gap-1.5 w-[580px] max-w-[calc(100vw-2rem)] z-50">
      {/* Peek / output panel */}
      <AnimatePresence mode="wait">
        {lastOutput && outputExpanded && (
          <motion.div
            key="peek-open"
            initial={{ opacity: 0, y: 6, scaleY: 0.95 }}
            animate={{ opacity: 1, y: 0, scaleY: 1 }}
            exit={{ opacity: 0, y: 6, scaleY: 0.95 }}
            transition={{ duration: 0.18, ease: [0.25, 0.1, 0.25, 1] }}
            style={{ transformOrigin: "bottom" }}
            className="w-full rounded-xl border border-[rgba(120,80,40,0.13)] bg-[rgba(253,246,238,0.95)] backdrop-blur-2xl shadow-[0_8px_32px_rgba(120,80,40,0.12)] overflow-hidden"
          >
            <div
              className="flex items-center justify-between px-4 h-8 border-b border-[rgba(120,80,40,0.1)] cursor-pointer hover:bg-[rgba(192,56,90,0.04)] transition-colors"
              onClick={() => setOutputExpanded(false)}
            >
              <span className="text-[9px] font-bold tracking-[0.2em] uppercase text-[#a07850] font-['Figtree']">
                Output
              </span>
              <div className="flex items-center gap-2.5">
                <span className="text-[10px] font-['JetBrains_Mono'] text-[#c0a880]">
                  {outputMsgs.length} entries
                </span>
                <ChevronDown size={11} className="text-[#a07850]" />
              </div>
            </div>

            <div className="max-h-56 overflow-y-auto px-4 py-3 space-y-3" style={{ scrollbarWidth: "none" }}>
              {outputMsgs.slice(-5).map((msg) => (
                <MessageRow key={msg.id} msg={msg} compact />
              ))}
            </div>
          </motion.div>
        )}

        {lastOutput && !outputExpanded && (
          <motion.div
            key="peek-closed"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            transition={{ duration: 0.12 }}
            className="w-full"
          >
            <div
              className="flex items-center gap-3 h-7 px-4 rounded-lg border border-[rgba(120,80,40,0.12)] bg-[rgba(253,246,238,0.8)] backdrop-blur-xl cursor-pointer hover:bg-[rgba(247,239,228,0.95)] transition-colors group shadow-sm"
              onClick={() => setOutputExpanded(true)}
            >
              <span className="rainbow-text-fast text-[10px] flex-1 truncate">
                {lastOutput.lines[lastOutput.lines.length - 1]}
              </span>
              {lastOutput.lines.length > 1 && (
                <span className="text-[10px] font-['JetBrains_Mono'] text-[#c0a880] shrink-0">
                  +{lastOutput.lines.length - 1}
                </span>
              )}
              <ChevronUp size={11} className="text-[#b09070] group-hover:text-[#8a6050] transition-colors shrink-0" />
            </div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* Input bar */}
      <div className="w-full h-[54px] rounded-2xl border border-[rgba(120,80,40,0.15)] bg-[rgba(253,246,238,0.92)] backdrop-blur-2xl shadow-[0_6px_32px_rgba(120,80,40,0.14),0_0_0_0.5px_rgba(120,80,40,0.08)] flex items-center gap-3 px-4">
        {/* Monogram */}
        <div className="shrink-0 w-8 h-8 rounded-[9px] bg-[rgb(var(--console-accent)/10%)] border border-[rgb(var(--console-accent)/22%)] flex items-center justify-center">
          <span className="text-[11px] font-['Figtree'] font-bold text-[rgb(var(--console-accent))] leading-none">{monogram}</span>
        </div>

        <span className="text-[11px] font-['JetBrains_Mono'] text-[#c0a880] shrink-0">~</span>

        <input
          ref={inputRef}
          className="flex-1 bg-transparent text-[13px] font-['JetBrains_Mono'] text-[#2c1a0e] placeholder:text-[#c8b098] outline-none caret-[rgb(var(--console-accent))]"
          placeholder="enter command..."
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyDown={onKeyDown}
          spellCheck={false}
          autoComplete="off"
        />

        <div className="flex items-center gap-1 shrink-0">
          <ThemeMenu
            themeId={themeId}
            onChange={onThemeChange}
            triggerClassName="w-7 h-7 rounded-[7px]"
            panelPosition="up"
          />
          <button
            onClick={onOpenWindow}
            className="w-7 h-7 rounded-[7px] flex items-center justify-center text-[#b09878] hover:text-[#8a6050] hover:bg-[rgba(120,80,40,0.08)] transition-all"
            title="Open window"
          >
            <Maximize2 size={12} />
          </button>
          <button
            onClick={onSubmit}
            className="w-7 h-7 rounded-[7px] flex items-center justify-center bg-[rgb(var(--console-accent)/12%)] border border-[rgb(var(--console-accent)/28%)] text-[rgb(var(--console-accent))] hover:bg-[rgb(var(--console-accent)/22%)] hover:border-[rgb(var(--console-accent)/45%)] transition-all"
            title="Send (Enter)"
          >
            <Send size={11} />
          </button>
        </div>
      </div>
    </div>
  );
}
