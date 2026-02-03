A desktop rhythm game developed with Qt Widgets. Players need to press corresponding keys at the right time to hit falling notes and get high scores. The project retains complete core functions, data structures and algorithm logic, supporting beatmap loading, audio synchronization, score statistics and leaderboard persistence.

Project Highlights

1. Complete Core Functions: Includes core rhythm game modules such as note falling, real-time judgment, combo system, score calculation, and audio synchronization.

2. Modular Design: Separates data layer, logic layer and UI layer with low coupling and high scalability, facilitating subsequent function expansion (e.g., adding new note types, skin system).

3. Rigorous Data Structures: Encapsulates complete core data classes to support game state recording, statistical analysis and data persistence.

4. Standard Qt Implementation: Builds graphical interface based on Qt Widgets, supporting cross-platform compatibility (Windows/macOS/Linux).

Environment Requirements

Development/Running Environment

- Qt Version: Qt 5.15 or higher (Qt 6.2 LTS is recommended for better compatibility).

- Compilers:
        

  - Windows: MinGW 8.1+ / MSVC 2019+

  - macOS: Clang 12+

  - Linux: GCC 9+

- C++ Standard: C++17 or higher.

- Additional Dependencies: Qt Multimedia module (for audio playback and synchronization).

Resource Preparation

Create a Resources folder in the project root directory in advance and place the corresponding resources (see directory structure below):

- Audio files (supports .mp3, .wav formats).

- Beatmap files (custom format, refer to JSON/XML specifications).

- Image resources (song covers, note skins, interface backgrounds, etc.).

Quick Start

1. Clone/Obtain the Project

Download the complete project to your local machine and ensure the directory structure is intact with no missing files.

2. Open the Project

1. Launch Qt Creator, click 「Open Project」, and select the RhythmMaster.pro file in the project root directory.

2. Qt Creator will automatically parse the project configuration; wait for dependencies to load (no manual configuration required, as the .pro file has preset complete dependencies).

3. Compile the Project

1. Select the corresponding compile kit (e.g., MinGW 64-bit).

2. Click 「Build Project」 (shortcut: Ctrl+B) and wait for compilation to complete (compilation is successful if there are no errors).

4. Run the Game

1. Click 「Run Project」 (shortcut: Ctrl+R).

2. After launching, enter the main window and select a song to start playing.

Project Structure

RhythmMaster/
│
├── README.md                          # Project documentation
├── main.cpp                          # Program entry (initializes Qt app and main window)
├── RhythmMaster.pro                  # Qt project configuration file (manages compilation, dependencies, resources)
│
├── Core/                             # Core data classes (encapsulates all game data structures, no business logic)
│   ├── Common.h                      # Global enums, constants, macros (basic dependency for all classes)
│   ├── Note.h/cpp                    # Note class (stores note properties, manages note states)
│   ├── Song.h/cpp                    # Song class (stores song metadata, difficulty info, play statistics)
│   ├── Player.h/cpp                  # Player class (stores player info, current game state, judgment stats)
│   ├── Beatmap.h/cpp                 # Beatmap class (stores beatmap data, manages note collections)
│   ├── GameConfig.h/cpp              # Config class (stores game config, supports save/load/validation)
│   ├── HitResult.h/cpp               # Hit result class (records detailed judgment info for single key presses)
│   ├── Statistics.h/cpp              # Statistics class (summarizes complete game data, generates stats reports)
│   └── Leaderboard.h/cpp             # Leaderboard class (manages high score records, supports sorting/persistence)
│
├── Logic/                            # Game logic layer (encapsulates all algorithms and business logic, no UI code)
│   ├── ScoreCalculator.h/cpp         # Score calculation class (judgment level mapping, combo bonus, final score calculation)
│   ├── NoteManager.h/cpp             # Note management class (note lifecycle, position update, collision detection)
│   ├── AudioSync.h/cpp               # Audio sync class (audio playback control, time synchronization, offset handling)
│   └── BeatmapLoader.h/cpp           # Beatmap loader class (beatmap parsing, data validation, note construction)
│
├── UI/                               # UI layer (built with Qt Widgets, responsible for rendering and user interaction)
│   ├── MainWindow.h/cpp              # Main window (song selection, menu entry, interface navigation)
│   ├── GameScene.h/cpp               # Game scene (note rendering, judgment line drawing, keyboard event handling)
│   ├── ResultDialog.h/cpp            # Result interface (displays play stats, leaderboard, restart entry)
│   └── SettingsDialog.h/cpp          # Settings interface (modifies game config, key bindings, volume)
│
└── Resources/                        # Resource directory (unified management of all non-code resources)
    ├── Audio/                        # Song audio files (.mp3/.wav)
    ├── Beatmaps/                     # Beatmap files (custom format, compatible with BeatmapLoader)
    ├── Images/                       # Image resources (covers, note skins, backgrounds, button icons)
    └── Configs/                      # Config files (default config, player saves, leaderboard data)


