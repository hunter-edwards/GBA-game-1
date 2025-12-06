# Meta Hunter - GBA Roguelite Code-Cracking Game
## Project Plan for Claude Code

---

## PROJECT OVERVIEW

**Game Title:** Meta Hunter  
**Platform:** Game Boy Advance (GBA)  
**Genre:** Roguelite / Puzzle / Action  
**Development Approach:** Iterative, starting with bare-bones prototype

### High-Level Concept
Meta Hunter is a roguelite where players are digital infiltrators diving into corrupted network systems. The core gameplay loop combines code-cracking puzzles with combat encounters. Each run is procedurally generated, with progression through unlockable abilities that literally break puzzle rules and enhance combat capabilities.

---

## CORE GAMEPLAY LOOP

1. **Enter a procedurally generated digital dungeon (run)**
2. **Navigate rooms** containing:
   - Code-cracking puzzles (resource gathering)
   - Enemy encounters (combat)
   - Shops/upgrades
   - Boss/mini-boss fights
3. **Crack codes under pressure** to earn:
   - Data fragments (currency)
   - Health/shields
   - Temporary buffs
   - Weapon upgrades
4. **Combat viruses and security programs** while solving puzzles
5. **Face bosses** with multi-phase puzzle + combat mechanics
6. **Die or complete run**, return to hub
7. **Spend accumulated data** to unlock permanent upgrades
8. **Repeat with new abilities and knowledge**

---

## TECHNICAL REQUIREMENTS

### Development Stack
- **Language:** C
- **Toolchain:** devkitARM (GBA development kit)
- **Build System:** Makefile-based
- **Assets:** Custom sprite work, tile maps
- **Target:** Real GBA hardware compatibility (via emulator testing)

### GBA Technical Constraints
- **Screen Resolution:** 240x160 pixels
- **Color Depth:** 15-bit color (32,768 colors)
- **Sprites:** Hardware supports 128 sprites, 64x64px max per sprite
- **Backgrounds:** 4 hardware background layers
- **Memory:** 256KB VRAM, 32KB RAM, 96KB IWRAM
- **Sound:** 6 channels (4 tone, 2 PCM)
- **Input:** D-pad, A, B, Start, Select, L, R

---

## GAME SYSTEMS BREAKDOWN

### 1. CORE SYSTEMS (Phase 1 - Bare Bones)

#### Player Character
- **Sprite:** 16x16 or 16x32 pixel sprite
- **Movement:** 4-directional (D-pad controlled)
- **Stats:**
  - Health (HP)
  - Shields (temporary damage absorption)
  - Movement speed
- **Actions:**
  - Move
  - Shoot (basic projectile)
  - Interact (examine puzzles, enter doors)

#### Room System
- **Room Types:**
  - Puzzle Room
  - Combat Room
  - Shop Room
  - Boss Room
  - Empty/Safe Room (rare)
- **Room Structure:**
  - Grid-based layout (16x16 tiles or similar)
  - Entry/exit doors
  - Spawn points for enemies/puzzles
  - Walls/obstacles
- **Generation:** Simple procedural room selection from templates initially

#### Basic Combat
- **Player Weapon:** Single projectile shooter (straight line)
- **Enemies:** 2-3 basic enemy types
  - Patrol AI (moves in pattern)
  - Chase AI (follows player)
  - Stationary turret
- **Collision Detection:** AABB (Axis-Aligned Bounding Box)
- **Damage System:** Numeric damage, health bars

#### Puzzle System (Simplified Start)
- **Puzzle Type 1: Mastermind/Code Breaker**
  - 4-digit code with numbers 0-9
  - Player guesses, gets feedback (correct position, correct number wrong position, incorrect)
  - Time limit (30-60 seconds)
  - Success = reward, failure = damage or enemy spawn
- **UI Elements:**
  - Input grid for entering guesses
  - Feedback display
  - Timer bar

#### Data/Currency System
- **Data Fragments:** Earned from puzzle completion, enemy kills
- **Display:** Simple counter in HUD
- **Usage:** Spending at shops (future phase)

---

### 2. PROGRESSION SYSTEMS (Phase 2)

