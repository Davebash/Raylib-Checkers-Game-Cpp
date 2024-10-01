// @file checkers.cpp
// @brief Contains the full code for the checkers game.
// @author Dawit Zelalem
// @date 2024-08-01
// @version 1.0
// @bug No known bugs. Report bugs to davezelalem00@gmail.com or @dave_zelalem_7 via instagram.

#include "raylib.h"
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

// Constants for the board and pieces
const int BOARD_WIDTH = 600;            // Adjusted width for the board
const int BOARD_HEIGHT = 600;           // Adjusted height to fit the screen
const int CELL_SIZE = 75;               // Size of each square in pixels
const int QORKI_SIZE = 20;              // Size of king markers
const int INFO_PANEL_WIDTH = 250;       // Width of the panel for player info(The extra space on the side for player information)
const int MAX_VALID_MOVES = 12;         // The most number of moves a player can make in one turn.

enum PieceType { NONE, REGULAR, KING }; // Enum to represent the type of a game piece.
enum Player { PLAYER1, PLAYER2 }; // Enum to represent the players in the game.

// Struct to represent a game piece.
struct Piece {
    PieceType type;
    Player player;
    bool isKing;
};


// Struct to represent the position(location) of an item on a board.
struct Position {
    int x; // Horizontal component
    int y; // Vertical component
};


// This Struct is like a giant box where we store all the information about the game at any point.
struct GameState {
    Piece board[BOARD_HEIGHT / CELL_SIZE][BOARD_WIDTH / CELL_SIZE]; // 2D array representing the game board with pieces
    Player currentPlayer; // Tracks which player's turn it is (PLAYER1 or PLAYER2)
    int player1Score; // Score for player 1
    int player2Score; // Score for player 2
    bool pieceSelected; // Indicates if a piece has been selected for movement
    int selectedX; // X-coordinate of the currently selected piece
    int selectedY; // Y-coordinate of the currently selected piece
    Position validMoves[MAX_VALID_MOVES]; // Stores list of possible moves for the selected piece
    int validMoveCount; // Number of valid moves available for the selected piece
    bool isCapturing; // Tracks if a piece is currently in the middle of a capture sequence
};


// Function Prototypes
void InitializeGame(GameState &gameState); //Sets up the game at the start, including placing pieces on the board.
void DrawBoard(GameState &gameState); // Draws the game board on the screen based on the current game state.
void HandleInput(GameState &gameState);// This function is responsible for managing user inputs, selecting pieces, and handling their movements, including capturing logic.
void SaveGame(const GameState &gameState, const string &filename);// Saves the current game state to a file for later retrieval.
void LoadGame(GameState &gameState, const string &filename);// Loads a previously saved game state from a file.
bool CheckGameOver(const GameState &gameState, int currentPlayer, int &winner); // Checks if the game is over, and if so, determines the winner.
void PromoteToKing(GameState &gameState, int x, int y); // Promotes a piece to a king if it reaches the opposite side of the board.
void SwitchTurn(GameState &gameState);// Switches the turn to the next player in the game.
bool IsValidMove(GameState &gameState, int startX, int startY, int endX, int endY);// Validates whether a move from (startX, startY) to (endX, endY) is legal.
void FindValidMoves(GameState &gameState, int x, int y, bool isAfterCapture);//  Finds all the places a piece can move to.



