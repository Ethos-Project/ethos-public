import { useEffect, useRef, useState } from "react";
import { Palette, Check } from "lucide-react";
import { THEMES } from "./themes";

export function ThemeMenu({
  themeId,
  onChange,
  triggerClassName = "w-6 h-6 rounded-[6px]",
  panelPosition = "down",
}: {
  themeId: string;
  onChange: (id: string) => void;
  triggerClassName?: string;
  panelPosition?: "up" | "down";
}) {
  const [open, setOpen] = useState(false);
  const rootRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (!open) return;
    const onMouseDown = (e: MouseEvent) => {
      if (rootRef.current && !rootRef.current.contains(e.target as Node)) setOpen(false);
    };
    document.addEventListener("mousedown", onMouseDown);
    return () => document.removeEventListener("mousedown", onMouseDown);
  }, [open]);

  useEffect(() => {
    if (!open) return;
    // Capture phase so this beats the page-level Escape-to-minimize listener.
    const onKeyDown = (e: globalThis.KeyboardEvent) => {
      if (e.key === "Escape") {
        e.stopPropagation();
        setOpen(false);
      }
    };
    window.addEventListener("keydown", onKeyDown, true);
    return () => window.removeEventListener("keydown", onKeyDown, true);
  }, [open]);

  return (
    <div ref={rootRef} className="relative">
      <button
        onClick={() => setOpen((v) => !v)}
        className={`${triggerClassName} flex items-center justify-center text-[#b09878] hover:text-[rgb(var(--console-accent))] hover:bg-[rgb(var(--console-accent)/10%)] transition-all`}
        title="Theme"
        aria-label="Change theme"
      >
        <Palette size={12} />
      </button>
      {open && (
        <div
          className={`absolute right-0 ${panelPosition === "up" ? "bottom-8" : "top-8"} z-[60] w-40 rounded-lg border border-[rgba(120,80,40,0.15)] bg-[rgba(253,246,238,0.98)] backdrop-blur-xl shadow-[0_8px_24px_rgba(120,80,40,0.18)] overflow-hidden py-1`}
        >
          {THEMES.map((t) => (
            <button
              key={t.id}
              onClick={() => {
                onChange(t.id);
                setOpen(false);
              }}
              className="w-full flex items-center gap-2 px-3 py-1.5 text-[11px] font-['Figtree'] text-[#4a2c18] hover:bg-[rgba(120,80,40,0.06)] transition-colors"
            >
              <span
                className="w-2.5 h-2.5 rounded-full shrink-0"
                style={{ backgroundColor: `rgb(${t.accentRgb})` }}
              />
              <span className="flex-1 text-left">{t.name}</span>
              {t.id === themeId && <Check size={11} className="text-[rgb(var(--console-accent))]" />}
            </button>
          ))}
        </div>
      )}
    </div>
  );
}
