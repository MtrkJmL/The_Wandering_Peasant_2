#pragma once
#include <string>

enum class PotionType {
    HEALING,          // +35 HP
    STAMINA_BREW,     // +3 stamina
    SHIELD_TONIC,     // +5 defense for 3 turns
    RAGE_DRAUGHT,     // next attack x1.5 damage
    ANTIDOTE,         // cure all status effects (bleed, poison, blind)
    // ── Town of Dead Ballads exclusives ──────────────────────────
    PHANTOMS_DRAUGHT, // next attack ignores enemy defense entirely
    GRAVEWARDENS_OIL, // next 3 attacks proc Bleed on hit (guaranteed)
    DEAD_MANS_COURAGE,// clear all status effects + restore 3 stamina
    ASH_MEMORY        // permanently +15 max HP (restores HP to new max)
};

struct Potion {
    PotionType  type;
    std::string name;
    std::string description;
    bool        locked            = false; // Soul Binder / combat lock (clears after fight)
    bool        permanentlyLocked = false; // Plague Shaman curse (persists entire run)

    Potion() : type(PotionType::HEALING), name("Healing Potion"),
               description("Restores 35 HP.") {}
    Potion(PotionType t);

    static std::string typeName(PotionType t);
    static std::string typeDesc(PotionType t);
};
