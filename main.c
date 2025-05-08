#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "raylib.h"

#define WINDOWWIDTH 720
#define WINDOWHEIGHT 720

typedef struct minesweeperCell {
  int i;
  int j;
  int number;
  bool mine;
  bool shown;
  bool flag;
} minesweeperCell;

typedef struct gridSize {
  int size;
  char name[12];
  bool selected;
  Rectangle rectangle;
} gridSize;

typedef struct gridMine {
  int multiplier;
  char name[12];
  bool selected;
  Rectangle rectangle;
} gridMine;

typedef struct vector2Int {
  int i;
  int j;
} vector2Int;

typedef enum gameState {
  won = 1,
  lost = 0,
  progress = -1,
  menu = -2,
} gameState;

minesweeperCell **grid;

int rows;
int cols;

void initializeHardware();
void initializeGrid();
void drawGrid(gameState);
void menuSizeFunctions(gridSize *);
void menuMineFunctions(gridMine *);
void menuScoreFunctions(Rectangle, Rectangle, int);
bool menuStartFunctions(Rectangle);
void updateMinesNumber(gridSize *, gridMine *);
void placeMines(int, int, gridMine *, int);
void placeNumbers();
void drawOutline(minesweeperCell);
void drawminesweeperCell(minesweeperCell, gameState);
void drawNumber(int, int, int);
void leftClickFunction(gameState *, bool *, vector2Int, gridMine *, int *,
                       int *);
void rightClickFunction(int *);
void showAll();
void recursivelyShowminesweeperCells(int, int, int *);
void activatePowers(vector2Int, int *);
vector2Int getMouseCoordinates();
void displayMines(int);
void displayScore(int *, gameState, double *, double);
void displayTimer(double, gameState, int *, int *);
gameState checkGameWon(gameState, int);
void drawEndScreen(gameState);
bool restartFunctions(Rectangle);
void deinitializeHardware();