#### Meta Progression (Between Runs)
- **Hub Area:** Safe zone where player returns after death/victory
- **Upgrade Menu:**
  - Unlock new abilities
  - Unlock new weapons
  - Permanent stat boosts
- **Data Bank:** Persistent storage of earned data across runs
- **Unlockables List:**
  - "Brute Force" - reveals 1 digit in puzzle
  - "Pattern Recognition" - highlights matching sequences
  - "Backdoor" - skip puzzle, reduced reward
  - "Overclock" - freeze timer for 5 seconds
  - "Memory Dump" - see last 3 attempts simultaneously
  - Weapon upgrades (spread shot, laser, etc.)
  - HP/Shield increases

#### In-Run Progression
- **Temporary Buffs:** Last for current run only
  - Fire rate increase
  - Movement speed boost
  - Shield regeneration
  - Puzzle timer extensions
- **Weapon Pickups:** Found in rooms, temporary or until death

---

### 3. ADVANCED PUZZLE MECHANICS (Phase 3)

#### Additional Puzzle Types
1. **Circuit Connection:**
   - Connect nodes to complete circuit
   - Limited moves/connections
   - Time pressure

2. **Pattern Matching:**
   - Memory game with symbols
   - Increasing complexity
   - Match sequences

3. **Logic Gates:**
   - Route signals through logic gates (AND, OR, NOT)
   - Reach target output
   - Programming-lite concept

#### Puzzle Difficulty Scaling
- Based on depth in dungeon
- More complex patterns
- Less time
- Higher rewards

#### Combat During Puzzles
- Enemies spawn mid-puzzle
- Must dodge while thinking
- Some abilities pause enemy spawns
- Creates risk/reward tension

---

### 4. ENEMY & BOSS DESIGN (Phase 4)

#### Enemy Variety
- **Virus Types:**
  - Worm: Fast, low HP, chase behavior
  - Trojan: Slow, high HP, blocks paths
  - Spyware: Ranged attacks, evasive
  - Malware: Spawns smaller enemies
  
#### Mini-Bosses
- Appear every 3-5 rooms
- Guard rare upgrade chips
- Single-phase combat encounters
- Unique attack patterns

#### Full Bosses
- **Multi-Phase Structure:**
  - Phase 1: Puzzle (crack firewall)
  - Phase 2: Combat (damage boss)
  - Phase 3: Harder puzzle + combat
  - Repeat or escalate
- **Boss Count:** 3-4 bosses total for full game
- **Themes:** Could represent different security systems (Antivirus, Firewall, Encryption, Admin)

---

### 5. DUNGEON GENERATION (Phase 5)

#### Procedural Generation
- **Layout:**
  - Start room → branching paths → boss room
  - Critical path guaranteed
  - Optional side rooms for rewards
  - Use graph-based generation or cellular automata
  
- **Room Templates:**
  - Pre-designed room layouts
  - Randomly populated with enemies/puzzles
  - Doorway positions predetermined

- **Difficulty Curve:**
  - Rooms get harder as player progresses
  - More enemies, harder puzzles, less time
  - Better rewards deeper in

#### Visual Theming
- Different "zones" with visual themes:
  - Corrupted Database (green/blue)
  - Firewall Zone (red/orange)
  - Core System (purple/white)
- Sprite palette swaps for zones

---

### 6. UI/UX DESIGN

#### HUD Elements
- **Top Bar:**
  - HP bar (red)
  - Shield bar (blue)
  - Data fragment counter
- **Ability Icons:** Show cooldowns/availability
- **Mini-map:** Optional, shows room layout

#### Menu Systems
- **Pause Menu:**
  - Resume
  - Controls
  - Quit Run
- **Hub Menu:**
  - Upgrades
  - Stats/Records
  - Start Run
- **Shop Interface:**
  - Item grid
  - Description panel
  - Cost display

#### Puzzle UI
- Clean, readable fonts (GBA pixel fonts)
- Clear feedback (colors, sounds)
- Timer with warning colors (green→yellow→red)
- Input cursor for guesses

---

### 7. AUDIO DESIGN (Phase 6)

#### Music
- **Hub Theme:** Calm, ambient
- **Dungeon Theme:** Tense, electronic
- **Boss Theme:** Intense, fast-paced
- **Victory/Defeat:** Short stings

