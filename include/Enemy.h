#pragma once
#include <string>
#include "Item.h"

enum class EnemyType {
    // Chapter 1 — Ashfields
    ASHEN_RAIDER,
    SCORCHED_BRUTE,
    EMBER_WITCH,
    ASH_HOUND,
    // Chapter 2 — Hollowed Marsh
    MARSH_WRAITH,
    BOG_COLOSSUS,
    PLAGUE_SHAMAN,
    MIRE_LURKER,
    // Chapter 3 — Obsidian Reach
    DARKBLOOD_SENTINEL,
    ASH_TITAN,
    SOUL_BINDER,
    DARKBLOOD_ZEALOT
};

// Special per-enemy combat behaviours
struct EnemyBehaviour {
    bool dodgesLightAttacks  = false; // Marsh Wraith
    bool regenHPPerTurn      = false; // Bog Colossus — +8 HP/turn
    bool poisonsInventory    = false; // Plague Shaman — permanently destroys a potion
    bool ambushesOnTurnOne   = false; // Mire Lurker — attacks before player turn 1
    bool interruptsTimingBar = false; // Darkblood Sentinel — random bar-stop
    bool reflectsHeavy       = false; // Ash Titan — reflects heavy dmg back
    bool locksItemSlot       = false; // Soul Binder — locks one slot whole fight
    bool attacksTwice        = false; // Ash Hound — two attacks per turn; applies Bleed
    bool appliesBlind        = false; // Ember Witch — player is blinded (30% miss, 2 rounds)
    bool appliesBleed        = false; // Ash Hound — player bleeds (2 dmg/turn, permanent)
    bool appliesPoison       = false; // Plague Shaman — player poisoned (5 stacks)
    bool isTank              = false; // Scorched Brute — telegraphs attacks
    bool isGlassCannon       = false; // Darkblood Zealot — fast bar, 2-hit kill
    int  regenAmount         = 0;
    int  statusProcChance    = 33;   // % chance to apply status per attack (enemies with status flags)
};

class Enemy {
private:
    std::string     name;
    EnemyType       type;
    int             chapter;
    int             health;
    int             maxHealth;
    int             goldReward;
    int             experienceReward;
    Item            weapon;
    Item            armor;
    ItemRarity      dropRarity;
    EnemyBehaviour  behaviour;
    bool            canBeTalkedTo;
    int             bribeCost;
    bool            telegraphing = false; // Brute pre-attack tell

    // Status effects on this enemy
    int bleedStacks  = 0; // 0 = not bleeding, 1 = bleeding (permanent until death)
    int poisonStacks = 0; // decays by 1 each turn, deals X dmg/turn
    int blindRounds  = 0; // enemy miss rounds remaining
    int attackDebuff = 0;

public:
    Enemy(EnemyType type, int chapter);

    std::string          getName()    const;
    EnemyType            getType()    const;
    int                  getHealth()  const;
    int                  getMaxHealth() const;
    int                  getGoldReward() const;
    int                  getExperienceReward() const;
    ItemRarity           getDropRarity() const;
    const EnemyBehaviour& getBehaviour() const;
    bool                 canTalk()    const;
    int                  getBribeCost() const;
    bool                 isTelegraphing() const;

    void takeDamage(int dmg);
    void heal(int amount);
    bool isAlive() const;

    int  rollAttack()  const;
    int  rollDefense() const;
    Item generateDrop(int luck) const;

    void setTelegraphing(bool t) { telegraphing = t; }
    std::string getDialogue() const;
    std::string getDescription() const;

    // Status effects
    void applyBleed();
    void applyPoison(int stacks = 5);  // resets to stacks (does not add)
    void applyBlind(int rounds = 3);
    bool isBleeding()          const;
    bool isPoisoned()          const;
    bool isBlinded()           const;
    int  tickBleed(int damage = 2);    // permanent; damage param for Hemorrhage stance
    int  tickPoison();                 // deals poisonStacks dmg, then stacks--
    void tickBlind();                  // decrements blindRounds
    bool rollBlindMiss(int missChance) const; // true = attack misses
    void addAttackDebuff(int amt);
    int  getAttackDebuff() const;
    int  getBleedStacks()  const;
    int  getPoisonStacks() const;
    int  getBlindRounds()  const;
};
