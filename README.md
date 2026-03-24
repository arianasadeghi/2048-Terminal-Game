## Terminal Game (C & Ncurses)

A feature-rich, terminal-based recreation of the classic 2048 puzzle game, developed in **C**. This project features a custom UI built with the **ncurses** library, state persistence, and a built-in "Auto-Move" algorithm.


* **Custom TUI with Ncurses:** Interactive grid rendering with dynamic color-coding for each tile value (2, 4, 8, etc.) to enhance gameplay visibility.
* **Game State Persistence:** Implements **Save** and **Resume** functionality using binary file I/O (.bin files) to store player scores and board configurations.
* **Undo System:** Allows players to revert their last move by caching the previous board state.
* **Auto-Play/Smart Hint Engine:** Includes a "Move Alone" (muta_singur) feature that determines the optimal direction based on score maximization and available free cells.
* **Real-time Stats:** Displays live score, current date/time, and an integrated command menu within the terminal.

##  Technologies & Tools
* **Language:** C
* **Libraries:** ncurses (Terminal User Interface)
* **Build System:** Makefile

##  How to Run Locally

### Prerequisites
You need the ncurses development library installed on your Linux/WSL environment:
* sudo apt-get install libncurses5-dev libncursesw5-dev

### Build & Execute

Navigate to the directory:
* cd 2048-Terminal-Game

Compile using the Makefile:
* make

Run the game:
* make run

## Controls
* **W / Arrow Up:** Move Up
* **S / Arrow Down:** Move Down
* **A / Arrow Left:** Move Left
* **D / Arrow Right:** Move Right
* **Z:** Undo last move
* **Q:** Quit & Save option