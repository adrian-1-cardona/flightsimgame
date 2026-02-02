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
#include <cstdio>

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    try {
        fprintf(stderr, "Starting Flight Simulator\n");
        fflush(stderr);
        
        std::cout << "==================================" << std::endl;
        std::cout << "   FLIGHT SIMULATOR PRO v1.0.0   " << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << std::endl;
        
        // Create and initialize the game
        fprintf(stderr, "Creating game object\n");
        fflush(stderr);
        auto game = std::make_unique<Game>();
        
        fprintf(stderr, "Initializing game\n");
        fflush(stderr);
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
