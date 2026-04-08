#include "../include/Game.h"
#include "../include/Rune.h"
#include "../include/AttackType.h"
#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
int _kbhit() {
    struct timeval tv = { 0L, 0L }; fd_set fds;
    FD_ZERO(&fds); FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}
int _getch() {
    int r; unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) return r;
    return c;
}
#endif

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

// ── Helpers ───────────────────────────────────────────────────────────────────

Game::Game() : isRunning(false), rng(std::time(nullptr)) {
#ifdef _WIN32
    system("chcp 65001 > nul");
    SetConsoleOutputCP(65001);
#endif
    discoveries.load("discoveries.dat");
}

void Game::saveDiscoveries() {
    discoveries.save("discoveries.dat");
}

int Game::randInt(int lo, int hi) {
    std::uniform_int_distribution<int> d(lo, hi);
    return d(rng);
}

void Game::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Game::pressEnter() const {
    std::cout << DIM << "\n[Press ENTER to continue]" << RESET;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void Game::slowPrint(const std::string& text, int delayMs) const {
    for (char c : text) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
    std::cout << "\n";
}

void Game::slowPrintSkippable(const std::string& text, int delayMs, bool& skipped) const {
    for (char c : text) {
        if (!skipped && _kbhit()) { _getch(); skipped = true; }
        if (skipped) { std::cout << c; }
        else { std::cout << c << std::flush;
               std::this_thread::sleep_for(std::chrono::milliseconds(delayMs)); }
    }
    if (skipped) std::cout << std::flush;
    std::cout << "\n";
}

// ── Main menu ─────────────────────────────────────────────────────────────────

void Game::run() {
    isRunning = true;
    displayMainMenu();
}

void Game::displayMainMenu() {
    while (isRunning) {
        clearScreen();

        // ── Border ───────────────────────────────────────────────────────────
        std::cout << DIM << YELLOW
                  << "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                  << RESET << "\n";

        // ── "THE WANDERING" header line ───────────────────────────────────────
        std::cout << DIM << YELLOW
                  << "        ───────  T H E   W A N D E R I N G  ───────\n\n"
                  << RESET;

        // ── "PEASANT" in block letters ────────────────────────────────────────
        std::cout << YELLOW << BOLD << R"(  ██████╗ ███████╗ █████╗ ███████╗ █████╗ ███╗   ██╗████████╗
  ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗████╗  ██║╚══██╔══╝
  ██████╔╝█████╗  ███████║███████╗███████║██╔██╗ ██║   ██║
  ██╔═══╝ ██╔══╝  ██╔══██║╚════██║██╔══██║██║╚██╗██║   ██║
  ██║     ███████╗██║  ██║███████║██║  ██║██║ ╚████║   ██║
  ╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝
)" << RESET;

        // ── Subtitle ─────────────────────────────────────────────────────────
        std::cout << DIM << YELLOW
                  << "               ~ G o s p e l   o f   A s h ~\n"
                  << RESET;
        std::cout << DIM << RED
                  << "                          [ B E T A ]\n"
                  << RESET << "\n";

        // ── Divider ───────────────────────────────────────────────────────────
        std::cout << DIM << RED
                  << "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                  << RESET << "\n";

        // ── Menu options ──────────────────────────────────────────────────────
        std::cout << BOLD << YELLOW  << "  [ 1 ]" << RESET << "  Begin New Journey\n";
        std::cout << BOLD << YELLOW  << "  [ 2 ]" << RESET << "  How to Play\n";
        std::cout << BOLD << YELLOW  << "  [ 3 ]" << RESET << "  Discoveries"
                  << DIM << "  · " << discoveries.attacks.size() << " skills, "
                  << discoveries.runes.size() << " runes found\n" << RESET;
        std::cout << BOLD << YELLOW  << "  [ 4 ]" << RESET << "  Exit\n\n";

        std::cout << DIM << "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << RESET;
        std::cout << "\n  Choice: ";

        int c = 0;
        std::cin >> c;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if      (c == 1) startNewGame();
        else if (c == 2) showTutorial();
        else if (c == 3) showDiscoveries();
        else if (c == 4) isRunning = false;
    }
}

// ── Character creation ────────────────────────────────────────────────────────

void Game::startNewGame() {
    runState.reset();

    // ── Opening crawl ─────────────────────────────────────────────────────────
    clearScreen();
    std::cout << DIM << YELLOW
              << "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n"
              << RESET;
    std::cout << DIM << "  [Press any key to skip]\n\n" << RESET;
    bool skipped = false;
    slowPrintSkippable(
        "  The sky has been hollow since the Supreme Peasant fell.\n"
        "  He traded his life to slay the Dragon King, but in the silence\n"
        "  of his absence, Peasaterra began to rot.\n", 18, skipped);
    std::cout << "\n";
    slowPrintSkippable(
        "  A grey blight now chokes our fields, turning golden wheat into\n"
        "  weeping ash. We are starving, brittle, and broken — and the\n"
        "  Darkbloods know it. They have crossed the borders of men,\n"
        "  slaughtering families and burning what little we have left.\n"
        "  They mistake our hunger for weakness.\n", 18, skipped);
    std::cout << "\n";
    slowPrintSkippable("  They are wrong.\n", 22, skipped);
    std::cout << "\n";
    slowPrintSkippable(
        "  We have traded our plows for pikes and our scythes for war.\n"
        "  Our hands are stained with soil, but they are hardened by a\n"
        "  lifetime of toil. As the Darkblood tide approaches, we do not\n"
        "  march for glory or gold. We march so that the earth might finally\n"
        "  drink the blood of those who dared to trample it.\n", 18, skipped);
    std::cout << "\n";
    slowPrintSkippable("  The harvest is here. And this time, we are the reapers.\n", 22, skipped);
    std::cout << "\n";
    std::cout << DIM << YELLOW
              << "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
              << RESET;
    pressEnter();

    characterCreation();

    // Run chapters
    for (int ch = 1; ch <= 3 && player.isAlive(); ++ch) {
        runState.currentChapter = ch;
        showChapterIntro(ch);
        runChapter(ch);
        if (!player.isAlive()) break;
        // Town of Dead Ballads rests between chapters (not after the final chapter)
        if (ch < 3) handleTownOfDeadBallads(ch);
        if (!player.isAlive()) break;
    }

    if (player.isAlive()) showVictory(BannerQuest::isComplete(runState));
}

void Game::characterCreation() {
    clearScreen();
    std::cout << YELLOW << BOLD
              << "  ╔══════════════════════════════════════════════════════════╗\n"
              << "  ║                  A NEW WANDERER RISES                   ║\n"
              << "  ╚══════════════════════════════════════════════════════════╝\n"
              << RESET << "\n";
    std::cout << DIM << "  The ash-grey road is patient. It has waited for you.\n"
                        "  Before you walk it, it asks one thing.\n\n" << RESET;
    std::cout << "  Name your wanderer: ";
    std::string name;
    std::getline(std::cin, name);
    if (name.empty()) name = "Peasant";
    player = Player(name);

    choosePerk();

    // Record starting attack discoveries
    {
        bool changed = false;
        for (int _i = 0; _i < 3; ++_i)
            changed |= discoveries.recordAttack(player.getAttack(_i).name);
        if (changed) saveDiscoveries();
    }

    // Starting potions
    player.addPotion(Potion(PotionType::HEALING));
    player.addPotion(Potion(PotionType::STAMINA_BREW));

    std::cout << "\n" << DIM
              << "  ──────────────────────────────────────────────────────────\n"
              << RESET;
    std::cout << GREEN << "  You set out with a Healing Potion and a Stamina Brew.\n" << RESET;
    pressEnter();
}

void Game::choosePerk() {
    std::cout << "\n";
    std::cout << YELLOW << BOLD
              << "  ╔══════════════════════════════════════════════════════════╗\n"
              << "  ║             EVERY WANDERER CARRIES A PAST               ║\n"
              << "  ╚══════════════════════════════════════════════════════════╝\n"
              << RESET << "\n";

    std::cout << BOLD << YELLOW  << "  [ 1 ]" << RESET << "  Ash Walker         "
              << GREEN << "+2 stamina        " << RESET << DIM << "you know these lands\n"           << RESET;
    std::cout << BOLD << YELLOW  << "  [ 2 ]" << RESET << "  Fortune's Child    "
              << GREEN << "+80 gold          " << RESET << DIM << "lucky coin from grandma\n"        << RESET;
    std::cout << BOLD << YELLOW  << "  [ 3 ]" << RESET << "  Blood of Ancients  "
              << GREEN << "2× Healing Potion " << RESET << DIM << "old family recipe\n"              << RESET;
    std::cout << BOLD << YELLOW  << "  [ 4 ]" << RESET << "  Survivor           "
              << GREEN << "+25 max HP        " << RESET << DIM << "you have died before. Briefly.\n" << RESET;
    std::cout << BOLD << YELLOW  << "  [ 5 ]" << RESET << "  Curious Mind       "
              << GREEN << "+4 luck           " << RESET << DIM << "you ask too many questions\n"     << RESET;
    std::cout << BOLD << YELLOW  << "  [ 6 ]" << RESET << "  Cursed             "
              << RED   << "-10 gold, +6 luck " << RESET << DIM << "something follows you\n"          << RESET;

    std::cout << "\n  Your past: ";

    int c = 0;
    while (c < 1 || c > 6) {
        std::cin >> c;
        if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000,'\n'); c=0; }
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "\n";
    switch (c) {
        case 1: player.addMaxStamina(2);
                std::cout << GREEN << "  ✓ Ash Walker — +2 stamina.\n" << RESET; break;
        case 2: player.addGold(80);
                std::cout << GREEN << "  ✓ Fortune's Child — +80 gold.\n" << RESET; break;
        case 3:
            player.addPotion(Potion(PotionType::HEALING));
            player.addPotion(Potion(PotionType::HEALING));
            std::cout << GREEN << "  ✓ Blood of Ancients — +2 Healing Potions.\n" << RESET; break;
        case 4: player.addMaxHealth(25);
                std::cout << GREEN << "  ✓ Survivor — +25 max HP.\n" << RESET; break;
        case 5: player.addLuck(4);
                std::cout << GREEN << "  ✓ Curious Mind — +4 luck.\n" << RESET; break;
        case 6:
            player.spendGold(10);
            player.addLuck(6);
            std::cout << YELLOW << "  ✓ Cursed — -10 gold. +6 luck. Something shifts in the air.\n" << RESET; break;
    }
}

// ── Chapter flow ──────────────────────────────────────────────────────────────

void Game::showChapterIntro(int ch) const {
    clearScreen();
    static const char* titles[] = { "",
        "C H A P T E R   I",
        "C H A P T E R   I I",
        "C H A P T E R   I I I"
    };
    static const char* areas[] = { "",
        "THE ASHFIELDS",
        "THE HOLLOWED MARSH",
        "THE OBSIDIAN REACH"
    };
    static const char* bodies[] = { "",
        "Scorched plains stretch in every direction. The crops are dead.\n"
        "  The farmhouses are dead. Even the smoke is old.\n\n"
        "  Somewhere ahead, the Darkbloods are moving. You don't know\n"
        "  what they want. You know only that people keep pointing in\n"
        "  this direction and not coming back.",
        "The ground is wet here. Always. The fog doesn't lift — it just\n"
        "  thins, enough to see the next dead tree, the next sunken ruin,\n"
        "  the next pair of eyes watching from the water.\n\n"
        "  The Witch of G'haal holds dominion here. Ask three different\n"
        "  marsh-dwellers about her and you'll get three different stories,\n"
        "  all of them wrong.",
        "Black stone. Dragon-bone architecture. Ash storms that taste\n"
        "  like old blood.\n\n"
        "  This is where the Darkbloods live. Or lived. The distinction\n"
        "  is becoming unclear. You know the truth now — or some of it.\n"
        "  Enough to be dangerous. Enough to finish this."
    };
    std::cout << "\n";
    std::cout << DIM << YELLOW
              << "  \xe2\x95\x94\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x97\n"
              << RESET;
    std::cout << YELLOW << BOLD << "  \xe2\x95\x91  " << titles[ch];
    // pad to fixed width
    int padLen = 55 - (int)std::string(titles[ch]).size();
    for (int _p = 0; _p < padLen; ++_p) std::cout << " ";
    std::cout << "\xe2\x95\x91\n" << RESET;
    std::cout << DIM << YELLOW
              << "  \xe2\x95\xa0\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\xa3\n"
              << RESET;
    std::cout << BOLD << "  \xe2\x95\x91  " << areas[ch] << "\n" << RESET;
    std::cout << DIM << YELLOW
              << "  \xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x9d\n\n"
              << RESET;
    std::cout << "  " << DIM << bodies[ch] << RESET << "\n\n";
    pressEnter();
}

void Game::runChapter(int ch) {
    ChapterMap map(ch, (unsigned int)(std::time(nullptr) + ch * 997));

    while (player.isAlive()) {
        clearScreen();
        map.render();
        player.showStatus();

        // Banner quest status (if started)
        if (runState.bannerFound) {
            std::cout << "\n" << CYAN << BannerQuest::statusString(runState) << RESET << "\n";
        }

        if (map.atBoss()) {
            handleBossNode(ch);
            break;
        }

        std::vector<int> avail = map.getAvailableIds();
        if (avail.empty()) break;

        std::cout << "\n" << BOLD << "Choose your path:\n" << RESET;
        for (int i = 0; i < (int)avail.size(); ++i) {
            const MapNode& n = map.getNode(avail[i]);
            std::cout << "  " << (i+1) << ". " << CYAN << n.typeLabel() << RESET << "\n";
        }
        std::cout << "  s. Status   i. Inventory   q. Quests\n\n";
        std::cout << "Choice: ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "s") { player.showStatus(); pressEnter(); continue; }
        if (input == "i") { player.showInventory(); pressEnter(); continue; }
        if (input == "q") { showQuestLog(); pressEnter(); continue; }

        int choice = 0;
        try { choice = std::stoi(input); } catch (...) { continue; }
        if (choice < 1 || choice > (int)avail.size()) continue;

        int selectedId = avail[choice - 1];
        map.selectNode(selectedId);
        resolveNode(map.getNode(selectedId), ch);

        if (!player.isAlive()) { gameOver(); return; }
    }
}

void Game::resolveNode(const MapNode& node, int ch) {
    switch (node.type) {
        case NodeType::COMBAT:  handleCombatNode(ch, false); break;
        case NodeType::ELITE:   handleCombatNode(ch, true);  break;
        case NodeType::EVENT:   handleEventNode(ch);         break;
        case NodeType::SHOP:    handleShopNode();             break;
        case NodeType::REST:    handleRestNode();             break;
        case NodeType::BOSS:    /* handled by map loop atBoss() check */ break;
        default: break;
    }
}

// ── Combat node ───────────────────────────────────────────────────────────────

