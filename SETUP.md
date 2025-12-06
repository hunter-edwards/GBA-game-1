# Meta Hunter - GBA Development Setup Guide

## Prerequisites

### Install devkitPro/devkitARM

#### Windows
1. Download the installer from: https://github.com/devkitPro/installer/releases
2. Run `devkitProUpdater-3.0.3.exe` as administrator
3. Select "GBA Development" during setup
4. Use MSYS2 terminal for building

#### macOS
```bash
# Install Xcode command line tools
xcode-select --install

# Download and install devkitPro pacman
# Get the .pkg from: https://github.com/devkitPro/pacman/releases

# Install GBA development tools
sudo dkp-pacman -S gba-dev
```

#### Linux (Debian/Ubuntu)
```bash
# Add devkitPro repository
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x install-devkitpro-pacman
sudo ./install-devkitpro-pacman

# Install GBA development tools
sudo dkp-pacman -S gba-dev

# Log out and back in for environment variables
```

### Environment Variables

Make sure these are set (usually automatic on Linux):

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
export PATH=$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH
```

## Building the Game

```bash
# Navigate to project directory
cd meta-hunter

# Build the ROM
make

# Clean build files
make clean
```

The compiled ROM will be `meta-hunter.gba` in the project root.

## Running the Game

### Recommended Emulator: mGBA

Download from: https://mgba.io/

```bash
# Run the game
mgba-qt meta-hunter.gba

# Run with debugging enabled
mgba-qt -g meta-hunter.elf
```

### Alternative Emulators
- **VBA-M**: Good visual debugging tools
- **No$GBA**: Excellent for deep debugging
- **NanoBoyAdvance**: Cycle-accurate testing

## Controls

### Gameplay
- **D-Pad**: Move player
- **A**: Shoot
- **START**: Pause
- **SELECT**: Open puzzle (testing)

### Puzzle Mode
- **D-Pad Left/Right**: Move cursor
- **D-Pad Up/Down**: Change digit
- **A**: Submit guess
- **B**: Cancel (fail puzzle)

## Project Structure

```
meta-hunter/
├── Makefile            # Build configuration
├── source/             # C source files
│   ├── main.c          # Entry point, game loop
│   ├── player.c        # Player logic
│   ├── enemy.c         # Enemy AI
│   ├── combat.c        # Projectiles, collision
│   ├── puzzle.c        # Mastermind puzzle
│   ├── ui.c            # HUD and menus
│   └── sprites.c       # Placeholder graphics
├── include/            # Header files
├── graphics/           # PNG files (add your own!)
├── audio/              # Audio files (MOD, XM, WAV)
└── build/              # Compiled objects (generated)
```

## Adding Custom Graphics

1. Create 16-color (4bpp) PNG files
2. Place them in `graphics/`
3. Create matching `.grit` files with conversion settings
4. The Makefile will automatically convert them

Example `player.grit`:
```
# 16x16 sprite, 4bpp
-gt -gB4 -Mw2 -Mh2
```

## Debugging with mGBA + VS Code

1. Start mGBA with GDB server:
   ```bash
   mgba-qt -g meta-hunter.elf
   ```

2. Configure `.vscode/launch.json`:
   ```json
   {
     "version": "0.2.0",
     "configurations": [{
       "name": "Debug GBA",
       "type": "cppdbg",
       "request": "launch",
       "program": "${workspaceFolder}/meta-hunter.elf",
       "miDebuggerServerAddress": "localhost:2345",
       "miDebuggerPath": "arm-none-eabi-gdb"
     }]
   }
   ```

## Next Steps (Phase 2)

Once this prototype is working, the next phase includes:
- Multiple room types with transitions
- Hub area with upgrade menu
- Persistent save data
- More enemy types
- Additional puzzle types

See `meta-hunter-project-plan.md` for the full development roadmap.
