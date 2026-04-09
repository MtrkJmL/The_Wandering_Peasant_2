#include "../include/AttackType.h"
#include <random>
#include <ctime>
#include <sstream>
#include <stdexcept>

// ── Static RNG ────────────────────────────────────────────────────────────────

static std::mt19937& atRng() {
    static std::mt19937 rng(std::time(nullptr));
    return rng;
}

// ── roll() ────────────────────────────────────────────────────────────────────

int AttackType::roll() const {
    if (diceCount <= 0) return modifier;
    auto& rng = atRng();
    std::uniform_int_distribution<int> dist(1, diceSides);
    int total = modifier;
    for (int i = 0; i < diceCount; ++i) total += dist(rng);
    return std::max(0, total);
}

// ── getStatsString() ──────────────────────────────────────────────────────────

std::string AttackType::getStatsString() const {
    std::ostringstream ss;
    if (diceCount > 0) {
        ss << diceCount << "d" << diceSides;
        if (modifier > 0) ss << "+" << modifier;
    } else if (modifier > 0) {
        ss << "+" << modifier;
    } else {
        ss << "—";
    }
    if (defenseBonus > 0) ss << ", +" << defenseBonus << " def";
    switch (effect) {
        case AttackEffect::APPLY_BLEED:           ss << ", bleed"; break;
        case AttackEffect::APPLY_POISON:          ss << ", psn(" << effectValue << ")"; break;
        case AttackEffect::APPLY_BLIND:           ss << ", blind"; break;
        case AttackEffect::COMBO_SETUP:           ss << ", +dmg next"; break;
        case AttackEffect::APPLY_BLEED_AND_POISON:ss << ", bleed+psn"; break;
        case AttackEffect::APPLY_ALL_THREE:       ss << ", bleed+blind+psn"; break;
        case AttackEffect::DOUBLE_NEXT_DAMAGE:    ss << ", x2 next"; break;
        case AttackEffect::DODGE_NEXT_ATTACK:     ss << ", dodge"; break;
        case AttackEffect::EXECUTE_HEAL:          ss << ", heal-on-kill"; break;
        case AttackEffect::CRIMSON_FURY:          ss << ", x2 if bleeding"; break;
        case AttackEffect::REFLECT_DAMAGE:        ss << ", reflect"; break;
        case AttackEffect::SKIP_ENEMY_TURN:       ss << ", skip-enemy"; break;
        case AttackEffect::APPLY_ALL_STATUSES:    ss << ", all-status"; break;
        case AttackEffect::PERM_BLEED_NO_REGEN:    ss << ", perm-bleed"; break;
        case AttackEffect::DAMAGE_FROM_MISSING_HP: ss << ", missing-HP"; break;
        case AttackEffect::SELF_BLEED:             ss << ", self-bleed"; break;
        case AttackEffect::REST_TO_HEAL:           ss << ", heal-on-rest(" << effectValue << ")"; break;
        case AttackEffect::BONUS_IF_RESTED:        ss << ", +dmg-if-rested"; break;
        case AttackEffect::APPLY_POISON_AND_COMBO: ss << ", psn(" << effectValue << ")+combo"; break;
        case AttackEffect::APPLY_BLEED_DEF_HEAL:   ss << ", bleed+heal(" << effectValue << ")"; break;
        case AttackEffect::APPLY_POISON_ADD:        ss << ", +psn(" << effectValue << ")"; break;
        case AttackEffect::DOUBLE_POISON_STACKS:    ss << ", 2x poison"; break;
        case AttackEffect::BLIND_IF_POISONED:       ss << ", bleed+blind-if-psn"; break;
        case AttackEffect::BLIND_AMPLIFY:           ss << ", x2-if-blind"; break;
        case AttackEffect::BLEED_BURST:             ss << ", burst-if-bleed(+" << effectValue << ")"; break;
        case AttackEffect::STATUS_OVERLOAD:         ss << ", +stacks*3"; break;
        case AttackEffect::DESPERATE_STRIKE:        ss << ", x2-if-low-hp"; break;
        case AttackEffect::POISON_BURST:            ss << ", psn-burst(" << effectValue << "/stk)"; break;
        default: break;
    }
    return ss.str();
}

// ── Attack pools ──────────────────────────────────────────────────────────────

