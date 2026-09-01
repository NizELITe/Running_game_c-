# Runner Game (C++ Console Edition)

A fast-paced, ASCII-art endless runner that plays entirely in the Windows console. Dodge obstacles, grab coins, outrun a chaser, and compete for the top spot on the local high-score board.

## Features

- **3-lane endless runner** with real-time, non-blocking keyboard controls
- **Coins (`o`) and obstacles (`X`)** that spawn and scroll down the track
- **Chaser enemy (`C`)** that appears after 2000m and hunts your lane
- **Progressive difficulty** — game speed increases at distance milestones (1000m, 2000m, 5000m)
- **Lives, score, and distance HUD** updated every frame
- **Pause / resume** support mid-run
- **Persistent high-score board** saved to `highscores.txt`
- **Simple menu system** — start a run, view high scores, or quit

## Requirements

- **Windows OS** (the game uses `<conio.h>` and `<windows.h>`, so it will not compile or run on Linux/macOS without significant changes)
- A C++ compiler with GCC extensions support (for `<bits/stdc++.h>`), such as **MinGW-w64** or **MSYS2**

## Build & Run

Using g++ (MinGW):

```bash
g++ runner.cpp -o runner.exe -std=c++17
runner.exe
```

If you're using an IDE like Code::Blocks or Visual Studio (with MinGW/Clang toolchain), just create a new console project, drop in `runner.cpp`, and build/run as usual.

## How to Play

1. Launch the game and choose **1. Start Game** from the main menu.
2. Enter your name when prompted.
3. Survive as long as you can — avoid obstacles (`X`), collect coins (`o`) for points, and steer clear of the chaser (`C`) once it appears.
4. When you run out of lives, your final score and distance are shown, and you can optionally save your score to the high-score board.

## Controls

| Key | Action |
|-----|--------|
| `A` | Move left |
| `D` | Move right |
| `W` | Move forward |
| `S` | Move backward |
| `P` | Pause |
| `R` | Resume |
| `Q` | Quit to menu |

## Scoring

- Each coin collected: **+10 points**
- Distance increases automatically over time and contributes to speed milestones
- Losing all lives (colliding with obstacles or getting caught by the chaser) ends the run

## High Scores

- Scores are appended to `highscores.txt` in the same directory as the executable
- View the top 10 scores anytime from the main menu (**2. Show High Scores**)

## File Structure

```
.
├── runner.cpp        # Full game source code
└── highscores.txt    # Auto-generated after your first saved score
```

## Notes & Known Limitations

- Windows-only due to `conio.h` / `windows.h` dependency
- `highscores.txt` uses whitespace-separated name/score pairs, so names containing spaces aren't supported
- Console window flicker may occur on some terminals due to `system("cls")` being used for screen clears

## License

Feel free to modify and distribute as you like.
