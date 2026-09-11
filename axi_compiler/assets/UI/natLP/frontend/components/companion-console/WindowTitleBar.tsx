import { X, Minus, Terminal } from "lucide-react";
import { ThemeMenu } from "./ThemeMenu";

export function WindowTitleBar({
  onMinimize,
  onClose,
  themeId,
  onThemeChange,
  label = "Console",
}: {
  onMinimize: () => void;
  onClose: () => void;
  themeId: string;
  onThemeChange: (id: string) => void;
  label?: string;
}) {
  return (
    <div className="h-10 px-4 flex items-center gap-3 border-b border-[rgba(120,80,40,0.1)] bg-[rgba(247,239,228,0.7)] shrink-0 select-none">
      <div className="flex items-center gap-1.5">
        <button
          onClick={onClose}
          className="p-1.5 -m-1.5 flex items-center justify-center"
          title="Close"
          aria-label="Close"
        >
          <span className="w-3 h-3 rounded-full bg-[#e8685a] hover:bg-[#d04030] border border-[rgba(180,60,40,0.4)] transition-colors group flex items-center justify-center">
            <X size={6} className="text-[rgba(80,20,10,0.35)] group-hover:text-[rgba(80,20,10,0.7)] transition-colors" />
          </span>
        </button>
        <button
          onClick={onMinimize}
          className="p-1.5 -m-1.5 flex items-center justify-center"
          title="Minimize"
          aria-label="Minimize"
        >
          <span className="w-3 h-3 rounded-full bg-[#f0c040] hover:bg-[#d8a820] border border-[rgba(160,120,20,0.4)] transition-colors group flex items-center justify-center">
            <Minus size={6} className="text-[rgba(80,60,0,0.4)] group-hover:text-[rgba(80,60,0,0.75)] transition-colors" />
          </span>
        </button>
        <div className="w-3 h-3 rounded-full bg-[#58c860] border border-[rgba(40,140,40,0.4)]" />
      </div>

      <div className="flex-1 flex items-center justify-center gap-2">
        <div className="w-[18px] h-[18px] rounded-[5px] bg-[rgb(var(--console-accent)/12%)] border border-[rgb(var(--console-accent)/25%)] flex items-center justify-center">
          <Terminal size={9} className="text-[rgb(var(--console-accent))]" />
        </div>
        <span className="text-[10px] font-['Figtree'] font-semibold text-[#8a6050] tracking-[0.18em] uppercase">
          {label}
        </span>
      </div>

      <div className="w-[54px] flex items-center justify-end">
        <ThemeMenu themeId={themeId} onChange={onThemeChange} />
      </div>
    </div>
  );
}
