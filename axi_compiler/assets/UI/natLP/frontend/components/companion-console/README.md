# Companion Console — standalone widget

A floating command console that mounts as a fixed-position overlay (compact
bar → expandable window) with its own in-memory command set, session tabs,
accent themes, and localStorage persistence. It has zero layout footprint,
so dropping it into any app's component tree cannot shift existing layout.

## What's in this folder

Everything needed to run the widget, with no dependency on the rest of this
app:

```
CompanionConsole.tsx   <- main component, import this
BarModePanel.tsx
WindowModePanel.tsx
WindowTitleBar.tsx
TabStrip.tsx
ThemeMenu.tsx
MessageRow.tsx
commands.ts            <- edit this to change what the console can do
storage.ts
themes.ts
types.ts
format.ts
theme.css              <- fonts + rainbow-text animation, imported by CompanionConsole.tsx
index.ts               <- barrel export
```

## Installing into another app

1. Copy this entire `companion-console/` folder into the target app's
   `/frontend/components/` directory (or anywhere under `/frontend/`).
2. Make sure the target app has these dependencies in `frontend/package.json`
   (all already present in apps built the same way as this one):
   - `lucide-react`
   - `motion`
   - `react` / `react-dom` 19+
   - Tailwind CSS configured (the widget is styled entirely with Tailwind
     utility classes plus the small `theme.css` file)
3. Render it once, near the root of the app (e.g. in `App.tsx`, outside of
   `<Routes>` so it persists across page navigation):

   ```tsx
   import { CompanionConsole } from "./components/companion-console";

   export default function App() {
     return (
       <>
         {/* ...routes / existing app content... */}
         <CompanionConsole />
       </>
     );
   }
   ```

That's it — no provider, no CSS import elsewhere, no backend required.

## Props

| Prop         | Default          | Purpose                                                                 |
|--------------|-------------------|--------------------------------------------------------------------------|
| `storageKey` | `"companion-console-state-v1"` | localStorage key for persisted sessions/history/theme. Give each mounted instance a unique key if an app ever renders more than one console at once. |
| `label`      | `"Console"`       | Title shown in the expanded window's title bar.                        |
| `monogram`   | `"C"`             | Single character shown in the floating bar's badge.                    |
| `prompt`     | `"console ~"`     | Prompt text shown in the expanded window's status bar and input row.   |
| `className`  | —                 | Extra classes on the (invisible, zero-size) wrapper element.           |

Example with rebranding:

```tsx
<CompanionConsole storageKey="acme-console-v1" label="Acme Console" monogram="A" prompt="acme ~" />
```

## Customizing behavior

- Commands live in `commands.ts` (`respond()`), plus a handful of stateful
  commands (`tab *`, `theme *`, `tabs`) handled directly in
  `CompanionConsole.tsx` because they need access to session/theme state.
- Accent color options live in `themes.ts`. Add/remove entries in the
  `THEMES` array; the currently-selected accent is exposed as the CSS
  variable `--console-accent` (an "R G B" triple) on the component's root.
- Seed messages shown on first load live in `storage.ts` (`seedMessages()`).

## Notes

- This is source you copy and own — there is no shared package registry to
  install from, so update each installed copy independently if you want to
  push a fix everywhere.
- The Google Fonts `@import` in `theme.css` (JetBrains Mono, Figtree) requires
  `https://fonts.googleapis.com` and `https://fonts.gstatic.com` to be
  reachable under the host app's Content Security Policy. Apps built the same
  way as this one already allow both by default.
