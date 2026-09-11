import { useEffect, useRef, useState } from "react";
import { Plus, X } from "lucide-react";
import type { Session } from "./types";

export function TabStrip({
  sessions,
  activeId,
  onSwitch,
  onClose,
  onAdd,
  onRename,
}: {
  sessions: Session[];
  activeId: string;
  onSwitch: (id: string) => void;
  onClose: (id: string) => void;
  onAdd: () => void;
  onRename: (id: string, label: string) => void;
}) {
  const [renamingId, setRenamingId] = useState<string | null>(null);
  const [draft, setDraft] = useState("");
  const inputRef = useRef<HTMLInputElement>(null);

  useEffect(() => {
    if (renamingId) inputRef.current?.select();
  }, [renamingId]);

  const startRename = (s: Session) => {
    setRenamingId(s.id);
    setDraft(s.label);
  };

  const commitRename = () => {
    if (renamingId) onRename(renamingId, draft);
    setRenamingId(null);
  };

  return (
    <div
      className="h-8 px-2 flex items-center gap-1 border-b border-[rgba(120,80,40,0.1)] bg-[rgba(240,232,220,0.55)] shrink-0 overflow-x-auto"
      style={{ scrollbarWidth: "none" }}
    >
      {sessions.map((s) => {
        const active = s.id === activeId;
        return (
          <div
            key={s.id}
            onClick={() => onSwitch(s.id)}
            onDoubleClick={() => startRename(s)}
            className={`group shrink-0 flex items-center gap-1.5 h-6 pl-2.5 pr-1.5 rounded-md cursor-pointer transition-colors ${
              active
                ? "bg-[rgb(var(--console-accent)/12%)] border border-[rgb(var(--console-accent)/28%)]"
                : "border border-transparent hover:bg-[rgba(120,80,40,0.06)]"
            }`}
          >
            {renamingId === s.id ? (
              <input
                ref={inputRef}
                autoFocus
                value={draft}
                onChange={(e) => setDraft(e.target.value)}
                onBlur={commitRename}
                onClick={(e) => e.stopPropagation()}
                onKeyDown={(e) => {
                  if (e.key === "Enter") {
                    e.preventDefault();
                    commitRename();
                  }
                  if (e.key === "Escape") {
                    e.preventDefault();
                    e.stopPropagation();
                    setRenamingId(null);
                  }
                }}
                className="bg-transparent outline-none text-[10.5px] font-['JetBrains_Mono'] text-[#2c1a0e] w-20"
              />
            ) : (
              <span
                className={`text-[10.5px] font-['JetBrains_Mono'] truncate max-w-[110px] ${
                  active ? "text-[rgb(var(--console-accent))]" : "text-[#8a6050]"
                }`}
              >
                {s.label}
              </span>
            )}
            {sessions.length > 1 && (
              <button
                onClick={(e) => {
                  e.stopPropagation();
                  onClose(s.id);
                }}
                className="w-3.5 h-3.5 shrink-0 rounded-sm flex items-center justify-center text-[#b09878] opacity-0 group-hover:opacity-100 hover:text-[#8a6050] hover:bg-[rgba(120,80,40,0.12)] transition-all"
                title="Close tab"
                aria-label={`Close ${s.label}`}
              >
                <X size={9} />
              </button>
            )}
          </div>
        );
      })}
      <button
        onClick={onAdd}
        className="shrink-0 w-6 h-6 rounded-md flex items-center justify-center text-[#b09878] hover:text-[rgb(var(--console-accent))] hover:bg-[rgb(var(--console-accent)/10%)] transition-all"
        title="New tab"
        aria-label="New session tab"
      >
        <Plus size={12} />
      </button>
    </div>
  );
}