void Game::handleCombatNode(int ch, bool isElite) {
    // Pick a random enemy type for this chapter
    static const EnemyType ch1[] = {
        EnemyType::ASHEN_RAIDER, EnemyType::SCORCHED_BRUTE,
        EnemyType::EMBER_WITCH,  EnemyType::ASH_HOUND
    };
    static const EnemyType ch2[] = {
        EnemyType::MARSH_WRAITH, EnemyType::BOG_COLOSSUS,
        EnemyType::PLAGUE_SHAMAN, EnemyType::MIRE_LURKER
    };
    static const EnemyType ch3[] = {
        EnemyType::DARKBLOOD_SENTINEL, EnemyType::ASH_TITAN,
        EnemyType::SOUL_BINDER,        EnemyType::DARKBLOOD_ZEALOT
    };

    EnemyType chosen;
    if (ch == 1)      chosen = ch1[randInt(0, isElite ? 1 : 3)];
    else if (ch == 2) chosen = ch2[randInt(0, isElite ? 1 : 3)];
    else              chosen = ch3[randInt(0, isElite ? 1 : 3)];

    Enemy enemy(chosen, ch);
    clearScreen();

    if (isElite) {
        std::cout << RED << BOLD
                  << "  \xe2\x95\x94\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x97\n"
                  << "  \xe2\x95\x91              \xe2\x98\xa0  E L I T E   E N C O U N T E R  \xe2\x98\xa0              \xe2\x95\x91\n"
                  << "  \xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x9d\n"
                  << RESET;
    } else {
        std::cout << RED
                  << "  \xe2\x94\x8c\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x90\n"
                  << "  \xe2\x94\x82                   \xe2\x9a\xa0  E N E M Y   A H E A D  \xe2\x9a\xa0                   \xe2\x94\x82\n"
                  << "  \xe2\x94\x94\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x98\n"
                  << RESET;
    }

    std::cout << "\n  " << BOLD << (isElite ? RED : "") << enemy.getName() << RESET << "\n\n";
    std::cout << DIM << "  " << enemy.getDescription() << RESET << "\n\n";

    // Special behaviour warnings
    const EnemyBehaviour& beh = enemy.getBehaviour();
    if (beh.dodgesLightAttacks)  std::cout << MAGENTA << "  \xe2\x9a\xa0 Phases through some attacks — keep the pressure varied.\n" << RESET;
    if (beh.reflectsHeavy)       std::cout << YELLOW  << "  \xe2\x9a\xa0 Reflects heavy damage back at you — time your strikes.\n" << RESET;
    if (beh.regenHPPerTurn)      std::cout << GREEN   << "  \xe2\x9a\xa0 Regenerates HP every turn — end the fight quickly.\n" << RESET;
    if (beh.ambushesOnTurnOne)   std::cout << RED     << "  \xe2\x9a\xa0 Ambush! It attacks before you can act.\n" << RESET;
    if (beh.interruptsTimingBar) std::cout << YELLOW  << "  \xe2\x9a\xa0 Disrupts your timing — stay focused.\n" << RESET;
    if (beh.locksItemSlot)       std::cout << MAGENTA << "  \xe2\x9a\xa0 Soul binding — one of your potion slots will be locked.\n" << RESET;
    if (beh.attacksTwice)        std::cout << RED     << "  \xe2\x9a\xa0 Attacks twice per turn and applies Bleed.\n" << RESET;

    std::cout << DIM << "\n  \xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\n" << RESET;
    pressEnter();

    bool survived = runCombat(enemy);
    if (!survived) return;

    // Rewards
    int gold = enemy.getGoldReward();
    int xp   = enemy.getExperienceReward();
    runState.totalGoldEarned += gold;
    runState.enemiesDefeated++;
    player.addGold(gold);

    // Quest 1: The Debt Ledger — track kills
    if (runState.debtLedgerFound && !runState.debtQuestDone) {
        runState.debtKillCount++;
        if (runState.debtKillCount >= 5) {
            runState.debtQuestDone = true;
            player.addGold(50);
            std::cout << YELLOW << BOLD << "\n[QUEST COMPLETE] The Debt Ledger — +50 gold.\n" << RESET;
        }
    }
    player.addExperience(xp);

    std::cout << GREEN << "\nVictory! +" << gold << " gold, +" << xp << " XP.\n" << RESET;

    if (isElite) {
        // ── Elite drop: flat-bonus weapon or armor ────────────────────────────
        // Rarity scales with chapter; luck gives a chance to upgrade one tier
        ItemRarity er = (ch == 1) ? ItemRarity::COMMON :
                        (ch == 2) ? ItemRarity::RARE   : ItemRarity::EPIC;
        {
            int upgradeChance = player.getLuck() * 2; // 2% per luck point
            if (randInt(0, 99) < upgradeChance) {
                if      (er == ItemRarity::COMMON) er = ItemRarity::RARE;
                else if (er == ItemRarity::RARE)   er = ItemRarity::EPIC;
                else if (er == ItemRarity::EPIC)   er = ItemRarity::LEGENDARY;
                std::cout << YELLOW << BOLD << "[LUCKY] Elite drop rarity upgraded!\n" << RESET;
            }
        }
        ItemType   et = (randInt(0,1) == 0) ? ItemType::WEAPON : ItemType::ARMOR;
        Item drop = Item::generateFlatItem(et, er);
        std::cout << Item::getRarityColor(drop.getRarity()) << BOLD
                  << "Elite Loot: " << drop.getName() << " [" << drop.getStatsString() << "]\n"
                  << RESET;
        std::cout << "  1. Equip   2. Sell (" << drop.getSellValue() << " gold)\n";
        std::cout << "Choice: ";
        int lc = 0; std::cin >> lc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (lc == 1) {
            if (drop.getType() == ItemType::WEAPON) player.equipWeapon(drop);
            else                                     player.equipArmor(drop);
        } else if (lc == 2) {
            player.addGold(drop.getSellValue());
            std::cout << YELLOW << "Sold for " << drop.getSellValue() << " gold.\n" << RESET;
        }
    } else {
        // ── Normal enemy drop: attack type ────────────────────────────────────
        bool allowLegendary = (randInt(0, 99) < 3); // 3% legendary chance from normal
        AttackType drop = randomAttackTypeDrop(ch, allowLegendary, player.getLuck());
        if (discoveries.recordAttack(drop.name)) saveDiscoveries();
        std::cout << Item::getRarityColor(static_cast<ItemRarity>(
                         static_cast<int>(drop.rarity))) << BOLD
                  << "Skill Drop: " << drop.name << RESET
                  << " [" << drop.getStatsString() << "]\n"
                  << DIM << "  " << drop.description << "\n" << RESET;
        std::cout << "  Replace a skill slot?\n";
        for (int _i = 0; _i < 3; ++_i)
            std::cout << "  " << (_i+1) << ". Replace [" << player.getAttack(_i).name << "]\n";
        std::cout << "  0. Sell (20 gold)\n  Choice: ";
        int lc = 0; std::cin >> lc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (lc >= 1 && lc <= 3) {
            player.setAttack(lc - 1, drop);
            std::cout << GREEN << "Slot " << lc << " updated to " << drop.name << ".\n" << RESET;
        } else if (lc == 0) {
            player.addGold(20);
            std::cout << YELLOW << "Sold for 20 gold.\n" << RESET;
        }
    }
    pressEnter();
}

// ── Event node ────────────────────────────────────────────────────────────────

void Game::handleEventNode(int ch) {
    clearScreen();

    // ── Priority 0: M'alid the Rune Teacher (25% chance if runes unlearned) ───
    {
        const std::vector<RuneType> allRunes = {
            RuneType::IRON_SKIN,   RuneType::BLOODHUNGER, RuneType::STEADY_EYE,
            RuneType::GHOST_STEP,  RuneType::PREDATOR,    RuneType::DEEP_CUTS,
            RuneType::TOXIN_RIDER, RuneType::PLAGUE_WARD, RuneType::ASH_VEIL,
            RuneType::NIGHTKEEPER
        };
        std::vector<RuneType> unlearned;
        for (auto& r : allRunes)
            if (!player.hasLearnedRune(r)) unlearned.push_back(r);

        if (!unlearned.empty() && randInt(0, 3) == 0) { // 25% chance
            std::cout << YELLOW << BOLD << "=== ? ===\n" << RESET;
            slowPrint(
                "An old figure sits cross-legged at the roadside. No fire. No shelter.\n"
                "He looks up as though he expected you specifically.\n\n"
                "\"M'alid,\" he says, by way of introduction. \"Sit.\"", 14);
            std::cout << "\n" << DIM << "\"I can show you something. Choose what you wish to carry.\"\n\n" << RESET;

            std::vector<RuneType> offered;
            std::vector<RuneType> pool = unlearned;
            while (offered.size() < 3 && !pool.empty()) {
                int idx = randInt(0, (int)pool.size() - 1);
                offered.push_back(pool[idx]);
                pool.erase(pool.begin() + idx);
            }

            for (int i = 0; i < (int)offered.size(); ++i)
                std::cout << "  " << (i+1) << ". " << CYAN << runeName(offered[i]) << RESET
                          << " — " << runeDescription(offered[i]) << "\n";
            std::cout << "  0. Decline\nYour choice: ";

            int c = 0; std::string inp; std::getline(std::cin, inp);
            try { c = std::stoi(inp); } catch (...) {}
            if (c >= 1 && c <= (int)offered.size()) {
                RuneType learned = offered[c - 1];
                player.learnRune(learned);
                if (discoveries.recordRune(learned)) saveDiscoveries();
                std::cout << GREEN << "\nYou learn " << runeName(learned) << ".\n" << RESET;
                std::cout << DIM << "M'alid nods and looks back at the road.\n" << RESET;

                // Offer to slot it immediately if a slot is free
                bool slot0free = player.getRuneSlot(0) == RuneType::NONE;
                bool slot1free = player.getRuneSlot(1) == RuneType::NONE;
                if (slot0free || slot1free) {
                    int target = slot0free ? 0 : 1;
                    player.setRuneSlot(target, learned);
                    std::cout << CYAN << runeName(learned) << " slotted into rune slot "
                              << (target+1) << ".\n" << RESET;
                } else {
                    std::cout << "\nBoth rune slots are filled. Replace a slot?\n";
                    std::cout << "  1. Replace slot 1 [" << runeName(player.getRuneSlot(0)) << "]\n";
                    std::cout << "  2. Replace slot 2 [" << runeName(player.getRuneSlot(1)) << "]\n";
                    std::cout << "  0. Save for later (reassign at rest sites)\nChoice: ";
                    std::string si; std::getline(std::cin, si);
                    int sc = 0; try { sc = std::stoi(si); } catch (...) {}
                    if (sc == 1) player.setRuneSlot(0, learned);
                    else if (sc == 2) player.setRuneSlot(1, learned);
                }
            } else {
                std::cout << DIM << "You walk on.\n" << RESET;
            }
            pressEnter();
            return;
        }

        if (unlearned.empty() && randInt(0, 3) == 0) {
            std::cout << YELLOW << BOLD << "=== ? ===\n" << RESET;
            slowPrint("M'alid sits at the roadside, as always.\n\n"
                      "\"You carry everything I know,\" he says. \"Go.\"", 14);
            pressEnter();
            return;
        }
    }

    // ── Priority 1: Banner quest chain ────────────────────────────────────────
    if (ch == 1 && BannerQuest::ch1TriggerCondition(runState)) {
        std::cout << YELLOW << BOLD << "=== " << BannerQuest::ch1EventTitle() << " ===\n" << RESET;
        slowPrint(BannerQuest::ch1EventText(), 15);
        BannerQuest::ch1Complete(runState);
        pressEnter();
        return;
    }
    if (ch == 2 && BannerQuest::ch2TriggerCondition(runState)) {
        std::cout << YELLOW << BOLD << "=== " << BannerQuest::ch2EventTitle() << " ===\n" << RESET;
        slowPrint(BannerQuest::ch2EventText(), 15);
        BannerQuest::ch2Complete(runState);
        pressEnter();
        return;
    }
    if (ch == 2 && !runState.twistRevealed && runState.currentChapter == 2) {
        runState.twistRevealed = true;
        showTwistReveal();
        return;
    }
    if (ch == 3 && BannerQuest::ch3TriggerCondition(runState)) {
        std::cout << YELLOW << BOLD << "=== " << BannerQuest::ch3EventTitle() << " ===\n" << RESET;
        slowPrint(BannerQuest::ch3EventText(), 15);
        BannerQuest::ch3Complete(runState);
        pressEnter();
        return;
    }

    // ── Priority 2: Side-quest completion events ──────────────────────────────

    // Quest 2: The Third Face — Ch2 vandal reveal
    if (ch == 2 && runState.shrineQuestActive && !runState.shrineQuestDone) {
        runState.shrineQuestDone = true;
        std::cout << YELLOW << BOLD << "=== THE THIRD FACE ===\n" << RESET;
        slowPrint(
            "Near a ruined waypost you find a leather satchel half-buried in marsh mud.\n"
            "Inside: charcoal sticks, a chisel, and a brass badge — Darkblood Civil Corps.\n"
            "A clerk. Someone sent a clerk to deface the shrines.\n\n"
            "Not a soldier. Not a zealot. A man with a job and a list.\n"
            "That, somehow, is worse.\n\n"
            "You leave the satchel. You walk faster.", 14);
        player.addMaxHealth(15);
        std::cout << GREEN << "\nThe third face's warmth reaches you, belatedly. +15 max HP.\n" << RESET;
        std::cout << YELLOW << BOLD << "[QUEST COMPLETE] The Third Face\n" << RESET;
        pressEnter();
        return;
    }

    // Quest 3: Something from the East — Ch3 soul-cage shard
    if (ch == 3 && runState.easternWarningFound && !runState.easternWarningDone) {
        runState.easternWarningDone = true;
        std::cout << YELLOW << BOLD << "=== SOMETHING FROM THE EAST ===\n" << RESET;
        slowPrint(
            "In a sealed alcove, half-cemented over, you find it: a fragment of black crystal "
            "the size of a fist, cracked clean through.\n"
            "It pulses faintly. Not with light — with absence. A hole where warmth should be.\n\n"
            "You have seen this shape before. In the patrol documents. In the Darkblood "
            "courier's ciphers. They weren't moving troops east. They were moving away from "
            "wherever this came from.\n\n"
            "The Dragon King's soul-cage. Broken long before anyone admits.\n"
            "The thing from the east wasn't coming. It was already here.\n"
            "It was always here.", 14);
        player.addLuck(4);
        std::cout << GREEN << "\nYou understand the pattern now. +4 luck.\n" << RESET;
        std::cout << YELLOW << BOLD << "[QUEST COMPLETE] Something from the East\n" << RESET;
        pressEnter();
        return;
    }

    // Quest 4: The Deserter's Cipher — Ch3 key decode
    if (ch == 3 && runState.deserterMet && !runState.cipherDecoded) {
        runState.cipherDecoded = true;
        std::cout << YELLOW << BOLD << "=== THE DESERTER'S CIPHER ===\n" << RESET;
        slowPrint(
            "A dead courier. A second satchel. Inside: the one-time pad for the deserter's "
            "cipher — a grid of letters on oilskin, still dry.\n\n"
            "You sit down and decode it.\n\n"
            "It takes ten minutes. The message is four words:\n\n"
            "HIT LEFT. ALWAYS LEFT.\n\n"
            "The Darkblood Lord has a dead right side. Old wound. He always pivots left "
            "to compensate — leaves his right flank open for exactly one strike.\n"
            "The deserter knew. He was trying to warn someone.\n\n"
            "Now you know.", 14);
        if (player.getWeapon().getModifier() < 10)
            player.getWeapon().addModifier(2);
        std::cout << GREEN << "\nYou adjust your grip. Weapon modifier +2.\n" << RESET;
        std::cout << YELLOW << BOLD << "[QUEST COMPLETE] The Deserter's Cipher\n" << RESET;
        pressEnter();
        return;
    }

    // ── Generic events (5 per chapter, picked randomly) ───────────────────────

    static const char* ch1Events[] = {
        // idx 0 — Notice board (+15 gold)
        "A half-burned notice board. One poster still readable: REWARD — any information "
        "on Darkblood movements. Contact Lord Something-Or-Other. The lord, presumably, "
        "is also ash now.\nYou find 15 gold coins tucked behind the board. Finders keepers.",

        // idx 1 — Empty cart (+stamina brew)
        "A farmer with a cart full of nothing. He looks at you with the eyes of someone "
        "who has run out of things to be afraid of.\n\"The Darkbloods passed through three "
        "days ago,\" he says. \"They didn't burn anything. They just... watched. That was worse.\"\n"
        "He gives you a Stamina Brew and drives the empty cart away.",

        // idx 2 — Shrine (Quest 2 start)
        "A shrine. Not to any god you recognise — just a flat stone with three carved "
        "faces. Two of them have been defaced. The third is still intact: a peasant with "
        "a crook. You leave a coin. The stone feels warmer than it should.\n"
        "Someone did this deliberately. Recently. The chisel marks are clean.",

        // idx 3 — Debt Ledger (Quest 1 start)
        "A dead merchant, seated upright against a milestone as though still conducting "
        "business. In his lap: a leather ledger. Five names. Five debts. Amounts that "
        "would mean something, once.\n"
        "The debtors are probably Darkblood soldiers now — or ash. Either way, the debt "
        "doesn't care.\nYou take the ledger. Old habit.",

        // idx 4 — Acid Road (NEGATIVE — weapon modifier -1)
        "The road surface here is wrong — slicked with something that smells like vinegar "
        "and old iron. You cross it quickly, but not quickly enough.\n"
        "The edge of your blade softens. Not visibly — but you feel it on the next test "
        "swing. Something in that film ate into the metal.",
    };

    static const char* ch2Events[] = {
        // idx 0 — Sunken boat (+healing)
        "A boat, half-submerged, with a locked chest in the bow. You pry it open.\n"
        "Inside: a Healing Potion and a note that reads 'Don't trust the fog. The fog "
        "knows.' Very helpful. Thanks.",

        // idx 1 — Confused wisp (+shield tonic)
        "A will-o'-wisp leads you in circles for twenty minutes before you realise it "
        "is not hostile — it is confused. When you stop following it, it drifts away, "
        "looking embarrassed. You find a Shield Tonic where it was hovering.",

        // idx 2 — Eastern warning (Quest 3 start)
        "An old fishing net, strung between two dead trees. Caught in it: a Darkblood "
        "document, waterlogged but legible. Tide schedules. Patrol rotations. "
        "Evidence that the Darkbloods are afraid of something coming from the east.\n"
        "The fear in the handwriting is not professional. It's personal.\n"
        "You keep it. It might mean something later.",

        // idx 3 — Deserter (Quest 4 start)
        "A shape drops from a willow tree and lands in front of you — Darkblood armour, "
        "no insignia, no weapon.\n\"I'm not fighting,\" he says immediately. \"I'm done.\"\n"
        "He presses a folded oilskin into your hands — a cipher, he says. Something he "
        "copied before he ran. \"Find the one-time pad,\" he says. \"It's worth finding.\"\n"
        "Then he's gone. Into the fog. Gone.",

        // idx 4 — Marsh Rot (NEGATIVE — armor modifier -1)
        "The path dips below the waterline for thirty feet. You wade through it.\n"
        "The water is the colour of old teeth and smells like things that stopped moving "
        "a long time ago. By the time you're out, the clasps on your armour are already "
        "pitting. The straps have softened. It'll hold — but it'll hold less well.",
    };

    static const char* ch3Events[] = {
        // idx 0 — Dead soldier (+rage draught)
        "A Darkblood soldier, propped against a wall, very much not alive. His journal "
        "is still clutched in one hand. The last entry: 'The Lord knows. He has always "
        "known. We are not containment. We are the warning that nobody listened to.'\n"
        "You take his Rage Draught. He doesn't need it.",

        // idx 1 — Cracked mirror (+healing)
        "A chamber with a cracked obsidian mirror. Your reflection is wrong — it is "
        "standing when you crouch, looking at something behind you. When you turn, "
        "nothing is there. When you look back, the reflection is normal.\n"
        "There is a Healing Potion on the floor in front of the mirror. You take it quickly.",

        // idx 2 — STILL HERE (no reward)
        "The chains on the wall are old. The scratches in the stone beside them are newer. "
        "Hundreds of them. All the same two words, repeated: STILL HERE. STILL HERE.\n"
        "You add nothing. You leave quickly.",

        // idx 3 — Architecture plans (Quest 5 instant-complete, +30g)
        "A planning chamber. Rolls of vellum in iron tubes, sealed against the damp.\n"
        "You unroll the first one. A floor plan. The entire Obsidian Reach, annotated in "
        "three different hands. The outer walls are marked FACADE. The inner corridors "
        "are labelled FUNNEL. The throne room at the centre reads, in red ink: KILL ZONE.\n\n"
        "This was never a fortress. It was always a trap.\n"
        "Not for enemies. For whatever was supposed to come through.\n"
        "For the wanderer.\n\n"
        "For you.\n\n"
        "You roll the plans back up and take them anyway. Worth 30 gold to the right "
        "historian — if any historians survive.",

        // idx 4 — Blood Tithe (NEGATIVE — -10 max HP)
        "A threshold. Old stone, carved with a glyph you don't recognise.\n"
        "You step over it.\n"
        "Something steps out of you.\n\n"
        "Not much — just a sliver. A fraction. But the Reach takes its toll on everyone "
        "who enters. You feel it in the way your lungs fill: slightly less than before.",
    };

    const char** events = (ch == 1) ? ch1Events : (ch == 2) ? ch2Events : ch3Events;
    int idx = randInt(0, 4);

    std::cout << YELLOW << BOLD << "=== EVENT ===\n" << RESET;
    slowPrint(events[idx], 14);

    // ── Rewards and consequences ──────────────────────────────────────────────
    if (ch == 1) {
        if (idx == 0) {
            player.addGold(15);
            std::cout << GREEN << "\n+15 gold.\n" << RESET;
        } else if (idx == 1) {
            player.addPotion(Potion(PotionType::STAMINA_BREW));
            std::cout << GREEN << "\n+Stamina Brew.\n" << RESET;
        } else if (idx == 2) {
            // Quest 2 start
            if (!runState.shrineQuestActive) {
                runState.shrineQuestActive = true;
                std::cout << YELLOW << "\n[QUEST STARTED] The Third Face — find the vandal in Chapter 2.\n" << RESET;
            }
        } else if (idx == 3) {
            // Quest 1 start
            if (!runState.debtLedgerFound) {
                runState.debtLedgerFound = true;
                std::cout << YELLOW << "\n[QUEST STARTED] The Debt Ledger — kill 5 enemies (" << runState.debtKillCount << "/5).\n" << RESET;
            }
        } else if (idx == 4) {
            // Acid road — weapon modifier -1 (min 0), else -10 gold
            if (player.getWeapon().getModifier() > 0) {
                player.getWeapon().addModifier(-1);
                std::cout << RED << "\nWeapon modifier reduced by 1. [" << player.getWeapon().getStatsString() << "]\n" << RESET;
            } else {
                player.addGold(-10);
                std::cout << RED << "\nNo modifier to lose — the acid eats your coin purse instead. -10 gold.\n" << RESET;
            }
        }
    }

    if (ch == 2) {
        if (idx == 0) {
            player.addPotion(Potion(PotionType::HEALING));
            std::cout << GREEN << "\n+Healing Potion.\n" << RESET;
        } else if (idx == 1) {
            player.addPotion(Potion(PotionType::SHIELD_TONIC));
            std::cout << GREEN << "\n+Shield Tonic.\n" << RESET;
        } else if (idx == 2) {
            // Quest 3 start
            if (!runState.easternWarningFound) {
                runState.easternWarningFound = true;
                std::cout << YELLOW << "\n[QUEST STARTED] Something from the East — follow the trail in Chapter 3.\n" << RESET;
            }
        } else if (idx == 3) {
            // Quest 4 start
            if (!runState.deserterMet) {
                runState.deserterMet = true;
                std::cout << YELLOW << "\n[QUEST STARTED] The Deserter's Cipher — find the one-time pad in Chapter 3.\n" << RESET;
            }
        } else if (idx == 4) {
            // Marsh rot — armor modifier -1 (min 0), else -10 gold
            if (player.getArmor().getModifier() > 0) {
                player.getArmor().addModifier(-1);
                std::cout << RED << "\nArmour corroded. Modifier reduced by 1. [" << player.getArmor().getStatsString() << "]\n" << RESET;
            } else {
                player.addGold(-10);
                std::cout << RED << "\nArmour already bare — the marsh takes your gold instead. -10 gold.\n" << RESET;
            }
        }
    }

    if (ch == 3) {
        if (idx == 0) {
            player.addPotion(Potion(PotionType::RAGE_DRAUGHT));
            std::cout << GREEN << "\n+Rage Draught.\n" << RESET;
        } else if (idx == 1) {
            player.addPotion(Potion(PotionType::HEALING));
            std::cout << GREEN << "\n+Healing Potion.\n" << RESET;
        } else if (idx == 2) {
            // No reward — pure dread
        } else if (idx == 3) {
            // Quest 5 instant-complete
            if (!runState.architectureFound) {
                runState.architectureFound = true;
                player.addGold(30);
                std::cout << GREEN << "\n+30 gold.\n" << RESET;
                std::cout << YELLOW << BOLD << "[QUEST COMPLETE] What They Built\n" << RESET;
            }
        } else if (idx == 4) {
            // Blood tithe — -10 max HP
            player.addMaxHealth(-10);
            std::cout << RED << "\nThe Reach takes its toll. -10 max HP.\n" << RESET;
        }
    }

    pressEnter();
}

