#include "../include/Game.h"
#include <iostream>

int main() {
    try {
        std::cout << "\033[33m\033[1m";
        std::cout << R"(
  _____ _            _    _                 _           _     ____
 |_   _| |__   ___  | |  | | __ _ _ __   __| | ___ _ __(_)_ _|___ \
   | | | '_ \ / _ \ | |/\| |/ _` | '_ \ / _` |/ _ \ '__| | '_ \ __) |
   | | | | | |  __/ \  /\  / (_| | | | | (_| |  __/ |  | | | | / __/
   |_| |_| |_|\___|  \/  \/ \__,_|_| |_|\__,_|\___|_|  |_|_| |_|_____|

              G O S P E L   O F   A S H
        )" << "\033[0m\n";

        std::cout << "The Dragon King stirs. The Darkbloods are moving.\n";
        std::cout << "A new peasant steps onto the ash-grey road.\n\n";

        Game game;
        game.run();

        std::cout << "\nThanks for playing The Wandering Peasant 2.\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown error.\n";
        return 1;
    }
}
