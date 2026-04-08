#pragma once
#include <set>
#include <string>
#include "Rune.h"

struct Discoveries {
    std::set<std::string> attacks; // attack names seen across all runs
    std::set<int>         runes;   // RuneType as int (excludes NONE)

    // Returns true if this was a new entry
    bool recordAttack(const std::string& name);
    bool recordRune(RuneType r);

    void load(const std::string& path);
    void save(const std::string& path) const;
};
