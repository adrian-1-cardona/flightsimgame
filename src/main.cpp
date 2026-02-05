/**
 * Flight Simulator Pro
 * A modern C++ flight simulator with realistic physics and graphics
 * 
 * Features:
 * - Multiple aircraft (Boeing 737, F-16 Fighter, F-22 Raptor, Cessna 172, A320)
 * - Bluetooth controller support
 * - Customizable controls
 * - Realistic flight physics
 * - 3D terrain and sky rendering
 * - Full HUD with instruments
 */

#include "Game.h"
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[]) {
    // Seed random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    try {
        std::cout << "==================================" << std::endl;
        std::cout << "   FLIGHT SIMULATOR PRO v1.0.0   " << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << std::endl;
        
        // Create and initialize the game
        auto game = std::make_unique<Game>();
        if (!game->initialize()) {
            std::cerr << "ERROR: Failed to initialize game!" << std::endl;
            return -1;
        }
        
        std::cout << "Game initialized successfully!" << std::endl;
        std::cout << "Starting game loop..." << std::endl;
        std::cout << std::endl;
        
        // Run the game
        game->run();
        
        // Cleanup
        game->shutdown();
        
        std::cout << std::endl;
        std::cout << "Thanks for playing Flight Simulator Pro!" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "UNKNOWN EXCEPTION" << std::endl;
        return -1;
    }
}
