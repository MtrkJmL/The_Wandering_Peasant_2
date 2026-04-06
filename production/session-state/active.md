# Session State — The Wandering Peasant 2: Gospel of Ash

Last updated: 2026-04-06

## Current State
Game is playable. All major systems from this session are implemented and built (Release).

---

## What Was Built This Session

### Stance System (NEW)
- 10 stances total; player starts with DEFENSIVE + OFFENSIVE
- Learned from M'alid (appears at ? nodes, 25% chance, shows 4 random unlearned stances)
- Free action to switch each turn (option 5 in combat, including boss fights)
- Stance modifiers:
  - DEFENSIVE: incoming damage -25%
  - OFFENSIVE: crit damage x1.5
  - FOCUSED: treats heavy attack as light for timing bar (wider crit zone)
  - EVASIVE: 20% miss chance on enemy attacks
  - BLOODLUST: heal 3 HP on kill
  - HEMORRHAGE: bleed deals 5 dmg/turn instead of 2
  - VENOMOUS: +1 poison stack on hit if enemy already poisoned
  - RESISTANT: -2 incoming damage from all sources
  - OBSCURED: enemy blind chance 50% (vs 30% base)
  - ETERNAL_DARK: refreshes enemy blind to 999 each enemy turn (perma-blind)
- File: `include/Stance.h`

### Status Effects (REWORKED)
- BLEED: 2 dmg/turn, permanent (never expires). Hemorrhage stance → 5 dmg/turn
- POISON: decaying stacks (X dmg → X-1 stacks). ADDITIVE stacking (4+5=9, not capped)
- BLIND: 30% miss chance, 2 rounds. (Obscured=50%, Eternal Dark=perma)
- BURN removed, replaced by BLIND throughout
- Status effects cleared on player after combat ends

### Weapon Proc Fix (THIS SESSION)
- Proc now fires BEFORE the Marsh Wraith dodge check
- Previously: every light attack vs Wraith had 0% proc chance (returned before proc)
- Now: 30% + luck*0.5% fires on any swing; Wraith still takes no damage from light attacks

### Other Changes
- M'alid named, merged into EVENT (? nodes) at 25% priority
- EVENT nodes renamed to ? on map
- Boss fights now have stance switch option
- Heavy attack crit = exactly 1 pixel (threshold 2.0f, was effectively ±1)
- Ch1 enemies nerfed: Raider 1d5+1, Brute 1d8+1, Witch 1d5, Hound 1d4
- Tutorial added at main menu (option 2)
- Plague Shaman permanently destroys one potion (RunState.shamanCurseApplied flag, fires once per run)
- Weapon upgrade costs: 75 / 150 / 250 gold (was 50 / 100 / 200)

---

## Key Files Modified This Session
- `src/Game.cpp` — all combat logic, event handling, M'alid, tutorial, upgrade costs
- `src/Player.cpp` — stance methods, status clearing, antidote, crit threshold
- `src/Enemy.cpp` — status procs, Ch1 nerfs, Plague Shaman, Ember Witch, Ash Hound
- `src/Item.cpp` — weapon proc 20% base, setStatusEffect(), BLIND in strings
- `src/Node.cpp` — EVENT typeLabel="?", typeSymbol="[  ?  ]"
- `src/ChapterMap.cpp` — MYSTERY removed, EVENT-only map
- `include/Stance.h` — NEW: StanceType enum, stanceName(), stanceDescription()
- `include/Player.h` — CombatBuffs reworked, stance fields + methods
- `include/Enemy.h` — blind/bleed/poison fields, EnemyBehaviour status flags
- `include/Item.h` — StatusEffect enum, setStatusEffect()
- `include/RunState.h` — shamanCurseApplied flag
- `include/Game.h` — showTutorial() declared
- `include/Potion.h` — permanentlyLocked field
- `design/gdd/stance-and-status-system.md` — NEW: full GDD
- `design/gdd/systems-index.md` — stances (#30), tutorial (#31) added
- `design/events-and-quests.txt` — NEW: all ? node events + side quest reference

---

## Known Issues / Next Steps
- No known bugs outstanding
- Possible future work:
  - Bad luck protection for status procs (guaranteed proc after N misses)
  - Balance pass on stance values after playtesting
  - Quest completion logic needs wiring into Game.cpp (Q1-Q5 + Banner chain)
  - Side quest rewards not yet implemented in code (design doc exists)