int main() {
    // Initialization
    InitWindow(BOARD_WIDTH + INFO_PANEL_WIDTH, BOARD_HEIGHT, "Ethiopian Checkers Game");
    SetTargetFPS(60);

    GameState gameState;
    InitializeGame(gameState);

    bool gameOver = false;
    int winner = -1;  // 1 for Player 1, 2 for Player 2
    int currentPlayer = PLAYER1;  // Start with Player 1

    // Main game loop
    while (!WindowShouldClose()) {
        if (!gameOver) {
            HandleInput(gameState);  // Pass current player for input handling

            // Check for save/load commands
            if (IsKeyPressed(KEY_S)) {
                SaveGame(gameState, "checkers_save.dat");
                cout << "Game saved!\n";
            }

            if (IsKeyPressed(KEY_L)) {
                if (FileExists("checkers_save.dat")) {
                    LoadGame(gameState, "checkers_save.dat");
                    cout << "Game loaded!\n";
                } else {
                    cout << "No saved game found!\n";
                }
            }

            // Update game over state if needed
            if (CheckGameOver(gameState, currentPlayer, winner)) {
                gameOver = true;  // Stop further moves
            } else {
                // Alternate the player if game is still ongoing
                currentPlayer = (currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;
            }
        }

        BeginDrawing();// This Raylib function signals the start of drawing operations.
        ClearBackground(RAYWHITE);//Clears the screen and fills the background with the color white

        DrawBoard(gameState);//This custom function draws the game board based on the current state of gameState

        if (gameOver) {
            // Create bold effect by drawing the text multiple times in dark colors with slight offsets
            for (int offsetX = -3; offsetX <= 3; offsetX += 2) {
                for (int offsetY = -3; offsetY <= 3; offsetY += 2) {
                    if (offsetX != 0 || offsetY != 0) {
                        // Display winner message with bold effect
                        if (winner == PLAYER1) {
                            DrawText("Player 1 Wins!", BOARD_WIDTH / 2 - MeasureText("Player 1 Wins!", 60) / 2 + offsetX, BOARD_HEIGHT / 2 - 30 + offsetY, 60, DARKGRAY);
                        } else if (winner == PLAYER2) {
                            DrawText("Player 2 Wins!", BOARD_WIDTH / 2 - MeasureText("Player 2 Wins!", 60) / 2 + offsetX, BOARD_HEIGHT / 2 - 30 + offsetY, 60, DARKGRAY);
                        }

                        // Display "Press 'R' to restart or 'Q' to quit" message with bold effect
                        DrawText("Press 'R' to restart", BOARD_WIDTH / 2 - MeasureText("Press 'R' to restart", 30) / 2 + offsetX, BOARD_HEIGHT / 2 + 100 + offsetY, 30, DARKGRAY);
                        // Display "Press Q to quit" message with bold effect
                        DrawText("Press 'Q' to quit", BOARD_WIDTH / 2 - MeasureText("Press 'Q' to quit", 30) / 2 + offsetX, BOARD_HEIGHT / 2 + 50 + offsetY, 30, DARKGRAY);
                    }
                }
            }

            // Draw the main text in their colors after the offset texts
            if (winner == PLAYER1) {
                DrawText("Player 1 Wins!", BOARD_WIDTH / 2 - MeasureText("Player 1 Wins!", 60) / 2, BOARD_HEIGHT / 2 - 30, 60, RED);
            } else if (winner == PLAYER2) {
                DrawText("Player 2 Wins!", BOARD_WIDTH / 2 - MeasureText("Player 2 Wins!", 60) / 2, BOARD_HEIGHT / 2 - 30, 60, BLUE);
            }
            DrawText("Press 'R' to restart", BOARD_WIDTH / 2 - MeasureText("Press 'R' to restart", 30) / 2, BOARD_HEIGHT / 2 + 100, 30, BLACK);
            DrawText("Press 'Q' to quit", BOARD_WIDTH / 2 - MeasureText("Press 'Q' to quit", 30) / 2, BOARD_HEIGHT / 2 + 50, 30, BLACK);

            if (IsKeyPressed(KEY_Q)) {
                break;  // Exit the game loop
            }

            if (IsKeyPressed(KEY_R)) {
                InitializeGame(gameState);  // Restart the game
                gameOver = false;
                winner = -1;
                currentPlayer = PLAYER1;  // Reset to Player 1
            }
        }

        EndDrawing(); //The Raylib function signals the end of drawing operations
    }

    CloseWindow();
    return 0;
}




void InitializeGame(GameState &gameState) {
    // Initialize board with default pieces
    int player1Rows = 3;
    int player2Rows = 3;
    int boardColumns = BOARD_WIDTH / CELL_SIZE;

    for (int y = 0; y < BOARD_HEIGHT / CELL_SIZE; y++) {
        for (int x = 0; x < boardColumns; x++) {
            gameState.board[y][x].type = NONE;
            gameState.board[y][x].isKing = false;

            if ((x + y) % 2 != 0 && y < player1Rows) {
                gameState.board[y][x].type = REGULAR;
                gameState.board[y][x].player = PLAYER1;
            } else if ((x + y) % 2 != 0 && y >= (BOARD_HEIGHT / CELL_SIZE - player2Rows)) {
                gameState.board[y][x].type = REGULAR;
                gameState.board[y][x].player = PLAYER2;
            }
        }
    }

    // Set initial game state
    gameState.currentPlayer = PLAYER1;
    gameState.player1Score = 0;
    gameState.player2Score = 0;
    gameState.pieceSelected = false;
    gameState.selectedX = -1;
    gameState.selectedY = -1;
}


void DrawBoard(GameState &gameState) {
    // Colors for the board
    Color lightSquareColor = (Color){255, 255, 204, 255}; // Off-white for light squares
    Color darkSquareColor = (Color){0, 51, 0, 255};       // Deep green for dark squares

    // Draw board and pieces
    for (int y = 0; y < BOARD_HEIGHT / CELL_SIZE; y++) {
        for (int x = 0; x < BOARD_WIDTH / CELL_SIZE; x++) {
            // Draw cells with high contrast
            Color cellColor = ((x + y) % 2 == 0) ? lightSquareColor : darkSquareColor;
            DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, cellColor);

            // Highlight selected piece and valid moves
            if (gameState.pieceSelected && gameState.selectedX == x && gameState.selectedY == y) {
                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN);
            }

            
            // Draw valid moves only if a piece is selected
            if (gameState.pieceSelected) {
                for (int i = 0; i < gameState.validMoveCount && i < MAX_VALID_MOVES; i++) { // Ensure we don't exceed the validMoves array bounds
                    if (gameState.validMoves[i].x == x && gameState.validMoves[i].y == y) {
                        DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, YELLOW);
                    }
                }
            }


            // Draw pieces with modern aesthetic
            Piece piece = gameState.board[y][x];
            if (piece.type != NONE) {
                Color pieceColor = (piece.player == PLAYER1) ? (Color){200, 0, 0, 255} : (Color){0, 0, 255, 255}; // Deep red for Player1, blue for Player2
                DrawCircle(x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 2 - 10, pieceColor);

                if (piece.isKing) {
                    DrawCircle(x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2, QORKI_SIZE, (Color){255, 215, 0, 255}); // Thin gold ring
                }
            }
        }
    }

    // Draw player info panel with high contrast colors
    int infoPanelX = BOARD_WIDTH;// where the info panal starts
    Color panelColor = lightSquareColor; // Light background for the panel
    Color titleColor = darkSquareColor; // Dark color for title
    Color playerTextColor = (Color){0, 0, 0, 255}; // Dark black for player names
    Color scoreTextColor = (Color){0, 100, 0, 255}; // Dark green for scores
    Color turnIndicatorColor = (gameState.currentPlayer == PLAYER1) ? (Color){200, 0, 0, 255} : (Color){0, 0, 255, 255}; // Deep red for Player1's turn, blue for Player2's turn

    DrawRectangle(infoPanelX, 0, INFO_PANEL_WIDTH, BOARD_HEIGHT, panelColor); // Info panel background

    // Draw scoreboard title
    DrawText("SCOREBOARD", infoPanelX + 15, 10, 30, titleColor); // Dark color for title

    // Draw Player 1's info
    DrawText("Player1", infoPanelX + 15, 50, 22, playerTextColor); // Dark black for player names
    DrawText(("Score: " + to_string(gameState.player1Score)).c_str(), infoPanelX + 15, 80, 22, scoreTextColor); // Dark green for scores
    int player1Pieces = 0;
    for (int y = 0; y < BOARD_HEIGHT / CELL_SIZE; y++) {
        for (int x = 0; x < BOARD_WIDTH / CELL_SIZE; x++) {
            if (gameState.board[y][x].type != NONE && gameState.board[y][x].player == PLAYER1) {
                player1Pieces++;
            }
        }
    }
    DrawText(("Pieces: " + to_string(player1Pieces)).c_str(), infoPanelX + 15, 110, 22, scoreTextColor); // Dark green for remaining pieces

    // Draw Player 2's info
    DrawText("Player2", infoPanelX + 15, 150, 22, playerTextColor); // Dark black for player names
    DrawText(("Score: " + to_string(gameState.player2Score)).c_str(), infoPanelX + 15, 180, 22, scoreTextColor); // Dark green for scores
    int player2Pieces = 0;
    for (int y = 0; y < BOARD_HEIGHT / CELL_SIZE; y++) {
        for (int x = 0; x < BOARD_WIDTH / CELL_SIZE; x++) {
            if (gameState.board[y][x].type != NONE && gameState.board[y][x].player == PLAYER2) {
                player2Pieces++;
            }
        }
    }
    DrawText(("Pieces: " + to_string(player2Pieces)).c_str(), infoPanelX + 15, 210, 22, scoreTextColor); // Dark green for remaining pieces

    // Draw turn indicator
    DrawText("Turn: ", infoPanelX + 15, 250, 22, playerTextColor); // Dark black for turn label
    DrawRectangle(infoPanelX + 90, 250, 20, 20, turnIndicatorColor); // Small colored box to indicate whose turn it is

    // Draw save/load instructions
    DrawText("To Save Press 'S'", infoPanelX + 15, 280, 22, DARKGRAY); 
    DrawText("To Load Press 'L'", infoPanelX + 15, 310, 22, DARKGRAY); 
}