int main(void) {
  initializeHardware();

  gridSize gridSizes[3] = {{12, "12 × 12", true, {82, 234, 180, 72}},
                           {16, "16 × 16", false, {279, 234, 182, 72}},
                           {20, "20 × 20", false, {478, 234, 180, 72}}};

  gridMine gridMines[3] = {{12, "", true, {82, 342, 180, 72}},
                           {10, "", false, {279, 342, 182, 72}},
                           {8, "", false, {478, 342, 180, 72}}};

  Rectangle startButton = {262, 702, 216, 72};
  Rectangle restartButton = {226, 630, 288, 72};
  Rectangle highScorebutton = {190, 468, 360, 72};
  Rectangle scoresbutton = {226, 576, 288, 72};

  rows = 14;
  cols = 14;

  FILE *file;
  file = fopen("saves/highScore.txt", "r");
  int highScore;
  fscanf(file, "%d", &highScore);
  fclose(file);

  Music music = LoadMusicStream("resources/music.mp3");
  PlayMusicStream(music);

  while (true) {
    initializeGrid();

    gameState state = menu;
    int score = 50;

    while (true) {
      UpdateMusicStream(music);
      if (WindowShouldClose()) {
        deinitializeHardware();
        return 0;
      }

      updateMinesNumber(gridSizes, gridMines);
      BeginDrawing();
      ClearBackground(RAYWHITE);

      drawGrid(state);
      DrawRectangle(10, 10, WINDOWWIDTH, WINDOWHEIGHT + 80,
                    Fade(LIGHTGRAY, 0.5f));

      menuScoreFunctions(highScorebutton, scoresbutton, highScore);
      menuSizeFunctions(gridSizes);
      menuMineFunctions(gridMines);

      if (menuStartFunctions(startButton)) {
        EndDrawing();
        break;
      }

      EndDrawing();
    }

    state = progress;
    int mines = 0;
    for (int i = 0; i < 3; i++) {
      if (gridMines[i].selected == true) {
        mines = (rows - 2) * (cols - 2) / gridMines[i].multiplier;
        break;
      }
    }

    int flagsLeft = mines;
    bool setMinesFlag = false;

    double startTime = GetTime();
    double lastTime = startTime;

    int minutes = 0;
    int seconds = 0;

    while (true) {
      UpdateMusicStream(music);
      if (WindowShouldClose()) {
        deinitializeHardware();
        return 0;
      }

      BeginDrawing();
      ClearBackground(RAYWHITE);

      drawGrid(state);

      vector2Int coordinate = getMouseCoordinates();
      if (IsKeyPressed(KEY_SPACE)) {
        showAll();
      }
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        leftClickFunction(&state, &setMinesFlag, coordinate, gridMines, &mines,
                          &score);
      } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        rightClickFunction(&flagsLeft);
      }

      displayMines(flagsLeft);
      displayTimer(startTime, state, &minutes, &seconds);
      displayScore(&score, state, &lastTime, startTime);

      EndDrawing();

      if (state != lost && checkGameWon(state, flagsLeft) == won) {
        state = won;
      }
      if (state != progress) {
        break;
      }
    }

    while (true) {
      UpdateMusicStream(music);

      if (WindowShouldClose()) {
        deinitializeHardware();
        return 0;
      }

      BeginDrawing();
      ClearBackground(RAYWHITE);

      drawGrid(state);
      drawEndScreen(state);

      displayMines(flagsLeft);
      displayTimer(startTime, state, &minutes, &seconds);
      displayScore(&score, state, &lastTime, startTime);

      if (restartFunctions(restartButton)) {
        EndDrawing();
        break;
      }

      EndDrawing();
    }

    FILE *file;
    file = fopen("saves/highScore.txt", "r");

    fscanf(file, "%d", &highScore);
    fclose(file);

    if (score > highScore) {
      file = fopen("saves/highScore.txt", "w");
      fprintf(file, "%03d", score);
      highScore = score;

      fclose(file);
    }

    file = fopen("saves/scores.txt", "a");

    time_t currentTime = time(NULL);
    fprintf(file, "Score: %03d\nDated: %s", score, ctime(&currentTime));
    fprintf(file, "-------------------------------\n");

    fclose(file);
  }

  deinitializeHardware();

  return 0;
}

void initializeHardware() {
  if (DirectoryExists("saves") == false) {
    system("mkdir saves");

    FILE *highScore, *scores;

    highScore = fopen("saves/highScore.txt", "w");
    scores = fopen("saves/scores.txt", "w");

    fprintf(highScore, "%d", 0);

    fclose(highScore);
    fclose(scores);
  }

  InitWindow(WINDOWWIDTH + 20, WINDOWHEIGHT + 20 + 80, "Minesweeper");
  SetTargetFPS(60);

  InitAudioDevice();

  srand(time(NULL));
}

void initializeGrid() {
  free(grid);
  grid = NULL;

  grid = (minesweeperCell **)calloc(rows, sizeof(minesweeperCell *));
  for (int i = 0; i < cols; i++) {
    grid[i] = (minesweeperCell *)calloc(cols, sizeof(minesweeperCell));
  }

  for (int i = 1; i < rows; i++) {
    for (int j = 1; j < cols; j++) {
      grid[i][j] = (minesweeperCell){.i = i - 1, .j = j - 1};
    }
  }
}

void menuSizeFunctions(gridSize *gridSizes) {
  for (int i = 0; i < 3; i++) {
    if (gridSizes[i].selected == true) {
      DrawRectangleRec(gridSizes[i].rectangle, GRAY);
    } else {
      DrawRectangleRec(gridSizes[i].rectangle, RAYWHITE);
    }

    DrawRectangleLinesEx(gridSizes[i].rectangle, 2, BLACK);
    DrawText(gridSizes[i].name,
             gridSizes[i].rectangle.x + gridSizes[i].rectangle.width / 2 -
                 MeasureText(gridSizes[i].name, 36) / 2,
             gridSizes[i].rectangle.y + gridSizes[i].rectangle.height / 2 - 18,
             35, BLACK);

    if (CheckCollisionPointRec(GetMousePosition(), gridSizes[i].rectangle) &&
        gridSizes[i].selected == false) {
      DrawRectangleLinesEx(gridSizes[i].rectangle, 5, DARKGREEN);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        gridSizes[i].selected = true;
        for (int j = 0; j < 3; j++) {
          if (j == i) {
            continue;
          }
          gridSizes[j].selected = false;
        }
        rows = gridSizes[i].size + 2;
        cols = gridSizes[i].size + 2;

        initializeGrid();
      }
    }
  }
}

