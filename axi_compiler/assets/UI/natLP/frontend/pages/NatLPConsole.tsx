import { useEffect, useState } from "react";
import "./natlp-console-theme.css";
import { CompanionConsole } from "../components/companion-console";
import { getTheme, DEFAULT_THEME_ID } from "../components/companion-console/themes";
import type { CSSVars } from "../components/companion-console/types";
import { OsBackground } from "./console/OsBackground";

// This page is a demo shell for the standalone Companion Console widget
// (see /frontend/components/companion-console). The simulated desktop
// background below is page-only decoration — it is not part of the
// installable widget, which is just <CompanionConsole /> and renders as a
// fixed-position overlay with no layout footprint.
export default function NatLPConsolePage() {
  const [time, setTime] = useState(new Date());

  useEffect(() => {
    const id = setInterval(() => setTime(new Date()), 1000);
    return () => clearInterval(id);
  }, []);

  const rootStyle: CSSVars = { "--console-accent": getTheme(DEFAULT_THEME_ID).accentRgb };

  return (
    <div
      className="natlp-console-root size-full relative overflow-hidden bg-background select-none font-['Figtree']"
      style={rootStyle}
    >
      <OsBackground time={time} />

      <CompanionConsole
        storageKey="natlp-console-state-v1"
        label="NatLP Console"
        monogram="N"
        prompt="natlp ~"
      />
    </div>
  );
}
