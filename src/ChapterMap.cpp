#include "../include/ChapterMap.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cstring>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static void enableVT() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#else
static void enableVT() {}
#endif

#define RESET   "\033[0m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define CYAN    "\033[96m"
#define BOLD    "\033[1m"
#define GRAY    "\033[90m"
#define WHITE   "\033[37m"

// Layout: 10 rows (0=START, 1-8=path, 9=BOSS). Columns 0-6.
// Depth 8, 7-column wide Slay-the-Spire-style map.

static const int COLS       = 7;
static const int INDENT     = 2;
static const int CELL_WIDTH = 9;   // 7-char symbol + 2-space padding
static const int SYMBOL_MID = 3;   // center offset within [XXXXX]

static int colCenter(int col) {
    return INDENT + col * CELL_WIDTH + SYMBOL_MID;
}

// ── Constructor ───────────────────────────────────────────────────────────────

ChapterMap::ChapterMap(int ch, unsigned int seed)
    : chapter(ch), currentNodeId(0), totalRows(10) {
    if (seed == 0) {
        static unsigned int s = 42;
        seed = ++s * ch * 1337;
    }
    generate(seed);
    markAvailable();
}

// ── Generation ───────────────────────────────────────────────────────────────

void ChapterMap::generate(unsigned int seed) {
    auto lcg = [&]() -> int {
        seed = seed * 1664525u + 1013904223u;
        return (int)(seed >> 16) & 0x7FFF;
    };

    int id = 0;
    nodes.clear();

    // Row 0: single START node, centered at col 3
    {
        MapNode n;
        n.id      = id++;
        n.type    = NodeType::START;
        n.row     = 0;
        n.col     = 3;
        n.visited = true;
        nodes.push_back(n);
    }

    // Rows 1-8: 3-5 nodes per row
    for (int row = 1; row <= 8; ++row) {
        int count = (lcg() % 3) + 3;   // 3, 4, or 5
        if (count > COLS) count = COLS;

        // Spreads across 7 columns — wide, even, distinct paths
        std::vector<int> cols;
        if      (count == 3) cols = {0, 3, 6};
        else if (count == 4) cols = {0, 2, 4, 6};
        else                 cols = {0, 1, 3, 5, 6};

        bool hasRest = false;

        for (int ci = 0; ci < count; ++ci) {
            MapNode n;
            n.id  = id++;
            n.row = row;
            n.col = cols[ci];

            int roll = lcg() % 100;

            if (row == 8) {
                // Pre-boss: elites and rests
                n.type = roll < 35 ? NodeType::ELITE  :
                         roll < 60 ? NodeType::COMBAT :
                         roll < 80 ? NodeType::REST   : NodeType::EVENT;
            } else if (row == 4) {
                // Mid-chapter: guarantee at least one REST
                if (ci == count - 1 && !hasRest) {
                    n.type = NodeType::REST;
                } else {
                    n.type = roll < 35 ? NodeType::COMBAT :
                             roll < 55 ? NodeType::EVENT  :
                             roll < 70 ? NodeType::SHOP   :
                             roll < 85 ? NodeType::REST   : NodeType::ELITE;
                }
            } else {
                n.type = roll < 40 ? NodeType::COMBAT :
                         roll < 55 ? NodeType::EVENT  :
                         roll < 70 ? NodeType::SHOP   :
                         roll < 82 ? NodeType::REST   : NodeType::ELITE;
            }

            if (n.type == NodeType::REST) hasRest = true;
            nodes.push_back(n);
        }
    }

    // Row 9: single BOSS node, centered at col 3
    {
        MapNode n;
        n.id   = id++;
        n.type = NodeType::BOSS;
        n.row  = 9;
        n.col  = 3;
        nodes.push_back(n);
    }

    // Connect adjacent rows
    connectRows(0, 1);
    for (int r = 1; r <= 7; ++r) connectRows(r, r + 1);
    connectRows(8, 9);
}

void ChapterMap::connectRows(int fromRow, int toRow) {
    const int MAX_FORWARD = 3;

    std::vector<int> fromIds, toIds;
    for (auto& n : nodes) if (n.row == fromRow) fromIds.push_back(n.id);
    for (auto& n : nodes) if (n.row == toRow)   toIds.push_back(n.id);
    if (fromIds.empty() || toIds.empty()) return;

    // Pass 1: connect each from-node to all to-nodes within ±1 column (cap at MAX_FORWARD)
    for (int fid : fromIds) {
        MapNode& fn = nodes[fid];
        for (int tid : toIds) {
            if ((int)fn.nextIds.size() >= MAX_FORWARD) break;
            if (std::abs(nodes[tid].col - fn.col) <= 1)
                fn.nextIds.push_back(tid);
        }
        // Guarantee every from-node has at least one forward path
        if (fn.nextIds.empty()) {
            int bestTid = toIds[0], bestDist = 999;
            for (int tid : toIds) {
                int d = std::abs(nodes[tid].col - fn.col);
                if (d < bestDist) { bestDist = d; bestTid = tid; }
            }
            fn.nextIds.push_back(bestTid);
        }
    }

    // Pass 2: ensure every to-node is reachable (no orphans)
    for (int tid : toIds) {
        bool reachable = false;
        for (int fid : fromIds) {
            for (int nid : nodes[fid].nextIds)
                if (nid == tid) { reachable = true; break; }
            if (reachable) break;
        }
        if (reachable) continue;

        // Find the closest from-node that still has room
        int bestFid = -1, bestDist = 999;
        for (int fid : fromIds) {
            if ((int)nodes[fid].nextIds.size() >= MAX_FORWARD) continue;
            int d = std::abs(nodes[fid].col - nodes[tid].col);
            if (d < bestDist) { bestDist = d; bestFid = fid; }
        }
        // If all from-nodes are full, pick the closest regardless
        if (bestFid == -1) {
            bestDist = 999;
            for (int fid : fromIds) {
                int d = std::abs(nodes[fid].col - nodes[tid].col);
                if (d < bestDist) { bestDist = d; bestFid = fid; }
            }
        }
        if (bestFid != -1) nodes[bestFid].nextIds.push_back(tid);
    }
}