void menuMineFunctions(gridMine *gridMines) {
  for (int i = 0; i < 3; i++) {
    if (gridMines[i].selected == true) {
      DrawRectangleRec(gridMines[i].rectangle, GRAY);
    } else {
      DrawRectangleRec(gridMines[i].rectangle, RAYWHITE);
    }

    DrawRectangleLinesEx(gridMines[i].rectangle, 2, BLACK);
    DrawText(gridMines[i].name,
             gridMines[i].rectangle.x + gridMines[i].rectangle.width / 2 -
                 MeasureText(gridMines[i].name, 36) / 2,
             gridMines[i].rectangle.y + gridMines[i].rectangle.height / 2 - 18,
             35, BLACK);

    if (CheckCollisionPointRec(GetMousePosition(), gridMines[i].rectangle) &&
        gridMines[i].selected == false) {
      DrawRectangleLinesEx(gridMines[i].rectangle, 5, DARKGREEN);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        gridMines[i].selected = true;
        for (int j = 0; j < 3; j++) {
          if (j == i) {
            continue;
          }
          gridMines[j].selected = false;
        }
      }
    }
  }
}

bool menuStartFunctions(Rectangle startButton) {
  DrawRectangleRec(startButton, WHITE);
  DrawText(
      "Start!",
      startButton.x + startButton.width / 2 - MeasureText("Start!", 36) / 2,
      startButton.y + startButton.height / 2 - 18, 36, BLACK);

  if (CheckCollisionPointRec(GetMousePosition(), startButton)) {
    DrawRectangleLinesEx(startButton, 5, DARKGREEN);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      return true;
    }
  } else {
    DrawRectangleLinesEx(startButton, 2, BLACK);
  }

  return false;
}

void menuScoreFunctions(Rectangle highScore, Rectangle scoresButton,
                        int score) {
  DrawRectangleRec(highScore, WHITE);
  DrawText(TextFormat("High Score: %03d", score),
           highScore.x + highScore.width / 2 -
               MeasureText(TextFormat("High Score: %03d", score), 36) / 2,
           highScore.y + highScore.height / 2 - 18, 36, BLACK);
  DrawRectangleLinesEx(highScore, 2, BLACK);

  DrawRectangleRec(scoresButton, WHITE);
  DrawText("Open Scores",
           scoresButton.x + scoresButton.width / 2 -
               MeasureText("Open Scores", 36) / 2,
           scoresButton.y + scoresButton.height / 2 - 18, 36, BLACK);

  if (CheckCollisionPointRec(GetMousePosition(), scoresButton)) {
    DrawRectangleLinesEx(scoresButton, 5, DARKGREEN);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      system("start saves/scores.txt");
    }
  } else {
    DrawRectangleLinesEx(scoresButton, 2, BLACK);
  }
}

void updateMinesNumber(gridSize *gridSizes, gridMine *gridMines) {
  int temp;
  if (gridSizes[0].selected == true) {
    temp = 12;
  } else if (gridSizes[1].selected == true) {
    temp = 16;
  } else {
    temp = 20;
  }
  for (int i = 0; i < 3; i++) {
    sprintf(gridMines[i].name, "%2d Mines",
            temp * temp / gridMines[i].multiplier);
  }
}