// ── Town of Dead Ballads ──────────────────────────────────────────────────────

void Game::handleTownOfDeadBallads(int afterChapter) {
    clearScreen();
    std::cout << YELLOW << BOLD;
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║           THE TOWN OF DEAD BALLADS                      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    std::cout << RESET;

    if (afterChapter == 1) {
        slowPrint(
            "\nThe town sits at the edge of the Ashfields like something the fire forgot to take.\n"
            "Half the buildings are empty. The other half hold people who stopped moving.\n"
            "They look at you the way survivors look at anyone still walking east — "
            "with recognition, and with pity.", 13);
    } else {
        slowPrint(
            "\nThe town is wetter here, closer to the marsh-line. The wood has gone grey.\n"
            "The people have too. They speak quietly, if at all, and they don't ask "
            "where you've been. They already know.\n"
            "What they want to know is whether you're coming back.", 13);
    }

    pressEnter();

    bool inTown = true;
    while (inTown) {
        clearScreen();
        std::cout << YELLOW << BOLD
                  << "  ╔══════════════════════════════════════════════════════════╗\n"
                  << "  ║              THE  TOWN  OF  DEAD  BALLADS               ║\n"
                  << "  ╚══════════════════════════════════════════════════════════╝\n"
                  << RESET << "\n";
        std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
        std::cout << YELLOW << "  Gold: " << player.getGold() << RESET << "\n\n";
        std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Skill Exchange\n";
        std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  The Wanderer's Curio\n";
        std::cout << BOLD << YELLOW << "  [ 3 ]" << RESET << "  The Crooked Spit\n";
        std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  Leave town\n";
        std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
        std::cout << "\n  Choice: ";

        int c = 0; std::cin >> c;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (c) {
            case 1: {
                // ── Skill Exchange ────────────────────────────────────────────
                clearScreen();
                std::cout << YELLOW << BOLD
                          << "  ╔══════════════════════════════════════════════════════════╗\n"
                          << "  ║                    SKILL  EXCHANGE                      ║\n"
                          << "  ╚══════════════════════════════════════════════════════════╝\n"
                          << RESET << "\n";
                slowPrint(
                    "  An old trader sits with a sealed case. She doesn't open it until you decide.\n"
                    "  \"What you know is what you are,\" she says.\n"
                    "  \"But what you could know — that's something else entirely.\"", 13);
                std::cout << "\n";

                // Show current skills
                std::cout << BOLD << "Current skills:\n" << RESET;
                for (int _i = 0; _i < 3; ++_i)
                    std::cout << "  [" << (_i+1) << "] " << player.getAttack(_i).name
                              << " [" << player.getAttack(_i).getStatsString() << "]\n";

                std::cout << DIM << "\n  ──────────────────────────────────────────────────────────\n" << RESET;
                std::cout << YELLOW << "  Gold: " << player.getGold() << RESET << "\n\n";
                std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Common skill   " << DIM << "40g\n"  << RESET;
                std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  Rare skill     " << DIM << "80g\n"  << RESET;
                std::cout << BOLD << YELLOW << "  [ 3 ]" << RESET << "  Epic skill     " << DIM << "150g\n" << RESET;
                // Legendary only available 25% of the time
                bool legendaryAvail = (randInt(0, 3) == 0);
                if (legendaryAvail)
                    std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << Item::getRarityColor(ItemRarity::LEGENDARY)
                              << "  Legendary skill " << RESET << DIM << "300g\n" << RESET;
                std::cout << BOLD << YELLOW << "  [ 0 ]" << RESET << "  Leave\n";
                std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
                std::cout << "\n  Choice: ";

                int bc = 0; std::cin >> bc;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                int costs[] = { 0, 40, 80, 150, 300 };
                ItemRarity rarities[] = {
                    ItemRarity::COMMON, ItemRarity::COMMON, ItemRarity::RARE,
                    ItemRarity::EPIC, ItemRarity::LEGENDARY
                };
                if (bc >= 1 && bc <= 4 && !(bc == 4 && !legendaryAvail)) {
                    int cost = costs[bc];
                    if (player.spendGold(cost)) {
                        AttackType drawn = randomAttackType(rarities[bc]);
                        std::cout << "\n" << Item::getRarityColor(rarities[bc]) << BOLD
                                  << "Drawn: " << drawn.name << RESET
                                  << " [" << drawn.getStatsString() << "]\n"
                                  << DIM << "  " << drawn.description << "\n" << RESET;
                        std::cout << "\n  Replace a slot? (or 0 to cancel and keep gold)\n";
                        for (int _i = 0; _i < 3; ++_i)
                            std::cout << "  " << (_i+1) << ". Replace [" << player.getAttack(_i).name << "]\n";
                        std::cout << "  0. Cancel (gold already spent)\n  Choice: ";
                        int sc = 0; std::cin >> sc;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        if (sc >= 1 && sc <= 3) {
                            player.setAttack(sc - 1, drawn);
                            std::cout << GREEN << "Slot " << sc << " updated.\n" << RESET;
                        }
                    } else {
                        std::cout << RED << "Not enough gold.\n" << RESET;
                    }
                    pressEnter();
                }
                break;
            }

            case 2: {
                // ── The Wanderer's Curio ──────────────────────────────────────
                struct PotionOffer { PotionType type; int cost; const char* desc; };
                static const PotionOffer offers[] = {
                    { PotionType::PHANTOMS_DRAUGHT,  80,  "Next attack bypasses enemy defense"    },
                    { PotionType::GRAVEWARDENS_OIL,  70,  "Next 3 attacks proc Bleed on hit"      },
                    { PotionType::DEAD_MANS_COURAGE, 60,  "Clear status effects + restore stamina"},
                    { PotionType::ASH_MEMORY,        100, "Permanently +15 max HP"                },
                };
                bool purchased[4] = { false, false, false, false };

                bool inCurio = true;
                while (inCurio) {
                    clearScreen();
                    std::cout << YELLOW << BOLD
                              << "  ╔══════════════════════════════════════════════════════════╗\n"
                              << "  ║                  THE WANDERER'S CURIO                   ║\n"
                              << "  ╚══════════════════════════════════════════════════════════╝\n"
                              << RESET;
                    std::cout << DIM
                              << "\n  A man with a cart full of things that shouldn't exist.\n"
                                 "  He smiles like someone who knows the price of everything\n"
                                 "  and has already decided you'll pay it.\n\n" << RESET;

                    std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
                    std::cout << YELLOW << "  Gold: " << player.getGold() << RESET << "\n\n";

                    for (int i = 0; i < 4; ++i) {
                        if (purchased[i]) {
                            std::cout << DIM << "  [ " << (i+1) << " ]  "
                                      << Potion::typeName(offers[i].type)
                                      << "  — SOLD\n" << RESET;
                        } else {
                            std::cout << BOLD << YELLOW << "  [ " << (i+1) << " ]" << RESET
                                      << "  " << Potion::typeName(offers[i].type)
                                      << "  " << DIM << offers[i].cost << "g  ·  " << offers[i].desc << "\n" << RESET;
                        }
                    }
                    std::cout << BOLD << YELLOW << "  [ 5 ]" << RESET << "  Leave\n";
                    std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
                    std::cout << "\n  Choice: ";

                    int cc = 0; std::cin >> cc;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if (cc >= 1 && cc <= 4) {
                        int idx = cc - 1;
                        if (purchased[idx]) {
                            std::cout << DIM << "  He shakes his head. \"Sold one already. One per visit.\"\n" << RESET;
                        } else if (player.spendGold(offers[idx].cost)) {
                            player.addPotion(Potion(offers[idx].type));
                            purchased[idx] = true;
                            std::cout << GREEN << "  Purchased: " << Potion::typeName(offers[idx].type) << "\n" << RESET;
                            std::cout << DIM << "  \"Wise,\" he says. He doesn't specify what for.\n" << RESET;
                        } else {
                            std::cout << RED << "  Not enough gold.\n" << RESET;
                        }
                        pressEnter();
                    } else if (cc == 5) {
                        inCurio = false;
                    }
                }
                break;
            }

            case 3: {
                // ── The Crooked Spit ─────────────────────────────────────────
                clearScreen();
                std::cout << YELLOW << BOLD << "=== THE CROOKED SPIT ===\n" << RESET;

                if (afterChapter == 1) {
                    slowPrint(
                        "Three strangers share a table by the fire. None of them look at you\n"
                        "when you walk in. All of them stop talking.", 13);
                } else {
                    slowPrint(
                        "The tavern smells of marsh reed and old smoke.\n"
                        "The three at the corner table have the look of people who have\n"
                        "been waiting — not for you specifically, but for whoever came through.", 13);
                }
                std::cout << "\n";

                bool inTavern = true;
                while (inTavern) {
                    if (afterChapter == 1) {
                        std::cout << "  1. The fleeing farmer\n";
                        std::cout << "  2. The wounded old soldier\n";
                        std::cout << "  3. The girl selling water\n";
                    } else {
                        std::cout << "  1. The marsh trader\n";
                        std::cout << "  2. The man in stripped Darkblood armour\n";
                        std::cout << "  3. The blind innkeeper\n";
                    }
                    std::cout << "  4. Leave\n\n";
                    std::cout << "Choice: ";

                    int tc = 0; std::cin >> tc;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if (tc == 4) { inTavern = false; break; }

                    clearScreen();

                    if (afterChapter == 1) {
                        if (tc == 1) {
                            std::cout << CYAN << BOLD << "The fleeing farmer\n" << RESET;
                            slowPrint(
                                "He keeps his hands wrapped around his cup like it's the only warm thing left.\n\n"
                                "\"The Lieutenant wasn't giving orders,\" he says, without looking up.\n"
                                "\"He was following a calendar. They had a date for something. I saw the\n"
                                "schedule in the barracks before they burned it. Columns of dates, all\n"
                                "the way to the end of the year.\"\n\n"
                                "He finally looks at you.\n\n"
                                "\"The last date was circled. I couldn't read the word next to it.\n"
                                "It was in a language I didn't recognise.\"", 13);
                        } else if (tc == 2) {
                            std::cout << CYAN << BOLD << "The wounded old soldier\n" << RESET;
                            slowPrint(
                                "He has the kind of wound that was supposed to be fatal and didn't get the message.\n\n"
                                "\"Three champions walked this road,\" he says. \"Everybody knows that part.\n"
                                "Nobody talks about how they came back.\"\n\n"
                                "\"Not together. Not triumphant. One at a time, over years — pieces\n"
                                "of them attached to rumours. A piece of armour. A story. A name\n"
                                "that shouldn't be spoken in the Reach.\"\n\n"
                                "\"The last piece was a banner. Ash-grey and deep red.\n"
                                "If you've seen it, you already know what it means.\n"
                                "If you haven't — keep your eyes open at the ruins.\"", 13);
                        } else if (tc == 3) {
                            std::cout << CYAN << BOLD << "The girl selling water\n" << RESET;
                            slowPrint(
                                "She can't be older than twelve. She has the eyes of someone\n"
                                "who has been listening to adults for years and filing everything away.\n\n"
                                "\"My father said the Darkbloods aren't from the Reach,\" she says,\n"
                                "matter-of-factly. \"He said they came from somewhere older.\n"
                                "A name that nobody uses anymore.\"\n\n"
                                "\"He wouldn't tell me the name. He said knowing it costs something.\"\n\n"
                                "She holds out a cup of water. You take it.\n\n"
                                "\"He said the cost isn't pain,\" she adds. \"He said the cost is that\n"
                                "you can't go back to not knowing. And then he left and didn't come back,\n"
                                "so I think he must have said it to someone.\"", 13);
                        }
                    } else {
                        if (tc == 1) {
                            std::cout << CYAN << BOLD << "The marsh trader\n" << RESET;
                            slowPrint(
                                "She's been running the marsh route for twenty years. She smells like\n"
                                "reed water and knows things by osmosis.\n\n"
                                "\"The Witch was afraid of the Darkblood Lord,\" she says, leaning forward.\n"
                                "\"That's why she was in the marsh. It's the one place he wouldn't\n"
                                "send soldiers — the rot kills them. Something about the marsh water\n"
                                "disagrees with whatever they've done to themselves.\"\n\n"
                                "\"She wasn't hiding from you. She was hiding from him.\n"
                                "You just happened to find her first.\"\n\n"
                                "She goes back to her drink. Conversation over.", 13);
                        } else if (tc == 2) {
                            std::cout << CYAN << BOLD << "The man in stripped Darkblood armour\n" << RESET;
                            slowPrint(
                                "He's removed the insignia. Roughly — you can see the tool marks.\n\n"
                                "\"The Lord hasn't left the Reach in forty years,\" he says quietly.\n"
                                "\"At first we thought he was guarding it. Protecting the outside\n"
                                "from whatever was inside.\"\n\n"
                                "He turns his cup over. Empty.\n\n"
                                "\"Then we started wondering if it was the other way around.\n"
                                "If the thing inside couldn't get out while he was there.\n"
                                "If he wasn't guarding the world from the Reach.\"\n\n"
                                "He looks at you steadily.\n\n"
                                "\"If he was guarding the Reach from whatever might walk into it\n"
                                "and reach the centre. From someone like you.\"", 13);
                        } else if (tc == 3) {
                            std::cout << CYAN << BOLD << "The blind innkeeper\n" << RESET;
                            slowPrint(
                                "She's been running this place long enough that she navigates it\n"
                                "by memory and sound. She sets a cup in front of you without being asked.\n\n"
                                "\"I've heard the name Slave King Amax three times in my life,\" she says.\n"
                                "\"Once from a soldier who came back from the Reach wrong.\n"
                                "Once from a priest who wouldn't explain it.\n"
                                "Once from a wanderer who was asking about a banner.\"\n\n"
                                "She tilts her head.\n\n"
                                "\"Are you the one with the banner?\"\n\n", 13);
                            if (runState.bannerFound) {
                                std::cout << GREEN;
                                slowPrint(
                                    "You tell her yes.\n\n"
                                    "She's quiet for a moment. Then: \"Good. Then you already know\n"
                                    "more than the others did. The name isn't a warning.\n"
                                    "It's a door. And the banner is the key.\"\n\n"
                                    "She doesn't say anything else.", 13);
                                std::cout << RESET;
                            } else {
                                std::cout << DIM;
                                slowPrint(
                                    "You tell her no.\n\n"
                                    "\"Then keep your eyes open,\" she says. \"In the ruins.\n"
                                    "Ash-grey and deep red. You'll know it when you see it.\n"
                                    "Pick it up. The road doesn't end where you think it does.\"", 13);
                                std::cout << RESET;
                            }
                        }
                    }

                    pressEnter();
                    clearScreen();
                    if (afterChapter == 1) {
                        std::cout << YELLOW << BOLD << "=== THE CROOKED SPIT ===\n\n" << RESET;
                    } else {
                        std::cout << YELLOW << BOLD << "=== THE CROOKED SPIT ===\n\n" << RESET;
                    }
                } // end inTavern
                break;
            }

            case 4:
                inTown = false;
                clearScreen();
                if (afterChapter == 1) {
                    slowPrint("You leave the town behind. The road east is colder than you remember.", 13);
                } else {
                    slowPrint("You leave. The town watches you go. Nobody waves.", 13);
                }
                pressEnter();
                break;

            default: break;
        }
    }
}