void HandleInput(GameState &gameState) {
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    
    // Convert the mouse position to board coordinates
    int x = mouseX / CELL_SIZE;
    int y = mouseY / CELL_SIZE;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!gameState.pieceSelected) {
            // Only allow selecting a new piece if no multi-capture is ongoing
            if (!gameState.isCapturing) {
                // Select a piece
                if (x >= 0 && x < BOARD_WIDTH / CELL_SIZE && y >= 0 && y < BOARD_HEIGHT / CELL_SIZE &&
                    gameState.board[y][x].type != NONE && gameState.board[y][x].player == gameState.currentPlayer) {
                    gameState.pieceSelected = true;
                    gameState.selectedX = x;
                    gameState.selectedY = y;
                    //calculates all the valid moves for the selected piece.
                    FindValidMoves(gameState, x, y, false);  // false indicates initial move.
                }
            }
        } else {
            // Prevent selecting another piece during multiple capture scenario
            if (!gameState.isCapturing && gameState.board[y][x].type != NONE && gameState.board[y][x].player == gameState.currentPlayer) {
                // Allow selecting another piece only if a capture sequence is not ongoing
                gameState.pieceSelected = true;
                gameState.selectedX = x;
                gameState.selectedY = y;
                FindValidMoves(gameState, x, y, false);  // Recalculate valid moves for the new piece
            } else if (IsValidMove(gameState, gameState.selectedX, gameState.selectedY, x, y)) {
                // Move the selected piece
                Piece &movingPiece = gameState.board[gameState.selectedY][gameState.selectedX];
                Piece &destinationPiece = gameState.board[y][x];
                
                // Determine if the move is a capture by checking if the piece jumps over an opponent's piece
                bool isCapture = abs(gameState.selectedX - x) > 1 && abs(gameState.selectedY - y) > 1;

                // Handle capture
                if (isCapture) {
                    // King long-range capture logic
                    if (movingPiece.isKing) {

                        // To determine direction of movement based on the destination piece(the direction in which the piece has moved)
                        int dx = (x > gameState.selectedX) ? 1 : -1;
                        int dy = (y > gameState.selectedY) ? 1 : -1;
                        // Coordinates of the first square that the piece will check after moving
                        int captureX = gameState.selectedX + dx;
                        int captureY = gameState.selectedY + dy;
                        bool captureAllowed = false;

                        // Traverse diagonally to find opponent's piece and capture
                        while (captureX != x && captureY != y) {
                            if (gameState.board[captureY][captureX].type != NONE &&
                                gameState.board[captureY][captureX].player != gameState.currentPlayer) {
                                captureAllowed = true;  // Capture is valid only if an opponent's piece is found
                                gameState.board[captureY][captureX].type = NONE;  // Capture opponent piece

                                // Update scores
                                if (gameState.currentPlayer == PLAYER1) {
                                    gameState.player1Score++;
                                } else {
                                    gameState.player2Score++;
                                }
                                break;  // Stop after capturing the first piece
                            }
                            captureX += dx;
                            captureY += dy;
                        }

                        if (!captureAllowed) {
                            // If no valid capture was found, treat it as a regular move
                            isCapture = false;
                        } else {
                            // Check for additional captures, but restrict backward captures
                            int nextX, nextY;  // potential positions for additional capture
                            bool additionalCapturePossible = false;

                            // The nested loops iterate through potential directions for capturing additional pieces.
                            for (int directionX = -1; directionX <= 1; directionX += 2) {
                                for (int directionY = 1; directionY >= -1; directionY -= 2) {  // Only forward captures
                                    nextX = x + directionX * 2;
                                    nextY = y + directionY * 2;
                                    
                                    if (nextX >= 0 && nextX < BOARD_WIDTH / CELL_SIZE &&
                                        nextY >= 0 && nextY < BOARD_HEIGHT / CELL_SIZE &&
                                        gameState.board[nextY][nextX].type == NONE &&
                                        gameState.board[y + directionY][x + directionX].player != gameState.currentPlayer &&
                                        gameState.board[y + directionY][x + directionX].type != NONE) {
                                        
                                        additionalCapturePossible = true;
                                        break;  // Additional capture found, so break out of the loop
                                    }
                                }

                                // If we found another piece to capture, stop looking for more captures in that direct
                                if (additionalCapturePossible) break;
                            }

                            if (!additionalCapturePossible) {
                                isCapture = false;
                            }
                        }
                    } else {
                        // Regular piece capture (same as before)
                        int capturedX = (gameState.selectedX + x) / 2;
                        int capturedY = (gameState.selectedY + y) / 2;
                        gameState.board[capturedY][capturedX].type = NONE;

                        // Update scores
                        if (gameState.currentPlayer == PLAYER1) {
                            gameState.player1Score++;
                        } else {
                            gameState.player2Score++;
                        }
                    }

                    // Mark that the capturing sequence is ongoing
                    gameState.isCapturing = true;
                }

                // Move the piece
                destinationPiece = movingPiece;
                movingPiece.type = NONE;

                bool wasKing = movingPiece.isKing;
                PromoteToKing(gameState, x, y);

                // If the piece was promoted to a King, force the player to switch turns
                if (!wasKing && destinationPiece.isKing) {
                    gameState.pieceSelected = false;
                    gameState.validMoveCount = 0;
                    gameState.isCapturing = false;  // Reset capturing state
                    SwitchTurn(gameState);
                    return;
                }

                // Check if the King can make additional captures
                FindValidMoves(gameState, x, y, true);  // true indicates after capture

                if (isCapture && gameState.validMoveCount > 0) {
                    // Keep the piece selected if more captures are possible
                    gameState.pieceSelected = true;
                    gameState.selectedX = x;
                    gameState.selectedY = y;
                } else {
                    // No additional captures or it's a regular move, switch the turn
                    gameState.pieceSelected = false;
                    gameState.validMoveCount = 0;
                    gameState.isCapturing = false;  // Reset capturing state
                    SwitchTurn(gameState);
                }
            } else {
                // Invalid move or trying to select a different piece during capture
                if (gameState.isCapturing) {
                    // Prevent selecting other pieces during multi-capture
                    return;
                }

                // Deselect the piece if it's an invalid move
                gameState.pieceSelected = false;
                gameState.validMoveCount = 0;
            }
        }
    }
}





