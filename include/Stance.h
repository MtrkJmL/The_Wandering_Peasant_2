#pragma once
#include <string>

enum class StanceType {
    DEFENSIVE,    // incoming dmg x0.75
    OFFENSIVE,    // crit dmg x1.5
    FOCUSED,      // heavy crit window +2 positions
    EVASIVE,      // 20% chance to fully dodge an attack
    BLOODLUST,    // bleeding enemy deals 40% less damage
    HEMORRHAGE,   // bleed deals 5 dmg/turn instead of 2
    VENOMOUS,     // each hit adds +1 poison stack if enemy already poisoned
    RESISTANT,    // immune to poison
    OBSCURED,     // blinded enemy has 50% miss chance
    ETERNAL_DARK  // blind is permanent while active; 40% miss chance
};

inline std::string stanceName(StanceType s) {
    switch (s) {
        case StanceType::DEFENSIVE:    return "Defensive";
        case StanceType::OFFENSIVE:    return "Offensive";
        case StanceType::FOCUSED:      return "Focused";
        case StanceType::EVASIVE:      return "Evasive";
        case StanceType::BLOODLUST:    return "Bloodlust";
        case StanceType::HEMORRHAGE:   return "Hemorrhage";
        case StanceType::VENOMOUS:     return "Venomous";
        case StanceType::RESISTANT:    return "Resistant";
        case StanceType::OBSCURED:     return "Obscured";
        case StanceType::ETERNAL_DARK: return "Eternal Dark";
    }
    return "Unknown";
}

inline std::string stanceDescription(StanceType s) {
    switch (s) {
        case StanceType::DEFENSIVE:    return "Incoming damage reduced to 75%.";
        case StanceType::OFFENSIVE:    return "Critical hits deal x1.5 damage.";
        case StanceType::FOCUSED:      return "Heavy attack crit window +2 positions.";
        case StanceType::EVASIVE:      return "20% chance to fully dodge an attack.";
        case StanceType::BLOODLUST:    return "Bleeding enemies deal 40% less damage.";
        case StanceType::HEMORRHAGE:   return "Bleed deals 5 damage per turn instead of 2.";
        case StanceType::VENOMOUS:     return "Each hit adds +1 poison stack if enemy is poisoned.";
        case StanceType::RESISTANT:    return "You are immune to poison.";
        case StanceType::OBSCURED:     return "Blinded enemies have 50% miss chance.";
        case StanceType::ETERNAL_DARK: return "Blind is permanent while active. 40% miss chance.";
    }
    return "";
}
