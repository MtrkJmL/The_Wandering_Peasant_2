#pragma once
#include <string>
#include <vector>
#include "Item.h"
#include "Potion.h"
#include "Rune.h"
#include "AttackType.h"

struct CombatBuffs {
    int  shieldTonicTurns         = 0;     // +5 defense for N turns
    bool rageDraughtActive        = false; // next attack x1.5
    int  lockedPotionIndex        = -1;    // Soul Binder effect (-1 = none)
    // Status effects applied to player by enemies
    bool bleeding                 = false; // Ash Hound — 2 dmg/turn, permanent
    int  poisonStacks             = 0;     // Plague Shaman — X dmg/turn, decays by 1
    int  blindRounds              = 0;     // Ember Witch — player misses 30%, 3 rounds
    // Town of Dead Ballads potion effects
    bool phantomsDraughtActive    = false; // next attack ignores enemy defense
    int  gravewardenOilCharges    = 0;     // next N attacks apply Bleed guaranteed
    // Attack type per-turn state
    int  attackTypeDefenseThisTurn = 0;   // defense granted by attack type used this turn
    bool dodgeNextAttack           = false; // Warding Blow
    bool doubleNextDamage          = false; // Void Bulwark: next attack x2
    int  comboSetupBonus           = 0;    // Braced Strike: +N to next attack damage
    bool skipEnemyNextTurn         = false; // Void Collapse: enemy skips next attack
    bool suppressEnemyRegen        = false; // Immortal Scar: enemy cannot regen this fight
    int  restHealBonus             = 0;    // Stillness: heal this amount on next rest
    bool restedLastTurn            = false; // Coiled Strike: +bonus if rested last turn
};

class Player {
private:
    std::string name;
    int         level;
    int         experience;
    int         gold;
    int         health;
    int         maxHealth;
    int         stamina;
    int         maxStamina;
    int         defense;
    int         luck;

    // Flat-bonus equipment (diceCount=0, modifier=bonus)
    Item        weapon;
    Item        armor;

    // Attack type system
    AttackType  attackSlots[3];
    int         attackCooldowns[3]; // turns remaining on cooldown (0 = ready)

    // Rune system
    std::vector<RuneType> learnedRunes;
    RuneType              runeSlots[2]; // RuneType::NONE if empty

    std::vector<Potion> potions;
    int         experienceToNextLevel;
    CombatBuffs buffs;

public:
    Player();
    Player(const std::string& playerName);

    // Getters
    std::string              getName()      const;
    int                      getLevel()     const;
    int                      getExperience()const;
    int                      getGold()      const;
    int                      getHealth()    const;
    int                      getMaxHealth() const;
    int                      getStamina()   const;
    int                      getMaxStamina()const;
    int                      getDefense()   const;
    int                      getLuck()      const;
    const Item&              getWeapon()    const;
          Item&              getWeapon();
    const Item&              getArmor()     const;
          Item&              getArmor();
    const std::vector<Potion>& getPotions() const;
          std::vector<Potion>& getPotions();
    const CombatBuffs&       getBuffs()     const;
          CombatBuffs&       getBuffs();

    // Stat mutators
    void addMaxHealth(int amount);
    void addMaxStamina(int amount);
    void addDefense(int amount);
    void addLuck(int amount);
    void addGold(int amount);
    bool spendGold(int amount);

    // Combat
    void takeDamage(int dmg);
    void heal(int amount);
    void useStamina(int amount);
    void restoreStamina(int amount);
    int  rollDefense() const;
    bool isAlive()     const;

    // Timing-based attack (baseDamageRoll already rolled by caller from AttackType)
    int  timingBasedAttack(int cursorPos, int critPos, int totalPositions,
                           int baseDamageRoll, bool& isCrit);

    // Potions (free action)
    bool usePotion(int index);
    void addPotion(const Potion& p);
    void lockPotionSlot(int index);
    void clearPotionLock();

    // Per-turn buff tick (call at start of player turn)
    void tickBuffs();

    // Attack type system
    const AttackType& getAttack(int slot)    const;
    int               getAttackCooldown(int slot) const;
    bool              isAttackReady(int slot) const;
    void              setAttack(int slot, const AttackType& at);
    void              startCooldown(int slot);   // set cooldown from attackSlots[slot].cooldown
    void              tickCooldowns();           // decrement all cooldowns (call at start of player turn)

    // Rune system
    void                        learnRune(RuneType r);
    bool                        hasLearnedRune(RuneType r) const;
    bool                        hasRune(RuneType r) const; // true if in an active slot
    const std::vector<RuneType>& getLearnedRunes() const;
    RuneType                    getRuneSlot(int slot) const;
    bool                        setRuneSlot(int slot, RuneType r); // true if learned
    void                        resetRunes();

    // Player status effect application (by enemies)
    void applyBleedToPlayer();
    void applyPoisonToPlayer(int stacks = 5);
    void applyBlindToPlayer(int rounds = 3);

    // Equipment
    void equipWeapon(const Item& w);
    void equipArmor(const Item& a);

    // Experience / levelling
    void addExperience(int xp);
    void levelUp();

    // Status display
    void showStatus()    const;
    void showInventory() const;
};
