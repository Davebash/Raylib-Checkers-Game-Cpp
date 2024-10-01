Here’s an updated README for your Checkers game project, incorporating the information about `checkers.cpp`:

---

# Raylib C++ Starter Template

Raylib C++ Starter Template for Visual Studio Code on Windows.

This demo project contains a bouncing ball raylib example program.

It works with raylib version 4.5.  
Tested on both Windows 10 and Windows 11.

# Ethiopian Checkers Game (`checkers.cpp`)

This is a C++ implementation of an Ethiopian Checkers game using the raylib library. Below are the key constants, structures, and function prototypes used in the game:

### Constants for the Board and Pieces
```cpp
const int BOARD_WIDTH = 600;            // Adjusted width for the board
const int BOARD_HEIGHT = 600;           // Adjusted height to fit the screen
const int CELL_SIZE = 75;               // Size of each square in pixels
const int QORKI_SIZE = 20;              // Size of king markers
const int INFO_PANEL_WIDTH = 250;       // Width of the panel for player info
const int MAX_VALID_MOVES = 12;         // The most number of moves a player can make in one turn.
```

### Enums
- **PieceType:** Represents the type of a game piece (`NONE`, `REGULAR`, `KING`).
- **Player:** Represents the players in the game (`PLAYER1`, `PLAYER2`).

### Structs
- **Piece:** A struct to represent a game piece.
  ```cpp
  struct Piece {
      PieceType type;
      Player player;
      bool isKing;
  };
  ```
- **Position:** A struct to represent the position (location) of an item on the board.
  ```cpp
  struct Position {
      int x; // Horizontal component
      int y; // Vertical component
  };
  ```
- **GameState:** A struct that stores all the information about the game at any point.
  ```cpp
  struct GameState {
      Piece board[BOARD_HEIGHT / CELL_SIZE][BOARD_WIDTH / CELL_SIZE]; // 2D array representing the game board
      Player currentPlayer; // Tracks which player's turn it is
      int player1Score; // Score for player 1
      int player2Score; // Score for player 2
      bool pieceSelected; // Indicates if a piece has been selected for movement
      int selectedX; // X-coordinate of the selected piece
      int selectedY; // Y-coordinate of the selected piece
      Position validMoves[MAX_VALID_MOVES]; // List of possible moves for the selected piece
      int validMoveCount; // Number of valid moves available
      bool isCapturing; // Tracks if a piece is in the middle of a capture sequence
  };
  ```

### Function Prototypes

- `void InitializeGame(GameState &gameState);`  
  Sets up the game at the start, including placing pieces on the board.

- `void DrawBoard(GameState &gameState);`  
  Draws the game board on the screen based on the current game state.

- `void HandleInput(GameState &gameState);`  
  Handles user input, selecting pieces, and managing their movements, including capturing logic.

- `void SaveGame(const GameState &gameState, const string &filename);`  
  Saves the current game state to a file.

- `void LoadGame(GameState &gameState, const string &filename);`  
  Loads a previously saved game state from a file.

- `bool CheckGameOver(const GameState &gameState, int currentPlayer, int &winner);`  
  Checks if the game is over and determines the winner.

- `void PromoteToKing(GameState &gameState, int x, int y);`  
  Promotes a piece to a king if it reaches the opposite side of the board.

- `void SwitchTurn(GameState &gameState);`  
  Switches the turn to the next player.

- `bool IsValidMove(GameState &gameState, int startX, int startY, int endX, int endY);`  
  Validates whether a move is legal.

- `void FindValidMoves(GameState &gameState, int x, int y, bool isAfterCapture);`  
  Finds all possible valid moves for a piece.

# Video Tutorial

<p align="center">
  <img src="preview.jpg" alt="Project Preview" width="800">
</p>

<p align="center">
🎥 <a href="https://www.youtube.com/watch?v=PaAcVk5jUd8">Video Tutorial on YouTube</a>
</p>

<br>
<br>
<p align="center">
| 📺 <a href="https://www.youtube.com/channel/UC3ivOTE5EgpmF2DHLBmWIWg">My YouTube Channel</a>
| 🌍 <a href="http://www.educ8s.tv">My Website</a> | <br>
</p>

---

This updated README now includes the details specific to your `checkers.cpp` file for the Ethiopian Checkers game.