#include "../include/Game.h"
#include "../include/Stance.h"
#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <algorithm>
#include <ctime>

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
        std::cout << BOLD << YELLOW  << "  [ 3 ]" << RESET << "  Exit\n\n";

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
        else if (c == 3) isRunning = false;
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
    slowPrint(
        "  The sky has been hollow since the Supreme Peasant fell.\n"
        "  He traded his life to slay the Dragon King, but in the silence\n"
        "  of his absence, Peasaterra began to rot.\n", 18);
    std::cout << "\n";
    slowPrint(
        "  A grey blight now chokes our fields, turning golden wheat into\n"
        "  weeping ash. We are starving, brittle, and broken — and the\n"
        "  Darkbloods know it. They have crossed the borders of men,\n"
        "  slaughtering families and burning what little we have left.\n"
        "  They mistake our hunger for weakness.\n", 18);
    std::cout << "\n";
    slowPrint("  They are wrong.\n", 22);
    std::cout << "\n";
    slowPrint(
        "  We have traded our plows for pikes and our scythes for war.\n"
        "  Our hands are stained with soil, but they are hardened by a\n"
        "  lifetime of toil. As the Darkblood tide approaches, we do not\n"
        "  march for glory or gold. We march so that the earth might finally\n"
        "  drink the blood of those who dared to trample it.\n", 18);
    std::cout << "\n";
    slowPrint("  The harvest is here. And this time, we are the reapers.\n", 22);
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
    static const char* intros[] = { "",
        // Chapter 1
        "THE ASHFIELDS\n\n"
        "Scorched plains stretch in every direction. The crops are dead. The farmhouses "
        "are dead. Even the smoke is old.\n"
        "Somewhere ahead, the Darkbloods are moving. You don't know what they want.\n"
        "You know only that people keep pointing in this direction and not coming back.",
        // Chapter 2
        "THE HOLLOWED MARSH\n\n"
        "The ground is wet here. Always. The fog doesn't lift — it just thins, enough "
        "to see the next dead tree, the next sunken ruin, the next pair of eyes watching "
        "from the water.\n"
        "The Witch of G'haal holds dominion here. Ask three different marsh-dwellers "
        "about her and you'll get three different stories, all of them wrong.",
        // Chapter 3
        "THE OBSIDIAN REACH\n\n"
        "Black stone. Dragon-bone architecture. Ash storms that taste like old blood.\n"
        "This is where the Darkbloods live. Or lived. The distinction is becoming unclear.\n"
        "You know the truth now — or some of it. Enough to be dangerous.\n"
        "Enough to finish this."
    };
    std::cout << YELLOW << BOLD << intros[ch] << RESET << "\n";
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
                  << "  ══════════════════════════════════════════════════════════\n"
                  << "                     ☠  ELITE ENCOUNTER  ☠\n"
                  << "  ══════════════════════════════════════════════════════════\n"
                  << RESET;
    } else {
        std::cout << RED
                  << "  ──────────────────────────────────────────────────────────\n"
                  << "                       ⚠  ENEMY AHEAD  ⚠\n"
                  << "  ──────────────────────────────────────────────────────────\n"
                  << RESET;
    }

    std::cout << "\n  " << BOLD << enemy.getName() << RESET << "\n\n";
    std::cout << DIM << "  " << enemy.getDescription() << RESET << "\n\n";
    std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
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

    // Loot drop
    Item drop = enemy.generateDrop(player.getLuck());
    std::cout << Item::getRarityColor(drop.getRarity()) << BOLD
              << "Loot: " << drop.getName() << " [" << drop.getStatsString() << "]\n"
              << RESET;
    std::cout << "  1. Equip   2. Sell (" << drop.getSellValue() << " gold)\n";
    std::cout << "Choice: ";
    int lc = 0; std::cin >> lc;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (lc == 1) {
        if (drop.getType() == ItemType::WEAPON) player.equipWeapon(drop);
        else                                     player.equipArmor(drop);
    } else if (lc == 2) {
        int sv = drop.getSellValue();
        player.addGold(sv);
        std::cout << YELLOW << "Sold for " << sv << " gold.\n" << RESET;
    }
    pressEnter();
}

// ── Event node ────────────────────────────────────────────────────────────────