void SwitchTurn(GameState &gameState) {
    gameState.currentPlayer = (gameState.currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;
}


bool IsValidMove(GameState &gameState, int startX, int startY, int endX, int endY) {
    for (int i = 0; i < gameState.validMoveCount; ++i) {
        Position move = gameState.validMoves[i];
        if (move.x == endX && move.y == endY) {
            return true;
        }
    }

    return false;
}


void FindValidMoves(GameState &gameState, int x, int y, bool isAfterCapture) {
    gameState.validMoveCount = 0;

    Piece piece = gameState.board[y][x];
    // Array to store movement direction vectors for a piece (dx, dy) for four diagonal directions
    int directions[4][2];

    if (piece.isKing) {
        // Kings can move and capture in all directions
        directions[0][0] = 1; directions[0][1] = 1;   // Down-right
        directions[1][0] = -1; directions[1][1] = 1;  // Down-left
        directions[2][0] = 1; directions[2][1] = -1;  // Up-right
        directions[3][0] = -1; directions[3][1] = -1; // Up-left
        int numDirections = 4;

        bool captureAvailable = false;  // Track if there's any capture available

        for (int i = 0; i < numDirections; i++) {
            int dx = directions[i][0]; //stores change in the x coordinates for the current piece.
            int dy = directions[i][1];//stores change in the y coordinates for the current piece.
            int newX = x;
            int newY = y;
            bool opponentPieceFound = false;  // Track if an opponent's piece is found

            // Check for possible captures
            while (true) {
                newX += dx;
                newY += dy;

                if (newX < 0 || newX >= BOARD_WIDTH / CELL_SIZE || newY < 0 || newY >= BOARD_HEIGHT / CELL_SIZE) {
                    break;  // Out of bounds
                }

                if (gameState.board[newY][newX].type == NONE) {
                    if (opponentPieceFound) {
                        // If an opponent's piece was found, this is a valid capture move
                        if (gameState.validMoveCount < MAX_VALID_MOVES) {
                            // King lands immediately after the opponent's piece (diagonal)
                            gameState.validMoves[gameState.validMoveCount].x = newX;
                            gameState.validMoves[gameState.validMoveCount].y = newY;
                            gameState.validMoveCount++;
                            captureAvailable = true;  // Indicate that captures are possible
                        }
                        break;  // Stop further movement after landing after capture
                    } else if (!isAfterCapture) {
                        // Allow regular moves only if not after a capture
                        if (gameState.validMoveCount < MAX_VALID_MOVES) {
                            gameState.validMoves[gameState.validMoveCount].x = newX;
                            gameState.validMoves[gameState.validMoveCount].y = newY;
                            gameState.validMoveCount++;
                        }
                    }
                } else {
                    // This marks that capturing may be possible in the future if there is an empty cell immediately after this opponent's piece.
                    if (gameState.board[newY][newX].player != piece.player) {
                        if (!opponentPieceFound) {
                            opponentPieceFound = true;  // Found an opponent piece
                        } else {
                            break;  // If we've already found one, can't capture beyond it
                        }
                    } else {
                        break;  // Blocked by own piece, stop further checks in this direction
                    }
                }
            }
        }

        // If it's after a capture, only allow moves that result in more captures
        if (isAfterCapture && !captureAvailable) {
            gameState.validMoveCount = 0;  // No valid moves if there are no captures
        }

    } else {
        // Regular pieces move forward only after a capture
        if (piece.player == PLAYER1) {
            // Player 1's pieces move down the board
            directions[0][0] = 1; directions[0][1] = 1; // Down-right
            directions[1][0] = -1; directions[1][1] = 1; // Down-left
        } else {
            // Player 2's pieces move up the board
            directions[0][0] = 1; directions[0][1] = -1; // Up-right
            directions[1][0] = -1; directions[1][1] = -1; // Up-left
        }
        int numDirections = 2;

        for (int i = 0; i < numDirections; i++) {
            int dx = directions[i][0];
            int dy = directions[i][1];
            int newX = x + dx;
            int newY = y + dy;

            if (newX >= 0 && newX < BOARD_WIDTH / CELL_SIZE && newY >= 0 && newY < BOARD_HEIGHT / CELL_SIZE) {
                if (gameState.board[newY][newX].type == NONE && !isAfterCapture) {
                    // Regular forward move (only if not after a capture)
                    if (gameState.validMoveCount < MAX_VALID_MOVES) {
                        gameState.validMoves[gameState.validMoveCount].x = newX;
                        gameState.validMoves[gameState.validMoveCount].y = newY;
                        gameState.validMoveCount++;
                    }
                }

                // Check for capturing move (can move only forward after capture)
                int captureX = x + 2 * dx;
                int captureY = y + 2 * dy;
                if (captureX >= 0 && captureX < BOARD_WIDTH / CELL_SIZE && 
                    captureY >= 0 && captureY < BOARD_HEIGHT / CELL_SIZE) {
                    if (gameState.board[captureY][captureX].type == NONE &&
                        gameState.board[newY][newX].type != NONE &&
                        gameState.board[newY][newX].player != piece.player) {
                        if (gameState.validMoveCount < MAX_VALID_MOVES) {
                            gameState.validMoves[gameState.validMoveCount].x = captureX;
                            gameState.validMoves[gameState.validMoveCount].y = captureY;
                            gameState.validMoveCount++;
                        }
                    }
                }
            }
        }
    }
}




void PromoteToKing(GameState &gameState, int x, int y) {
    if (gameState.currentPlayer == PLAYER1 && y == BOARD_HEIGHT / CELL_SIZE - 1) {
        gameState.board[y][x].isKing = true;
        gameState.board[y][x].type = KING;
    } else if (gameState.currentPlayer == PLAYER2 && y == 0) {
        gameState.board[y][x].isKing = true;
        gameState.board[y][x].type = KING;
    }
}




bool CheckGameOver(const GameState &gameState, int currentPlayer, int &winner) {
    // Variables to count the number of pieces for each player
    int player1Pieces = 0;
    int player2Pieces = 0;
    
    // Flags to check if each player has valid moves
    bool player1HasMoves = false;
    bool player2HasMoves = false;

    // Iterate through the board to count pieces and check for legal moves
    for (int y = 0; y < BOARD_HEIGHT / CELL_SIZE; y++) {
        for (int x = 0; x < BOARD_WIDTH / CELL_SIZE; x++) {
            // Check if there's a piece in the current cell
            if (gameState.board[y][x].type != NONE) {
                if (gameState.board[y][x].player == PLAYER1) {
                    player1Pieces++;
                    
                    // Check if Player 1 has any valid moves or captures
                    if (!player1HasMoves) {
                        // Check normal piece moves for Player 1
                        if ((y + 1 < BOARD_HEIGHT / CELL_SIZE && x + 1 < BOARD_WIDTH / CELL_SIZE && gameState.board[y + 1][x + 1].type == NONE) ||  // Move to bottom-right
                            (y + 1 < BOARD_HEIGHT / CELL_SIZE && x - 1 >= 0 && gameState.board[y + 1][x - 1].type == NONE)) {  // Move to bottom-left
                            player1HasMoves = true;
                        } 
                        
                        // Check normal piece captures for Player 1
                        if ((y + 2 < BOARD_HEIGHT / CELL_SIZE && x + 2 < BOARD_WIDTH / CELL_SIZE && gameState.board[y + 1][x + 1].player == PLAYER2 && gameState.board[y + 2][x + 2].type == NONE) ||  // Capture bottom-right
                            (y + 2 < BOARD_HEIGHT / CELL_SIZE && x - 2 >= 0 && gameState.board[y + 1][x - 1].player == PLAYER2 && gameState.board[y + 2][x - 2].type == NONE)) {  // Capture bottom-left
                            player1HasMoves = true;
                        }

                        // Check king moves and captures for Player 1
                        if (gameState.board[y][x].type == KING) {
                            if ((y + 1 < BOARD_HEIGHT / CELL_SIZE && x + 1 < BOARD_WIDTH / CELL_SIZE && gameState.board[y + 1][x + 1].type == NONE) ||  // Move to bottom-right
                                (y + 1 < BOARD_HEIGHT / CELL_SIZE && x - 1 >= 0 && gameState.board[y + 1][x - 1].type == NONE) ||  // Move to bottom-left
                                (y - 1 >= 0 && x + 1 < BOARD_WIDTH / CELL_SIZE && gameState.board[y - 1][x + 1].type == NONE) ||  // Move to top-right
                                (y - 1 >= 0 && x - 1 >= 0 && gameState.board[y - 1][x - 1].type == NONE)) {  // Move to top-left
                                player1HasMoves = true;
                            }
                            
                            // Check king captures for Player 1
                            if ((y + 2 < BOARD_HEIGHT / CELL_SIZE && x + 2 < BOARD_WIDTH / CELL_SIZE && gameState.board[y + 1][x + 1].player == PLAYER2 && gameState.board[y + 2][x + 2].type == NONE) ||  // Capture bottom-right
                                (y + 2 < BOARD_HEIGHT / CELL_SIZE && x - 2 >= 0 && gameState.board[y + 1][x - 1].player == PLAYER2 && gameState.board[y + 2][x - 2].type == NONE) ||  // Capture bottom-left
                                (y - 2 >= 0 && x + 2 < BOARD_WIDTH / CELL_SIZE && gameState.board[y - 1][x + 1].player == PLAYER2 && gameState.board[y - 2][x + 2].type == NONE) ||  // Capture top-right
                                (y - 2 >= 0 && x - 2 >= 0 && gameState.board[y - 1][x - 1].player == PLAYER2 && gameState.board[y - 2][x - 2].type == NONE)) {  // Capture top-left
                                player1HasMoves = true;
                            }
                        }
                    }
                } else if (gameState.board[y][x].player == PLAYER2) {
                    player2Pieces++;
                    
                    // Check if Player 2 has any valid moves or captures
                    if (!player2HasMoves) {
                        // Check normal piece moves for Player 2
                        if ((y - 1 >= 0 && x + 1 < BOARD_WIDTH / CELL_SIZE && gameState.board[y - 1][x + 1].type == NONE) ||  // Move to top-right
                            (y - 1 >= 0 && x - 1 >= 0 && gameState.board[y - 1][x - 1].type == NONE)) {  // Move to top-left
                            player2HasMoves = true;
                        }
                        
                        // Check normal piece captures for Player 2
                        if ((y - 2 >= 0 && x + 2 < BOARD_WIDTH / CELL_SIZE && gameState.board[y - 1][x + 1].player == PLAYER1 && gameState.board[y - 2][x + 2].type == NONE) ||  // Capture top-right
                            (y - 2 >= 0 && x - 2 >= 0 && gameState.board[y - 1][x - 1].player == PLAYER1 && gameState.board[y - 2][x - 2].type == NONE)) {  // Capture top-left
                            player2HasMoves = true;
                        }

                        // Check king moves and captures for Player 2
                        if (gameState.board[y][x].type == KING) {
                            if ((y + 1 < BOARD_HEIGHT / CELL_SIZE && x + 1 < BOARD_WIDTH / CELL_SIZE && gameState.board[y + 1][x + 1].type == NONE) ||  // Move to bottom-right
                                (y + 1 < BOARD_HEIGHT / CELL_SIZE && x - 1 >= 0 && gameState.board[y + 1][x - 1].type == NONE) ||  // Move to bottom-left
                                (y - 1 >= 0 && x + 1 < BOARD_WIDTH / CELL_SIZE && gameState.board[y - 1][x + 1].type == NONE) ||  // Move to top-right
                                (y - 1 >= 0 && x - 1 >= 0 && gameState.board[y - 1][x - 1].type == NONE)) {  // Move to top-left
                                player2HasMoves = true;
                            }
                            
                            // Check king captures for Player 2
                            if ((y + 2 < BOARD_HEIGHT / CELL_SIZE && x + 2 < BOARD_WIDTH / CELL_SIZE && gameState.board[y + 1][x + 1].player == PLAYER1 && gameState.board[y + 2][x + 2].type == NONE) ||  // Capture bottom-right
                                (y + 2 < BOARD_HEIGHT / CELL_SIZE && x - 2 >= 0 && gameState.board[y + 1][x - 1].player == PLAYER1 && gameState.board[y + 2][x - 2].type == NONE) ||  // Capture bottom-left
                                (y - 2 >= 0 && x + 2 < BOARD_WIDTH / CELL_SIZE && gameState.board[y - 1][x + 1].player == PLAYER1 && gameState.board[y - 2][x + 2].type == NONE) ||  // Capture top-right
                                (y - 2 >= 0 && x - 2 >= 0 && gameState.board[y - 1][x - 1].player == PLAYER1 && gameState.board[y - 2][x - 2].type == NONE)) {  // Capture top-left
                                player2HasMoves = true;
                            }
                        }
                    }
                }
            }
        }
    }

    // Check if any player has no pieces left
    if (player1Pieces == 0) {
        winner = PLAYER2;  // Player 2 wins
        return true;
    } else if (player2Pieces == 0) {
        winner = PLAYER1;  // Player 1 wins
        return true;
    }

    // Check if any player has no legal moves left
    if (currentPlayer == PLAYER1 && !player1HasMoves) {
        winner = PLAYER2;  // Player 2 wins due to no legal moves for Player 1
        return true;
    } else if (currentPlayer == PLAYER2 && !player2HasMoves) {
        winner = PLAYER1;  // Player 1 wins due to no legal moves for Player 2
        return true;
    }

    return false;  // No winner yet
}





void SaveGame(const GameState &gameState, const string &filename) {
    ofstream outFile(filename, ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(&gameState), sizeof(GameState));
        outFile.close();
        cout << "Game saved to " << filename << "!\n";
    } else {
        cout << "Error: Could not open " << filename << " for saving.\n";
    }
}

void LoadGame(GameState &gameState, const string &filename) {
    if (FileExists(filename.c_str())) {
        ifstream inFile(filename, ios::binary);
        if (inFile.is_open()) {
            inFile.read(reinterpret_cast<char*>(&gameState), sizeof(GameState));
            inFile.close();
            cout << "Game loaded from " << filename << "!\n";
        } else {
            cout << "Error: Could not open " << filename << " for loading.\n";
        }
    } else {
        cout << "Error: No saved game found with the filename: " << filename << "\n";
    }
}