void placeMines(int i, int j, gridMine *gridMines, int mines) {
  int count = 0;
  while (count < mines) {
    int randomRow = rand() % rows;
    int randomCol = rand() % cols;

    if ((abs(randomRow - i) <= 1 && abs(randomCol - j) <= 1) || randomRow < 1 ||
        randomRow >= rows - 1 || randomCol < 1 || randomCol >= cols - 1) {
      continue;
    }

    if (grid[randomRow][randomCol].mine == false) {
      int surroundingMinesCount = 0;
      for (int a = randomRow - 1; a <= randomRow + 1; a++) {
        for (int b = randomCol - 1; b <= randomCol + 1; b++) {
          if (grid[a][b].mine == true) {
            surroundingMinesCount++;
          }
        }
      }
      if (surroundingMinesCount < 4) {
        grid[randomRow][randomCol].mine = true;
        count++;
      }
    }
  }
}

void placeNumbers() {
  for (int i = 1; i < rows - 1; i++) {
    for (int j = 1; j < cols - 1; j++) {
      if (grid[i][j].mine == true) {
        continue;
      }
      for (int a = i - 1; a <= i + 1; a++) {
        for (int b = j - 1; b <= j + 1; b++) {
          if (a == i && b == j) {
            continue;
          }

          if (grid[a][b].mine == true) {
            grid[i][j].number++;
          }
        }
      }
    }
  }
}

void drawGrid(gameState state) {
  for (int i = 1; i < rows - 1; i++) {
    for (int j = 1; j < cols - 1; j++) {
      drawminesweeperCell(grid[i][j], state);
      drawOutline(grid[i][j]);
    }
  }
}

void drawminesweeperCell(minesweeperCell cell, gameState state) {
  int cellWidth = (WINDOWWIDTH / (cols - 2));
  int cellHeight = (WINDOWHEIGHT / (rows - 2));

  if (cell.shown == true) {
    if (cell.flag == true) {
      DrawRectangle(10 + cell.i * cellWidth, 90 + cell.j * cellHeight,
                    cellWidth, cellHeight, YELLOW);
    } else if (cell.mine == true) {
      DrawRectangle(10 + cell.i * cellWidth, 90 + cell.j * cellHeight,
                    cellWidth, cellHeight, RED);
    } else {
      DrawRectangle(10 + cell.i * cellWidth, 90 + cell.j * cellHeight,
                    cellWidth, cellHeight, LIGHTGRAY);
      drawNumber(cell.i, cell.j, cell.number);
    }
  } else {
    DrawRectangle(10 + cell.i * cellWidth, 90 + cell.j * cellHeight, cellWidth,
                  cellHeight, RAYWHITE);
  }

  vector2Int coordinate = getMouseCoordinates();
  if (state == progress && coordinate.i > 0 && coordinate.i < rows - 1 &&
      coordinate.j > 0 && coordinate.j < cols - 1) {
    DrawRectangleLines(11 + (coordinate.i - 1) * cellWidth,
                       11 + 80 + (coordinate.j - 1) * cellHeight, cellWidth - 2,
                       cellHeight - 2, DARKGREEN);
    DrawRectangleLines(12 + (coordinate.i - 1) * cellWidth,
                       12 + 80 + (coordinate.j - 1) * cellHeight, cellWidth - 4,
                       cellHeight - 4, DARKGREEN);
  }
}

void drawNumber(int i, int j, int number) {
  int cellWidth = (WINDOWWIDTH / (cols - 2));
  int cellHeight = (WINDOWHEIGHT / (rows - 2));

  if (number == 0) {
    // DO NOTHING
  } else if (number == 1) {
    DrawText(TextFormat("%c", number + '0'),
             i * cellHeight + (cellHeight / 2) + 6,
             j * cellWidth + (cellWidth / 2) - 3 + 80, 30, BLUE);
  } else {
    DrawText(TextFormat("%c", number + '0'),
             i * cellHeight + (cellHeight / 2) + 2,
             j * cellWidth + (cellWidth / 2) - 3 + 80, 30, BLUE);
  }
}

