#pragma once
#include <string>

enum class PassiveType {
    NONE,
    IRON_SKIN,     // incoming dmg ×0.75
    BLOODHUNGER,   // crit dmg ×1.5
    STEADY_EYE,    // heavy crit window +2 positions
    GHOST_STEP,    // 20% chance to fully dodge an attack
    PREDATOR,      // bleeding enemy deals 40% less damage
    DEEP_CUTS,     // bleed deals 5 dmg/turn instead of 2
    TOXIN_RIDER,   // each hit adds +1 poison if enemy already poisoned
    PLAGUE_WARD,   // immune to poison
    ASH_VEIL,      // blinded enemy 50% miss chance
    NIGHTKEEPER    // blind you apply doesn't tick down (permanent)
};

inline std::string passiveName(PassiveType p) {
    switch (p) {
        case PassiveType::NONE:        return "None";
        case PassiveType::IRON_SKIN:   return "Iron Skin";
        case PassiveType::BLOODHUNGER: return "Bloodhunger";
        case PassiveType::STEADY_EYE:  return "Steady Eye";
        case PassiveType::GHOST_STEP:  return "Ghost Step";
        case PassiveType::PREDATOR:    return "Predator";
        case PassiveType::DEEP_CUTS:   return "Deep Cuts";
        case PassiveType::TOXIN_RIDER: return "Toxin Rider";
        case PassiveType::PLAGUE_WARD: return "Plague Ward";
        case PassiveType::ASH_VEIL:    return "Ash Veil";
        case PassiveType::NIGHTKEEPER: return "Nightkeeper";
    }
    return "Unknown";
}

inline std::string passiveDescription(PassiveType p) {
    switch (p) {
        case PassiveType::NONE:        return "No passive equipped.";
        case PassiveType::IRON_SKIN:   return "Incoming damage reduced by 25%.";
        case PassiveType::BLOODHUNGER: return "Critical hits deal x1.5 damage.";
        case PassiveType::STEADY_EYE:  return "Heavy attack crit window is 2 positions wider.";
        case PassiveType::GHOST_STEP:  return "20% chance to fully dodge any incoming attack.";
        case PassiveType::PREDATOR:    return "Bleeding enemies deal 40% less damage to you.";
        case PassiveType::DEEP_CUTS:   return "Your bleed deals 5 damage per turn instead of 2.";
        case PassiveType::TOXIN_RIDER: return "Each hit adds +1 poison stack if enemy is already poisoned.";
        case PassiveType::PLAGUE_WARD: return "You are immune to poison.";
        case PassiveType::ASH_VEIL:    return "Blinded enemies have 50% miss chance instead of 30%.";
        case PassiveType::NIGHTKEEPER: return "Blind you apply to enemies does not tick down.";
    }
    return "";
}
