# Flight Simulator Pro



## TO BUILD AND RUN : 
cd ~/Desktop/flgiht_sim_game
brew install cmake sdl2    # Install dependencies
mkdir build && cd build
cmake .. && make -j4
./FlightSimulator
A comprehensive C++ flight simulator game with realistic physics, multiple aircraft, and controller support.

## Features

- **5 Flyable Aircraft**
  - Boeing 737 - Commercial airliner
  - F-16 Fighter - Agile military jet
  - F-22 Raptor - Advanced stealth fighter
  - Cessna 172 - Light single-engine trainer
  - Airbus A320 - Modern commercial aircraft

- **Realistic Flight Physics**
  - Lift, drag, thrust, and weight simulation
  - Stall mechanics based on airspeed
  - Ground effect during landing
  - Terrain-following collision detection

- **Multiple Camera Views**
  - Cockpit (first-person)
  - Chase (follow) camera
  - Orbit camera (rotate around aircraft)
  - Flyby camera
  - Tower camera

- **Complete Control Systems**
  - Full keyboard support
  - Bluetooth/USB controller support
  - Customizable key bindings
  - Analog stick support for smooth control

- **Audio System**
  - Synthesized engine sounds
  - Wind/speed-based audio
  - Menu music
  - Volume controls (Master, Music, SFX, Engine)

- **Menu System**
  - Loading screen with tips
  - Aircraft selection with specs display
  - Pause menu (top-right corner)
  - Volume settings
  - Control customization

## Requirements

### macOS
- macOS 10.13 or later
- Xcode Command Line Tools
- CMake 3.16+
- SDL2 library

### Installing Dependencies

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install required packages
brew install cmake sdl2
```

## Building the Game

### Quick Build
```bash
cd /path/to/flgiht_sim_game
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

### Detailed Build Steps

1. **Clone/Navigate to the project**
   ```bash
   cd /path/to/flgiht_sim_game
   ```

2. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Generate build files**
   ```bash
   cmake ..
   ```

4. **Compile**
   ```bash
   make -j4
   ```

5. **Run the game**
   ```bash
   ./FlightSimulator
   ```

## Controls

### Keyboard Controls

| Action | Key |
|--------|-----|
| Pitch Up | W or Up Arrow |
| Pitch Down | S or Down Arrow |
| Roll Left | A or Left Arrow |
| Roll Right | D or Right Arrow |
| Yaw Left | Q |
| Yaw Right | E |
| Increase Throttle | Shift |
| Decrease Throttle | Ctrl |
| Toggle Landing Gear | G |
| Flaps Up | V |
| Flaps Down | F |
| Toggle Brakes | B |
| Cycle Camera | C |
| Pause | Escape or P |

### Controller Controls (Xbox/PlayStation)

| Action | Control |
|--------|---------|
| Pitch/Roll | Left Stick |
| Yaw | Right Stick X-axis |
| Throttle | Right Trigger |
| Brake | Left Trigger |
| Landing Gear | Y/Triangle |
| Flaps Up/Down | D-Pad Up/Down |
| Camera Cycle | RB/R1 |
| Pause | Start |
| Menu Select | A/X |
| Menu Back | B/Circle |

## Game Tips

1. **Takeoff**: Set flaps to 1-2, full throttle, pull up gently at 150+ km/h
2. **Landing**: Reduce speed, lower landing gear, use flaps, aim for runway
3. **Avoid Stalling**: Watch your airspeed - each aircraft has different stall speeds
4. **Banking**: Combine roll with slight pitch up to maintain altitude in turns
5. **The F-22 Raptor**: Fastest aircraft with best acceleration - great for aerobatics

## Project Structure

```
flgiht_sim_game/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files
│   ├── Aircraft.h
│   ├── AudioManager.h
│   ├── Camera.h
│   ├── Game.h
│   ├── InputManager.h
│   ├── LoadingScreen.h
│   ├── MenuSystem.h
│   ├── Physics.h
│   ├── Renderer.h
│   ├── SettingsManager.h
│   ├── Sky.h
│   ├── Terrain.h
│   └── Types.h
├── src/                    # Source files
│   ├── Aircraft.cpp
│   ├── AudioManager.cpp
│   ├── Camera.cpp
│   ├── Game.cpp
│   ├── InputManager.cpp
│   ├── LoadingScreen.cpp
│   ├── main.cpp
│   ├── MenuSystem.cpp
│   ├── Physics.cpp
│   ├── Renderer.cpp
│   ├── SettingsManager.cpp
│   ├── Sky.cpp
│   └── Terrain.cpp
└── assets/                 # Game assets (if any)
```

## Troubleshooting

### SDL2 Not Found
If CMake can't find SDL2:
```bash
# Reinstall SDL2
brew reinstall sdl2

# Or specify the path manually
cmake .. -DSDL2_DIR=$(brew --prefix sdl2)/lib/cmake/SDL2
```

### Controller Not Detected
1. Make sure your controller is in pairing mode
2. Connect via Bluetooth settings on macOS
3. The game scans for controllers automatically
4. Check System Preferences > Bluetooth

### Black Screen on Launch
1. Ensure your graphics drivers are up to date
2. Try resizing the window
3. Check console output for OpenGL errors

### Low FPS
1. Reduce window size
2. Lower terrain detail (modify gridSize in Terrain::generate)
3. Close other applications

## License

This project is provided as-is for educational purposes.

## Credits

Developed as a comprehensive C++ flight simulator demonstration featuring:
- OpenGL rendering
- SDL2 windowing and input
- Procedural terrain generation
- Synthesized audio
- Multi-camera system
- Full aircraft physics simulation
