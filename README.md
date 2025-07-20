# Minesweeper Clone

A simple clone of the classic Minesweeper game — written in C using [raylib](https://www.raylib.com/).  
This was my first ever project — and somehow it still compiles.

## 🎮 Features

- Classic Minesweeper gameplay (left-click reveal, right-click flag).
- Simple grid rendering using raylib.
- Multiple grid sizes and difficulties.
- Scoring support which is saved as a readable .txt file.
- Power-ups (see below).
- Background music support (see below).

## 🛠️ Build Instructions (Windows Only)

### 🔧 Prerequisites:
- A C99 compiler.
- CMake 3.11+.

### ⚙️ Build Steps:

```sh
git clone https://github.com/DarkiCraft/Minesweeper-Clone.git
cd Minesweeper-Clone

mkdir build
cmake -S . -B build
cmake --build build
```
Then to run:
```sh
./minesweeper
```

## 🚀 Power-Ups

This project's twist, basically. There is a 10% chance for a power-up to trigger on every tile reveal.

Included power-ups are:
1. Reveals a 3x3 area (50% chance).
2. Reveals all tiles in that row (20% chance).
3. Reveals all tiles in that column (20% chance).
4. Reveals all tiles in that row and column (10% chance).

Tiles revealed via power-ups stay revealed permanently but DO NOT recursively reveal adjacent tiles.

## 🎵 Music

Uses copywrite-free music.

To enable custom background music:
1. Drop any `music.mp3` file in the same directory as `minesweeper.exe`, replacing any existing file.
2. The game will auto-load and play it on launch.

## 💡 Possible Improvements

- Resizable window and scalable UI.
- Sprites instead of colors denoting flags and mines.
- Animations on reveal / power-up use.
- Sound effects accompanying actions.

## 📄 License

This project is released under the [MIT License](LICENSE).