// ── Navigation ───────────────────────────────────────────────────────────────

void ChapterMap::markAvailable() {
    for (auto& n : nodes) n.available = false;
    for (int nid : getCurrentNode().nextIds)
        nodes[nid].available = true;
}

std::vector<int> ChapterMap::getAvailableIds() const {
    std::vector<int> out;
    for (auto& n : nodes)
        if (n.available) out.push_back(n.id);
    return out;
}

void ChapterMap::selectNode(int id) {
    if (id < 0 || id >= (int)nodes.size())
        throw std::runtime_error("Invalid node id");
    if (!nodes[id].available)
        throw std::runtime_error("Node not available");
    nodes[id].visited   = true;
    nodes[id].available = false;
    currentNodeId = id;
    markAvailable();
}

int            ChapterMap::getCurrentId()    const { return currentNodeId; }
const MapNode& ChapterMap::getNode(int id)   const { return nodes.at(id); }
const MapNode& ChapterMap::getCurrentNode()  const { return nodes.at(currentNodeId); }
bool           ChapterMap::atBoss()          const { return getCurrentNode().type == NodeType::BOSS; }
int            ChapterMap::getChapter()      const { return chapter; }

// ── Rendering ────────────────────────────────────────────────────────────────

std::string ChapterMap::buildConnectorLine(int lowerRow, float t) const {
    const int lineWidth = INDENT + COLS * CELL_WIDTH + 1;
    std::string line(lineWidth, ' ');

    for (auto& n : nodes) {
        if (n.row != lowerRow) continue;
        int c1 = colCenter(n.col);
        for (int tid : n.nextIds) {
            int  c2  = colCenter(nodes[tid].col);
            int  pos = (int)(c1 + (c2 - c1) * t + 0.5f);
            if (pos < 0 || pos >= lineWidth) continue;

            char ch;
            if      (n.col == nodes[tid].col)    ch = '|';
            else if (nodes[tid].col > n.col)     ch = '/';
            else                                 ch = '\\';

            if (line[pos] == ' ' || line[pos] == ch) line[pos] = ch;
            else                                     line[pos] = '+';
        }
    }
    return line;
}

void ChapterMap::render() const {
    enableVT();

    static const char* chapterNames[] = {
        "", "Chapter 1: The Ashfields",
            "Chapter 2: The Hollowed Marsh",
            "Chapter 3: The Obsidian Reach"
    };

    // Header sized to map width: INDENT + COLS*CELL_WIDTH = 2 + 7*9 = 65
    std::cout << "\n" << BOLD << YELLOW;
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << chapterNames[chapter];
    int nameLen = (int)strlen(chapterNames[chapter]);
    for (int i = nameLen; i < 61; ++i) std::cout << ' ';
    std::cout << "║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << RESET;

    // Populate grid[row][col] = node id, -1 = empty
    int grid[10][7];
    std::memset(grid, -1, sizeof(grid));
    for (auto& n : nodes)
        if (n.row < 10 && n.col < COLS)
            grid[n.row][n.col] = n.id;

    // Print from top (boss = row 9) down to start (row 0)
    for (int row = 9; row >= 0; --row) {
        std::cout << std::string(INDENT, ' ');
        for (int col = 0; col < COLS; ++col) {
            int nid = grid[row][col];
            if (nid == -1) {
                std::cout << std::string(CELL_WIDTH, ' ');
                continue;
            }
            const MapNode& n = nodes[nid];
            if      (nid == currentNodeId) std::cout << GREEN << BOLD << n.typeSymbol() << RESET << "  ";
            else if (n.available)          std::cout << CYAN  << BOLD << n.typeSymbol() << RESET << "  ";
            else if (n.visited)            std::cout << GRAY        << n.typeSymbol() << RESET << "  ";
            else                           std::cout << WHITE       << n.typeSymbol() << RESET << "  ";
        }
        std::cout << "\n";

        if (row > 0) {
            std::cout << buildConnectorLine(row - 1, 2.0f / 3.0f) << "\n";
            std::cout << buildConnectorLine(row - 1, 1.0f / 3.0f) << "\n";
        }
    }

    std::cout << "\n";
    std::cout << GREEN << BOLD << "[XXXXX]" << RESET << " = You   ";
    std::cout << CYAN  << BOLD << "[XXXXX]" << RESET << " = Available   ";
    std::cout << GRAY        << "[XXXXX]" << RESET << " = Visited   ";
    std::cout << WHITE       << "[XXXXX]" << RESET << " = Ahead\n";
}