Core Function Description

1. Core Game Flow

1. Players select a song and difficulty in the main window.

2. The game loads the corresponding beatmap and audio file, initializing player state and game configuration.

3. Audio plays, notes fall with the rhythm, and players press corresponding keys to hit the notes.

4. Real-time judgment of key press results (Perfect/Great/Good/Miss), updating scores and combos.

5. After the game ends, play statistics are summarized, the result interface is displayed, and the leaderboard is updated.

2. Key Module Functions

Module

Core Responsibilities

Key Features

NoteManager

Note lifecycle management

Note activation/expiration judgment, real-time position update, active note filtering

AudioSync

Audio and note synchronization

Accurate audio playback time acquisition, offset compensation, play/pause/stop control

ScoreCalculator

Score and judgment calculation

Judgment level mapping based on time difference, combo multiplier bonus, accuracy calculation

BeatmapLoader

Beatmap parsing and loading

Data validation, error handling, complete Beatmap instance construction

3. Judgment and Scoring Rules

- Judgment levels (sorted by time difference from smallest to largest): Perfect > Great > Good > Miss

- Base Score: Perfect (300 points), Great (100 points), Good (50 points), Miss (0 points)

- Combo Bonus: The higher the combo count, the higher the extra score multiplier; the bonus resets when the combo is broken.

- Accuracy: (Perfect*300 + Great*100 + Good*50) / (Total Notes*300) * 100%

Development Guide

1. Development Priority

Follow the order of 「Data Layer → Logic Layer → UI Layer」 to ensure each layer is complete and independently testable:

1. First implement Core/Common.h (enums/constants).

2. Implement all data classes in the Core directory (ensure getters/setters are correct and functional methods form a closed loop).

3. Implement core logic classes in the Logic directory (prioritize NoteManager andScoreCalculator).

4. Implement UI classes in the UI directory (prioritize GameScene to ensure visualization effects).

5. Integrate all modules, test the complete game flow, and improve persistence functions.

2. Code Specifications

- Class Names: PascalCase (e.g., NoteManager, GameScene).

- Function/Variable Names: camelCase (e.g.,calculateAccuracy, totalScore).

- Constants: UPPER_CASE_UNDERSCORE (e.g., DEFAULT_NOTE_SPEED).

- Comments: Use Doxygen specifications; all classes and public methods must include comments (@brief, @param, @return).

3. Testing Specifications

- After completing each core class, write independent test code to verify functions (e.g., assert tests in acceptance criteria).

- Prioritize unit testing for logic layer methods to ensure no business logic errors.

- During integration testing, focus on verifying audio-note synchronization, judgment accuracy, and score calculation correctness.

Future Iteration Plan

1. Add note skin and interface theme switching functions.

2. Support more beatmap formats (e.g., osu! beatmap format).

3. Add online multiplayer function.

4. Optimize audio playback performance and support more audio formats.

5. Add game tutorials and new player guidance interfaces.

Notes

1. Ensure the Qt Multimedia module is correctly installed; otherwise, audio cannot be played normally.

2. Beatmap file format must be compatible with the parsing logic in BeatmapLoader; otherwise, the beatmap cannot be loaded.

3. Compress large files (e.g., high-definition audio, large images) to avoid game lag.

4. Persistence files (configurations, leaderboards) are stored in the Resources/Configs/ directory by default; ensure this directory has read/write permissions.