void drawOutline(minesweeperCell cell) {
  int cellWidth = (WINDOWWIDTH / (cols - 2));
  int cellHeight = (WINDOWHEIGHT / (rows - 2));

  DrawRectangleLines(10 + cell.i * cellWidth, 90 + cell.j * cellHeight,
                     cellWidth, cellHeight, BLACK);

  DrawRectangleLines(10, 10, WINDOWWIDTH, WINDOWHEIGHT + 80, BLACK);
  DrawRectangleLines(11, 11, WINDOWWIDTH, WINDOWHEIGHT + 80, BLACK);
  DrawRectangleLines(11, 91, WINDOWWIDTH, WINDOWHEIGHT, BLACK);
}

void leftClickFunction(gameState *state, bool *setMinesFlag,
                       vector2Int coordinate, gridMine *gridMines, int *mines,
                       int *score) {
  if (grid[coordinate.i][coordinate.j].flag == true) {
    // DO NOTHING
  } else if (grid[coordinate.i][coordinate.j].mine == true) {
    showAll();
    *state = lost;
  } else {
    if (*setMinesFlag == false) {
      placeMines(coordinate.i, coordinate.j, gridMines, *mines);
      placeNumbers();
      *setMinesFlag = true;

      recursivelyShowminesweeperCells(coordinate.i, coordinate.j, score);
    } else if (rand() % 10 == 0) {
      activatePowers(coordinate, mines);
    } else {
      recursivelyShowminesweeperCells(coordinate.i, coordinate.j, score);
    }
  }
}

void rightClickFunction(int *flagsLeft) {
  vector2Int coordinate = getMouseCoordinates();

  if (grid[coordinate.i][coordinate.j].shown == true &&
      grid[coordinate.i][coordinate.j].flag == false) {
    // DO NOTHING
  } else {
    if (grid[coordinate.i][coordinate.j].flag == false && *flagsLeft > 0) {
      (*flagsLeft)--;
      grid[coordinate.i][coordinate.j].flag = true;
      grid[coordinate.i][coordinate.j].shown = true;
    } else if (grid[coordinate.i][coordinate.j].flag == true) {
      (*flagsLeft)++;
      grid[coordinate.i][coordinate.j].flag = false;
      grid[coordinate.i][coordinate.j].shown = false;
    }
  }
}

void showAll() {
  for (int i = 1; i < rows; i++) {
    for (int j = 1; j < rows; j++) {
      if (grid[i][j].mine == true) {
        grid[i][j].shown = true;
      }
      grid[i][j].flag = false;
    }
  }
}

void recursivelyShowminesweeperCells(int i, int j, int *score) {
  if (i < 1 || i >= rows - 1 || j < 1 || j >= cols - 1 ||
      grid[i][j].shown == true || grid[i][j].mine == true) {
    return;
  }

  grid[i][j].shown = true;
  (*score)++;

  if (grid[i][j].number != 0) {
    return;
  }

  for (int a = -1; a <= 1; a++) {
    for (int b = -1; b <= 1; b++) {
      recursivelyShowminesweeperCells(i + a, j + b, score);
    }
  }
}

void activatePowers(vector2Int coordinate, int *mines) {
  switch (rand() % 10) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      for (int a = coordinate.i - 1; a <= coordinate.i + 1; a++) {
        for (int b = coordinate.j - 1; b <= coordinate.j + 1; b++) {
          grid[a][b].shown = true;
          if (grid[a][b].mine == true) {
            grid[a][b].flag = true;
            (*mines)--;
          }
        }
      }
      break;
    case 5:
    case 6:
      for (int a = 1; a < cols; a++) {
        grid[coordinate.i][a].shown = true;
      }
      break;
    case 7:
    case 8:
      for (int a = 1; a < rows; a++) {
        grid[a][coordinate.j].shown = true;
      }
      break;
    case 9:
      for (int a = 1; a < cols; a++) {
        grid[coordinate.i][a].shown = true;
      }
      for (int a = 1; a < rows; a++) {
        grid[a][coordinate.j].shown = true;
      }
      break;
  }
}