// ── Shop node ─────────────────────────────────────────────────────────────────

void Game::handleShopNode() {
    clearScreen();
    std::cout << YELLOW << BOLD
              << "  ╔══════════════════════════════════════════════════════════╗\n"
              << "  ║                  SHOP  &  BLACKSMITH                    ║\n"
              << "  ╚══════════════════════════════════════════════════════════╝\n"
              << RESET;
    std::cout << DIM << "\n  A merchant leans against a scorched cart. Next to him, a blacksmith\n"
                        "  hammers at something that was definitely once a weapon.\n\n" << RESET;

    bool inShop = true;
    while (inShop) {
        std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
        std::cout << YELLOW << "  Gold: " << player.getGold() << RESET << "\n\n";
        std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Healing Potion      " << DIM << "30g\n"         << RESET;
        std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  Stamina Brew        " << DIM << "25g\n"         << RESET;
        std::cout << BOLD << YELLOW << "  [ 3 ]" << RESET << "  Shield Tonic        " << DIM << "35g\n"         << RESET;
        std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  Rage Draught        " << DIM << "40g\n"         << RESET;
        std::cout << BOLD << YELLOW << "  [ 5 ]" << RESET << "  Antidote            " << DIM << "20g\n"         << RESET;
        std::cout << BOLD << YELLOW << "  [ 6 ]" << RESET << "  Upgrade Weapon      " << DIM << "75 / 150 / 250g\n" << RESET;
        std::cout << BOLD << YELLOW << "  [ 7 ]" << RESET << "  Upgrade Armour      " << DIM << "75 / 150 / 250g\n" << RESET;
        std::cout << BOLD << YELLOW << "  [ 8 ]" << RESET << "  Leave\n";
        std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
        std::cout << "\n  Choice: ";

        int c = 0; std::cin >> c;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        auto buyPotion = [&](PotionType pt, int cost) {
            if (player.spendGold(cost)) {
                player.addPotion(Potion(pt));
                std::cout << GREEN << "Purchased! " << Potion::typeName(pt) << "\n" << RESET;
            } else {
                std::cout << RED << "Not enough gold.\n" << RESET;
            }
        };

        auto upgradeItem = [&](Item& item, const std::string& label) {
            static const int costs[] = { 75, 150, 250 };
            int lv = item.getUpgradeLevel();
            if (lv >= 3) { std::cout << YELLOW << label << " is already max tier.\n" << RESET; return; }
            int cost = costs[lv];
            std::cout << label << " upgrade costs " << cost << "g. Proceed? (y/n): ";
            char yn; std::cin >> yn;
            std::cin.ignore(10000, '\n');
            if ((yn == 'y' || yn == 'Y') && player.spendGold(cost)) {
                item.upgrade();
                std::cout << GREEN << label << " upgraded! " << item.getStatsString() << "\n" << RESET;
            } else if (yn != 'y' && yn != 'Y') {
                std::cout << "Cancelled.\n";
            } else {
                std::cout << RED << "Not enough gold.\n" << RESET;
            }
        };

        switch (c) {
            case 1: buyPotion(PotionType::HEALING,      30); break;
            case 2: buyPotion(PotionType::STAMINA_BREW, 25); break;
            case 3: buyPotion(PotionType::SHIELD_TONIC, 35); break;
            case 4: buyPotion(PotionType::RAGE_DRAUGHT, 40); break;
            case 5: buyPotion(PotionType::ANTIDOTE,     20); break;
            case 6: upgradeItem(player.getWeapon(), "Weapon"); break;
            case 7: upgradeItem(player.getArmor(),  "Armour"); break;
            case 8: inShop = false; break;
            default: break;
        }
    }
}

// ── Rest node ─────────────────────────────────────────────────────────────────

void Game::handleRestNode() {
    clearScreen();
    std::cout << YELLOW << BOLD
              << "  ╔══════════════════════════════════════════════════════════╗\n"
              << "  ║                      REST  SITE                         ║\n"
              << "  ╚══════════════════════════════════════════════════════════╝\n"
              << RESET << "\n";

    static const char* flavour[] = {
        "A hollow tree, wide enough to sit inside. The bark smells like old rain.",
        "The remains of a campfire, still warm. Someone was here recently.",
        "A flat rock beside a thin stream. The water tastes of iron, but it's cold.",
        "An overturned cart. You prop up one side and sleep underneath it.",
    };
    std::cout << flavour[randInt(0, 3)] << "\n\n";

    std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Rest  — +" << (player.getMaxHealth() / 4)
              << " HP, full stamina, clear status\n";
    std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  Reassign runes\n";
    std::cout << "\n  Choice: ";
    int rc = 0; std::cin >> rc;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (rc == 2 && !player.getLearnedRunes().empty()) {
        std::cout << "\n  Learned runes:\n";
        const auto& lr = player.getLearnedRunes();
        for (int i = 0; i < (int)lr.size(); ++i)
            std::cout << "  " << (i+1) << ". " << CYAN << runeName(lr[i]) << RESET
                      << " — " << runeDescription(lr[i]) << "\n";
        std::cout << "  0. None (empty slot)\n";

        std::cout << "\n  Slot 1 [" << runeName(player.getRuneSlot(0)) << "]: ";
        std::string s1; std::getline(std::cin, s1);
        int p1 = 0; try { p1 = std::stoi(s1); } catch (...) {}
        if (p1 >= 0 && p1 <= (int)lr.size()) {
            RuneType chosen = (p1 == 0) ? RuneType::NONE : lr[p1-1];
            player.setRuneSlot(0, chosen);
        }

        std::cout << "  Slot 2 [" << runeName(player.getRuneSlot(1)) << "]: ";
        std::string s2; std::getline(std::cin, s2);
        int p2 = 0; try { p2 = std::stoi(s2); } catch (...) {}
        if (p2 >= 0 && p2 <= (int)lr.size()) {
            RuneType chosen = (p2 == 0) ? RuneType::NONE : lr[p2-1];
            player.setRuneSlot(1, chosen);
        }
        std::cout << CYAN << "\nRunes updated: ["
                  << runeName(player.getRuneSlot(0)) << "] ["
                  << runeName(player.getRuneSlot(1)) << "]\n" << RESET;
    } else {
        int healAmt = player.getMaxHealth() / 4;
        player.heal(healAmt);
        player.restoreStamina(player.getMaxStamina());
        player.getBuffs().bleeding     = false;
        player.getBuffs().poisonStacks = 0;
        player.getBuffs().blindRounds  = 0;
        std::cout << GREEN << "You rest. +" << healAmt << " HP. Full stamina restored. All status effects cleared.\n" << RESET;
        std::cout << "Current HP: " << player.getHealth() << "/" << player.getMaxHealth() << "\n";
    }
    pressEnter();
}

// ── Boss node ─────────────────────────────────────────────────────────────────

void Game::handleBossNode(int ch) {
    BossType bType;
    if      (ch == 1) bType = BossType::DARKBLOOD_LIEUTENANT;
    else if (ch == 2) bType = BossType::WITCH_OF_GHAAL;
    else              bType = BossType::DARKBLOOD_LORD;

    clearScreen();
    Boss boss(bType, ch);
    std::cout << RED << BOLD
              << "  ╔══════════════════════════════════════════════════════════╗\n"
              << "  ║                   ☠  CHAPTER BOSS  ☠                   ║\n"
              << "  ╚══════════════════════════════════════════════════════════╝\n"
              << RESET << "\n";
    std::cout << "  " << RED << BOLD << boss.getName() << RESET << "\n\n";
    slowPrint(boss.getIntroText(), 16);
    pressEnter();

    bool survived = runBossFight(boss);
    if (!survived) return;

    // After ch2 boss: witch reveals truth
    if (ch == 2) {
        clearScreen();
        std::cout << YELLOW << BOLD << "=== REVELATION ===\n\n" << RESET;
        slowPrint(boss.getDefeatText(), 16);
        runState.witchDefeated = true;
        pressEnter();
    } else {
        std::cout << "\n";
        slowPrint(boss.getDefeatText(), 14);
        pressEnter();
    }

    // Rewards
    player.addGold(boss.getGoldReward());
    player.addExperience(boss.getExperienceReward());
    std::cout << GREEN << "\n+" << boss.getGoldReward() << " gold. +" << boss.getExperienceReward() << " XP.\n" << RESET;

    // Legendary skill drop from boss
    {
        AttackType legendDrop = randomAttackType(ItemRarity::LEGENDARY);
        if (discoveries.recordAttack(legendDrop.name)) saveDiscoveries();
        std::cout << "\n" << Item::getRarityColor(ItemRarity::LEGENDARY) << BOLD
                  << "★ LEGENDARY SKILL: " << legendDrop.name << RESET
                  << " [" << legendDrop.getStatsString() << "]\n"
                  << DIM << "  " << legendDrop.description << "\n" << RESET;
        std::cout << "  Replace a skill slot?\n";
        for (int _i = 0; _i < 3; ++_i)
            std::cout << "  " << (_i+1) << ". Replace [" << player.getAttack(_i).name << "]\n";
        std::cout << "  0. Pass\n  Choice: ";
        int lc = 0; std::cin >> lc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (lc >= 1 && lc <= 3)
            player.setAttack(lc - 1, legendDrop);
    }

    // Mark chapter defeated
    if      (ch == 1) runState.ch1BossDefeated = true;
    else if (ch == 2) runState.ch2BossDefeated = true;
    else {
        runState.ch3BossDefeated = true;
        // True ending check
        if (BannerQuest::isComplete(runState)) {
            pressEnter();
            clearScreen();
            std::cout << YELLOW << BOLD << "\n=== THE BANNER GLOWS ===\n\n" << RESET;
            slowPrint(
                "The three champion-spirits appear behind you. Not triumphant — just present.\n"
                "The tallest one points past the fallen Darkblood Lord, into the dark beyond.\n"
                "One name comes into your mind, placed there by the blood-curse like a key "
                "into a lock.\n\nSlave King Amax.\n\nYou walk forward.", 16);
            pressEnter();

            // True final boss
            clearScreen();
            std::cout << RED << BOLD << "=== TRUE FINAL BOSS ===\n" << RESET;
            Boss amax(BossType::SLAVE_KING_AMAX, 3);
            std::cout << BOLD << amax.getName() << RESET << "\n\n";
            slowPrint(amax.getIntroText(), 16);
            pressEnter();

            bool amaxSurvived = runBossFight(amax);
            if (!amaxSurvived) return;

            clearScreen();
            std::cout << YELLOW << BOLD << "=== SLAVE KING AMAX DEFEATED ===\n\n" << RESET;
            slowPrint(amax.getDefeatText(), 16);
            player.addGold(amax.getGoldReward());
            player.addExperience(amax.getExperienceReward());
            pressEnter();
        }
    }
}

// ── Combat system ─────────────────────────────────────────────────────────────

// Timing bar with real-time input (matches TWP1 approach).
// Shows a static crit-zone label row, then animates cursor below.
// The crit zone position is randomised each call.
int Game::runTimingBar(bool isHeavy, int& outCritPos, int critWidthBonus) {
    const int TOTAL  = 20;
    const int CRIT_W = (isHeavy ? 1 : 3) + critWidthBonus; // Steady Eye adds +2
    outCritPos = randInt(CRIT_W, TOTAL - CRIT_W - 1);

    // Build colored crit-zone guide row
    std::cout << "\n  " << DIM << "Time your attack — press SPACE!\n" << RESET;
    std::cout << "  \xe2\x94\x82";  // │
    for (int i = 0; i < TOTAL; ++i) {
        int lo = outCritPos - CRIT_W/2, hi = outCritPos + CRIT_W/2;
        if (i >= lo && i <= hi)
            std::cout << YELLOW << BOLD << "\xe2\x96\x93" << RESET; // ▓ yellow = crit zone
        else
            std::cout << DIM << "\xe2\x96\x91" << RESET;           // ░ dim   = safe
    }
    std::cout << "\xe2\x94\x82" << RESET << "  " << YELLOW << "▓" << RESET << " = crit\n";

    // Switch console to raw / non-blocking mode (same as TWP1)
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT) & (~ENABLE_LINE_INPUT));
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    int  cursor = 0, dir = 1;
    bool pressed = false;
    int  delayMs = isHeavy ? 45 : 65;

    while (!pressed) {
        std::cout << "\r  \xe2\x94\x82";  // │
        for (int i = 0; i < TOTAL; ++i) {
            if (i == cursor)
                std::cout << BOLD << "\xe2\x96\x88" << RESET; // █ white cursor
            else {
                int lo = outCritPos - CRIT_W/2, hi = outCritPos + CRIT_W/2;
                if (i >= lo && i <= hi)
                    std::cout << YELLOW << "\xe2\x96\x93" << RESET; // ▓ yellow crit zone
                else
                    std::cout << DIM << "\xe2\x96\x91" << RESET;   // ░ dim safe
            }
        }
        std::cout << "\xe2\x94\x82" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));

        if (_kbhit()) {
            char ch = (char)_getch();
            if (ch == ' ') { pressed = true; break; }
        }
        cursor += dir;
        if (cursor >= TOTAL - 1) dir = -1;
        if (cursor <= 0)         dir =  1;
    }
    std::cout << "\n";

    // Restore console mode
#ifdef _WIN32
    SetConsoleMode(hStdin, mode);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    return cursor;
}

// Shared helpers for combat display
static std::string makeBar(int val, int maxVal, int width = 20) {
    int filled = (maxVal > 0) ? (val * width) / maxVal : 0;
    filled = std::max(0, std::min(width, filled));
    std::string bar;
    for (int i = 0; i < width; ++i) bar += (i < filled ? "\xe2\x96\x88" : "\xe2\x96\x91"); // █ ░
    return bar;
}
static const char* hpColor(int hp, int maxHp) {
    float pct = (maxHp > 0) ? (float)hp / maxHp : 0.f;
    if (pct > 0.6f) return "\033[32m";
    if (pct > 0.3f) return "\033[33m";
    return "\033[31m";
}

