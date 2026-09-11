import { fmtTime } from "./format";
import type { Message } from "./types";

function typeTagColor(type: Message["type"]) {
  switch (type) {
    case "system": return "text-[#b09060]";
    case "ai":     return "text-[rgb(var(--console-accent))]";
    case "user":   return "text-[#4a7aaa]";
    case "error":  return "text-[#c03040]";
  }
}

function typeTag(type: Message["type"]) {
  switch (type) {
    case "system": return "sys";
    case "ai":     return "ai";
    case "user":   return "you";
    case "error":  return "err";
  }
}

function rainbowClass(type: Message["type"]) {
  if (type === "user") return "text-[#2c3a5a] font-['JetBrains_Mono']";
  if (type === "error") return "text-[#c03040] font-['JetBrains_Mono']";
  return "rainbow-text font-['JetBrains_Mono']";
}

export function MessageRow({ msg, compact = false }: { msg: Message; compact?: boolean }) {
  const timestamp = new Date(msg.timestamp);

  if (compact) {
    return (
      <div className="space-y-[3px]">
        {msg.lines.map((line, i) => (
          <p key={i} className={`text-[11px] leading-snug ${rainbowClass(msg.type)}`}>
            {line}
          </p>
        ))}
      </div>
    );
  }

  return (
    <div className="flex gap-4 group">
      <div className="flex flex-col items-end shrink-0 w-10 pt-px">
        <span className={`text-[10px] font-['JetBrains_Mono'] font-semibold ${typeTagColor(msg.type)}`}>
          {typeTag(msg.type)}
        </span>
        <span className="text-[9px] font-['JetBrains_Mono'] text-[#b09878] mt-0.5">
          {fmtTime(timestamp)}
        </span>
      </div>
      <div className="flex-1 space-y-[3px] pt-px">
        {msg.lines.map((line, i) => (
          <p key={i} className={`text-[12.5px] leading-relaxed ${rainbowClass(msg.type)}`}>
            {line}
          </p>
        ))}
      </div>
    </div>
  );
}
