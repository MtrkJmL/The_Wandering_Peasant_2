#include "../include/Achievement.h"
#include <fstream>

static const AchievementData ACHIEVEMENT_DATA[ACH_COUNT] = {
    // key                       name                          description                                              secret
    { "ROAD_TO_ASH",            "The Road to Ash",            "Complete Chapter 1.",                                   false },
    { "INTO_THE_FOG",           "Into the Fog",               "Complete Chapter 2.",                                   false },
    { "GOSPEL_OF_ASH",          "Gospel of Ash",              "Complete Chapter 3.",                                   false },
    { "TRUTH_WAS_ALWAYS_THERE", "The Truth Was Always There", "Defeat Slave King Amax — true ending.",                 true  },
    { "NO_POTIONS",             "Dry Run",                    "Complete a run without using any potions.",             false },
    { "PEASANT_CLOTHES",        "Dressed for the Road",       "Complete a run still wearing your Peasant Clothes.",    false },
    { "RUNELESS",               "Runeless",                   "Complete a run without ever equipping a rune.",         false },
    { "FIRST_RUNE",             "M'alid's First Lesson",      "Learn your first rune from M'alid.",                   false },
    { "RUNE_SCHOLAR",           "Rune Scholar",               "Learn 5 different runes across all your runs.",         false },
    { "ALL_RUNES",              "Archivist of the Ash",       "Discover all 10 runes across your runs.",               false },
    { "WORDSMITH",              "Wordsmith",                  "Exit combat via Talk 3 times in one run.",              false },
    { "BARGAINER",              "Bargainer",                  "Exit combat via Bargain 3 times in one run.",           false },
    { "FULLY_FORGED",           "Fully Forged",               "Upgrade a weapon to its maximum tier.",                 false },
    { "ALL_SKILLS",             "The Full Arsenal",           "Discover every skill across all your runs.",            false },
    { "MASTER_PEASANT",         "Master Peasant",             "Complete 3 runs in a row without dying.",               false },
};

const AchievementData& achievementData(AchievementID id) {
    return ACHIEVEMENT_DATA[static_cast<int>(id)];
}

bool Achievements::unlock(AchievementID id) {
    int i = static_cast<int>(id);
    if (unlocked[i]) return false;
    unlocked[i] = true;
    return true;
}

bool Achievements::isUnlocked(AchievementID id) const {
    return unlocked[static_cast<int>(id)];
}

int Achievements::unlockedCount() const {
    int n = 0;
    for (int i = 0; i < ACH_COUNT; ++i) if (unlocked[i]) ++n;
    return n;
}

bool Achievements::onRunCompleted() {
    ++consecutiveRuns;
    if (consecutiveRuns >= 3)
        return unlock(AchievementID::MASTER_PEASANT);
    return false;
}

void Achievements::onRunFailed() {
    consecutiveRuns = 0;
}

void Achievements::load(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return;
    std::string line;
    while (std::getline(f, line)) {
        if (line.size() > 7 && line.substr(0, 7) == "UNLOCK ") {
            std::string key = line.substr(7);
            for (int i = 0; i < ACH_COUNT; ++i)
                if (key == ACHIEVEMENT_DATA[i].key) unlocked[i] = true;
        } else if (line.size() > 12 && line.substr(0, 12) == "CONSEC_RUNS ") {
            try { consecutiveRuns = std::stoi(line.substr(12)); } catch (...) {}
        }
    }
}

void Achievements::save(const std::string& path) const {
    std::ofstream f(path);
    for (int i = 0; i < ACH_COUNT; ++i)
        if (unlocked[i]) f << "UNLOCK " << ACHIEVEMENT_DATA[i].key << "\n";
    f << "CONSEC_RUNS " << consecutiveRuns << "\n";
}