#### Sound Effects
- Weapon fire
- Enemy death
- Damage taken
- Puzzle success/failure
- Door open/close
- Menu navigation
- Ability activation

---

## DEVELOPMENT PHASES

### Phase 1: Foundation (Bare Bones Prototype)
**Goal:** Playable core loop with minimal features

**Deliverables:**
1. Player sprite that moves in 4 directions
2. Single room with boundaries
3. One enemy type that can be shot
4. Basic collision detection
5. One puzzle type (Mastermind) with simple UI
6. Health system
7. Win/lose conditions (solve puzzle = win, HP reaches 0 = lose)

**Success Criteria:**
- Can move player around room
- Can shoot and kill enemy
- Can attempt and solve a code-breaking puzzle
- Game loops back to start on win/lose

---

### Phase 2: Core Loop Expansion
**Goal:** Multiple rooms, basic procedural generation, meta progression

**Deliverables:**
1. Room transition system (doors)
2. 3-5 room templates
3. Simple procedural room ordering
4. 2-3 enemy types with different behaviors
5. Data fragment collection and display
6. Hub area with upgrade menu
7. 2-3 unlockable abilities
8. Persistent save data

**Success Criteria:**
- Can navigate through multiple rooms
- Enemies behave differently
- Can collect data and spend it on upgrades
- Upgrades persist between runs

---

### Phase 3: Puzzle & Combat Integration
**Goal:** Puzzles feel integrated with combat, more variety

**Deliverables:**
1. 2-3 total puzzle types implemented
2. Enemies spawn during puzzles
3. Puzzle difficulty scaling
4. 4-6 unlockable abilities that affect puzzles
5. Temporary buff system
6. Weapon variety (2-3 weapon types)

**Success Criteria:**
- Puzzles feel challenging but fair
- Combat during puzzles creates tension
- Abilities meaningfully impact puzzle-solving
- Runs feel different based on unlocks

---

### Phase 4: Boss & Enemy Polish
**Goal:** Memorable boss encounters, enemy variety

**Deliverables:**
1. 5+ enemy types with distinct behaviors
2. 1-2 mini-boss encounters
3. 1 full boss with multi-phase fight
4. Boss-specific puzzle mechanics
5. Enemy spawning logic based on room difficulty

**Success Criteria:**
- Bosses feel like climactic encounters
- Enemy variety keeps combat fresh
- Multi-phase boss puzzles work smoothly

---

### Phase 5: Procedural Generation & Content
**Goal:** True roguelite variety, replayability

**Deliverables:**
1. Improved procedural generation (branching paths)
2. 10+ room templates per zone
3. 3 visual zones/themes
4. Shop system with item variety
5. Risk/reward room choices
6. Seeded run support (optional)

**Success Criteria:**
- Each run feels different
- Dungeon layouts are logical and fair
- Visual variety keeps game interesting

---

### Phase 6: Polish & Balance
**Goal:** Complete, polished game experience

**Deliverables:**
1. Full music and SFX implementation
2. Particle effects and visual feedback
3. Difficulty balancing
4. Tutorial/onboarding
5. Stats tracking (runs completed, enemies killed, etc.)
6. Multiple difficulty modes (optional)
7. Secrets/unlockables for replayability

**Success Criteria:**
- Game feels complete and polished
- Difficulty curve is satisfying
- Audio enhances experience
- Players understand mechanics without external help

---

## FILE STRUCTURE

```
meta-hunter/
├── Makefile
├── README.md
├── src/
│   ├── main.c                 # Entry point, main game loop
│   ├── player.c/.h            # Player movement, stats, actions
│   ├── enemy.c/.h             # Enemy AI, behaviors
│   ├── combat.c/.h            # Shooting, collision, damage
│   ├── puzzle.c/.h            # Puzzle logic and UI
│   ├── room.c/.h              # Room management, transitions
│   ├── generation.c/.h        # Procedural generation
│   ├── progression.c/.h       # Unlocks, meta progression
│   ├── ui.c/.h                # HUD, menus
│   ├── audio.c/.h             # Sound and music
│   ├── save.c/.h              # Save/load system
│   └── utils.c/.h             # Helper functions
├── assets/
│   ├── sprites/               # .png source files
│   ├── backgrounds/           # Background tiles
│   ├── audio/                 # .wav or module files
│   └── fonts/                 # Bitmap fonts
├── build/                     # Compiled output
└── tools/                     # Asset conversion scripts
```