static const std::vector<AttackType> COMMON_POOL = {
    {
        "Ember Guard",
        "A forward push that blocks as much as it hits.",
        ItemRarity::COMMON, 1, 4, 1, 5, 1,
        AttackEffect::GAIN_DEFENSE, 0
    },
    {
        "Scorch",
        "A searing gouge that leaves toxin behind.",
        ItemRarity::COMMON, 1, 5, 1, 0, 1,
        AttackEffect::APPLY_POISON, 2
    },
    {
        "Gash",
        "A crude hack. Won't kill anything alone — but it opens a wound that will.",
        ItemRarity::COMMON, 1, 4, 1, 0, 1,
        AttackEffect::APPLY_BLEED, 0
    },
    {
        "Plague Touch",
        "Coat the blow with filth.",
        ItemRarity::COMMON, 1, 4, 0, 0, 1,
        AttackEffect::APPLY_POISON_ADD, 3
    },
    {
        "Desperate Strike",
        "When death is close, something takes over. Pain becomes power.",
        ItemRarity::COMMON, 1, 6, 0, 0, 1,
        AttackEffect::DESPERATE_STRIKE, 0
    },
    {
        "Braced Strike",
        "Set your footing, then hit. The setup pays off next swing.",
        ItemRarity::COMMON, 1, 6, 2, 4, 2,
        AttackEffect::COMBO_SETUP, 3
    },
    {
        "Ember Strike",
        "One heavy swing. High ceiling. No technique.",
        ItemRarity::COMMON, 2, 4, 2, 0, 2,
        AttackEffect::NONE, 0
    },
    {
        "Haymaker",
        "No technique. Just everything you have. Pairs well with a setup attack.",
        ItemRarity::COMMON, 2, 6, 3, 0, 3,
        AttackEffect::NONE, 0
    },
    {
        "Bloodprice",
        "Hit like a brute. Pay with your own blood. Enables Crimson Fury permanently.",
        ItemRarity::COMMON, 1, 6, 2, 0, 3,
        AttackEffect::SELF_BLEED, 0
    },
};

static const std::vector<AttackType> RARE_POOL = {
    {
        "Razor Slash",
        "A precise cut that leaves you better protected on the follow-through.",
        ItemRarity::RARE, 1, 6, 3, 5, 1,
        AttackEffect::GAIN_DEFENSE, 0
    },
    {
        "Blood Reaper",
        "Open a wound, guard yourself, and let it mend a little.",
        ItemRarity::RARE, 1, 4, 2, 4, 2,
        AttackEffect::APPLY_BLEED_DEF_HEAL, 3  // effectValue = HP healed
    },
    {
        "Toxin Barb",
        "Jab and follow through. Poisons the wound and sets up the next strike.",
        ItemRarity::RARE, 1, 4, 1, 0, 1,
        AttackEffect::APPLY_POISON_AND_COMBO, 3
    },
    {
        "Stillness",
        "A careful strike that centers you. Rest soon and your body will recover.",
        ItemRarity::RARE, 1, 3, 1, 0, 1,
        AttackEffect::REST_TO_HEAL, 12
    },
    {
        "Cinder Slash",
        "A clean diagonal cut. Reliable, no frills.",
        ItemRarity::RARE, 2, 4, 3, 0, 2,
        AttackEffect::NONE, 0
    },
    {
        "Blinding Strike",
        "Sand and ash thrown into the face. Hard to miss at this range.",
        ItemRarity::RARE, 1, 6, 2, 0, 2,
        AttackEffect::APPLY_BLIND, 3
    },
    {
        "Warding Blow",
        "A strike that repositions you just enough to evade the next hit.",
        ItemRarity::RARE, 1, 4, 1, 0, 2,
        AttackEffect::DODGE_NEXT_ATTACK, 0
    },
    {
        "Venom Lunge",
        "Deep and dirty. Poison seeps into the wound.",
        ItemRarity::RARE, 1, 6, 2, 0, 2,
        AttackEffect::APPLY_POISON, 4
    },
    {
        "Venom Surge",
        "Press the wound. The toxin spreads faster, doubles back on itself.",
        ItemRarity::RARE, 1, 3, 1, 0, 2,
        AttackEffect::DOUBLE_POISON_STACKS, 0
    },
    {
        "Shadow Press",
        "They can't see it coming. A blind enemy is a dead one.",
        ItemRarity::RARE, 1, 6, 3, 0, 2,
        AttackEffect::BLIND_AMPLIFY, 0
    },
    {
        "Infected Wound",
        "Tear it open. If there's already poison inside, the blindness follows.",
        ItemRarity::RARE, 1, 4, 2, 0, 2,
        AttackEffect::BLIND_IF_POISONED, 0
    },
    {
        "Bloodletter",
        "Opens a wound — and if it was already open, drives straight to the bone.",
        ItemRarity::RARE, 1, 6, 2, 0, 2,
        AttackEffect::APPLY_BLEED, 6
    },
    {
        "Coiled Strike",
        "The tension of stillness becomes force. Rest first — then strike.",
        ItemRarity::RARE, 1, 8, 5, 0, 3,
        AttackEffect::BONUS_IF_RESTED, 10
    },
};