void Game::showCombatStatus(const Enemy& enemy) const {
    std::cout << "\n";
    std::cout << DIM << "  ──────────────────────────────────────────────────────\n" << RESET;
    std::cout << "\n";

    // ── Player ────────────────────────────────────────────────────────────────
    int ph = player.getHealth(), phm = player.getMaxHealth();
    int ps = player.getStamina(), psm = player.getMaxStamina();

    std::cout << "  " << GREEN << BOLD << player.getName() << RESET
              << DIM << "  Lv." << player.getLevel() << RESET;
    // Show active runes
    for (int _pi = 0; _pi < 2; ++_pi)
        if (player.getRuneSlot(_pi) != RuneType::NONE)
            std::cout << "  " << CYAN << "[" << runeName(player.getRuneSlot(_pi)) << "]" << RESET;
    std::cout << "\n";

    std::cout << "  HP  " << hpColor(ph, phm) << makeBar(ph, phm) << RESET
              << "  " << ph << "/" << phm << "\n";
    std::cout << "  ST  " << CYAN << makeBar(ps, psm) << RESET
              << "  " << ps << "/" << psm << "\n";

    // Attack slots with cooldowns
    for (int _ai = 0; _ai < 3; ++_ai) {
        const AttackType& at = player.getAttack(_ai);
        int cdLeft = player.getAttackCooldown(_ai);
        // Cooldown squares: ■ per locked turn, □ per spent turn
        std::string cdBar;
        if (cdLeft > 0) {
            for (int _cd = 0; _cd < at.cooldown; ++_cd)
                cdBar += (_cd < cdLeft) ? "\xe2\x96\xa0" : "\xe2\x96\xa1"; // ■ □
        }
        std::cout << "  [" << (_ai+1) << "] "
                  << Item::getRarityColor(at.rarity) << BOLD << at.name << RESET
                  << " " << DIM << at.getStatsString() << RESET;
        if (cdLeft > 0)
            std::cout << RED << "  " << cdBar << " cd:" << cdLeft << RESET;
        else
            std::cout << GREEN << "  \xe2\x9c\x93" << RESET; // ✓
        std::cout << "\n";
    }

    if (player.getBuffs().bleeding)
        std::cout << "      " << RED    << "\xe2\x99\xa5 Bleeding (2 dmg/turn)" << RESET << "\n";
    if (player.getBuffs().poisonStacks > 0)
        std::cout << "      " << "\033[32m" << "\xe2\x98\xa0 Poisoned (" << player.getBuffs().poisonStacks << " stacks)" << RESET << "\n";
    if (player.getBuffs().blindRounds > 0)
        std::cout << "      " << YELLOW << "\xe2\x97\x89 Blinded (" << player.getBuffs().blindRounds << " rounds)" << RESET << "\n";
    if (player.getBuffs().shieldTonicTurns > 0)
        std::cout << "      " << CYAN   << "\xe2\x96\xa0 Shield Tonic (" << player.getBuffs().shieldTonicTurns << " turns)" << RESET << "\n";
    if (player.getBuffs().rageDraughtActive)
        std::cout << "      " << YELLOW << "\xe2\x98\x85 Rage Draught (next hit x1.5)" << RESET << "\n";
    if (player.getBuffs().phantomsDraughtActive)
        std::cout << "      " << MAGENTA << "\xe2\x97\x86 Phantom's Draught (next hit bypasses defense)" << RESET << "\n";
    if (player.getBuffs().gravewardenOilCharges > 0)
        std::cout << "      " << RED    << "\xe2\x9c\xa6 Gravewarden Oil (" << player.getBuffs().gravewardenOilCharges << " charges)" << RESET << "\n";
    if (player.getBuffs().doubleNextDamage)
        std::cout << "      " << YELLOW << "\xe2\x98\x85 Void Bulwark (next attack x2)" << RESET << "\n";
    if (player.getBuffs().comboSetupBonus > 0)
        std::cout << "      " << CYAN   << "\xe2\x96\xb6 Combo Setup (+" << player.getBuffs().comboSetupBonus << " next attack)" << RESET << "\n";
    if (player.getBuffs().dodgeNextAttack)
        std::cout << "      " << CYAN   << "\xe2\x97\x8f Warding (next attack dodged)" << RESET << "\n";

    std::cout << "\n";
    std::cout << DIM << "  ┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄\n" << RESET;
    std::cout << "\n";

    // ── Enemy ─────────────────────────────────────────────────────────────────
    int eh = enemy.getHealth(), ehm = enemy.getMaxHealth();
    std::cout << "  " << RED << BOLD << enemy.getName() << RESET << "\n";
    std::cout << "  HP  " << hpColor(eh, ehm) << makeBar(eh, ehm) << RESET
              << "  " << eh << "/" << ehm << "\n";

    if (enemy.isBleeding())
        std::cout << "      " << RED    << "\xe2\x99\xa5 Bleeding" << RESET << "\n";
    if (enemy.isPoisoned())
        std::cout << "      " << "\033[32m" << "\xe2\x98\xa0 Poisoned (" << enemy.getPoisonStacks() << " stacks)" << RESET << "\n";
    if (enemy.isBlinded())
        std::cout << "      " << YELLOW << "\xe2\x97\x89 Blinded (" << enemy.getBlindRounds() << " rounds)" << RESET << "\n";

    const EnemyBehaviour& beh = enemy.getBehaviour();
    if (beh.dodgesLightAttacks) std::cout << "      " << MAGENTA << "⚠ Dodges light attacks"       << RESET << "\n";
    if (beh.reflectsHeavy)      std::cout << "      " << YELLOW  << "⚠ Reflects heavy attacks"     << RESET << "\n";
    if (beh.regenHPPerTurn)     std::cout << "      " << GREEN   << "⚠ Regenerates " << beh.regenAmount << " HP/turn" << RESET << "\n";
    if (enemy.isTelegraphing()) std::cout << "      " << YELLOW  << "⚠ Winding up a massive blow!" << RESET << "\n";

    std::cout << "\n";
    std::cout << DIM << "  ──────────────────────────────────────────────────────\n" << RESET;
    std::cout << "\n";
}

void Game::showCombatStatusBoss(const Boss& boss) const {
    std::cout << "\n";
    std::cout << RED << BOLD
              << "  ══════════════════════ BOSS BATTLE ══════════════════════\n"
              << RESET << "\n";

    // ── Player ────────────────────────────────────────────────────────────────
    int ph = player.getHealth(), phm = player.getMaxHealth();
    int ps = player.getStamina(), psm = player.getMaxStamina();

    std::cout << "  " << GREEN << BOLD << player.getName() << RESET
              << DIM << "  Lv." << player.getLevel() << RESET;
    for (int _pi = 0; _pi < 2; ++_pi)
        if (player.getRuneSlot(_pi) != RuneType::NONE)
            std::cout << "  " << CYAN << "[" << runeName(player.getRuneSlot(_pi)) << "]" << RESET;
    std::cout << "\n";

    std::cout << "  HP  " << hpColor(ph, phm) << makeBar(ph, phm) << RESET
              << "  " << ph << "/" << phm << "\n";
    std::cout << "  ST  " << CYAN << makeBar(ps, psm) << RESET
              << "  " << ps << "/" << psm << "\n";

    for (int _ai = 0; _ai < 3; ++_ai) {
        const AttackType& at = player.getAttack(_ai);
        int cdLeft = player.getAttackCooldown(_ai);
        std::string cdBar;
        if (cdLeft > 0)
            for (int _cd = 0; _cd < at.cooldown; ++_cd)
                cdBar += (_cd < cdLeft) ? "\xe2\x96\xa0" : "\xe2\x96\xa1";
        std::cout << "  [" << (_ai+1) << "] "
                  << Item::getRarityColor(at.rarity) << BOLD << at.name << RESET
                  << " " << DIM << at.getStatsString() << RESET;
        if (cdLeft > 0)
            std::cout << RED << "  " << cdBar << " cd:" << cdLeft << RESET;
        else
            std::cout << GREEN << "  \xe2\x9c\x93" << RESET;
        std::cout << "\n";
    }

    if (player.getBuffs().bleeding)
        std::cout << "      " << RED    << "\xe2\x99\xa5 Bleeding" << RESET << "\n";
    if (player.getBuffs().poisonStacks > 0)
        std::cout << "      " << "\033[32m" << "\xe2\x98\xa0 Poisoned (" << player.getBuffs().poisonStacks << " stacks)" << RESET << "\n";
    if (player.getBuffs().blindRounds > 0)
        std::cout << "      " << YELLOW << "\xe2\x97\x89 Blinded (" << player.getBuffs().blindRounds << " rounds)" << RESET << "\n";
    if (player.getBuffs().shieldTonicTurns > 0)
        std::cout << "      " << CYAN   << "\xe2\x96\xa0 Shield Tonic (" << player.getBuffs().shieldTonicTurns << " turns)" << RESET << "\n";
    if (player.getBuffs().rageDraughtActive)
        std::cout << "      " << YELLOW << "\xe2\x98\x85 Rage Draught (next hit x1.5)" << RESET << "\n";
    if (player.getBuffs().phantomsDraughtActive)
        std::cout << "      " << MAGENTA << "\xe2\x97\x86 Phantom's Draught (next hit bypasses defense)" << RESET << "\n";
    if (player.getBuffs().gravewardenOilCharges > 0)
        std::cout << "      " << RED    << "\xe2\x9c\xa6 Gravewarden Oil (" << player.getBuffs().gravewardenOilCharges << " charges)" << RESET << "\n";

    std::cout << "\n";
    std::cout << DIM << "  ┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄\n" << RESET;
    std::cout << "\n";

    // ── Boss ──────────────────────────────────────────────────────────────────
    int bh = boss.getHealth(), bhm = boss.getMaxHealth();
    std::cout << "  " << RED << BOLD << boss.getName() << RESET
              << DIM << "  Phase " << boss.getCurrentPhase()+1 << RESET << "\n";
    std::cout << "  HP  " << hpColor(bh, bhm) << makeBar(bh, bhm) << RESET
              << "  " << bh << "/" << bhm << "\n";

    std::cout << "\n";
    std::cout << RED << BOLD
              << "  ══════════════════════════════════════════════════════════\n"
              << RESET << "\n";
}

// ── Player attack turn (enemy) ────────────────────────────────────────────────

