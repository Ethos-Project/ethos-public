import type { ThemeDef } from "./types";

const ROSE: ThemeDef = { id: "rose", name: "Rose", accentRgb: "192 56 90" };

export const THEMES: ThemeDef[] = [
  ROSE,
  { id: "ocean", name: "Ocean", accentRgb: "64 144 192" },
  { id: "forest", name: "Forest", accentRgb: "96 168 112" },
  { id: "amber", name: "Amber", accentRgb: "224 144 64" },
  { id: "violet", name: "Violet", accentRgb: "144 96 192" },
];

export const DEFAULT_THEME_ID = ROSE.id;

export function getTheme(id: string): ThemeDef {
  return THEMES.find((t) => t.id === id) ?? ROSE;
}
