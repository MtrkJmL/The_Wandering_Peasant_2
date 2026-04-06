#include "../include/Potion.h"

std::string Potion::typeName(PotionType t) {
    switch (t) {
        case PotionType::HEALING:           return "Healing Potion";
        case PotionType::STAMINA_BREW:      return "Stamina Brew";
        case PotionType::SHIELD_TONIC:      return "Shield Tonic";
        case PotionType::RAGE_DRAUGHT:      return "Rage Draught";
        case PotionType::ANTIDOTE:          return "Antidote";
        case PotionType::PHANTOMS_DRAUGHT:  return "Phantom's Draught";
        case PotionType::GRAVEWARDENS_OIL:  return "Gravewarden Oil";
        case PotionType::DEAD_MANS_COURAGE: return "Dead Man's Courage";
        case PotionType::ASH_MEMORY:        return "Ash Memory";
    }
    return "Unknown";
}

std::string Potion::typeDesc(PotionType t) {
    switch (t) {
        case PotionType::HEALING:           return "Restores 35 HP.";
        case PotionType::STAMINA_BREW:      return "Restores 3 stamina.";
        case PotionType::SHIELD_TONIC:      return "+5 defense for 3 turns.";
        case PotionType::RAGE_DRAUGHT:      return "Next attack deals x1.5 damage.";
        case PotionType::ANTIDOTE:          return "Cures all status effects.";
        case PotionType::PHANTOMS_DRAUGHT:  return "Next attack ignores enemy defense entirely.";
        case PotionType::GRAVEWARDENS_OIL:  return "Next 3 attacks apply Bleed on hit (guaranteed).";
        case PotionType::DEAD_MANS_COURAGE: return "Clears all status effects and restores 3 stamina.";
        case PotionType::ASH_MEMORY:        return "Permanently +15 max HP. Restores HP to new maximum.";
    }
    return "";
}

Potion::Potion(PotionType t)
    : type(t), name(typeName(t)), description(typeDesc(t)) {}