void Game::playerAttackTurn(Enemy& enemy, bool& enemyDead) {
    const EnemyBehaviour& beh = enemy.getBehaviour();

    // Brute telegraph: set randomly if not already set
    if (beh.isTank && !enemy.isTelegraphing())
        enemy.setTelegraphing(randInt(0, 2) == 0);

    // Reset per-turn defense from attack type
    player.getBuffs().attackTypeDefenseThisTurn = 0;

    // Tick cooldowns at start of player's turn
    player.tickCooldowns();
    player.getBuffs().restedLastTurn = false;

    while (true) {
        std::cout << DIM << "  ─────────────────────── YOUR TURN ───────────────────────\n" << RESET;

        // Show attack slots
        for (int _i = 0; _i < 3; ++_i) {
            const AttackType& at = player.getAttack(_i);
            if (player.isAttackReady(_i)) {
                std::cout << BOLD << YELLOW << "  [ " << (_i+1) << " ]" << RESET
                          << "  " << at.name << DIM << "  · " << at.getStatsString() << RESET << "\n";
            } else {
                std::cout << DIM << "  [ " << (_i+1) << " ]"
                          << "  " << at.name << "  [cooldown: " << player.getAttackCooldown(_i) << "]\n" << RESET;
            }
        }
        std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  Rest             " << DIM << "· recover all stamina\n" << RESET;
        std::cout << BOLD << YELLOW << "  [ 5 ]" << RESET << "  Inventory        " << DIM << "· free action\n" << RESET;
        if (enemy.canTalk())
            std::cout << BOLD << YELLOW << "  [ 6 ]" << RESET << "  Talk\n";
        std::cout << BOLD << YELLOW << "  [ 7 ]" << RESET << "  Bargain          " << DIM << "· pay " << enemy.getBribeCost() << "g\n" << RESET;
        std::cout << DIM << "  ──────────────────────────────────────────────────────\n" << RESET;
        std::cout << "  Choice: ";

        std::string input;
        std::getline(std::cin, input);
        int c = 0;
        try { c = std::stoi(input); } catch (...) {}

        if (c == 5) {
            player.showInventory();
            if (!player.getPotions().empty()) {
                std::cout << "  0. Cancel\n  Use which potion? ";
                int idx = -1;
                std::string pidx; std::getline(std::cin, pidx);
                try { idx = std::stoi(pidx) - 1; } catch (...) {}
                if (idx >= 0) player.usePotion(idx);
            } else {
                pressEnter();
            }
            showCombatStatus(enemy);
            continue; // free action
        }

        if (c == 4) {
            if (player.getBuffs().restHealBonus > 0) {
                player.heal(player.getBuffs().restHealBonus);
                std::cout << GREEN << "[STILLNESS] You recover " << player.getBuffs().restHealBonus << " HP.\n" << RESET;
                player.getBuffs().restHealBonus = 0;
            }
            player.restoreStamina(player.getMaxStamina());
            player.getBuffs().restedLastTurn = true;
            std::cout << CYAN << "You catch your breath. Stamina fully restored.\n" << RESET;
            return;
        }

        if (c == 6 && enemy.canTalk()) {
            if (attemptTalk(enemy)) { enemyDead = true; }
            return;
        }

        if (c == 7) {
            if (attemptBargain(enemy)) { enemyDead = true; }
            return;
        }

        // Attack slot selection (1, 2, or 3)
        if (c >= 1 && c <= 3) {
            int slot = c - 1;
            if (!player.isAttackReady(slot)) {
                std::cout << RED << "That skill is on cooldown (" << player.getAttackCooldown(slot) << " turns).\n" << RESET;
                continue;
            }

            const AttackType& at = player.getAttack(slot);
            bool isCrit = false;

            // Blind check: player is blinded — 30% miss chance
            if (player.getBuffs().blindRounds > 0 && randInt(0, 99) < 30) {
                std::cout << YELLOW << "[BLIND] You swing and miss! Your vision fails you.\n" << RESET;
                --player.getBuffs().blindRounds;
                if (player.getBuffs().blindRounds == 0)
                    std::cout << "Your sight returns.\n";
                player.startCooldown(slot);
                return;
            }
            if (player.getBuffs().blindRounds > 0)
                --player.getBuffs().blindRounds;

            // Apply defense from this attack type
            player.getBuffs().attackTypeDefenseThisTurn = at.defenseBonus;

            // Special: Darkblood Sentinel interrupts timing bar
            bool interrupted = false;
            if (beh.interruptsTimingBar && randInt(1,3) == 1) {
                std::cout << RED << "The Sentinel interrupts your swing!\n" << RESET;
                interrupted = true;
            }

            // Steady Eye passive widens crit window
            int critWidthBonus = player.hasRune(RuneType::STEADY_EYE) ? 2 : 0;
            int critPos  = 10;
            int cursorPos = interrupted ? randInt(0, 5) : runTimingBar(true, critPos, critWidthBonus);

            // Death's Bargain: damage is 40% of missing HP, skip timing roll
            int dmg = 0;
            if (at.effect == AttackEffect::POISON_BURST) {
                int stacks = enemy.getPoisonStacks();
                dmg = std::max(1, stacks * at.effectValue);
                enemy.clearPoison();
                std::cout << RED << "[TOXIC BLOOM] " << stacks << " stacks × " << at.effectValue
                          << " = " << dmg << " damage! Poison cleared.\n" << RESET;
            } else if (at.effect == AttackEffect::DAMAGE_FROM_MISSING_HP) {
                int missing = player.getMaxHealth() - player.getHealth();
                dmg = std::max(1, static_cast<int>(missing * 0.4f));
                std::cout << YELLOW << "[DEATH'S BARGAIN] Missing HP: " << missing
                          << " → " << dmg << " damage!\n" << RESET;
                // Self-heal component
                if (at.effectValue > 0) {
                    player.heal(at.effectValue);
                    std::cout << GREEN << "[DEATH'S BARGAIN] Restored " << at.effectValue << " HP.\n" << RESET;
                }
                if (player.getBuffs().rageDraughtActive) {
                    dmg = static_cast<int>(dmg * 1.5f);
                    player.getBuffs().rageDraughtActive = false;
                    std::cout << YELLOW << "Rage Draught surges through you!\n" << RESET;
                }
            } else {
                int baseRoll = at.roll() + player.getWeapon().getModifier();

                // Combo setup bonus from previous turn
                if (player.getBuffs().comboSetupBonus > 0) {
                    baseRoll += player.getBuffs().comboSetupBonus;
                    std::cout << CYAN << "[COMBO] +" << player.getBuffs().comboSetupBonus << " bonus damage!\n" << RESET;
                    player.getBuffs().comboSetupBonus = 0;
                }

                // Coiled Strike: bonus if rested last turn
                if (at.effect == AttackEffect::BONUS_IF_RESTED && player.getBuffs().restedLastTurn) {
                    baseRoll += at.effectValue;
                    std::cout << CYAN << "[COILED STRIKE] Tension released! +" << at.effectValue << " damage!\n" << RESET;
                }

                // Crimson Fury: x2 if player is bleeding
                if (at.effect == AttackEffect::CRIMSON_FURY && player.getBuffs().bleeding) {
                    baseRoll *= 2;
                    std::cout << RED << "[CRIMSON FURY] Your blood fuels the strike!\n" << RESET;
                }

                // Shadow Press: x2 if enemy is blind
                if (at.effect == AttackEffect::BLIND_AMPLIFY && enemy.isBlinded()) {
                    baseRoll *= 2;
                    std::cout << YELLOW << "[SHADOW PRESS] They can't see it coming!\n" << RESET;
                }

                // Desperate Strike: x2 if player below 40% HP
                if (at.effect == AttackEffect::DESPERATE_STRIKE &&
                    player.getHealth() < player.getMaxHealth() * 0.4f) {
                    baseRoll *= 2;
                    std::cout << RED << "[DESPERATE STRIKE] Pain becomes power!\n" << RESET;
                }

                // Void Bulwark: x2 next attack buff
                if (player.getBuffs().doubleNextDamage) {
                    baseRoll *= 2;
                    std::cout << YELLOW << "[VOID BULWARK] Double damage!\n" << RESET;
                    player.getBuffs().doubleNextDamage = false;
                }

                dmg = player.timingBasedAttack(cursorPos, critPos, 20, baseRoll, isCrit);
                if (dmg == 0) { continue; }

                // Bloodhunger rune: crit x1.5
                if (isCrit && player.hasRune(RuneType::BLOODHUNGER))
                    dmg = static_cast<int>(dmg * 1.5f);
            }

            // Proc chance: 80% base + 0.5% per luck point, capped at 95%
            auto statusProc = [&]() -> bool {
                int chancePct = std::min(95, 80 + player.getLuck() / 2);
                return randInt(0, 99) < chancePct;
            };

            // Gravewarden Oil
            if (player.getBuffs().gravewardenOilCharges > 0) {
                if (statusProc()) {
                    enemy.applyBleed();
                    --player.getBuffs().gravewardenOilCharges;
                    std::cout << RED << "[GRAVEWARDEN OIL] Bleed applied! ("
                              << player.getBuffs().gravewardenOilCharges << " charges left)\n" << RESET;
                } else {
                    --player.getBuffs().gravewardenOilCharges;
                    std::cout << DIM << "[GRAVEWARDEN OIL] Bleed resisted. ("
                              << player.getBuffs().gravewardenOilCharges << " charges left)\n" << RESET;
                }
            }

            // Marsh Wraith: phased — but since all attacks are now "heavy style",
            // Wraith dodges on a 50% chance instead of guaranteed
            if (beh.dodgesLightAttacks && randInt(0, 1) == 0) {
                std::cout << MAGENTA << "The Wraith phases through your attack!\n" << RESET;
                player.startCooldown(slot);
                return;
            }

            // Ash Titan: reflect 50% of damage back
            if (beh.reflectsHeavy) {
                int reflected = dmg / 2;
                player.takeDamage(reflected);
                std::cout << RED << "The Titan reflects " << reflected << " damage back at you!\n" << RESET;
                dmg /= 2;
            }

            int def = 0;
            if (player.getBuffs().phantomsDraughtActive) {
                std::cout << MAGENTA << "[PHANTOM'S DRAUGHT] Your strike bypasses their guard!\n" << RESET;
                player.getBuffs().phantomsDraughtActive = false;
            } else {
                def = enemy.rollDefense();
            }

            // Mirror Strike: reflect effect (handled before dealing dmg)
            if (at.effect == AttackEffect::REFLECT_DAMAGE) {
                player.getBuffs().attackTypeDefenseThisTurn += (def * at.effectValue / 100);
                // Reflect value stored; enemy takes it later in enemyAttackTurn via rollDefense
            }

            int net = std::max(1, dmg - def);

            // Bloodletter: extra damage if already bleeding
            if (at.effect == AttackEffect::APPLY_BLEED && enemy.isBleeding() && at.effectValue > 0) {
                net += at.effectValue;
                std::cout << RED << "[" << at.name << "] Wound exploited! +" << at.effectValue << " damage.\n" << RESET;
            }

            // Bloodburst: burst bonus if bleeding, then consume bleed
            if (at.effect == AttackEffect::BLEED_BURST && enemy.isBleeding()) {
                net += at.effectValue;
                enemy.clearBleed();
                std::cout << RED << "[BLOODBURST] +" << at.effectValue << " burst damage! Bleed consumed.\n" << RESET;
            }

            // Overload: bonus damage per total status stacks
            if (at.effect == AttackEffect::STATUS_OVERLOAD) {
                int totalStacks = (enemy.isBleeding() ? 1 : 0)
                                + enemy.getPoisonStacks()
                                + enemy.getBlindRounds();
                if (totalStacks > 0) {
                    int bonus = totalStacks * 3;
                    net += bonus;
                    std::cout << RED << "[OVERLOAD] " << totalStacks << " stacks × 3 = +"
                              << bonus << " damage!\n" << RESET;
                }
            }

            enemy.takeDamage(net);
            if (isCrit) {
                std::cout << YELLOW << BOLD
                          << "  \xe2\x95\x94\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x97\n"
                          << "  \xe2\x95\x91  \xe2\x98\x85 CRITICAL HIT!  \xe2\x95\x91  "
                          << GREEN << net << YELLOW << " dmg\n"
                          << "  \xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x9d\n"
                          << RESET;
                std::cout << DIM << "  [" << at.name << "]\n" << RESET;
            } else {
                std::cout << "You deal " << GREEN << net << " damage" << RESET
                          << " [" << at.name << "]\n";
            }

            // ── Apply attack type effects ─────────────────────────────────────
            switch (at.effect) {
                case AttackEffect::APPLY_BLEED:
                    if (statusProc()) {
                        enemy.applyBleed();
                        std::cout << RED << "[BLEED] applied to " << enemy.getName() << "!\n" << RESET;
                    } else {
                        std::cout << DIM << "[BLEED] " << enemy.getName() << " resists.\n" << RESET;
                    }
                    break;
                case AttackEffect::APPLY_POISON:
                    if (statusProc()) {
                        enemy.applyPoison(at.effectValue);
                        std::cout << RED << "[POISON] " << at.effectValue << " stacks on "
                                  << enemy.getName() << "!\n" << RESET;
                    } else {
                        std::cout << DIM << "[POISON] " << enemy.getName() << " resists.\n" << RESET;
                    }
                    break;
                case AttackEffect::APPLY_BLIND:
                    if (statusProc()) {
                        enemy.applyBlind(at.effectValue);
                        std::cout << YELLOW << "[BLIND] " << enemy.getName()
                                  << " blinded (" << at.effectValue << " rounds)!\n" << RESET;
                    } else {
                        std::cout << DIM << "[BLIND] " << enemy.getName() << " resists.\n" << RESET;
                    }
                    break;
                case AttackEffect::COMBO_SETUP:
                    player.getBuffs().comboSetupBonus = at.effectValue;
                    std::cout << CYAN << "[COMBO] Next attack +" << at.effectValue << " damage.\n" << RESET;
                    break;
                case AttackEffect::APPLY_BLEED_AND_POISON:
                    if (statusProc()) {
                        enemy.applyBleed();
                        enemy.applyPoison(at.effectValue);
                        std::cout << RED << "[ASH CURSE] Bleed + " << at.effectValue << " poison on "
                                  << enemy.getName() << "!\n" << RESET;
                    } else {
                        std::cout << DIM << "[ASH CURSE] " << enemy.getName() << " resists.\n" << RESET;
                    }
                    break;
                case AttackEffect::APPLY_ALL_THREE:
                    if (statusProc()) {
                        enemy.applyBleed();
                        enemy.applyBlind(3);
                        enemy.applyPoison(at.effectValue);
                        std::cout << RED << "[GOSPEL OF ASH] Bleed, Blind (3), Poison ("
                                  << at.effectValue << ") applied!\n" << RESET;
                    } else {
                        std::cout << DIM << "[GOSPEL OF ASH] " << enemy.getName() << " resists.\n" << RESET;
                    }
                    break;
                case AttackEffect::DOUBLE_NEXT_DAMAGE:
                    player.getBuffs().doubleNextDamage = true;
                    std::cout << YELLOW << "[VOID BULWARK] Next attack deals x2 damage.\n" << RESET;
                    break;
                case AttackEffect::DODGE_NEXT_ATTACK:
                    player.getBuffs().dodgeNextAttack = true;
                    std::cout << CYAN << "[WARDING BLOW] Next incoming attack will be dodged.\n" << RESET;
                    break;
                case AttackEffect::EXECUTE_HEAL:
                    if (!enemy.isAlive()) {
                        player.heal(at.effectValue);
                        std::cout << GREEN << "[SOUL CLEAVE] Kill heals " << at.effectValue << " HP!\n" << RESET;
                    }
                    break;
                case AttackEffect::SKIP_ENEMY_TURN:
                    enemy.setSkipNextTurn();
                    std::cout << YELLOW << "[VOID COLLAPSE] " << enemy.getName()
                              << " will skip their next attack!\n" << RESET;
                    break;
                case AttackEffect::APPLY_ALL_STATUSES: {
                    bool anyActive = enemy.isBleeding() || enemy.isPoisoned() || enemy.isBlinded();
                    if (anyActive) {
                        if (!enemy.isBleeding() && statusProc()) { enemy.applyBleed();   std::cout << RED    << "[RECKONING] Bleed!\n" << RESET; }
                        if (!enemy.isPoisoned() && statusProc()) { enemy.applyPoison(4); std::cout << RED    << "[RECKONING] Poison (4)!\n" << RESET; }
                        if (!enemy.isBlinded()  && statusProc()) { enemy.applyBlind(3);  std::cout << YELLOW << "[RECKONING] Blind (3)!\n" << RESET; }
                    } else {
                        std::cout << DIM << "[ASHEN RECKONING] No active status to spread.\n" << RESET;
                    }
                    break;
                }
                case AttackEffect::PERM_BLEED_NO_REGEN:
                    enemy.suppressRegen(); // regen suppression always lands
                    if (statusProc()) {
                        enemy.applyBleed();
                        std::cout << RED << "[IMMORTAL SCAR] Permanent bleed. Regen suppressed.\n" << RESET;
                    } else {
                        std::cout << DIM << "[IMMORTAL SCAR] Bleed resisted. Regen still suppressed.\n" << RESET;
                    }
                    break;
                case AttackEffect::REFLECT_DAMAGE:
                    // Reflect is passive this turn — handled via attackTypeDefenseThisTurn
                    std::cout << CYAN << "[MIRROR STRIKE] Reflecting " << at.effectValue
                              << "% of incoming damage.\n" << RESET;
                    break;
                case AttackEffect::SELF_BLEED:
                    player.getBuffs().bleeding = true;
                    std::cout << RED << "[BLOODPRICE] You open your own wound. You are now bleeding.\n" << RESET;
                    break;
                case AttackEffect::REST_TO_HEAL:
                    player.getBuffs().restHealBonus += at.effectValue;
                    std::cout << CYAN << "[STILLNESS] Rest soon to recover " << at.effectValue << " HP.\n" << RESET;
                    break;
                case AttackEffect::APPLY_POISON_AND_COMBO:
                    enemy.applyPoison(at.effectValue);
                    player.getBuffs().comboSetupBonus += at.effectValue;
                    std::cout << RED << "[TOXIN BARB] " << at.effectValue << " poison stacks. Next attack +"
                              << at.effectValue << " damage.\n" << RESET;
                    break;
                case AttackEffect::APPLY_BLEED_DEF_HEAL:
                    if (statusProc()) {
                        enemy.applyBleed();
                        std::cout << RED << "[BLOOD REAPER] Bleed applied!" << RESET;
                    } else {
                        std::cout << DIM << "[BLOOD REAPER] Bleed resisted." << RESET;
                    }
                    player.heal(at.effectValue);
                    std::cout << GREEN << " Healed " << at.effectValue << " HP." << RESET
                              << " (+" << at.defenseBonus << " def this turn)\n";
                    break;
                case AttackEffect::APPLY_POISON_ADD: {
                    if (statusProc()) {
                        int addStacks = at.effectValue;
                        if (enemy.isBleeding()) addStacks += at.effectValue;
                        enemy.applyPoison(addStacks);
                        std::cout << RED << "[PLAGUE TOUCH] +" << addStacks << " poison stacks";
                        if (addStacks > at.effectValue)
                            std::cout << " (doubled — enemy bleeding)";
                        std::cout << "!\n" << RESET;
                    } else {
                        std::cout << DIM << "[PLAGUE TOUCH] " << enemy.getName() << " resists.\n" << RESET;
                    }
                    break;
                }
                case AttackEffect::DOUBLE_POISON_STACKS: {
                    int cur = enemy.getPoisonStacks();
                    if (cur > 0) {
                        if (statusProc()) {
                            enemy.applyPoison(cur);
                            std::cout << RED << "[VENOM SURGE] Poison doubled: "
                                      << cur << " → " << (cur * 2) << " stacks!\n" << RESET;
                        } else {
                            std::cout << DIM << "[VENOM SURGE] Poison spread resisted.\n" << RESET;
                        }
                    } else {
                        std::cout << DIM << "[VENOM SURGE] No poison to double.\n" << RESET;
                    }
                    break;
                }
                case AttackEffect::BLIND_IF_POISONED:
                    if (statusProc()) {
                        enemy.applyBleed();
                        std::cout << RED << "[INFECTED WOUND] Bleed applied!\n" << RESET;
                        if (enemy.isPoisoned()) {
                            enemy.applyBlind(3);
                            std::cout << YELLOW << "[INFECTED WOUND] Poison detected — Blind (3) applied!\n" << RESET;
                        }
                    } else {
                        std::cout << DIM << "[INFECTED WOUND] " << enemy.getName() << " resists.\n" << RESET;
                    }
                    break;
                case AttackEffect::BLIND_AMPLIFY:
                    if (!enemy.isBlinded())
                        std::cout << DIM << "[SHADOW PRESS] No blind — normal damage.\n" << RESET;
                    break;
                case AttackEffect::BLEED_BURST:
                    if (!enemy.isBleeding())
                        std::cout << DIM << "[BLOODBURST] No bleed to burst.\n" << RESET;
                    break;
                case AttackEffect::STATUS_OVERLOAD:
                case AttackEffect::DESPERATE_STRIKE:
                case AttackEffect::POISON_BURST:
                    // All handled before this switch
                    break;
                default: break;
            }

            // Toxin Rider rune: +1 poison if already poisoned (subject to proc)
            if (player.hasRune(RuneType::TOXIN_RIDER) && enemy.isPoisoned() && statusProc()) {
                enemy.applyPoison(1);
                std::cout << RED << "[TOXIN RIDER] Poison +1 → " << enemy.getPoisonStacks() << "\n" << RESET;
            }

            player.startCooldown(slot);
            if (!enemy.isAlive()) enemyDead = true;
            return;
        }

        // Unknown input — loop back
    } // end while(true)
}

// ── Enemy attack turn ─────────────────────────────────────────────────────────

void Game::enemyAttackTurn(Enemy& enemy) {
    const EnemyBehaviour& beh = enemy.getBehaviour();

    // Void Collapse: enemy skips this turn
    if (enemy.isSkippingTurn()) {
        enemy.clearSkipNextTurn();
        std::cout << YELLOW << enemy.getName() << " is stunned and cannot act!\n" << RESET;
        return;
    }

    // Tick enemy status effects
    if (enemy.isBleeding()) {
        int bleedDmg = player.hasRune(RuneType::DEEP_CUTS) ? 5 : 2;
        int dmg = enemy.tickBleed(bleedDmg);
        std::cout << RED << "[BLEED] " << enemy.getName() << " takes " << dmg
                  << " damage. (permanent)\n" << RESET;
        if (!enemy.isAlive()) return;
    }
    if (enemy.isPoisoned()) {
        int dmg = enemy.tickPoison();
        std::cout << RED << "[POISON] " << enemy.getName() << " takes " << dmg
                  << " damage. [" << (dmg) << " -> " << enemy.getPoisonStacks() << "]\n" << RESET;
        if (!enemy.isAlive()) return;
    }

    // Nightkeeper passive: refresh blind on enemy only if already blinded (bug fix — no auto-blind)
    if (player.hasRune(RuneType::NIGHTKEEPER) && enemy.isBlinded()) {
        enemy.applyBlind(999); // prevent expiry — does NOT create new blind
    }

    // HP regen (Bog Colossus) — suppressed by Immortal Scar
    if (beh.regenHPPerTurn && !enemy.isRegenSuppressed()) {
        enemy.heal(beh.regenAmount);
        std::cout << YELLOW << enemy.getName() << " regenerates " << beh.regenAmount << " HP.\n" << RESET;
    } else if (beh.regenHPPerTurn && enemy.isRegenSuppressed()) {
        std::cout << DIM << enemy.getName() << "'s regeneration is suppressed.\n" << RESET;
    }

    // Warding Blow: player dodges this entire turn
    if (player.getBuffs().dodgeNextAttack) {
        player.getBuffs().dodgeNextAttack = false;
        std::cout << CYAN << "[WARDING] You sidestep " << enemy.getName() << "'s attack completely!\n" << RESET;
        if (enemy.isBlinded()) enemy.tickBlind();
        return;
    }

    // Determine miss chance for blinded enemy (additive: base 30%, +20% Ash Veil, +10% Nightkeeper)
    auto getMissChance = [&]() -> int {
        if (!enemy.isBlinded()) return 0;
        int chance = 30;
        if (player.hasRune(RuneType::ASH_VEIL))    chance += 20;
        if (player.hasRune(RuneType::NIGHTKEEPER))  chance += 10;
        return chance;
    };

    auto singleAttack = [&]() {
        // Ghost Step: 20% dodge
        if (player.hasRune(RuneType::GHOST_STEP) && randInt(0, 99) < 20) {
            std::cout << CYAN << "[GHOST STEP] You ghost through " << enemy.getName() << "'s attack!\n" << RESET;
            return;
        }
        // Blind miss
        int mc = getMissChance();
        if (mc > 0 && enemy.rollBlindMiss(mc)) {
            std::cout << YELLOW << enemy.getName() << " attacks — MISS! [BLIND]\n" << RESET;
            return;
        }
        int atk = enemy.rollAttack();
        int def = player.rollDefense();
        int dmg = std::max(1, atk - def);

        // Iron Skin passive: 25% damage reduction
        if (player.hasRune(RuneType::IRON_SKIN))
            dmg = std::max(1, static_cast<int>(dmg * 0.75f));
        // Predator passive: bleeding enemy deals 40% less
        if (player.hasRune(RuneType::PREDATOR) && enemy.isBleeding())
            dmg = std::max(1, static_cast<int>(dmg * 0.60f));

        // Mirror Strike reflect: deal back a portion of damage
        if (player.getBuffs().attackTypeDefenseThisTurn > 0) {
            // Mirror Strike stores reflect as defense buff; reflect = atk * 50%
            // We already reduced dmg by rollDefense which includes attackTypeDefenseThisTurn.
            // Reflect: enemy takes atk * effectValue% / 100
        }

        player.takeDamage(dmg);
        std::cout << RED << enemy.getName() << " hits you for " << dmg << " damage.\n" << RESET;

        // Enemy status procs on player
        if (beh.appliesBleed && !player.getBuffs().bleeding
            && randInt(0, 99) < beh.statusProcChance) {
            player.applyBleedToPlayer();
            std::cout << RED << enemy.getName() << " BLEEDS you! (permanent)\n" << RESET;
        }
        if (beh.appliesPoison && player.getBuffs().poisonStacks == 0
            && !player.hasRune(RuneType::PLAGUE_WARD)
            && randInt(0, 99) < beh.statusProcChance) {
            player.applyPoisonToPlayer(5);
            std::cout << RED << enemy.getName() << " POISONS you! (5 stacks)\n" << RESET;
        }
        if (beh.appliesBlind && player.getBuffs().blindRounds == 0
            && randInt(0, 99) < beh.statusProcChance) {
            player.applyBlindToPlayer(2);
            std::cout << YELLOW << enemy.getName() << " BLINDS you! (2 rounds)\n" << RESET;
        }
    };

    // Tank telegraph: deliver heavy blow
    if (beh.isTank && enemy.isTelegraphing()) {
        int mc = getMissChance();
        if (mc > 0 && enemy.rollBlindMiss(mc)) {
            std::cout << YELLOW << enemy.getName() << " winds up but MISSES! [BLIND]\n" << RESET;
            enemy.setTelegraphing(false);
            return;
        }
        int atk = enemy.rollAttack() * 2;
        int def = player.rollDefense();
        int dmg = std::max(1, atk - def);
        if (player.hasRune(RuneType::IRON_SKIN))
            dmg = std::max(1, static_cast<int>(dmg * 0.75f));
        player.takeDamage(dmg);
        std::cout << RED << BOLD << enemy.getName() << " unleashes a devastating blow! "
                  << dmg << " damage!\n" << RESET;
        enemy.setTelegraphing(false);
        return;
    }

    // Decrement blind after enemy attacks
    if (enemy.isBlinded() && !player.hasRune(RuneType::NIGHTKEEPER))
        enemy.tickBlind();

    // Double attack (Ash Hound)
    if (beh.attacksTwice) {
        std::cout << YELLOW << enemy.getName() << " strikes twice!\n" << RESET;
        singleAttack();
        singleAttack();
        return;
    }

    singleAttack();
}

