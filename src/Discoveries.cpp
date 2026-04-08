#include "../include/Discoveries.h"
#include <fstream>

bool Discoveries::recordAttack(const std::string& name) {
    return attacks.insert(name).second;
}

bool Discoveries::recordRune(RuneType r) {
    if (r == RuneType::NONE) return false;
    return runes.insert(static_cast<int>(r)).second;
}

void Discoveries::load(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return;
    std::string line;
    while (std::getline(f, line)) {
        if (line.size() > 7 && line.substr(0, 7) == "ATTACK ") {
            attacks.insert(line.substr(7));
        } else if (line.size() > 5 && line.substr(0, 5) == "RUNE ") {
            try { runes.insert(std::stoi(line.substr(5))); } catch (...) {}
        }
    }
}

void Discoveries::save(const std::string& path) const {
    std::ofstream f(path);
    for (const auto& name : attacks) f << "ATTACK " << name << "\n";
    for (int r : runes)              f << "RUNE "   << r    << "\n";
}
