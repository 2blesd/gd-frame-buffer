# Frame Buffer (Geode mod for Geometry Dash)

Early clicks are buffered for a few frames and fired the moment a jump becomes possible.

## Files
- `mod.json`        mod metadata + settings
- `CMakeLists.txt`  build config
- `src/main.cpp`    mod logic
- `about.md`, `changelog.md`, `logo.png`
- `.github/workflows/build.yml`  builds Windows/Mac/Android for you

## Build the .geode file

### Option A: GitHub (no local setup, builds every platform)
1. Create a new GitHub repo and push this folder.
2. Open the **Actions** tab, run **Build Binaries**.
3. Download **Build Output**: it contains `yourname.frame-buffer.geode`.
4. Drop that file into your Geometry Dash `geode/mods` folder.

### Option B: Local build
1. Install the Geode CLI and SDK: https://docs.geode-sdk.org/getting-started/
2. `geode sdk install` and `geode sdk install-binaries`
3. In this folder:
   ```
   geode build
   ```
   The `.geode` file is created in `build/`.

## Before you build
- Change `"id"` and `"developer"` in `mod.json` (id format: `developername.modname`).
- If your GD/Geode versions differ, update the `gd` and `geode` fields in `mod.json`.

## How it works
- `handleButton` hook: if jump is pressed while airborne, the press is remembered for N frames.
- `processCommands` hook (once per physics step): when the player lands within the window, a jump is injected; otherwise it expires.
- Setting `Buffer Window (frames)` controls N (default 3).

Gameplay-altering mod: don't use it for records on leaderboards that disallow input assistance.
