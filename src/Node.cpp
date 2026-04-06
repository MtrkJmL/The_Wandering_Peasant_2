#include "../include/Node.h"

std::string MapNode::typeLabel() const {
    switch (type) {
        case NodeType::START:   return "Start";
        case NodeType::COMBAT:  return "Combat";
        case NodeType::ELITE:   return "Elite";
        case NodeType::EVENT:   return "?";
        case NodeType::SHOP:    return "Shop";
        case NodeType::REST:    return "Rest";
        case NodeType::BOSS:    return "Boss";
    }
    return "?";
}

std::string MapNode::typeSymbol() const {
    switch (type) {
        case NodeType::START:   return "[ YOU ]";
        case NodeType::COMBAT:  return "[FIGHT]";
        case NodeType::ELITE:   return "[ELITE]";
        case NodeType::EVENT:   return "[  ?  ]";
        case NodeType::SHOP:    return "[SHOP ]";
        case NodeType::REST:    return "[ REST]";
        case NodeType::BOSS:    return "[ BOSS]";
    }
    return "[  ?  ]";
}
