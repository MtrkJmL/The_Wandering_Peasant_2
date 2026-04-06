# 🔥 The Wandering Peasant 2 — *Gospel of Ash* `[BETA]`

*The Dragon King stirs in his tomb of ash.  
The Darkbloods move in the shadows.  
A new peasant wanders into a dying world —  
and nobody told them what they were walking into.*

---

## 🌍 About

**The Wandering Peasant 2: Gospel of Ash** is the sequel to the original Wandering Peasant — a terminal roguelike RPG built in C++. This isn't a rehash. The world is darker, the conspiracies are deeper, and the NPCs will lie to your face.

Three chapters. Every quest recontextualizes the last. The truth about the Dragon King's rebirth is something nobody wanted to find.

> ⚠️ **This is a BETA release.** Core systems are complete and playable, but balance, content, and side quests are still being tuned. Expect rough edges and welcome feedback.

---

## ✨ What's New in Gospel of Ash

- ⚔️ **Stance System** — 10 stances taught by the wandering master M'alid. Switch freely each turn to control the flow of combat.
- 🩸 **Status Effects** — Bleed, Poison, and Blind with distinct stacking and decay rules. No two fights play the same.
- 🗺️ **Slay the Spire-style Map** — Chart your path through branching nodes: battles, merchants, mystery events, and elite encounters.
- ❓ **Mystery Nodes** — 5 per chapter. Strange events, NPC encounters, and narrative beats that shift the story.
- 🧙 **M'alid Encounters** — A wandering stance teacher who appears at mystery nodes (25% priority) and expands your fighting style.
- ⏱️ **Timing-Bar Combat** — Precision matters. Heavy attacks demand a 1-pixel crit window. Light attacks forgive by ±1. Miss the bar and regret it.
- 🧪 **Plague Shaman** — A unique encounter that permanently locks your potion slot for the run. Choose your battles carefully.
- 📜 **Side Quests** — Five questlines (Q1–Q5) plus the Banner chain woven across chapters. *(Full reward wiring in progress.)*
- 🎓 **Tutorial** — Available from the main menu. Learn the systems before the world kills you.

---

## 🚧 Known Beta Limitations

- Side quest reward logic is partially wired — quest tracking works, full rewards in progress
- Balance pass ongoing after playtesting
- Chapter 2 and 3 content still being authored

---

## 🎮 Features

- 🖥️ **Pure terminal / ASCII** — No dependencies, runs anywhere with a C++ compiler
- 🔁 **Roguelike runs** — Each run is a fresh path through the map
- ⚔️ **Tactical depth** — Stances, timing, status effects, and resource management
- 🐉 **Lore-driven** — The Darkbloods conspiracy unfolds across chapters
- 🆓 **Free and open source** — MIT License

---

## 🔮 Roadmap

- 🎭 **Full side quest wiring** — Q1–Q5 + Banner chain rewards
- ⚖️ **Balance pass** — Enemy stats, status proc rates, weapon upgrade costs
- 📖 **Chapters 2 & 3** — The full Gospel of Ash arc
- 🛡️ **Factions & Reputation** — Darkbloods, wanderers, and the broken church
- 🔮 **More relics & events** — Secrets the first game only hinted at

---

# Building the Game

This project uses CMake. You'll need CMake 3.10+ and a C++17-compatible compiler.

## Prerequisites

- **CMake:** 3.10 or higher — [Download CMake](https://cmake.org/download/)
- **C++ Compiler:**
  - **Linux:** GCC or Clang (`sudo apt install build-essential cmake`)
  - **macOS:** Clang via Xcode Command Line Tools (`xcode-select --install`) + CMake via Homebrew (`brew install cmake`)
  - **Windows:** Visual Studio with C++ workload — [Download Visual Studio](https://visualstudio.microsoft.com/downloads/)

## Build Steps

### Linux / macOS

```bash
mkdir build && cd build
cmake ..
make
```

The executable `wandering_peasant_2` will be inside `build/`.

### Windows (Visual Studio)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

The executable `wandering_peasant_2.exe` will be in `build\Release\`.

> Replace `"Visual Studio 17 2022"` with your installed generator if different. Run `cmake --help` to list available generators.

## Running the Game

After building, navigate to the output directory and run:

- **Linux/macOS:** `./wandering_peasant_2`
- **Windows:** `.\wandering_peasant_2.exe`

---

## 🛠️ License

**MIT License** — Free to play, fork, and reforge.  
The ash is yours to wander through.

Full license text: [MIT License](https://opensource.org/licenses/MIT)
