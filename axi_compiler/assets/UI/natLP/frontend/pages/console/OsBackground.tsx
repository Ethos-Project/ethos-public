import { Wifi, Activity } from "lucide-react";
import { fmtClock, fmtDate } from "../../components/companion-console/format";

export function OsBackground({ time }: { time: Date }) {
  return (
    <div className="absolute inset-0 overflow-hidden pointer-events-none">
      {/* warm ambient gradients */}
      <div className="absolute inset-0 bg-[radial-gradient(ellipse_70%_50%_at_15%_10%,rgba(255,200,160,0.35)_0%,transparent_65%)]" />
      <div className="absolute inset-0 bg-[radial-gradient(ellipse_50%_40%_at_90%_80%,rgb(var(--console-accent)/8%)_0%,transparent_60%)]" />
      <div className="absolute inset-0 bg-[radial-gradient(ellipse_40%_30%_at_60%_30%,rgba(255,240,200,0.2)_0%,transparent_60%)]" />

      {/* subtle dot grid */}
      <div
        className="absolute inset-0 opacity-[0.06]"
        style={{
          backgroundImage: "radial-gradient(circle, rgba(120,80,40,0.8) 1px, transparent 1px)",
          backgroundSize: "28px 28px",
        }}
      />

      {/* OS top bar */}
      <div className="absolute top-0 left-0 right-0 h-7 border-b border-[rgba(120,80,40,0.1)] bg-[rgba(253,246,238,0.8)] backdrop-blur-sm flex items-center px-5 gap-4">
        <span className="text-[10px] font-['Figtree'] font-bold text-[#a07850] tracking-[0.22em] uppercase">
          NatLP OS
        </span>
        <span className="flex-1" />
        <span className="text-[10px] font-['JetBrains_Mono'] text-[#b09070]">
          {fmtDate(time)} · {fmtClock(time)}
        </span>
        <Wifi size={10} className="text-[#b09070]" />
        <Activity size={10} className="text-[#b09070]" />
      </div>

      {/* ghost windows */}
      <div className="absolute top-12 left-6 w-52 h-32 rounded-[14px] border border-[rgba(120,80,40,0.1)] bg-[rgba(247,239,228,0.55)] backdrop-blur-[2px] shadow-sm">
        <div className="h-[22px] border-b border-[rgba(120,80,40,0.08)] flex items-center px-3 gap-1.5">
          <div className="w-2 h-2 rounded-full bg-[rgba(224,100,80,0.4)]" />
          <div className="w-2 h-2 rounded-full bg-[rgba(220,180,40,0.4)]" />
          <div className="w-2 h-2 rounded-full bg-[rgba(80,180,80,0.4)]" />
        </div>
        <div className="p-3 space-y-2">
          {[72, 55, 88, 42].map((w, i) => (
            <div key={i} className="h-[5px] rounded-full bg-[rgba(120,80,40,0.1)]" style={{ width: `${w}%` }} />
          ))}
        </div>
      </div>

      <div className="absolute top-12 right-6 w-60 h-40 rounded-[14px] border border-[rgba(120,80,40,0.1)] bg-[rgba(247,239,228,0.45)] backdrop-blur-[2px] shadow-sm">
        <div className="h-[22px] border-b border-[rgba(120,80,40,0.08)] flex items-center px-3 gap-1.5">
          <div className="w-2 h-2 rounded-full bg-[rgb(var(--console-accent)/45%)]" />
          <div className="w-2 h-2 rounded-full bg-[rgba(120,80,40,0.2)]" />
        </div>
        <div className="p-3 grid grid-cols-5 gap-2">
          {[...Array(10)].map((_, i) => (
            <div key={i} className="h-8 rounded-lg bg-[rgba(120,80,40,0.07)]" />
          ))}
        </div>
      </div>

      <div className="absolute top-[210px] left-[230px] w-40 h-20 rounded-[14px] border border-[rgba(120,80,40,0.08)] bg-[rgba(247,239,228,0.35)] shadow-sm">
        <div className="h-[22px] border-b border-[rgba(120,80,40,0.06)] flex items-center px-3">
          <div className="w-16 h-[3px] rounded bg-[rgba(120,80,40,0.1)]" />
        </div>
        <div className="p-3 space-y-1.5">
          {[60, 40].map((w, i) => (
            <div key={i} className="h-[4px] rounded-full bg-[rgba(120,80,40,0.08)]" style={{ width: `${w}%` }} />
          ))}
        </div>
      </div>
    </div>
  );
}