// ── Full combat loop (enemy) ──────────────────────────────────────────────────

bool Game::runCombat(Enemy& enemy) {
    const EnemyBehaviour& beh = enemy.getBehaviour();
    player.clearPotionLock();
    player.getBuffs() = CombatBuffs{}; // reset combat buffs

    // Mire Lurker ambush: enemy goes first, player can't act
    if (beh.ambushesOnTurnOne) {
        std::cout << RED << "The Mire Lurker strikes from the shadows before you can react!\n" << RESET;
        enemyAttackTurn(enemy);
        if (!player.isAlive()) return false;
    }

    // Plague Shaman: permanently lock a potion for the entire run (once only)
    if (beh.poisonsInventory && !runState.shamanCurseApplied) {
        if (!player.getPotions().empty()) {
            int idx = randInt(0, (int)player.getPotions().size() - 1);
            player.getPotions()[idx].permanentlyLocked = true;
            runState.shamanCurseApplied = true;
            std::cout << MAGENTA << "The Plague Shaman destroys your "
                      << player.getPotions()[idx].name << "! (Gone for this run)\n" << RESET;
        } else {
            runState.shamanCurseApplied = true;
            std::cout << MAGENTA << "The Plague Shaman reaches for your pouch — nothing to take.\n" << RESET;
        }
    }

    // Soul Binder: lock a random potion slot too (same mechanic, different flavour)
    if (beh.locksItemSlot && !player.getPotions().empty()) {
        int idx = randInt(0, (int)player.getPotions().size() - 1);
        player.lockPotionSlot(idx);
        std::cout << MAGENTA << "The Soul Binder locks your "
                  << player.getPotions()[idx].name << " for this fight.\n" << RESET;
    }

    while (enemy.isAlive() && player.isAlive()) {
        showCombatStatus(enemy);
        player.tickBuffs();
        if (!player.isAlive()) break;

        bool enemyDead = false;
        playerAttackTurn(enemy, enemyDead);
        if (enemyDead) break;
        if (!player.isAlive()) break;

        enemyAttackTurn(enemy);
    }

    player.clearPotionLock();
    player.getBuffs().bleeding              = false;
    player.getBuffs().poisonStacks          = 0;
    player.getBuffs().blindRounds           = 0;
    player.getBuffs().attackTypeDefenseThisTurn = 0;
    player.getBuffs().dodgeNextAttack       = false;
    player.getBuffs().doubleNextDamage      = false;
    player.getBuffs().comboSetupBonus       = 0;
    player.getBuffs().skipEnemyNextTurn     = false;
    player.getBuffs().suppressEnemyRegen    = false;
    return player.isAlive();
}

// ── Boss fight loop ───────────────────────────────────────────────────────────

void Game::playerAttackTurnBoss(Boss& boss, bool& bossDead) {
    // Reset per-turn defense from attack type
    player.getBuffs().attackTypeDefenseThisTurn = 0;
    player.tickCooldowns();
    player.getBuffs().restedLastTurn = false;

    while (true) {
        std::cout << DIM << "  ─────────────────────── YOUR TURN ───────────────────────\n" << RESET;
        for (int _i = 0; _i < 3; ++_i) {
            const AttackType& at = player.getAttack(_i);
            if (player.isAttackReady(_i))
                std::cout << BOLD << YELLOW << "  [ " << (_i+1) << " ]" << RESET
                          << "  " << at.name << DIM << "  · " << at.getStatsString() << RESET << "\n";
            else
                std::cout << DIM << "  [ " << (_i+1) << " ]  " << at.name
                          << "  [cooldown: " << player.getAttackCooldown(_i) << "]\n" << RESET;
        }
        std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  Rest             " << DIM << "· recover all stamina\n" << RESET;
        std::cout << BOLD << YELLOW << "  [ 5 ]" << RESET << "  Inventory        " << DIM << "· free action\n" << RESET;
        std::cout << DIM << "  ──────────────────────────────────────────────────────\n" << RESET;
        std::cout << "  Choice: ";

        std::string input;
        std::getline(std::cin, input);
        int c = 0;
        try { c = std::stoi(input); } catch (...) {}

        if (c == 5) {
            player.showInventory();
            if (!player.getPotions().empty()) {
                std::cout << "  0. Cancel\n  Use which potion? ";
                std::string pidx; std::getline(std::cin, pidx);
                int idx = -1;
                try { idx = std::stoi(pidx) - 1; } catch (...) {}
                if (idx >= 0) player.usePotion(idx);
            } else {
                pressEnter();
            }
            showCombatStatusBoss(boss);
            continue; // free action
        }

        if (c == 4) {
            if (player.getBuffs().restHealBonus > 0) {
                player.heal(player.getBuffs().restHealBonus);
                std::cout << GREEN << "[STILLNESS] You recover " << player.getBuffs().restHealBonus << " HP.\n" << RESET;
                player.getBuffs().restHealBonus = 0;
            }
            player.restoreStamina(player.getMaxStamina());
            player.getBuffs().restedLastTurn = true;
            std::cout << CYAN << "You catch your breath. Stamina fully restored.\n" << RESET;
            return;
        }

        if (c >= 1 && c <= 3) {
            int slot = c - 1;
            if (!player.isAttackReady(slot)) {
                std::cout << RED << "That skill is on cooldown (" << player.getAttackCooldown(slot) << " turns).\n" << RESET;
                continue;
            }
            const AttackType& at = player.getAttack(slot);
            bool isCrit = false;

            // Blind miss
            if (player.getBuffs().blindRounds > 0 && randInt(0, 99) < 30) {
                std::cout << YELLOW << "[BLIND] You swing and miss! Your vision fails you.\n" << RESET;
                --player.getBuffs().blindRounds;
                if (player.getBuffs().blindRounds == 0)
                    std::cout << "Your sight returns.\n";
                player.startCooldown(slot);
                return;
            }
            if (player.getBuffs().blindRounds > 0) --player.getBuffs().blindRounds;

            player.getBuffs().attackTypeDefenseThisTurn = at.defenseBonus;

            // Death's Bargain: damage is 40% of missing HP, skip timing roll
            int dmg = 0;
            if (at.effect == AttackEffect::DAMAGE_FROM_MISSING_HP) {
                int missing = player.getMaxHealth() - player.getHealth();
                dmg = std::max(1, static_cast<int>(missing * 0.4f));
                std::cout << YELLOW << "[DEATH'S BARGAIN] Missing HP: " << missing
                          << " → " << dmg << " damage!\n" << RESET;
                if (at.effectValue > 0) {
                    player.heal(at.effectValue);
                    std::cout << GREEN << "[DEATH'S BARGAIN] Restored " << at.effectValue << " HP.\n" << RESET;
                }
                if (player.getBuffs().rageDraughtActive) {
                    dmg = static_cast<int>(dmg * 1.5f);
                    player.getBuffs().rageDraughtActive = false;
                    std::cout << YELLOW << "Rage Draught surges through you!\n" << RESET;
                }
            } else {
                int baseRoll = at.roll() + player.getWeapon().getModifier();

                if (player.getBuffs().comboSetupBonus > 0) {
                    baseRoll += player.getBuffs().comboSetupBonus;
                    std::cout << CYAN << "[COMBO] +" << player.getBuffs().comboSetupBonus << " bonus damage!\n" << RESET;
                    player.getBuffs().comboSetupBonus = 0;
                }

                // Coiled Strike: bonus if rested last turn
                if (at.effect == AttackEffect::BONUS_IF_RESTED && player.getBuffs().restedLastTurn) {
                    baseRoll += at.effectValue;
                    std::cout << CYAN << "[COILED STRIKE] Tension released! +" << at.effectValue << " damage!\n" << RESET;
                }

                if (at.effect == AttackEffect::CRIMSON_FURY && player.getBuffs().bleeding) {
                    baseRoll *= 2;
                    std::cout << RED << "[CRIMSON FURY] Your blood fuels the strike!\n" << RESET;
                }

                // Desperate Strike: x2 if player below 40% HP
                if (at.effect == AttackEffect::DESPERATE_STRIKE &&
                    player.getHealth() < player.getMaxHealth() * 0.4f) {
                    baseRoll *= 2;
                    std::cout << RED << "[DESPERATE STRIKE] Pain becomes power!\n" << RESET;
                }

                if (player.getBuffs().doubleNextDamage) {
                    baseRoll *= 2;
                    std::cout << YELLOW << "[VOID BULWARK] Double damage!\n" << RESET;
                    player.getBuffs().doubleNextDamage = false;
                }

                int critWidthBonus = player.hasRune(RuneType::STEADY_EYE) ? 2 : 0;
                int critPos  = 10;
                int cursorPos = runTimingBar(true, critPos, critWidthBonus);
                dmg = player.timingBasedAttack(cursorPos, critPos, 20, baseRoll, isCrit);
                if (dmg == 0) { continue; }

                if (isCrit && player.hasRune(RuneType::BLOODHUNGER))
                    dmg = static_cast<int>(dmg * 1.5f);
            }

            int def = 0;
            if (player.getBuffs().phantomsDraughtActive) {
                std::cout << MAGENTA << "[PHANTOM'S DRAUGHT] Your strike bypasses their guard!\n" << RESET;
                player.getBuffs().phantomsDraughtActive = false;
            } else {
                def = boss.rollDefense();
            }
            int net = std::max(1, dmg - def);
            boss.takeDamage(net);
            if (isCrit) {
                std::cout << YELLOW << BOLD
                          << "  \xe2\x95\x94\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x97\n"
                          << "  \xe2\x95\x91  \xe2\x98\x85 CRITICAL HIT!  \xe2\x95\x91  "
                          << GREEN << net << YELLOW << " dmg\n"
                          << "  \xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x9d\n"
                          << RESET;
                std::cout << DIM << "  [" << at.name << "]\n" << RESET;
            } else {
                std::cout << "You deal " << GREEN << net << " damage" << RESET
                          << " [" << at.name << "]\n";
            }

            // Attack effects (player-side buffs; boss has no status effect support)
            switch (at.effect) {
                case AttackEffect::COMBO_SETUP:
                    player.getBuffs().comboSetupBonus = at.effectValue;
                    std::cout << CYAN << "[COMBO] Next attack +" << at.effectValue << " damage.\n" << RESET;
                    break;
                case AttackEffect::DOUBLE_NEXT_DAMAGE:
                    player.getBuffs().doubleNextDamage = true;
                    std::cout << YELLOW << "[VOID BULWARK] Next attack deals x2 damage.\n" << RESET;
                    break;
                case AttackEffect::DODGE_NEXT_ATTACK:
                    player.getBuffs().dodgeNextAttack = true;
                    std::cout << CYAN << "[WARDING BLOW] Next incoming attack will be dodged.\n" << RESET;
                    break;
                case AttackEffect::EXECUTE_HEAL:
                    if (!boss.isAlive()) {
                        player.heal(at.effectValue);
                        std::cout << GREEN << "[SOUL CLEAVE] Kill heals " << at.effectValue << " HP!\n" << RESET;
                    }
                    break;
                case AttackEffect::SELF_BLEED:
                    player.getBuffs().bleeding = true;
                    std::cout << RED << "[BLOODPRICE] You open your own wound. You are now bleeding.\n" << RESET;
                    break;
                case AttackEffect::REST_TO_HEAL:
                    player.getBuffs().restHealBonus += at.effectValue;
                    std::cout << CYAN << "[STILLNESS] Rest soon to recover " << at.effectValue << " HP.\n" << RESET;
                    break;
                case AttackEffect::APPLY_POISON_AND_COMBO:
                    // Boss has no poison support — apply combo bonus only
                    player.getBuffs().comboSetupBonus += at.effectValue;
                    std::cout << CYAN << "[TOXIN BARB] Next attack +" << at.effectValue << " damage.\n" << RESET;
                    break;
                case AttackEffect::APPLY_BLEED_DEF_HEAL:
                    player.heal(at.effectValue);
                    std::cout << GREEN << "[BLOOD REAPER] Healed " << at.effectValue << " HP."
                              << " (+" << at.defenseBonus << " def this turn)\n" << RESET;
                    break;
                case AttackEffect::APPLY_POISON_ADD:
                case AttackEffect::DOUBLE_POISON_STACKS:
                case AttackEffect::BLIND_IF_POISONED:
                case AttackEffect::BLIND_AMPLIFY:
                case AttackEffect::BLEED_BURST:
                case AttackEffect::STATUS_OVERLOAD:
                case AttackEffect::POISON_BURST:
                    // Boss has no status effect support — no effect
                    break;
                case AttackEffect::DESPERATE_STRIKE:
                    // Handled pre-roll
                    break;
                default: break;
            }

            player.startCooldown(slot);
            if (boss.checkPhaseTransition())
                std::cout << "\n" << YELLOW << BOLD << boss.getPhaseText() << "\n" << RESET;
            if (!boss.isAlive()) bossDead = true;
            return;
        }
        // Unknown input — loop back
    }
}

void Game::enemyAttackTurnBoss(Boss& boss) {
    // Warding Blow: player dodges this entire turn
    if (player.getBuffs().dodgeNextAttack) {
        player.getBuffs().dodgeNextAttack = false;
        std::cout << CYAN << "[WARDING] You sidestep " << boss.getName() << "'s attack completely!\n" << RESET;
        return;
    }

    // Ghost Step: 20% dodge
    if (player.hasRune(RuneType::GHOST_STEP) && randInt(0, 99) < 20) {
        std::cout << CYAN << "[GHOST STEP] You ghost through " << boss.getName() << "'s attack!\n" << RESET;
        return;
    }

    int atk = boss.rollAttack();
    int def = player.rollDefense();
    int dmg = std::max(1, atk - def);

    // Iron Skin passive: 25% damage reduction
    if (player.hasRune(RuneType::IRON_SKIN))
        dmg = std::max(1, static_cast<int>(dmg * 0.75f));

    player.takeDamage(dmg);
    std::cout << RED << boss.getName() << " attacks for " << dmg << " damage.\n" << RESET;
}

bool Game::runBossFight(Boss& boss) {
    player.clearPotionLock();

    while (boss.isAlive() && player.isAlive()) {
        showCombatStatusBoss(boss);
        player.tickBuffs();
        if (!player.isAlive()) break;

        bool bossDead = false;
        playerAttackTurnBoss(boss, bossDead);
        if (bossDead) break;
        if (!player.isAlive()) break;

        enemyAttackTurnBoss(boss);
    }

    player.getBuffs().bleeding     = false;
    player.getBuffs().poisonStacks = 0;
    player.getBuffs().blindRounds  = 0;
    if (!player.isAlive()) { gameOver(); return false; }
    return true;
}

// ── Potion / bargain ──────────────────────────────────────────────────────────

void Game::potionMenu(Enemy* /*enemy*/) {
    player.showInventory();
    if (player.getPotions().empty()) { pressEnter(); return; }

    std::cout << "  0. Cancel\n";
    std::cout << "Use which potion? ";
    int c = 0; std::cin >> c;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (c > 0) player.usePotion(c - 1);
}

