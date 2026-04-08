#pragma once
#include <string>
#include <vector>
#include "Item.h"

enum class AttackEffect {
    NONE,
    APPLY_BLEED,
    APPLY_POISON,           // effectValue = stacks
    APPLY_BLIND,            // effectValue = rounds
    GAIN_DEFENSE,           // effectValue unused; defenseBonus field carries the value
    COMBO_SETUP,            // effectValue = bonus damage added to next attack
    APPLY_BLEED_AND_POISON, // Ash Curse: effectValue = poison stacks
    APPLY_ALL_THREE,        // Gospel of Ash: bleed + blind(3) + poison(3)
    DOUBLE_NEXT_DAMAGE,     // Void Bulwark: next attack deals x2; defenseBonus grants armor
    DODGE_NEXT_ATTACK,      // Warding Blow: next incoming attack auto-dodged
    EXECUTE_HEAL,           // Soul Cleave: effectValue = HP restored if kill
    CRIMSON_FURY,           // x2 damage if player is bleeding
    REFLECT_DAMAGE,         // effectValue = percent of incoming damage reflected this turn
    SKIP_ENEMY_TURN,        // Void Collapse: enemy skips their next attack
    APPLY_ALL_STATUSES,     // Ashen Reckoning: apply all statuses enemy lacks if any active
    PERM_BLEED_NO_REGEN,    // Immortal Scar: permanent bleed + suppresses enemy regen
    DAMAGE_FROM_MISSING_HP,   // Death's Bargain: 40% missing HP as damage; effectValue = self-heal
    SELF_BLEED,               // Bloodprice: player becomes bleeding (permanent until fight ends)
    REST_TO_HEAL,             // Stillness: effectValue = HP healed next time player rests
    BONUS_IF_RESTED,          // Coiled Strike: effectValue = bonus dmg if player rested last turn
    APPLY_POISON_AND_COMBO,   // Toxin Barb: effectValue = poison stacks; same value added as combo bonus
    // New build-oriented effects
    APPLY_BLEED_DEF_HEAL,     // Blood Reaper: apply bleed + defenseBonus def + effectValue HP heal
    APPLY_POISON_ADD,         // Plague Touch: add effectValue poison; if enemy bleeding, add effectValue more
    DOUBLE_POISON_STACKS,     // Venom Surge: double enemy's current poison stacks
    BLIND_IF_POISONED,        // Infected Wound: apply bleed; if enemy poisoned, also blind (3 rounds)
    BLIND_AMPLIFY,            // Shadow Press: deal x2 damage if enemy is blind
    BLEED_BURST,              // Bloodburst: if enemy bleeding, deal +effectValue damage and remove bleed
    STATUS_OVERLOAD,          // Overload: deal +(total status stacks * 3) bonus damage
    DESPERATE_STRIKE,         // deal x2 damage if player HP < 40% of maxHP
    POISON_BURST,             // Toxic Bloom: deal effectValue dmg per poison stack, then remove all
};

struct AttackType {
    std::string  name;
    std::string  description;
    ItemRarity   rarity;
    int          diceCount;
    int          diceSides;
    int          modifier;
    int          defenseBonus;  // flat defense added to player this turn (0 = none)
    int          cooldown;      // turns this slot is locked after use (1 or 2)
    AttackEffect effect;
    int          effectValue;   // context-specific param: stacks, heal amount, %, etc.

    int         roll() const;
    std::string getStatsString() const;
};

// Full pools per rarity
std::vector<AttackType> getAttackPool(ItemRarity rarity);

// Pick one random AttackType from the given rarity pool
AttackType randomAttackType(ItemRarity rarity);

// Pick a random AttackType weighted by chapter tier.
// Legendaries excluded unless allowLegendary = true.
// luck shifts rarity thresholds toward higher tiers (luck/3 bonus).
AttackType randomAttackTypeDrop(int chapter, bool allowLegendary = false, int luck = 0);

// Cooldown guide: cd1 = light utility, cd2 = standard, cd3 = commitment (big effect/damage)
