import type { CSSProperties } from "react";

export type ViewMode = "bar" | "window";

export type MessageType = "system" | "user" | "ai" | "error";

export interface Message {
  id: string;
  type: MessageType;
  lines: string[];
  /** ISO timestamp string (serializable for localStorage persistence). */
  timestamp: string;
}

export interface Session {
  id: string;
  label: string;
  messages: Message[];
  cmdHistory: string[];
}

export interface ThemeDef {
  id: string;
  name: string;
  /** "R G B" channel string for Tailwind's rgb(var(--x)/alpha%) syntax. */
  accentRgb: string;
}

/** Allows setting CSS custom properties (e.g. "--console-accent") via a style prop. */
export type CSSVars = CSSProperties & Record<`--${string}`, string>;
