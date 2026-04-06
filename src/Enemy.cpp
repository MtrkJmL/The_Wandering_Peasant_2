#include "../include/Enemy.h"
#include <random>
#include <ctime>
#include <stdexcept>

static std::mt19937& enemyRng() {
    static std::mt19937 rng(std::time(nullptr));
    return rng;
}

struct EnemyTemplate {
    std::string    name;
    int            baseHP;
    int            hpPerChapter; // added for ch2/3 scaling
    int            goldBase;
    int            xpBase;
    EnemyBehaviour behaviour;
    // weapon: diceCount, diceSides, modifier
    int wDC, wDS, wMod;
    // armor : diceCount, diceSides, modifier
    int aDC, aDS, aMod;
    bool talkable;
    int  bribeMult; // bribeCost = bribeMult * chapter * 20
};

static EnemyTemplate tmpl(EnemyType t) {
    EnemyTemplate e{};
    switch (t) {
    // ── Chapter 1 ──────────────────────────────────────────────────────────
    case EnemyType::ASHEN_RAIDER:
        e.name = "Ashen Raider";
        e.baseHP = 55; e.hpPerChapter = 0;
        e.goldBase = 15; e.xpBase = 20;
        e.wDC=1; e.wDS=5; e.wMod=1;   // was 1d6+2 (~5.5) → 1d5+1 (~4)
        e.aDC=1; e.aDS=4; e.aMod=1;
        e.talkable = true; e.bribeMult = 3;
        break;
    case EnemyType::SCORCHED_BRUTE:
        e.name = "Scorched Brute";
        e.baseHP = 110; e.hpPerChapter = 0;
        e.goldBase = 20; e.xpBase = 30;
        e.behaviour.isTank = true;
        e.wDC=1; e.wDS=8; e.wMod=1;   // was 2d6+3 (~10) → 1d8+1 (~5.5); telegraph still bites
        e.aDC=2; e.aDS=4; e.aMod=2;
        e.talkable = false; e.bribeMult = 5;
        break;
    case EnemyType::EMBER_WITCH:
        e.name = "Ember Witch";
        e.baseHP = 65; e.hpPerChapter = 0;
        e.goldBase = 18; e.xpBase = 25;
        e.behaviour.appliesBlind = true;
        e.behaviour.statusProcChance = 33;
        e.wDC=1; e.wDS=5; e.wMod=0;   // was 1d6+1 (~4.5) → 1d5 (~3)
        e.aDC=1; e.aDS=4; e.aMod=0;
        e.talkable = true; e.bribeMult = 3;
        break;
    case EnemyType::ASH_HOUND:
        e.name = "Ash Hound";
        e.baseHP = 40; e.hpPerChapter = 0;
        e.goldBase = 12; e.xpBase = 18;
        e.behaviour.attacksTwice = true;
        e.behaviour.appliesBleed = true;
        e.behaviour.statusProcChance = 25;
        e.wDC=1; e.wDS=4; e.wMod=0;   // was 1d5+1 (~4×2=8) → 1d4 (~2.5×2=5)
        e.aDC=1; e.aDS=3; e.aMod=0;
        e.talkable = false; e.bribeMult = 2;
        break;
    // ── Chapter 2 ──────────────────────────────────────────────────────────
    case EnemyType::MARSH_WRAITH:
        e.name = "Marsh Wraith";
        e.baseHP = 70; e.hpPerChapter = 20;
        e.goldBase = 25; e.xpBase = 35;
        e.behaviour.dodgesLightAttacks = true;
        e.wDC=1; e.wDS=8; e.wMod=2;
        e.aDC=1; e.aDS=4; e.aMod=1;
        e.talkable = false; e.bribeMult = 4;
        break;
    case EnemyType::BOG_COLOSSUS:
        e.name = "Bog Colossus";
        e.baseHP = 150; e.hpPerChapter = 30;
        e.goldBase = 30; e.xpBase = 45;
        e.behaviour.regenHPPerTurn = true;
        e.behaviour.regenAmount    = 8;
        e.behaviour.isTank         = true;
        e.wDC=2; e.wDS=8; e.wMod=3;
        e.aDC=2; e.aDS=6; e.aMod=2;
        e.talkable = false; e.bribeMult = 6;
        break;
    case EnemyType::PLAGUE_SHAMAN:
        e.name = "Plague Shaman";
        e.baseHP = 75; e.hpPerChapter = 15;
        e.goldBase = 28; e.xpBase = 38;
        e.behaviour.poisonsInventory = true;  // permanently destroys a potion
        e.behaviour.appliesPoison    = true;
        e.behaviour.statusProcChance = 50;
        e.wDC=1; e.wDS=7; e.wMod=2;
        e.aDC=1; e.aDS=4; e.aMod=1;
        e.talkable = true; e.bribeMult = 4;
        break;
    case EnemyType::MIRE_LURKER:
        e.name = "Mire Lurker";
        e.baseHP = 60; e.hpPerChapter = 15;
        e.goldBase = 22; e.xpBase = 32;
        e.behaviour.ambushesOnTurnOne = true;
        e.wDC=2; e.wDS=6; e.wMod=2;
        e.aDC=1; e.aDS=4; e.aMod=0;
        e.talkable = false; e.bribeMult = 3;
        break;
    // ── Chapter 3 ──────────────────────────────────────────────────────────
    case EnemyType::DARKBLOOD_SENTINEL:
        e.name = "Darkblood Sentinel";
        e.baseHP = 120; e.hpPerChapter = 40;
        e.goldBase = 40; e.xpBase = 55;
        e.behaviour.interruptsTimingBar = true;
        e.wDC=2; e.wDS=8; e.wMod=4;
        e.aDC=2; e.aDS=6; e.aMod=3;
        e.talkable = false; e.bribeMult = 8;
        break;
    case EnemyType::ASH_TITAN:
        e.name = "Ash Titan";
        e.baseHP = 200; e.hpPerChapter = 50;
        e.goldBase = 45; e.xpBase = 60;
        e.behaviour.reflectsHeavy = true;
        e.behaviour.isTank        = true;
        e.wDC=2; e.wDS=10; e.wMod=4;
        e.aDC=2; e.aDS=8;  e.aMod=3;
        e.talkable = false; e.bribeMult = 9;
        break;
    case EnemyType::SOUL_BINDER:
        e.name = "Soul Binder";
        e.baseHP = 90; e.hpPerChapter = 25;
        e.goldBase = 38; e.xpBase = 50;
        e.behaviour.locksItemSlot = true;
        e.wDC=2; e.wDS=8; e.wMod=3;
        e.aDC=1; e.aDS=6; e.aMod=2;
        e.talkable = false; e.bribeMult = 7;
        break;
    case EnemyType::DARKBLOOD_ZEALOT:
        e.name = "Darkblood Zealot";
        e.baseHP = 35; e.hpPerChapter = 10;
        e.goldBase = 35; e.xpBase = 50;
        e.behaviour.isGlassCannon = true;
        e.wDC=3; e.wDS=10; e.wMod=5;
        e.aDC=1; e.aDS=4;  e.aMod=0;
        e.talkable = false; e.bribeMult = 6;
        break;
    default:
        throw std::runtime_error("Unknown EnemyType");
    }
    return e;
}