void Game::handleEventNode(int ch) {
    clearScreen();

    // ── Priority 0: M'alid the Stance Teacher (25% chance if stances unlearned) ─
    {
        const std::vector<StanceType> allStances = {
            StanceType::DEFENSIVE, StanceType::OFFENSIVE, StanceType::FOCUSED,
            StanceType::EVASIVE,   StanceType::BLOODLUST,  StanceType::HEMORRHAGE,
            StanceType::VENOMOUS,  StanceType::RESISTANT,  StanceType::OBSCURED,
            StanceType::ETERNAL_DARK
        };
        std::vector<StanceType> unlearned;
        for (auto& s : allStances)
            if (!player.hasLearnedStance(s)) unlearned.push_back(s);

        if (!unlearned.empty() && randInt(0, 3) == 0) { // 25% chance
            std::cout << YELLOW << BOLD << "=== ? ===\n" << RESET;
            slowPrint(
                "An old figure sits cross-legged at the roadside. No fire. No shelter.\n"
                "He looks up as though he expected you specifically.\n\n"
                "\"M'alid,\" he says, by way of introduction. \"Sit.\"", 14);
            std::cout << "\n" << DIM << "\"I can show you something. Choose what you wish to learn.\"\n\n" << RESET;

            std::vector<StanceType> offered;
            std::vector<StanceType> pool = unlearned;
            while (offered.size() < 4 && !pool.empty()) {
                int idx = randInt(0, (int)pool.size() - 1);
                offered.push_back(pool[idx]);
                pool.erase(pool.begin() + idx);
            }

            for (int i = 0; i < (int)offered.size(); ++i)
                std::cout << "  " << (i+1) << ". " << CYAN << stanceName(offered[i]) << RESET
                          << " — " << stanceDescription(offered[i]) << "\n";
            std::cout << "  0. Decline\nYour choice: ";

            int c = 0; std::string inp; std::getline(std::cin, inp);
            try { c = std::stoi(inp); } catch (...) {}
            if (c >= 1 && c <= (int)offered.size()) {
                player.learnStance(offered[c - 1]);
                std::cout << GREEN << "\nYou learn " << stanceName(offered[c - 1]) << ".\n" << RESET;
                std::cout << DIM << "M'alid nods and looks back at the road.\n" << RESET;
            } else {
                std::cout << DIM << "You walk on.\n" << RESET;
            }
            pressEnter();
            return;
        }

        if (unlearned.empty() && randInt(0, 3) == 0) {
            std::cout << YELLOW << BOLD << "=== ? ===\n" << RESET;
            slowPrint("M'alid sits at the roadside, as always.\n\n"
                      "\"You carry every form I know,\" he says. \"Go.\"", 14);
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
        std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Blacksmith's Ashen Forge\n";
        std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  The Wanderer's Curio\n";
        std::cout << BOLD << YELLOW << "  [ 3 ]" << RESET << "  The Crooked Spit\n";
        std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  Leave town\n";
        std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
        std::cout << "\n  Choice: ";

        int c = 0; std::cin >> c;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (c) {
            case 1: {
                // ── Blacksmith's Ashen Forge ──────────────────────────────────
                clearScreen();
                std::cout << YELLOW << BOLD
                          << "  ╔══════════════════════════════════════════════════════════╗\n"
                          << "  ║               BLACKSMITH'S  ASHEN  FORGE                ║\n"
                          << "  ╚══════════════════════════════════════════════════════════╝\n"
                          << RESET << "\n";
                slowPrint(
                    "  She doesn't make weapons. She breaks them open and puts something worse inside.\n"
                    "  The forge behind her burns the wrong colour — too blue, too still.\n"
                    "  \"Infusion costs 120 gold,\" she says, not looking up.\n"
                    "  \"Replaces what's already there, if anything is.\"", 13);
                std::cout << "\n";

                Item& w = player.getWeapon();
                std::string curEffect = w.getStatusEffectName();
                std::cout << BOLD << "Current weapon: " << RESET
                          << w.getName() << " [" << w.getStatsString() << "]\n";
                if (curEffect == "None") {
                    std::cout << DIM << "  No infusion yet.\n" << RESET;
                } else {
                    std::cout << RED << "  Current infusion: " << curEffect << RESET
                              << " — will be replaced.\n";
                }

                std::cout << DIM << "\n  ──────────────────────────────────────────────────────────\n" << RESET;
                std::cout << YELLOW << "  Gold: " << player.getGold() << RESET << "\n\n";
                std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Bleed Edge    " << DIM << "120g  ·  30%+luck chance to Bleed on hit\n"  << RESET;
                std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  Marsh Toxin   " << DIM << "120g  ·  30%+luck chance to Poison on hit\n" << RESET;
                std::cout << BOLD << YELLOW << "  [ 3 ]" << RESET << "  Ash Veil      " << DIM << "120g  ·  30%+luck chance to Blind on hit\n"  << RESET;
                std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  Leave\n";
                std::cout << DIM << "  ──────────────────────────────────────────────────────────\n" << RESET;
                std::cout << "\n  Choice: ";

                int bc = 0; std::cin >> bc;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (bc >= 1 && bc <= 3) {
                    if (player.spendGold(120)) {
                        StatusEffect chosen = StatusEffect::NONE;
                        std::string effectName;
                        if      (bc == 1) { chosen = StatusEffect::BLEED;  effectName = "Bleed Edge"; }
                        else if (bc == 2) { chosen = StatusEffect::POISON; effectName = "Marsh Toxin"; }
                        else              { chosen = StatusEffect::BLIND;  effectName = "Ash Veil"; }
                        w.setStatusEffect(chosen);
                        std::cout << GREEN << "\nDone. Your weapon carries " << effectName << " now.\n" << RESET;
                        std::cout << RED << DIM
                                  << "She doesn't look satisfied. She never does.\n" << RESET;
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

    int healAmt = player.getMaxHealth() / 4;
    player.heal(healAmt);
    player.restoreStamina(player.getMaxStamina());
    player.getBuffs().bleeding     = false;
    player.getBuffs().poisonStacks = 0;
    player.getBuffs().blindRounds  = 0;

    std::cout << GREEN << "You rest. +" << healAmt << " HP. Full stamina restored. All status effects cleared.\n" << RESET;
    std::cout << "Current HP: " << player.getHealth() << "/" << player.getMaxHealth() << "\n";
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
int Game::runTimingBar(bool isHeavy, int& outCritPos) {
    const int TOTAL    = 20;
    const int CRIT_W   = isHeavy ? 1 : 3; // heavy crit: 1 pixel — very tight
    outCritPos = randInt(CRIT_W, TOTAL - CRIT_W - 1);

    // Build label row showing crit zone with '*'
    std::string label(TOTAL, '-');
    for (int i = outCritPos - CRIT_W/2;
             i <= outCritPos + CRIT_W/2 && i < TOTAL; ++i)
        if (i >= 0) label[i] = '*';

    std::cout << "\n  Time your attack! Press SPACE at the right moment!\n";
    std::cout << "  [" << label << "]  <- crit zone\n";

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
        std::string bar(TOTAL, ' ');
        bar[cursor] = isHeavy ? 'H' : 'L';
        std::cout << "\r  [" << bar << "]" << std::flush;
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
    if (player.hasStanceActive())
        std::cout << "  " << CYAN << "[" << stanceName(player.getActiveStance()) << "]" << RESET;
    std::cout << "\n";

    std::cout << "  HP  " << hpColor(ph, phm) << makeBar(ph, phm) << RESET
              << "  " << ph << "/" << phm << "\n";
    std::cout << "  ST  " << CYAN << makeBar(ps, psm) << RESET
              << "  " << ps << "/" << psm << "\n";

    if (player.getBuffs().bleeding)
        std::cout << "      " << RED    << "♦ Bleeding (2 dmg/turn)" << RESET << "\n";
    if (player.getBuffs().poisonStacks > 0)
        std::cout << "      " << RED    << "♦ Poisoned (" << player.getBuffs().poisonStacks << " stacks)" << RESET << "\n";
    if (player.getBuffs().blindRounds > 0)
        std::cout << "      " << YELLOW << "♦ Blinded (" << player.getBuffs().blindRounds << " rounds)" << RESET << "\n";
    if (player.getBuffs().shieldTonicTurns > 0)
        std::cout << "      " << CYAN   << "♦ Shield Tonic (" << player.getBuffs().shieldTonicTurns << " turns)" << RESET << "\n";
    if (player.getBuffs().rageDraughtActive)
        std::cout << "      " << YELLOW << "♦ Rage Draught (next hit ×1.5)" << RESET << "\n";
    if (player.getBuffs().phantomsDraughtActive)
        std::cout << "      " << MAGENTA << "♦ Phantom's Draught (next hit bypasses defense)" << RESET << "\n";
    if (player.getBuffs().gravewardenOilCharges > 0)
        std::cout << "      " << RED    << "♦ Gravewarden Oil (" << player.getBuffs().gravewardenOilCharges << " charges)" << RESET << "\n";

    std::cout << "\n";
    std::cout << DIM << "  ┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄\n" << RESET;
    std::cout << "\n";

    // ── Enemy ─────────────────────────────────────────────────────────────────
    int eh = enemy.getHealth(), ehm = enemy.getMaxHealth();
    std::cout << "  " << RED << BOLD << enemy.getName() << RESET << "\n";
    std::cout << "  HP  " << hpColor(eh, ehm) << makeBar(eh, ehm) << RESET
              << "  " << eh << "/" << ehm << "\n";

    if (enemy.isBleeding())
        std::cout << "      " << RED    << "♦ Bleeding" << RESET << "\n";
    if (enemy.isPoisoned())
        std::cout << "      " << RED    << "♦ Poisoned (" << enemy.getPoisonStacks() << " stacks)" << RESET << "\n";
    if (enemy.isBlinded())
        std::cout << "      " << YELLOW << "♦ Blinded (" << enemy.getBlindRounds() << " rounds)" << RESET << "\n";

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
    if (player.hasStanceActive())
        std::cout << "  " << CYAN << "[" << stanceName(player.getActiveStance()) << "]" << RESET;
    std::cout << "\n";

    std::cout << "  HP  " << hpColor(ph, phm) << makeBar(ph, phm) << RESET
              << "  " << ph << "/" << phm << "\n";
    std::cout << "  ST  " << CYAN << makeBar(ps, psm) << RESET
              << "  " << ps << "/" << psm << "\n";

    if (player.getBuffs().bleeding)
        std::cout << "      " << RED    << "♦ Bleeding" << RESET << "\n";
    if (player.getBuffs().poisonStacks > 0)
        std::cout << "      " << RED    << "♦ Poisoned (" << player.getBuffs().poisonStacks << " stacks)" << RESET << "\n";
    if (player.getBuffs().blindRounds > 0)
        std::cout << "      " << YELLOW << "♦ Blinded (" << player.getBuffs().blindRounds << " rounds)" << RESET << "\n";
    if (player.getBuffs().shieldTonicTurns > 0)
        std::cout << "      " << CYAN   << "♦ Shield Tonic (" << player.getBuffs().shieldTonicTurns << " turns)" << RESET << "\n";
    if (player.getBuffs().rageDraughtActive)
        std::cout << "      " << YELLOW << "♦ Rage Draught (next hit ×1.5)" << RESET << "\n";
    if (player.getBuffs().phantomsDraughtActive)
        std::cout << "      " << MAGENTA << "♦ Phantom's Draught (next hit bypasses defense)" << RESET << "\n";
    if (player.getBuffs().gravewardenOilCharges > 0)
        std::cout << "      " << RED    << "♦ Gravewarden Oil (" << player.getBuffs().gravewardenOilCharges << " charges)" << RESET << "\n";

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

    // Inventory loop: potions are free — keep showing menu until player takes a real action
    while (true) {
        std::cout << DIM << "  ─────────────────────── YOUR TURN ───────────────────────\n" << RESET;
        std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Light Attack     " << DIM << "· 1 stamina\n"             << RESET;
        std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  Heavy Attack     " << DIM << "· 2 stamina\n"             << RESET;
        std::cout << BOLD << YELLOW << "  [ 3 ]" << RESET << "  Rest             " << DIM << "· recover all stamina\n"   << RESET;
        std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  Inventory        " << DIM << "· free action\n"           << RESET;
        std::cout << BOLD << YELLOW << "  [ 5 ]" << RESET << "  Stance           " << DIM << "· "
                  << (player.hasStanceActive() ? stanceName(player.getActiveStance()) : "None") << "  (free)\n" << RESET;
        if (enemy.canTalk())
            std::cout << BOLD << YELLOW << "  [ 6 ]" << RESET << "  Talk\n";
        std::cout << BOLD << YELLOW << "  [ 7 ]" << RESET << "  Bargain          " << DIM << "· pay " << enemy.getBribeCost() << "g\n" << RESET;
        std::cout << DIM << "  ──────────────────────────────────────────────────────\n" << RESET;
        std::cout << "  Choice: ";

        std::string input;
        std::getline(std::cin, input);
        int c = 0;
        try { c = std::stoi(input); } catch (...) {}

        if (c == 4) {
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

        if (c == 5) {
            // Stance switch — free action
            const auto& stances = player.getLearnedStances();
            std::cout << CYAN << "\nKnown stances:\n" << RESET;
            for (int i = 0; i < (int)stances.size(); ++i) {
                std::cout << "  " << (i+1) << ". " << stanceName(stances[i]);
                if (stances[i] == player.getActiveStance()) std::cout << " [active]";
                std::cout << " — " << stanceDescription(stances[i]) << "\n";
            }
            std::cout << "  0. Cancel\nSwitch to: ";
            std::string si; std::getline(std::cin, si);
            int sc = 0;
            try { sc = std::stoi(si); } catch (...) {}
            if (sc >= 1 && sc <= (int)stances.size()) {
                player.switchStance(stances[sc - 1]);
                std::cout << CYAN << "You shift into " << stanceName(player.getActiveStance()) << " stance.\n" << RESET;
            }
            showCombatStatus(enemy);
            continue; // free action
        }

        if (c == 3) {
            player.restoreStamina(player.getMaxStamina());
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

        // Attack actions
        if (c == 1 || c == 2) {
            bool isHeavy  = (c == 2);
            bool isCrit   = false;

            // Focused stance: +2 crit positions for heavy attacks
            // (runTimingBar uses CRIT_W from its local const — we pass a flag via a helper)
            // For now, Focused widens crit visually by passing wider CRIT_W — handled in runTimingBar
            // We store the focused bonus in a local var for timingBasedAttack comparison
            bool focusedBonus = isHeavy && player.hasStanceActive()
                                && player.getActiveStance() == StanceType::FOCUSED;

            // Blind check: player is blinded — 30% miss chance
            if (player.getBuffs().blindRounds > 0 && randInt(0, 99) < 30) {
                std::cout << YELLOW << "[BLIND] You swing and miss! Your vision fails you.\n" << RESET;
                --player.getBuffs().blindRounds;
                if (player.getBuffs().blindRounds == 0)
                    std::cout << "Your sight returns.\n";
                return; // turn wasted
            }
            if (player.getBuffs().blindRounds > 0)
                --player.getBuffs().blindRounds; // successful attack still ticks blind

            // Darkblood Sentinel: interrupt timing bar
            bool interrupted = false;
            if (beh.interruptsTimingBar && randInt(1,3) == 1) {
                std::cout << RED << "The Sentinel interrupts your swing!\n" << RESET;
                interrupted = true;
            }

            // Focused stance: treat heavy as light for timing bar → wider (3-pos) crit zone
            bool heavyForBar = isHeavy && !focusedBonus;
            int critPos   = 10;
            int cursorPos = interrupted ? randInt(0, 5) : runTimingBar(heavyForBar, critPos);
            int dmg = player.timingBasedAttack(cursorPos, critPos, 20, isHeavy, isCrit);
            if (dmg == 0) { continue; }

            // Offensive stance: crit damage x1.5
            if (isCrit && player.hasStanceActive() && player.getActiveStance() == StanceType::OFFENSIVE)
                dmg = static_cast<int>(dmg * 1.5f);

            // Gravewarden Oil: guaranteed Bleed on hit (charges from town potion)
            if (player.getBuffs().gravewardenOilCharges > 0) {
                enemy.applyBleed();
                --player.getBuffs().gravewardenOilCharges;
                std::cout << RED << "[GRAVEWARDEN OIL] Bleed applied to " << enemy.getName()
                          << "! (" << player.getBuffs().gravewardenOilCharges << " charges left)\n" << RESET;
            }

            // Weapon status proc (30% + luck*0.5%) — fires on any swing, even dodged
            StatusEffect weaponStatus = player.getWeapon().getStatusEffect();
            if (weaponStatus != StatusEffect::NONE) {
                int procChance = 30 + static_cast<int>(player.getLuck() * 0.5f);
                if (randInt(0, 99) < procChance) {
                    switch (weaponStatus) {
                        case StatusEffect::BLEED:
                            enemy.applyBleed();
                            std::cout << RED << "[BLEED] applied to " << enemy.getName() << "!\n" << RESET;
                            break;
                        case StatusEffect::POISON:
                            enemy.applyPoison(5);
                            std::cout << RED << "[POISON] applied to " << enemy.getName() << "! (5 stacks)\n" << RESET;
                            break;
                        case StatusEffect::BLIND:
                            enemy.applyBlind(3);
                            std::cout << YELLOW << "[BLIND] applied to " << enemy.getName() << "! (2 rounds)\n" << RESET;
                            break;
                        default: break;
                    }
                }
            }

            // Marsh Wraith: dodge light attacks (proc already applied above)
            if (!isHeavy && beh.dodgesLightAttacks) {
                std::cout << MAGENTA << "The Wraith phases through your attack!\n" << RESET;
                return;
            }

            // Ash Titan: reflect heavy attacks
            if (isHeavy && beh.reflectsHeavy) {
                int reflected = dmg / 2;
                player.takeDamage(reflected);
                std::cout << RED << "The Titan reflects " << reflected << " damage back at you!\n" << RESET;
                dmg /= 2;
            }

            int def = 0;
            if (player.getBuffs().phantomsDraughtActive) {
                std::cout << MAGENTA << "[PHANTOM'S DRAUGHT] Your strike passes through their guard!\n" << RESET;
                player.getBuffs().phantomsDraughtActive = false;
            } else {
                def = enemy.rollDefense();
            }
            int net = std::max(1, dmg - def);
            enemy.takeDamage(net);
            std::cout << "You deal " << GREEN << net << " damage" << RESET
                      << " (" << (isHeavy ? "heavy" : "light") << ")";
            if (isCrit) std::cout << " [CRIT]";
            std::cout << "\n";

            // Venomous: +1 poison stack if enemy already poisoned
            if (player.hasStanceActive() && player.getActiveStance() == StanceType::VENOMOUS
                && enemy.isPoisoned()) {
                enemy.applyPoison(1);
                std::cout << RED << "[VENOMOUS] Poison stack +1 -> " << enemy.getPoisonStacks() << "\n" << RESET;
            }

            if (!enemy.isAlive()) enemyDead = true;
            return;
        }

        // Unknown input — loop back
    } // end while(true)
}

// ── Enemy attack turn ─────────────────────────────────────────────────────────

void Game::enemyAttackTurn(Enemy& enemy) {
    const EnemyBehaviour& beh = enemy.getBehaviour();

    // Tick enemy status effects at start of their turn
    if (enemy.isBleeding()) {
        int bleedDmg = (player.hasStanceActive() && player.getActiveStance() == StanceType::HEMORRHAGE)
                       ? 5 : 2;
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

    // Eternal Dark: keep enemy blinded while stance is active
    if (player.hasStanceActive() && player.getActiveStance() == StanceType::ETERNAL_DARK) {
        enemy.applyBlind(999); // effectively permanent while stance held
    }

    // HP regen (Bog Colossus)
    if (beh.regenHPPerTurn) {
        enemy.heal(beh.regenAmount);
        std::cout << YELLOW << enemy.getName() << " regenerates " << beh.regenAmount << " HP.\n" << RESET;
    }

    // Determine miss chance for blinded enemy
    auto getMissChance = [&]() -> int {
        if (!enemy.isBlinded()) return 0;
        if (player.hasStanceActive()) {
            if (player.getActiveStance() == StanceType::OBSCURED)     return 50;
            if (player.getActiveStance() == StanceType::ETERNAL_DARK) return 40;
        }
        return 30; // base blind
    };

    auto singleAttack = [&]() {
        // Check Evasive dodge
        if (player.hasStanceActive() && player.getActiveStance() == StanceType::EVASIVE
            && randInt(0, 99) < 20) {
            std::cout << CYAN << "[EVADE] You sidestep " << enemy.getName() << "'s attack!\n" << RESET;
            return;
        }
        // Check blind miss
        int mc = getMissChance();
        if (mc > 0 && enemy.rollBlindMiss(mc)) {
            std::cout << YELLOW << enemy.getName() << " attacks — MISS! [BLIND]\n" << RESET;
            return;
        }
        int atk = enemy.rollAttack();
        int def = player.rollDefense();
        int dmg = std::max(1, atk - def);

        // Defensive stance
        if (player.hasStanceActive() && player.getActiveStance() == StanceType::DEFENSIVE)
            dmg = std::max(1, static_cast<int>(dmg * 0.75f));
        // Bloodlust: bleeding enemy deals 40% less
        if (player.hasStanceActive() && player.getActiveStance() == StanceType::BLOODLUST
            && enemy.isBleeding())
            dmg = std::max(1, static_cast<int>(dmg * 0.60f));

        player.takeDamage(dmg);
        std::cout << RED << enemy.getName() << " hits you for " << dmg << " damage.\n" << RESET;

        // Enemy status procs on player
        if (beh.appliesBleed && !player.getBuffs().bleeding
            && randInt(0, 99) < beh.statusProcChance) {
            player.applyBleedToPlayer();
            std::cout << RED << enemy.getName() << " BLEEDS you! (permanent)\n" << RESET;
        }
        if (beh.appliesPoison && player.getBuffs().poisonStacks == 0
            && (player.hasStanceActive() && player.getActiveStance() != StanceType::RESISTANT)
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

    // Status application (Ember Witch blind)
    if (beh.appliesBlind && player.getBuffs().blindRounds == 0
        && randInt(0, 99) < beh.statusProcChance) {
        player.applyBlindToPlayer(2);
        std::cout << YELLOW << "The Ember Witch blinds you! (2 rounds)\n" << RESET;
    }

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
        if (player.hasStanceActive() && player.getActiveStance() == StanceType::DEFENSIVE)
            dmg = std::max(1, static_cast<int>(dmg * 0.75f));
        player.takeDamage(dmg);
        std::cout << RED << BOLD << enemy.getName() << " unleashes a devastating blow! "
                  << dmg << " damage!\n" << RESET;
        enemy.setTelegraphing(false);
        return;
    }

    // Decrement blind after enemy attacks (one attack cycle = 1 round)
    if (enemy.isBlinded()) enemy.tickBlind();

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
    player.getBuffs().bleeding     = false;
    player.getBuffs().poisonStacks = 0;
    player.getBuffs().blindRounds  = 0;
    return player.isAlive();
}

// ── Boss fight loop ───────────────────────────────────────────────────────────

void Game::playerAttackTurnBoss(Boss& boss, bool& bossDead) {
    while (true) {
        std::cout << DIM << "  ─────────────────────── YOUR TURN ───────────────────────\n" << RESET;
        std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  Light Attack     " << DIM << "· 1 stamina\n"             << RESET;
        std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  Heavy Attack     " << DIM << "· 2 stamina\n"             << RESET;
        std::cout << BOLD << YELLOW << "  [ 3 ]" << RESET << "  Rest             " << DIM << "· recover all stamina\n"   << RESET;
        std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  Inventory        " << DIM << "· free action\n"           << RESET;
        std::cout << BOLD << YELLOW << "  [ 5 ]" << RESET << "  Stance           " << DIM << "· "
                  << (player.hasStanceActive() ? stanceName(player.getActiveStance()) : "None") << "  (free)\n" << RESET;
        std::cout << DIM << "  ──────────────────────────────────────────────────────\n" << RESET;
        std::cout << "  Choice: ";

        std::string input;
        std::getline(std::cin, input);
        int c = 0;
        try { c = std::stoi(input); } catch (...) {}

        if (c == 4) {
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

        if (c == 5) {
            const auto& stances = player.getLearnedStances();
            std::cout << CYAN << "\nKnown stances:\n" << RESET;
            for (int i = 0; i < (int)stances.size(); ++i) {
                std::cout << "  " << (i+1) << ". " << stanceName(stances[i]);
                if (stances[i] == player.getActiveStance()) std::cout << " [active]";
                std::cout << " — " << stanceDescription(stances[i]) << "\n";
            }
            std::cout << "  0. Cancel\nSwitch to: ";
            std::string si; std::getline(std::cin, si);
            int sc = 0;
            try { sc = std::stoi(si); } catch (...) {}
            if (sc >= 1 && sc <= (int)stances.size()) {
                player.switchStance(stances[sc - 1]);
                std::cout << CYAN << "You shift into " << stanceName(player.getActiveStance()) << " stance.\n" << RESET;
            }
            showCombatStatusBoss(boss);
            continue; // free action
        }

        if (c == 3) {
            player.restoreStamina(player.getMaxStamina());
            std::cout << CYAN << "You catch your breath. Stamina fully restored.\n" << RESET;
            return;
        }

        if (c == 1 || c == 2) {
            bool isHeavy  = (c == 2);
            bool isCrit   = false;
            int  critPos  = 10;
            int  cursorPos = runTimingBar(isHeavy, critPos);
            int  dmg = player.timingBasedAttack(cursorPos, critPos, 20, isHeavy, isCrit);
            if (dmg == 0) { continue; }

            int def = 0;
            if (player.getBuffs().phantomsDraughtActive) {
                std::cout << MAGENTA << "[PHANTOM'S DRAUGHT] Your strike passes through their guard!\n" << RESET;
                player.getBuffs().phantomsDraughtActive = false;
            } else {
                def = boss.rollDefense();
            }
            int net = std::max(1, dmg - def);
            boss.takeDamage(net);
            std::cout << "You deal " << GREEN << net << " damage.\n" << RESET;

            if (boss.checkPhaseTransition()) {
                std::cout << "\n" << YELLOW << BOLD << boss.getPhaseText() << "\n" << RESET;
            }
            if (!boss.isAlive()) bossDead = true;
            return;
        }
        // Unknown input — loop back
    }
}

void Game::enemyAttackTurnBoss(Boss& boss) {
    int atk = boss.rollAttack();
    int def = player.rollDefense();
    int dmg = std::max(1, atk - def);
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

void Game::showTutorial() {
    bool inTutorial = true;
    while (inTutorial) {
        clearScreen();
        std::cout << YELLOW << BOLD << "═══════════════════════════════════════════════\n";
        std::cout <<                   "              HOW TO PLAY\n";
        std::cout <<                   "═══════════════════════════════════════════════\n" << RESET;
        std::cout << "\n  1. Combat Basics\n";
        std::cout <<   "  2. Status Effects\n";
        std::cout <<   "  3. Stances\n";
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
                "\nEach combat turn you choose:\n"
                "  1. Light Attack  — costs 1 stamina\n"
                "  2. Heavy Attack  — costs 2 stamina, harder to land a crit\n"
                "  3. Rest          — skip your turn, recover all stamina\n"
                "  4. Inventory     — use a potion (FREE, no turn cost)\n"
                "  5. Switch Stance — change your stance (FREE, no turn cost)\n\n"
                << BOLD << "Timing Bar\n" << RESET <<
                "  A cursor moves across the bar. Press SPACE to stop it.\n"
                "  Hit the marked zone for a critical hit.\n"
                "  Light attacks have a 3-position crit zone.\n"
                "  Heavy attacks have a 1-position crit zone — very tight, high reward.\n\n"
                << BOLD << "Stamina\n" << RESET <<
                "  You have limited stamina per fight. Light attacks cost 1, heavy cost 2.\n"
                "  Running out means you must Rest (losing your turn) to recover.\n\n"
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
            std::cout << YELLOW << BOLD << "── STANCES ─────────────────────────────────────\n" << RESET;
            std::cout <<
                "\nStances are passive modifiers active during combat.\n"
                "Switching stance is FREE — it does not cost your turn.\n"
                "You start with Defensive and Offensive stances.\n"
                "Learn more from the Stance Teacher at [?] nodes on the map.\n\n"
                << BOLD << "Basic Stances:\n" << RESET <<
                "  Defensive   — Incoming damage reduced to 75%\n"
                "  Offensive   — Critical hits deal x1.5 damage\n"
                "  Focused     — Heavy attack crit window +2 positions (3 instead of 1)\n"
                "  Evasive     — 20% chance to fully dodge an incoming attack\n\n"
                << BOLD << "Bleed Stances:\n" << RESET <<
                "  Bloodlust   — Bleeding enemies deal 40% less damage to you\n"
                "  Hemorrhage  — Bleed deals 5 damage per turn instead of 2\n\n"
                << BOLD << "Poison Stances:\n" << RESET <<
                "  Venomous    — Each hit adds +1 poison stack (only if enemy is poisoned)\n"
                "  Resistant   — You are immune to poison\n\n"
                << BOLD << "Blind Stances:\n" << RESET <<
                "  Obscured    — Blinded enemies miss 50% of attacks (up from 30%)\n"
                "  Eternal Dark — Blind is permanent while in this stance; 40% miss chance\n";
            break;
        case 4:
            std::cout << YELLOW << BOLD << "── THE MAP ─────────────────────────────────────\n" << RESET;
            std::cout <<
                "\nEach chapter has a branching path of nodes. Choose your route.\n\n"
                "  [FIGHT]  — Standard enemy encounter\n"
                "  [ELITE]  — Stronger enemy, better rewards\n"
                "  [EVENT]  — Story event, quest triggers, random rewards or penalties\n"
                "  [  ?  ]  — Random event or M'alid the Stance Teacher (25% chance)\n"
                "  [SHOP ]  — Buy potions and upgrade your weapon or armour\n"
                "  [ REST]  — Recover 25% HP, full stamina, and clear all status effects\n"
                "  [ BOSS]  — Chapter boss. Defeat it to progress.\n\n"
                << BOLD << "Death is permanent.\n" << RESET <<
                "  When you die, the run ends. All progress resets.\n"
                "  Your stance pool resets to [Defensive, Offensive].\n\n"
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