vector2Int getMouseCoordinates() {
  int cellWidth = (WINDOWWIDTH / (cols - 2));
  int cellHeight = (WINDOWHEIGHT / (rows - 2));

  Vector2 mousePosition = GetMousePosition();
  vector2Int result = {
      .i = (int)(mousePosition.x - 10) / cellWidth + 1,
      .j = (int)(mousePosition.y - 90) / cellHeight + 1,
  };
  return result;
}

void displayTimer(double startTime, gameState state, int *minutes,
                  int *seconds) {
  if (state == progress) {
    double elapsedTime = GetTime() - startTime;
    *minutes = (int)floor(elapsedTime / 60);
    *seconds = (int)fmod(elapsedTime, 60);
  }

  DrawText(TextFormat("Time: %02d:%02d", *minutes, *seconds), 500, 30, 40,
           BLACK);
}

void displayScore(int *score, gameState state, double *lastTime,
                  double startTime) {
  if (state == progress) {
    double currentTime = GetTime();

    if (currentTime - *lastTime >= 1.0) {
      if (*score > 0) {
        (*score)--;
      }
      *lastTime = currentTime;
    }
  }
  DrawText(
      TextFormat("Score: %03d", *score),
      (740 / 2) - MeasureText(TextFormat("Score: %03d", *score), 40) / 2 - 10,
      10 + (80 - 40) / 2, 40, BLACK);
}

void displayMines(int flagsLeft) {
  DrawText(TextFormat("Flags: %02d", flagsLeft), 30, 30, 40, BLACK);
}

gameState checkGameWon(gameState state, int flagsLeft) {
  if (state != progress) {
    return state;
  }

  for (int i = 1; i < rows - 1; i++) {
    for (int j = 1; j < cols - 1; j++) {
      if (grid[i][j].mine == false &&
          (grid[i][j].shown == false || grid[i][j].flag == true)) {
        return progress;
      }
    }
  }

  return won;
}

void drawEndScreen(gameState state) {
  DrawRectangle(10, 10, WINDOWWIDTH, WINDOWHEIGHT + 80, Fade(LIGHTGRAY, 0.5f));
  DrawRectangle(WINDOWWIDTH / 4 + 10, WINDOWHEIGHT / 4 + 18,
                WINDOWWIDTH / 2 + 1, WINDOWHEIGHT / 2, BLACK);
  DrawRectangle(WINDOWWIDTH / 4 + 15, WINDOWHEIGHT / 4 + 23,
                WINDOWWIDTH / 2 - 9, WINDOWHEIGHT / 2 - 10, RAYWHITE);

  if (state == won) {
    DrawText("You Won!", WINDOWWIDTH / 2 - MeasureText("You Won!", 30) / 2 + 10,
             WINDOWHEIGHT / 2 - 7, 30, BLACK);
  } else if (state == lost) {
    DrawText("You Lost!",
             WINDOWWIDTH / 2 - MeasureText("You Lost!", 30) / 2 + 10,
             WINDOWHEIGHT / 2 + 8, 30, BLACK);
  }
}

bool restartFunctions(Rectangle restartButton) {
  DrawRectangleRec(restartButton, WHITE);
  DrawText("Back to Menu",
           restartButton.x + restartButton.width / 2 -
               MeasureText("Back to Menu", 36) / 2,
           restartButton.y + restartButton.height / 2 - 18, 36, BLACK);

  if (CheckCollisionPointRec(GetMousePosition(), restartButton)) {
    DrawRectangleLinesEx(restartButton, 5, DARKGREEN);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      return true;
    }
  } else {
    DrawRectangleLinesEx(restartButton, 3, BLACK);
  }

  return false;
}

void deinitializeHardware() {
  CloseAudioDevice();
  CloseWindow();

  for (int i = 0; i < rows; i++) {
    free(grid[i]);
  }
  free(grid);
}