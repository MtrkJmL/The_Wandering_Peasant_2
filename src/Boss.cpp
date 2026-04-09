#include "../include/Boss.h"
#include <stdexcept>
#include <algorithm>

Boss::Boss(BossType t, int chapter) : type(t), phase(0) {
    switch (t) {
    case BossType::DARKBLOOD_LIEUTENANT:
        name             = "Darkblood Lieutenant Varek";
        maxHealth        = 200;
        goldReward       = 80;
        experienceReward = 100;
        weapon = Item(ItemType::WEAPON, "Darkblood Blade",  2, 8,  4);
        armor  = Item(ItemType::ARMOR,  "Ashened Plate",    2, 6,  3);
        phases = {
            { 100, 2, "Varek snarls. \"You actually hurt me. Interesting.\"" },
        };
        bleedResistPct = 15;
        break;
    case BossType::WITCH_OF_GHAAL:
        name             = "The Witch of G'haal";
        maxHealth        = 280;
        goldReward       = 120;
        experienceReward = 150;
        weapon = Item(ItemType::WEAPON, "Cursed Staff",      2, 10, 5);
        armor  = Item(ItemType::ARMOR,  "Swamp-soaked Robes",1, 6,  2);
        phases = {
            { 140, 3, "The Witch laughs — a horrible wet sound. \"You have no idea, do you?\"" },
            {  60, 5, "Spectral fire pours from her eyes. \"Fine. I'll show you what we've been fighting.\"" },
        };
        poisonResistPct = 15;
        break;
    case BossType::DARKBLOOD_LORD:
        name             = "Darkblood Lord Szathos";
        maxHealth        = 380;
        goldReward       = 200;
        experienceReward = 250;
        weapon = Item(ItemType::WEAPON, "Lord's Obsidian Maul", 3, 10, 6);
        armor  = Item(ItemType::ARMOR,  "Dragon-bone Armour",   2, 8,  5);
        phases = {
            { 200, 4, "Szathos slams the earth. \"I will not fall to a peasant.\"" },
            {  80, 7, "The obsidian cracks. Something inside him screams." },
        };
        blindResistPct = 15;
        break;
    case BossType::SLAVE_KING_AMAX:
        name             = "Slave King Amax";
        maxHealth        = 450;
        goldReward       = 300;
        experienceReward = 400;
        weapon = Item(ItemType::WEAPON, "The Broken Crown",    3, 12, 8);
        armor  = Item(ItemType::ARMOR,  "Chains of the Bound", 2, 10, 6);
        phases = {
            { 300, 5, "Amax stops. \"You carry the banner. You know what I am.\"" },
            { 150, 8, "\"I was a peasant. Just like you. I saw what power could do.\"" },
            {  50, 3, "His voice breaks. \"End it. Please.\"" },
        };
        break;
    default:
        throw std::runtime_error("Unknown BossType");
    }
    health = maxHealth;
}

std::string Boss::getName()          const { return name; }
BossType    Boss::getType()          const { return type; }
int         Boss::getHealth()        const { return health; }
int         Boss::getMaxHealth()     const { return maxHealth; }
int         Boss::getCurrentPhase()  const { return phase; }
int         Boss::getGoldReward()    const { return goldReward; }
int         Boss::getExperienceReward() const { return experienceReward; }

void Boss::applyBleed()            { bleedStacks = 1; }
void Boss::applyPoison(int stacks) { poisonStacks += stacks; }
void Boss::applyBlind(int rounds)  { blindRounds = std::max(blindRounds, rounds); }
void Boss::clearBleed()            { bleedStacks = 0; }
void Boss::clearPoison()           { poisonStacks = 0; }
bool Boss::isBleeding()  const { return bleedStacks > 0; }
bool Boss::isPoisoned()  const { return poisonStacks > 0; }
bool Boss::isBlinded()   const { return blindRounds > 0; }
int  Boss::getPoisonStacks() const { return poisonStacks; }
int  Boss::getBlindRounds()  const { return blindRounds; }
int  Boss::tickBleed()  { return 2; }
int  Boss::tickPoison() {
    if (poisonStacks <= 0) return 0;
    int dmg = poisonStacks;
    --poisonStacks;
    return dmg;
}
void Boss::tickBlind()  { if (blindRounds > 0) --blindRounds; }
int  Boss::getBleedResistPct()  const { return bleedResistPct; }
int  Boss::getPoisonResistPct() const { return poisonResistPct; }
int  Boss::getBlindResistPct()  const { return blindResistPct; }