---

## KEY TECHNICAL CONSIDERATIONS

### GBA-Specific Optimizations
1. **Use hardware sprites** for player, enemies, projectiles
2. **Use tile-based backgrounds** for rooms (efficient VRAM usage)
3. **Limit sprite counts** per room (max 128, but aim for <64 for performance)
4. **Use object pooling** for projectiles/effects
5. **Optimize collision detection** (spatial partitioning, only check active objects)
6. **Use mode 0** (tile mode) for most game elements
7. **IWRAM placement** for frequently accessed code (faster than ROM)

### Memory Management
- Pre-allocate arrays for enemies, projectiles (no dynamic allocation)
- Reuse objects via pooling
- Compress backgrounds/sprites when possible
- Stream music if needed (don't load all at once)

### Save Data
- Use SRAM or Flash memory for save data
- Store: unlocked abilities, total data collected, high scores
- Keep save data small (<64KB)

---

## TESTING & ITERATION PLAN

### Testing Checklist (Per Phase)
- [ ] Runs on emulator without crashes
- [ ] All inputs respond correctly
- [ ] No visual glitches
- [ ] Collision detection works as expected
- [ ] Performance is smooth (60 FPS target)
- [ ] Save/load works correctly
- [ ] Puzzle logic is correct and fair
- [ ] Enemy AI behaves as intended

### Recommended Emulator
- **mGBA** or **VisualBoyAdvance** for testing
- Test on actual hardware if possible (via flash cart)

---

## DESIGN PHILOSOPHY

### Core Pillars
1. **Puzzle-Action Hybrid:** Neither pure puzzler nor pure shooter - the tension between them is the fun
2. **Meaningful Progression:** Unlocks should fundamentally change how you play
3. **Fair Challenge:** Difficulty from complexity, not randomness or cheap tricks
4. **Snappy Feel:** GBA games feel great when responsive - prioritize tight controls

### Inspiration References
- **Puzzle Mechanics:** Mastermind, Wordle, pipe puzzles
- **Combat Feel:** Gunstar Super Heroes, Metroid Fusion
- **Roguelite Structure:** Binding of Isaac, Enter the Gungeon
- **GBA Style:** Golden Sun, Advance Wars (clean UI, readable sprites)

---

## STRETCH GOALS (Post-Launch)

- Daily challenge mode with fixed seeds
- Multiple playable characters with different abilities
- Endless mode
- Co-op via link cable (ambitious!)
- Level editor/custom puzzle mode
- Achievement system
- New game+ with harder puzzles

---

## SUCCESS METRICS

**Prototype Success:**
- Core loop is fun for 5-10 minutes
- Puzzle mechanics are clear and satisfying
- Combat feels responsive

**Full Game Success:**
- Replayable for 5+ hours
- Multiple viable strategies/builds
- Players understand all mechanics without external guide
- Runs feel varied and unpredictable

---

## NOTES FOR CLAUDE CODE

- Start with **Phase 1** deliverables only
- Use clear, commented code (this is a learning project)
- Follow GBA best practices (Tonc tutorials are great reference)
- Prioritize getting something playable over perfect code
- Test frequently in emulator
- Ask clarifying questions when design is ambiguous
- Keep asset creation simple at first (placeholder sprites are fine)
- Document any technical limitations encountered

**Communication Style:** Hunter will guide you along the way, so treat this as a collaborative build. Be ready to iterate, try different approaches, and adjust based on playtesting feedback.

---

## IMMEDIATE FIRST STEPS

1. Set up GBA development environment (devkitARM)
2. Create basic project structure with Makefile
3. Implement simple sprite rendering (player character)
4. Implement D-pad movement
5. Implement room boundaries/collision
6. Create simple enemy sprite that moves
7. Implement basic shooting (one projectile type)
8. Implement enemy health and death
9. Create placeholder puzzle UI (Mastermind style)
10. Implement puzzle win/lose logic

Once these are working, we have a foundation to build everything else on.

---

**END OF PROJECT PLAN**

Let me know when you're ready to start Phase 1, and I'll begin with the absolute basics!
