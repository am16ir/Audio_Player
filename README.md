# Audio Player (JUCE)

OOP Assignment 2 under supervising of doctor Mohamed El-Rammly.

## Overview
This is a **C++ GUI audio player** built with **JUCE**. The project is configured using a Projucer project file (`.jucer`), which generates the platform-specific build files (Visual Studio / Xcode / Linux Makefiles).

The application focuses on core audio-player features (load/play/seek/loop) with a simple UI, plus some extra playback utilities like markers, segment looping, and speed control.

## Features
- **Load audio files** from disk into a playlist/table.
- **Playback controls:** Play, Pause, Restart, Jump to End.
- **Seeking:**
  - Position slider / timeline seeking
  - Forward `+10s` and Backward `-10s`
- **Looping options:**
  - Full-track looping
  - **Segment looping** between a user-defined **start point** and **end point**
- **Markers:** add markers during playback and clear them.
- **Mute & volume control** (gain).
- **Speed control** (playback rate changes via resampling).
- **Waveform display** with a moving playhead and a time readout.
- (Advanced/extra) Two internal players exist in code (`player1` and `player2`) with a **Mix** button in the UI.

## Project Structure
- `NewProject.jucer` — Projucer project file (open this with Projucer)
- `Source/`
  - `Main.cpp` — application entry point
  - `PlayerGui.*` — GUI (buttons, sliders, table/playlist, waveform component)
  - `PlayerAudio.*` — audio engine (transport, gain, speed, loop/segment loop, markers)

## Requirements
- **JUCE / Projucer** installed
- A supported IDE/toolchain:
  - **Windows:** Visual Studio 2022
  - **macOS:** Xcode
  - **Linux:** GCC/Clang + Make

## Build & Run (using Projucer / .jucer)
1. Clone the repository:
   ```bash
   git clone https://github.com/am16ir/Audio_Player.git
   cd Audio_Player
   ```
2. Install **JUCE** and open **Projucer**.
3. Open the project file:
   - `NewProject.jucer`
4. In Projucer, set the JUCE modules path:
   - **File → Global Paths…**
   - Set the path to: `<your JUCE folder>/modules`

   This repo’s `.jucer` expects JUCE modules to be available and will fail to generate/build if the modules path is not configured.
5. Export/generate your platform project:
   - **Windows (VS2022):** save the project to generate
     - `Builds/VisualStudio2022/NewProject.sln`
6. Open the generated project in your IDE and build/run.

### Windows / Visual Studio 2022
- Open: `Builds/VisualStudio2022/NewProject.sln`
- Select **Debug** or **Release**
- Build and run from Visual Studio.

## Usage
- Click **Load Files** to add audio files.
- Double-click a row in the table to load/play that file.
- Use **Play / Pause / Restart / End** for playback.
- Use the position slider or click the waveform to seek.
- Use **Loop** for full-track looping.
- Use **Starting point** / **Ending point** and **LOOP ON POINTS** to loop a segment.
- Use **Add Marker** and **Clear Markers** to manage markers.

## Notes
- If you move the JUCE folder, re-check **Global Paths** in Projucer.
- If you add new source files, add them via Projucer so they appear in generated IDE projects.

## License
No license file is currently provided.
