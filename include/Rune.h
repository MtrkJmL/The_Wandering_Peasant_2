#pragma once
#include <string>

enum class RuneType {
    NONE,
    IRON_SKIN,     // incoming dmg x0.75
    BLOODHUNGER,   // crit dmg x1.5
    STEADY_EYE,    // heavy crit window +2 positions
    GHOST_STEP,    // 20% chance to fully dodge an attack
    PREDATOR,      // bleeding enemy deals 40% less damage
    DEEP_CUTS,     // bleed deals 5 dmg/turn instead of 2
    TOXIN_RIDER,   // each hit adds +1 poison if enemy already poisoned
    PLAGUE_WARD,   // immune to poison
    ASH_VEIL,      // blinded enemy 50% miss chance
    NIGHTKEEPER    // blind you apply doesn't tick down (permanent)
};

inline std::string runeName(RuneType r) {
    switch (r) {
        case RuneType::NONE:        return "None";
        case RuneType::IRON_SKIN:   return "Iron Skin";
        case RuneType::BLOODHUNGER: return "Bloodhunger";
        case RuneType::STEADY_EYE:  return "Steady Eye";
        case RuneType::GHOST_STEP:  return "Ghost Step";
        case RuneType::PREDATOR:    return "Predator";
        case RuneType::DEEP_CUTS:   return "Deep Cuts";
        case RuneType::TOXIN_RIDER: return "Toxin Rider";
        case RuneType::PLAGUE_WARD: return "Plague Ward";
        case RuneType::ASH_VEIL:    return "Ash Veil";
        case RuneType::NIGHTKEEPER: return "Nightkeeper";
    }
    return "Unknown";
}

inline std::string runeDescription(RuneType r) {
    switch (r) {
        case RuneType::NONE:        return "No rune equipped.";
        case RuneType::IRON_SKIN:   return "Incoming damage reduced by 25%.";
        case RuneType::BLOODHUNGER: return "Critical hits deal x1.5 damage.";
        case RuneType::STEADY_EYE:  return "Heavy attack crit window is 2 positions wider.";
        case RuneType::GHOST_STEP:  return "20% chance to fully dodge any incoming attack.";
        case RuneType::PREDATOR:    return "Bleeding enemies deal 40% less damage to you.";
        case RuneType::DEEP_CUTS:   return "Your bleed deals 5 damage per turn instead of 2.";
        case RuneType::TOXIN_RIDER: return "Each hit adds +1 poison stack if enemy is already poisoned.";
        case RuneType::PLAGUE_WARD: return "You are immune to poison.";
        case RuneType::ASH_VEIL:    return "Blinded enemies have +20% miss chance (base 30% → 50%).";
        case RuneType::NIGHTKEEPER: return "Blind you apply persists. +10% miss chance for blinded enemies.";
    }
    return "";
}