// ── Constructor ──────────────────────────────────────────────────────────────

Enemy::Enemy(EnemyType t, int ch)
    : type(t), chapter(ch) {
    EnemyTemplate e = tmpl(t);
    name      = e.name;
    behaviour = e.behaviour;
    maxHealth = e.baseHP + e.hpPerChapter * (ch - 1);
    health    = maxHealth;
    goldReward       = e.goldBase + ch * 5;
    experienceReward = e.xpBase  + ch * 8;
    weapon = Item(ItemType::WEAPON, e.name + "'s Weapon",
                  e.wDC, e.wDS + (ch-1), e.wMod + ch - 1);
    armor  = Item(ItemType::ARMOR,  e.name + "'s Armour",
                  e.aDC, e.aDS + (ch-1), e.aMod + ch - 1);
    canBeTalkedTo = e.talkable;
    bribeCost     = e.bribeMult * ch * 20;

    // Determine drop rarity
    auto& rng = enemyRng();
    int roll = rng() % 100;
    int bonus = ch * 5;
    if      (roll < 60 - bonus) dropRarity = ItemRarity::COMMON;
    else if (roll < 85 - bonus) dropRarity = ItemRarity::RARE;
    else if (roll < 97)         dropRarity = ItemRarity::EPIC;
    else                        dropRarity = ItemRarity::LEGENDARY;
}

// ── Getters ──────────────────────────────────────────────────────────────────

std::string          Enemy::getName()         const { return name; }
EnemyType            Enemy::getType()         const { return type; }
int                  Enemy::getHealth()       const { return health; }
int                  Enemy::getMaxHealth()    const { return maxHealth; }
int                  Enemy::getGoldReward()   const { return goldReward; }
int                  Enemy::getExperienceReward() const { return experienceReward; }
ItemRarity           Enemy::getDropRarity()   const { return dropRarity; }
const EnemyBehaviour& Enemy::getBehaviour()  const { return behaviour; }
bool                 Enemy::canTalk()         const { return canBeTalkedTo; }
int                  Enemy::getBribeCost()    const { return bribeCost; }
bool                 Enemy::isTelegraphing()  const { return telegraphing; }

void Enemy::takeDamage(int dmg) { health = std::max(0, health - dmg); }
void Enemy::heal(int amt)       { health = std::min(maxHealth, health + amt); }
bool Enemy::isAlive()           const { return health > 0; }

int Enemy::rollAttack()  const { return weapon.roll(); }
int Enemy::rollDefense() const { return armor.roll(); }