static const std::vector<AttackType> EPIC_POOL = {
    {
        "Crimson Fury",
        "Your own blood powers it. Pain becomes force.",
        ItemRarity::EPIC, 1, 6, 2, 0, 1,
        AttackEffect::CRIMSON_FURY, 0
    },
    {
        "Soul Cleave",
        "A killing blow that feeds something back to you.",
        ItemRarity::EPIC, 2, 6, 2, 0, 2,
        AttackEffect::EXECUTE_HEAL, 8
    },
    {
        "Ash Curse",
        "A single motion that opens a wound and poisons it deeply.",
        ItemRarity::EPIC, 2, 4, 3, 0, 2,
        AttackEffect::APPLY_BLEED_AND_POISON, 4
    },
    {
        "Mirror Strike",
        "You hit them. Some of what they were going to do to you comes back.",
        ItemRarity::EPIC, 2, 4, 2, 0, 2,
        AttackEffect::REFLECT_DAMAGE, 50
    },
    {
        "Bloodburst",
        "Slam the wound shut — with force. The bleed ends. So might they.",
        ItemRarity::EPIC, 2, 4, 2, 0, 2,
        AttackEffect::BLEED_BURST, 12  // effectValue = bonus damage if bleeding
    },
    {
        "Overload",
        "Every status they carry makes this hit harder. The sicker, the worse.",
        ItemRarity::EPIC, 1, 6, 2, 0, 2,
        AttackEffect::STATUS_OVERLOAD, 0
    },
    {
        "Toxic Bloom",
        "Let it all out at once. Every stack of poison becomes a burst of pain.",
        ItemRarity::EPIC, 0, 0, 0, 0, 3,
        AttackEffect::POISON_BURST, 4  // effectValue = damage per stack
    },
    {
        "Void Bulwark",
        "Brace and absorb, then strike twice as hard.",
        ItemRarity::EPIC, 1, 4, 2, 11, 3,
        AttackEffect::DOUBLE_NEXT_DAMAGE, 0
    },
};

static const std::vector<AttackType> LEGENDARY_POOL = {
    {
        "Gospel of Ash",
        "Everything burns. Everything bleeds. Everything ends.",
        ItemRarity::LEGENDARY, 2, 6, 4, 0, 3,
        AttackEffect::APPLY_ALL_THREE, 3
    },
    {
        "Death's Bargain",
        "The closer to death you are, the more it costs them. dmg = floor((maxHP - HP) * 0.40).",
        ItemRarity::LEGENDARY, 0, 0, 0, 0, 3,
        AttackEffect::DAMAGE_FROM_MISSING_HP, 5  // effectValue = self-heal
    },
    {
        "Void Collapse",
        "A blow that stuns them so completely they forget to attack.",
        ItemRarity::LEGENDARY, 1, 6, 2, 0, 3,
        AttackEffect::SKIP_ENEMY_TURN, 0
    },
    {
        "Ashen Reckoning",
        "One strike that spreads every ailment they don't already carry.",
        ItemRarity::LEGENDARY, 2, 8, 3, 0, 3,
        AttackEffect::APPLY_ALL_STATUSES, 0
    },
    {
        "Immortal Scar",
        "The wound never closes. The healing stops.",
        ItemRarity::LEGENDARY, 1, 6, 2, 0, 3,
        AttackEffect::PERM_BLEED_NO_REGEN, 0
    },
};

// ── Public API ────────────────────────────────────────────────────────────────

std::vector<AttackType> getAttackPool(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::COMMON:    return COMMON_POOL;
        case ItemRarity::RARE:      return RARE_POOL;
        case ItemRarity::EPIC:      return EPIC_POOL;
        case ItemRarity::LEGENDARY: return LEGENDARY_POOL;
    }
    return COMMON_POOL;
}

AttackType randomAttackType(ItemRarity rarity) {
    auto pool = getAttackPool(rarity);
    return pool[atRng()() % pool.size()];
}

AttackType randomAttackTypeDrop(int chapter, bool allowLegendary, int luck) {
    auto& rng = atRng();
    int roll = rng() % 100;
    int bonus = luck * 2 / 3;  // e.g. luck=9→+6, luck=15→+10, luck=30→+20
    ItemRarity rarity;

    if (chapter == 1) {
        // Ch1 base: 60% common, 35% rare, 5% epic
        // bonus lowers the common threshold, shifting rolls into rare
        if      (roll < std::max(5, 60 - bonus)) rarity = ItemRarity::COMMON;
        else if (roll < 95)                      rarity = ItemRarity::RARE;
        else                                     rarity = ItemRarity::EPIC;
    } else if (chapter == 2) {
        // Ch2 base: 30% common, 45% rare, 22% epic, 3% legendary
        if      (roll < std::max(2, 30 - bonus)) rarity = ItemRarity::COMMON;
        else if (roll < 75)                      rarity = ItemRarity::RARE;
        else if (roll < 97)                      rarity = ItemRarity::EPIC;
        else                rarity = allowLegendary ? ItemRarity::LEGENDARY : ItemRarity::EPIC;
    } else {
        // Ch3 base: 10% common, 35% rare, 52% epic, 3% legendary
        if      (roll < std::max(1, 10 - bonus / 2)) rarity = ItemRarity::COMMON;
        else if (roll < 45)                          rarity = ItemRarity::RARE;
        else if (roll < 97)                          rarity = ItemRarity::EPIC;
        else                rarity = allowLegendary ? ItemRarity::LEGENDARY : ItemRarity::EPIC;
    }
    return randomAttackType(rarity);
}