bool Game::attemptBargain(Enemy& enemy) {
    int cost = enemy.getBribeCost();
    std::cout << "You offer " << cost << " gold. (You have " << player.getGold() << "g)\n";
    std::cout << "Confirm? (y/n): ";
    char yn; std::cin >> yn;
    std::cin.ignore(10000, '\n');
    if ((yn == 'y' || yn == 'Y') && player.spendGold(cost)) {
        std::cout << GREEN << "The enemy takes your gold and backs away.\n" << RESET;
        return true;
    }
    if (yn == 'y' || yn == 'Y')
        std::cout << RED << "Not enough gold.\n" << RESET;
    return false;
}

bool Game::attemptTalk(Enemy& enemy) {
    std::cout << "\n" << DIM << enemy.getDialogue() << RESET << "\n\n";
    std::cout << "  1. Continue talking (non-hostile exit)\n";
    std::cout << "  2. Attack anyway\n";
    std::cout << "Choice: ";
    int c = 0; std::cin >> c;
    std::cin.ignore(10000, '\n');
    return (c == 1);
}

// ── Victory / game over ───────────────────────────────────────────────────────

void Game::showTwistReveal() {
    clearScreen();
    std::cout << YELLOW << BOLD << "=== A DISCOVERY IN THE MARSH ===\n\n" << RESET;
    slowPrint(
        "In a sunken ruin beneath the fog, you find a chamber with walls covered in "
        "inscriptions.\n\nOld script. Older than the marsh. You cannot read most of it.\n"
        "But someone has translated sections — crude, hurried letters scratched above "
        "the original text.\n\n"
        "\"Three champions volunteered. They understood: to cage the Dragon King, "
        "something had to draw him. A soul without armour. A soul that would wander.\"\n\n"
        "\"They placed the curse on the road itself. On anyone foolish enough to keep "
        "walking east when every sign said stop.\"\n\n"
        "You keep reading.\n\n"
        "\"The wanderer does not choose the path. The path chooses the wanderer.\"\n\n"
        "You look at the road behind you. Every choice. Every turn you took.\n"
        "Every time something pushed you forward.\n\n"
        "You were the bait.\n\n"
        "You still are.", 14);
    pressEnter();
}

// ── Discoveries ───────────────────────────────────────────────────────────────

void Game::showDiscoveries() {
    // Build the master lists from the pools (used for total counts and ordering)
    static const ItemRarity rarityOrder[] = {
        ItemRarity::COMMON, ItemRarity::RARE, ItemRarity::EPIC, ItemRarity::LEGENDARY
    };
    static const char* rarityLabel[] = { "COMMON", "RARE", "EPIC", "LEGENDARY" };

    // Count totals
    int totalAttacks = 0;
    for (auto r : rarityOrder) totalAttacks += (int)getAttackPool(r).size();
    const int totalRunes = 10;

    while (true) {
        clearScreen();
        std::cout << YELLOW << BOLD
                  << "  \xe2\x95\x94\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x97\n"
                  << "  \xe2\x95\x91                      DISCOVERIES                       \xe2\x95\x91\n"
                  << "  \xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x9d\n"
                  << RESET;

        int knownAttacks = (int)discoveries.attacks.size();
        int knownRunes   = (int)discoveries.runes.size();
        std::cout << "\n  " << CYAN << "Skills: " << knownAttacks << " / " << totalAttacks << RESET
                  << "    " << CYAN << "Runes: " << knownRunes << " / " << totalRunes << RESET << "\n\n";

        std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Skills\n";
        std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  Runes\n";
        std::cout << BOLD << YELLOW << "  [ 0 ]" << RESET << "  Back\n";
        std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
        std::cout << "\n  Choice: ";

        int c = 0; std::cin >> c;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (c == 0) return;

        if (c == 1) {
            // ── Skills ────────────────────────────────────────────────────────
            clearScreen();
            std::cout << YELLOW << BOLD
                      << "  ─────────────────────── SKILLS ────────────────────────\n\n" << RESET;

            for (int ri = 0; ri < 4; ++ri) {
                ItemRarity rar = rarityOrder[ri];
                auto pool = getAttackPool(rar);
                int knownInTier = 0;
                for (auto& at : pool)
                    if (discoveries.attacks.count(at.name)) ++knownInTier;

                std::cout << Item::getRarityColor(rar) << BOLD
                          << "  " << rarityLabel[ri]
                          << DIM << "  (" << knownInTier << "/" << (int)pool.size() << ")"
                          << RESET << "\n";

                for (auto& at : pool) {
                    if (discoveries.attacks.count(at.name)) {
                        std::cout << "  " << Item::getRarityColor(at.rarity) << BOLD
                                  << at.name << RESET;
                        std::cout << DIM << "  [" << at.getStatsString() << "]\n";
                        std::cout << "    " << at.description << "\n" << RESET;
                    } else {
                        std::cout << DIM << "  ???\n" << RESET;
                    }
                }
                std::cout << "\n";
            }
            pressEnter();
        }

        if (c == 2) {
            // ── Runes ─────────────────────────────────────────────────────────
            clearScreen();
            std::cout << CYAN << BOLD
                      << "  ──────────────────────── RUNES ────────────────────────\n\n" << RESET;

            static const RuneType allRunes[] = {
                RuneType::IRON_SKIN,   RuneType::BLOODHUNGER, RuneType::STEADY_EYE,
                RuneType::GHOST_STEP,  RuneType::PREDATOR,    RuneType::DEEP_CUTS,
                RuneType::TOXIN_RIDER, RuneType::PLAGUE_WARD, RuneType::ASH_VEIL,
                RuneType::NIGHTKEEPER
            };

            for (auto r : allRunes) {
                if (discoveries.runes.count(static_cast<int>(r))) {
                    std::cout << "  " << CYAN << BOLD << runeName(r) << RESET << "\n";
                    std::cout << DIM << "    " << runeDescription(r) << "\n\n" << RESET;
                } else {
                    std::cout << DIM << "  ???\n\n" << RESET;
                }
            }
            pressEnter();
        }
    }
}

void Game::showTutorial() {
    bool inTutorial = true;
    while (inTutorial) {
        clearScreen();
        std::cout << YELLOW << BOLD << "═══════════════════════════════════════════════\n";
        std::cout <<                   "              HOW TO PLAY\n";
        std::cout <<                   "═══════════════════════════════════════════════\n" << RESET;
        std::cout << "\n  1. Combat Basics\n";
        std::cout <<   "  2. Status Effects\n";
        std::cout <<   "  3. Runes\n";
        std::cout <<   "  4. The Map\n";
        std::cout <<   "  0. Back\n\n";
        std::cout << "Choice: ";

        int c = 0; std::cin >> c;
        if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000,'\n'); continue; }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (c == 0) { inTutorial = false; break; }

        clearScreen();
        switch (c) {
        case 1:
            std::cout << YELLOW << BOLD << "── COMBAT BASICS ──────────────────────────────\n" << RESET;
            std::cout <<
                "\nEach combat turn you choose an action:\n"
                "  [1-3] Attack Slot — use one of your three attack types\n"
                "  [4]   Rest        — skip your turn, recover all stamina\n"
                "  [5]   Inventory   — use a potion (FREE, no turn cost)\n"
                "  [6]   Talk        — attempt to reason with the enemy\n"
                "  [7]   Bargain     — bribe the enemy with gold\n\n"
                << BOLD << "Attack Types\n" << RESET <<
                "  You carry 3 attack slots. Each attack has a cooldown (1-2 turns).\n"
                "  With 3 slots and max 2-turn cooldowns, you always have at least one ready.\n"
                "  Attacks come in rarities: Common, Rare, Epic, Legendary.\n"
                "  Each attack has unique dice damage, effects, and defense bonuses.\n\n"
                << BOLD << "Timing Bar\n" << RESET <<
                "  A cursor moves across the bar. Press SPACE to stop it.\n"
                "  Hit the marked crit zone for bonus damage (very tight — 1 position).\n"
                "  The Steady Eye rune widens the crit zone by 2 positions.\n\n"
                << BOLD << "Stamina\n" << RESET <<
                "  All attacks cost 1 stamina. Rest recovers all stamina (loses your turn).\n\n"
                << BOLD << "Bargaining & Talking\n" << RESET <<
                "  Some enemies can be talked to or bribed with gold to end the fight.\n";
            break;
        case 2:
            std::cout << YELLOW << BOLD << "── STATUS EFFECTS ─────────────────────────────\n" << RESET;
            std::cout <<
                "\nStatus effects are applied by weapons on hit (30% base chance).\n"
                "Your Luck stat increases this chance (+0.5% per point).\n"
                "Each weapon can only carry one status type.\n\n"
                << RED << BOLD << "Bleed\n" << RESET <<
                "  Enemy loses 2 HP at the start of each of their turns.\n"
                "  Permanent — only ends when they die.\n\n"
                << RED << BOLD << "Poison\n" << RESET <<
                "  Applies 5 stacks. Each turn: enemy takes X damage, stacks drop by 1.\n"
                "  5 stacks = 15 total damage (5+4+3+2+1) if nothing adds stacks.\n"
                "  Re-applying poison adds to existing stacks (4 + 5 = 9).\n\n"
                << YELLOW << BOLD << "Blind\n" << RESET <<
                "  Enemy has a 30% chance to miss each attack for 2 rounds.\n\n"
                << BOLD << "Enemies can apply status effects to you too:\n" << RESET <<
                "  Ash Hound    — Bleed (each hit, 25% chance)\n"
                "  Ember Witch  — Blind (per attack, 33% chance)\n"
                "  Plague Shaman — Poison (50% chance) + permanently destroys one potion\n\n"
                "  Use an " << GREEN << "Antidote" << RESET << " to cure all status effects at once.\n";
            break;
        case 3:
            std::cout << YELLOW << BOLD << "── RUNES ────────────────────────────────────────\n" << RESET;
            std::cout <<
                "\nRunes are always-on bonuses that shape how you fight.\n"
                "You have 2 rune slots. Unequipped runes are saved in your library.\n"
                "Reassign runes at Rest nodes or the Town of Dead Ballads.\n"
                "Learn new runes from M'alid at [?] nodes on the map.\n\n"
                << BOLD << "General Runes:\n" << RESET <<
                "  Iron Skin    — Incoming damage reduced by 25%\n"
                "  Bloodhunger  — Critical hits deal x1.5 damage\n"
                "  Steady Eye   — Crit zone on the timing bar is 2 positions wider\n"
                "  Ghost Step   — 20% chance to fully dodge an incoming attack\n"
                "  Predator     — Bleeding enemies deal 40% less damage to you\n\n"
                << BOLD << "Bleed / Poison Runes:\n" << RESET <<
                "  Deep Cuts    — Bleed deals 5 damage per turn instead of 2\n"
                "  Toxin Rider  — Each hit adds +1 poison stack (only if enemy is poisoned)\n"
                "  Plague Ward  — You are immune to poison\n\n"
                << BOLD << "Blind Runes:\n" << RESET <<
                "  Ash Veil     — Blinded enemies miss 50% of attacks (up from 30%)\n"
                "  Nightkeeper  — While an enemy is blinded: blind persists; 40% miss chance\n";
            break;
        case 4:
            std::cout << YELLOW << BOLD << "── THE MAP ─────────────────────────────────────\n" << RESET;
            std::cout <<
                "\nEach chapter has a branching path of nodes. Choose your route.\n\n"
                "  [FIGHT]  — Standard enemy encounter\n"
                "  [ELITE]  — Stronger enemy, better rewards\n"
                "  [EVENT]  — Story event, quest triggers, random rewards or penalties\n"
                "  [  ?  ]  — Random event or M'alid the Rune Teacher (25% chance)\n"
                "  [SHOP ]  — Buy potions and equipment\n"
                "  [ REST]  — Recover 25% HP, full stamina, reassign runes, clear statuses\n"
                "  [ BOSS]  — Chapter boss. Defeat it to progress.\n\n"
                << BOLD << "Death is permanent.\n" << RESET <<
                "  When you die, the run ends. All progress resets.\n\n"
                << BOLD << "Luck\n" << RESET <<
                "  Luck improves item drop quality, status effect proc chances,\n"
                "  and the chance a looted weapon carries a status effect.\n";
            break;
        default:
            continue;
        }
        pressEnter();
    }
}

void Game::showVictory(bool trueEnding) {
    clearScreen();
    if (trueEnding) {
        std::cout << YELLOW << BOLD;
        std::cout << "\n╔══════════════════════════════════════════════════╗\n";
        std::cout << "║              TRUE ENDING ACHIEVED                ║\n";
        std::cout << "╚══════════════════════════════════════════════════╝\n" << RESET;
        slowPrint(
            "\nThe chains are gone. The Dragon King's half-form collapses into ash "
            "without its herald.\n\nThe three champion-spirits stand at the edge of the "
            "Obsidian Reach, facing east. One by one, they dissolve — not violently. "
            "Like breath on a cold morning.\n\n"
            "You are a peasant, standing in the ruins of a fortress, holding a banner "
            "that belongs to someone who has been dead for a hundred years.\n\n"
            "You fold it carefully.\n\n"
            "The road west is open.\n\n"
            "You start walking.", 14);
    } else {
        std::cout << GREEN << BOLD;
        std::cout << "\n╔══════════════════════════════════════════════════╗\n";
        std::cout << "║                    VICTORY                       ║\n";
        std::cout << "╚══════════════════════════════════════════════════╝\n" << RESET;
        slowPrint(
            "\nThe Darkblood Lord is defeated. The fortress goes quiet.\n\n"
            "You don't know what happens next. The Darkbloods were a warning nobody "
            "listened to. The Dragon King is still out there, somewhere — half-free, "
            "half-bound.\n\n"
            "But you are alive. And the road is still open.\n\n"
            "For now, that is enough.", 14);
    }

    std::cout << "\n\n" << YELLOW;
    std::cout << "  Enemies defeated: " << runState.enemiesDefeated << "\n";
    std::cout << "  Gold earned:      " << runState.totalGoldEarned << "\n";
    std::cout << "  True ending:      " << (trueEnding ? "YES" : "No") << "\n" << RESET;
    pressEnter();
}

void Game::showQuestLog() const {
    std::cout << "\n─── Quest Log ───────────────────────────────────────────\n";

    bool any = false;

    // Banner quest (always show once found)
    if (runState.bannerFound || runState.veteranMet || runState.bannerComplete) {
        any = true;
        std::cout << (runState.bannerComplete ? GREEN "[x]" : CYAN "[>]") << RESET
                  << " The Supreme Peasant's Road";
        if (runState.bannerComplete)       std::cout << " — COMPLETE";
        else if (runState.veteranMet)      std::cout << " (2/3 — find the grave in Ch.3)";
        else if (runState.bannerFound)     std::cout << " (1/3 — show the banner to someone)";
        std::cout << "\n";
    }

    // Quest 1: The Debt Ledger
    if (runState.debtLedgerFound || runState.debtQuestDone) {
        any = true;
        std::cout << (runState.debtQuestDone ? GREEN "[x]" : CYAN "[>]") << RESET
                  << " The Debt Ledger";
        if (runState.debtQuestDone) std::cout << " — COMPLETE (+50 gold)";
        else std::cout << " (" << runState.debtKillCount << "/5 kills)";
        std::cout << "\n";
    }

    // Quest 2: The Third Face
    if (runState.shrineQuestActive || runState.shrineQuestDone) {
        any = true;
        std::cout << (runState.shrineQuestDone ? GREEN "[x]" : CYAN "[>]") << RESET
                  << " The Third Face";
        if (runState.shrineQuestDone) std::cout << " — COMPLETE (+15 max HP)";
        else std::cout << " (find the vandal in Ch.2)";
        std::cout << "\n";
    }

    // Quest 3: Something from the East
    if (runState.easternWarningFound || runState.easternWarningDone) {
        any = true;
        std::cout << (runState.easternWarningDone ? GREEN "[x]" : CYAN "[>]") << RESET
                  << " Something from the East";
        if (runState.easternWarningDone) std::cout << " — COMPLETE (+4 luck)";
        else std::cout << " (follow the trail in Ch.3)";
        std::cout << "\n";
    }

    // Quest 4: The Deserter's Cipher
    if (runState.deserterMet || runState.cipherDecoded) {
        any = true;
        std::cout << (runState.cipherDecoded ? GREEN "[x]" : CYAN "[>]") << RESET
                  << " The Deserter's Cipher";
        if (runState.cipherDecoded) std::cout << " — COMPLETE (+2 weapon modifier)";
        else std::cout << " (find the one-time pad in Ch.3)";
        std::cout << "\n";
    }

    // Quest 5: What They Built
    if (runState.architectureFound) {
        any = true;
        std::cout << GREEN "[x]" << RESET << " What They Built — COMPLETE (+30 gold)\n";
    }

    if (!any)
        std::cout << "  (no quests found yet — explore ? nodes)\n";

    std::cout << "─────────────────────────────────────────────────────────\n";
}

void Game::gameOver() {
    clearScreen();
    std::cout << RED << BOLD;
    std::cout << "\n╔══════════════════════════════════════════════════╗\n";
    std::cout << "║                  YOU HAVE FALLEN                 ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n" << RESET;
    slowPrint(
        "\nAnother peasant who walked east and didn't come back.\n"
        "The road doesn't care. It's been here longer than you,\n"
        "and it will be here long after.\n\n"
        "But you know more now than when you started.\n"
        "The next one will know a little more still.", 14);
    std::cout << "\n" << DIM << "(Permadeath — knowledge survives, the peasant does not)" << RESET << "\n";
    pressEnter();
}