Item Enemy::generateDrop(int luck) const {
    auto& rng = enemyRng();
    int roll  = rng() % 100;
    int bonus = chapter * 5 + luck / 3;
    ItemRarity r;
    if      (roll < 60 - bonus) r = ItemRarity::COMMON;
    else if (roll < 85 - bonus) r = ItemRarity::RARE;
    else if (roll < 97)         r = ItemRarity::EPIC;
    else                        r = ItemRarity::LEGENDARY;
    ItemType t = (rng() % 2 == 0) ? ItemType::WEAPON : ItemType::ARMOR;
    Item item(t, r);

    // Luck bonus: if weapon rolled no status effect, give a second-chance roll
    // Base is 20% (in Item ctor). Luck adds 0.5% per point as a second-chance.
    if (t == ItemType::WEAPON && item.getStatusEffect() == StatusEffect::NONE) {
        int luckBonus = static_cast<int>(luck * 0.5f);
        if (luckBonus > 0 && (int)(rng() % 100) < luckBonus) {
            int pick = rng() % 3;
            item.setStatusEffect(pick == 0 ? StatusEffect::POISON
                               : pick == 1 ? StatusEffect::BLEED
                               :             StatusEffect::BLIND);
        }
    }
    return item;
}

// ── Status effects ────────────────────────────────────────────────────────────

void Enemy::applyBleed() {
    bleedStacks = 1; // permanent until death — re-applying just refreshes
}

void Enemy::applyPoison(int stacks) {
    poisonStacks += stacks; // stacks add — 4 existing + 5 applied = 9
}

void Enemy::applyBlind(int rounds) {
    blindRounds = rounds; // re-applying resets the timer
}

bool Enemy::isBleeding() const { return bleedStacks > 0; }
bool Enemy::isPoisoned() const { return poisonStacks > 0; }
bool Enemy::isBlinded()  const { return blindRounds > 0; }

int  Enemy::getBleedStacks()  const { return bleedStacks; }
int  Enemy::getPoisonStacks() const { return poisonStacks; }
int  Enemy::getBlindRounds()  const { return blindRounds; }

int Enemy::tickBleed(int damage) {
    if (bleedStacks <= 0) return 0;
    takeDamage(damage); // permanent — never expires
    return damage;
}

int Enemy::tickPoison() {
    if (poisonStacks <= 0) return 0;
    int dmg = poisonStacks;
    takeDamage(dmg);
    --poisonStacks;
    return dmg;
}

void Enemy::tickBlind() {
    if (blindRounds > 0) --blindRounds;
}

bool Enemy::rollBlindMiss(int missChance) const {
    static std::mt19937 rng(std::time(nullptr));
    return (int)(rng() % 100) < missChance;
}

void Enemy::addAttackDebuff(int amt) { attackDebuff += amt; }
int  Enemy::getAttackDebuff() const  { return attackDebuff; }

std::string Enemy::getDescription() const {
    switch (type) {
    case EnemyType::ASHEN_RAIDER:       return "A corrupted soldier wreathed in soot. Fast and relentless.";
    case EnemyType::SCORCHED_BRUTE:     return "A hulking mass of char and fury. Slow — but telegraphs devastating blows.";
    case EnemyType::EMBER_WITCH:        return "Fingers like burned twigs. Leaves you smouldering.";
    case EnemyType::ASH_HOUND:          return "A skeletal dog made of cinders. Strikes twice before you blink.";
    case EnemyType::MARSH_WRAITH:       return "A ghost of smoke and rot. Light attacks pass straight through.";
    case EnemyType::BOG_COLOSSUS:       return "Ancient mud given form. It heals unless you hit hard enough to matter.";
    case EnemyType::PLAGUE_SHAMAN:      return "Its touch rots your supplies. A potion will fail when you need it most.";
    case EnemyType::MIRE_LURKER:        return "You didn't see it until it was already moving.";
    case EnemyType::DARKBLOOD_SENTINEL: return "Armoured in obsidian plate. It will interrupt your swing mid-arc.";
    case EnemyType::ASH_TITAN:          return "Immense. Every heavy blow you land comes back at you.";
    case EnemyType::SOUL_BINDER:        return "It reaches into your pack and locks something shut. You won't know what until you need it.";
    case EnemyType::DARKBLOOD_ZEALOT:   return "Frenzied devotee. It will either kill you or die in two hits.";
    }
    return "";
}

std::string Enemy::getDialogue() const {
    switch (type) {
    case EnemyType::ASHEN_RAIDER:   return "\"You don't know what you're walking into, peasant.\"";
    case EnemyType::EMBER_WITCH:    return "\"The ash remembers everything that burned.\"";
    case EnemyType::PLAGUE_SHAMAN:  return "\"Your remedies are already failing you.\"";
    default:                        return "It shows no interest in talking.";
    }
}