void Boss::takeDamage(int dmg) { health = std::max(0, health - dmg); }
bool Boss::isAlive()           const { return health > 0; }

bool Boss::checkPhaseTransition() {
    if (phase < (int)phases.size() &&
        health <= phases[phase].healthThreshold) {
        ++phase;
        return true;
    }
    return false;
}

int Boss::rollAttack() const {
    int bonus = (phase < (int)phases.size()) ? phases[phase].attackBonus : 0;
    return weapon.roll() + bonus;
}

int Boss::rollDefense() const { return armor.roll(); }

std::string Boss::getPhaseText() const {
    int p = phase - 1;
    if (p >= 0 && p < (int)phases.size()) return phases[p].phaseMessage;
    return "";
}

std::string Boss::getIntroText() const {
    switch (type) {
    case BossType::DARKBLOOD_LIEUTENANT:
        return "A figure steps from the ash. Armoured in black plate, face hidden behind a visor. "
               "One gauntleted fist rests on a long, dark blade.\n"
               "\"Turn back, peasant. The only thing ahead of you is ash.\"";
    case BossType::WITCH_OF_GHAAL:
        return "She is older than the marsh. Her eyes are lit with something that predates fire. "
               "She watches you from the centre of a ring of dead reeds and does not move.\n"
               "\"You've come a long way just to hear something you won't want to know.\"";
    case BossType::DARKBLOOD_LORD:
        return "Szathos does not announce himself. He simply appears — filling the obsidian archway "
               "like a shadow given armour. The air around him tastes of ash and old blood.\n"
               "\"Every peasant thinks they are different. None of them are.\"";
    case BossType::SLAVE_KING_AMAX:
        return "He has the face of someone who was once ordinary. Crow's feet around hollow eyes. "
               "Chains of obsidian run from his wrists to nothing — to somewhere you cannot see.\n"
               "\"You found the banner. Then you know my name.\"\n"
               "A pause. \"I am what happens when a peasant chooses power over people.\"";
    }
    return "";
}

std::string Boss::getDefeatText() const {
    switch (type) {
    case BossType::DARKBLOOD_LIEUTENANT:
        return "Varek falls to one knee. His visor cracks. Behind it — not a monster, but a face "
               "worn smooth by decades of grief.\n\"You don't understand what you've started.\" "
               "He says it like a warning. Then he is still.";
    case BossType::WITCH_OF_GHAAL:
        return "The witch collapses like a puppet whose strings were cut. But she is laughing — "
               "still laughing — and when you reach her, she grabs your wrist with one burned hand.\n"
               "\"The Darkbloods did not wake the Dragon King. We tried to CONTAIN him. "
               "For centuries we bled so the world could sleep.\"\n"
               "Her grip tightens. \"And you... you've broken the seal. He is already HERE.\"";
    case BossType::DARKBLOOD_LORD:
        return "Szathos kneels. The obsidian armour splinters from the inside.\n"
               "\"It doesn't matter,\" he breathes. \"He is already free. You cannot stop what is already done.\"\n"
               "He looks at you — not with hatred, but with something worse. Pity.";
    case BossType::SLAVE_KING_AMAX:
        return "The chains dissolve. Amax is on his knees in a circle of ash.\n"
               "He looks at his hands — ordinary hands, a peasant's hands — and something in "
               "his face unlocks.\n"
               "\"I thought power would free me from them,\" he says quietly. "
               "\"Instead I handed them the key.\"\n"
               "He closes his eyes. The ash settles around him like snow.\n"
               "\"Tell them... tell them a peasant broke the chain.\"";
    }
    return "";
}
