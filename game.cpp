#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <vector>
#include <algorithm>

using namespace std;

// Game constants
const int BOARD_SIZE = 3;
const int WIN_CONDITION = 3;
const int DEFAULT_ROUNDS = 5;
const int DEFAULT_WINS_NEEDED = 3;

// Enum for player types
enum PlayerType { HUMAN, COMPUTER };

// Enum for difficulty levels
enum DifficultyLevel { EASY = 1, MEDIUM, HARD };

// Struct for game state
struct GameState {
    string player1Name, player2Name;
    int player1Score = 0, player2Score = 0, ties = 0;
    int totalRounds = DEFAULT_ROUNDS;
    int winsNeeded = DEFAULT_WINS_NEEDED;
    PlayerType player2Type = HUMAN;
    DifficultyLevel difficulty = EASY;
    char board[BOARD_SIZE][BOARD_SIZE];
};

// Function prototypes
void displayMenu();
void playGame(GameState& game);
void initializeGame(GameState& game);
void howToPlay();
void developers();
bool exitGame();
void displayBoard(const char board[BOARD_SIZE][BOARD_SIZE]);
bool makeMove(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, char symbol);
bool checkWin(const char board[BOARD_SIZE][BOARD_SIZE], char symbol);
bool isBoardFull(const char board[BOARD_SIZE][BOARD_SIZE]);
void resetBoard(char board[BOARD_SIZE][BOARD_SIZE]);
void computerMove(char board[BOARD_SIZE][BOARD_SIZE], DifficultyLevel difficulty);
int minimax(char board[BOARD_SIZE][BOARD_SIZE], int depth, bool isMaximizing, int alpha = -1000, int beta = 1000);
pair<int, int> getPlayerMove();
void clearScreen();
void waitForEnter();
inline bool isValidPosition(int row, int col);
void displayGameStats(const GameState& game);

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed for random number generation
    
    int choice;
    bool running = true;
    GameState gameState;
    
    while (running) {
        displayMenu();
        cout << "Enter your choice (1-4): ";
        
        // Input validation
        while (!(cin >> choice) || choice < 1 || choice > 4) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 4: ";
        }
        
        switch (choice) {
            case 1:
                playGame(gameState);
                break;
            case 2:
                howToPlay();
                break;
            case 3:
                developers();
                break;
            case 4:
                running = !exitGame();
                break;
        }
    }
    
    return 0;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void waitForEnter() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void displayMenu() {
    clearScreen();
    cout << "======================================\n";
    cout << "           TIC TAC TOE GAME          \n";
    cout << "======================================\n";
    cout << "1. Play Game\n";
    cout << "2. How to Play\n";
    cout << "3. Developers\n";
    cout << "4. Exit\n";
    cout << "======================================\n";
}

void initializeGame(GameState& game) {
    clearScreen();
    cout << "======================================\n";
    cout << "              PLAY GAME              \n";
    cout << "======================================\n";
    cout << "Choose game mode:\n";
    cout << "1. Player vs Player\n";
    cout << "2. Player vs Computer\n";
    cout << "Enter your choice (1-2): ";
    
    int modeChoice;
    while (!(cin >> modeChoice) || modeChoice < 1 || modeChoice > 2) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter 1 or 2: ";
    }
    
    game.player2Type = (modeChoice == 2) ? COMPUTER : HUMAN;
    
    // Get player names
    cin.ignore(); // Clear input buffer
    cout << "Enter Player 1 name: ";
    getline(cin, game.player1Name);
    
    if (game.player2Type == COMPUTER) {
        cout << "Choose difficulty level:\n";
        cout << "1. Easy\n";
        cout << "2. Medium\n";
        cout << "3. Hard\n";
        cout << "Enter your choice (1-3): ";
        
        int diffLevel;
        while (!(cin >> diffLevel) || diffLevel < 1 || diffLevel > 3) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 3: ";
        }
        
        game.difficulty = static_cast<DifficultyLevel>(diffLevel);
        game.player2Name = "Computer";
        cin.ignore(); // Clear input buffer
    } else {
        cout << "Enter Player 2 name: ";
        getline(cin, game.player2Name);
    }
    
    cout << "Enter number of rounds (default is " << DEFAULT_ROUNDS << "): ";
    string roundsInput;
    getline(cin, roundsInput);
    
    if (!roundsInput.empty()) {
        try {
            game.totalRounds = stoi(roundsInput);
            if (game.totalRounds < 1) game.totalRounds = DEFAULT_ROUNDS;
        } catch (...) {
            game.totalRounds = DEFAULT_ROUNDS;
        }
    }
    
    cout << "Enter number of wins needed to win the game (default is " << DEFAULT_WINS_NEEDED << "): ";
    string winsInput;
    getline(cin, winsInput);
    
    if (!winsInput.empty()) {
        try {
            game.winsNeeded = stoi(winsInput);
            if (game.winsNeeded < 1) game.winsNeeded = DEFAULT_WINS_NEEDED;
        } catch (...) {
            game.winsNeeded = DEFAULT_WINS_NEEDED;
        }
    }
    
    // Reset scores
    game.player1Score = 0;
    game.player2Score = 0;
    game.ties = 0;
}

void playGame(GameState& game) {
    initializeGame(game);
    
    // Start the game rounds
    int currentRound = 1;
    bool continueGame = true;
    
    while (continueGame && 
           (currentRound <= game.totalRounds) && 
           (game.player1Score < game.winsNeeded && game.player2Score < game.winsNeeded)) {
        
        resetBoard(game.board);
        bool roundOver = false;
        bool playerTurn = true; // true for player 1, false for player 2
        
        clearScreen();
        cout << "======================================\n";
        cout << "             ROUND " << currentRound << "/" << game.totalRounds << "             \n";
        cout << "======================================\n";
        displayGameStats(game);
        
        while (!roundOver) {
            displayBoard(game.board);
            
            if (playerTurn) {
                cout << game.player1Name << "'s turn (X).\n";
                pair<int, int> move = getPlayerMove();
                
                while (!makeMove(game.board, move.first, move.second, 'X')) {
                    cout << "Cell already occupied. Try again.\n";
                    move = getPlayerMove();
                }
                
                if (checkWin(game.board, 'X')) {
                    displayBoard(game.board);
                    cout << game.player1Name << " wins this round!\n";
                    game.player1Score++;
                    roundOver = true;
                }
            } else { // Player 2 or Computer's turn
                if (game.player2Type == COMPUTER) {
                    cout << "Computer's turn (O)...\n";
                    computerMove(game.board, game.difficulty);
                } else {
                    cout << game.player2Name << "'s turn (O).\n";
                    pair<int, int> move = getPlayerMove();
                    
                    while (!makeMove(game.board, move.first, move.second, 'O')) {
                        cout << "Cell already occupied. Try again.\n";
                        move = getPlayerMove();
                    }
                }
                
                if (checkWin(game.board, 'O')) {
                    displayBoard(game.board);
                    cout << game.player2Name << " wins this round!\n";
                    game.player2Score++;
                    roundOver = true;
                }
            }
            
            if (!roundOver && isBoardFull(game.board)) {
                displayBoard(game.board);
                cout << "This round is a tie!\n";
                game.ties++;
                roundOver = true;
            }
            
            playerTurn = !playerTurn; // Switch turns
        }
        
        // Check if game should continue to next round
        if (isBoardFull(game.board) && !checkWin(game.board, 'X') && !checkWin(game.board, 'O')) {
            cout << "Tied game will be reset without advancing round.\n";
        } else {
            currentRound++;
        }
        
        // Check if a player reached required wins
        if (game.player1Score >= game.winsNeeded) {
            cout << game.player1Name << " wins the game with " << game.winsNeeded << " victories!\n";
            continueGame = false;
        } else if (game.player2Score >= game.winsNeeded) {
            cout << game.player2Name << " wins the game with " << game.winsNeeded << " victories!\n";
            continueGame = false;
        }
        
        if (continueGame && currentRound <= game.totalRounds) {
            waitForEnter();
        }
    }
    
    // Game over
    cout << "======================================\n";
    cout << "           FINAL RESULTS             \n";
    cout << "======================================\n";
    displayGameStats(game);
    
    // Ask to play again or return to menu
    cout << "Do you want to play again? (y/n): ";
    char choice;
    cin >> choice;
    
    if (tolower(choice) == 'y') {
        playGame(game);
    }
}

void displayGameStats(const GameState& game) {
    cout << game.player1Name << " (X): " << game.player1Score << " | " 
         << game.player2Name << " (O): " << game.player2Score << " | Ties: " << game.ties << endl;
}

pair<int, int> getPlayerMove() {
    int row, col;
    cout << "Enter row (0-2) and column (0-2): ";
    
    while (true) {
        if (cin >> row >> col) {
            if (isValidPosition(row, col)) {
                return {row, col};
            } else {
                cout << "Invalid position. Enter row (0-2) and column (0-2): ";
            }
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter row (0-2) and column (0-2): ";
        }
    }
}

inline bool isValidPosition(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

void displayBoard(const char board[BOARD_SIZE][BOARD_SIZE]) {
    cout << "  0 1 2\n";
    for (int i = 0; i < BOARD_SIZE; i++) {
        cout << i << " ";
        for (int j = 0; j < BOARD_SIZE; j++) {
            cout << board[i][j];
            if (j < BOARD_SIZE - 1) cout << "|";
        }
        cout << endl;
        if (i < BOARD_SIZE - 1) cout << "  -+-+-\n";
    }
}

bool makeMove(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, char symbol) {
    if (board[row][col] == ' ') {
        board[row][col] = symbol;
        return true;
    }
    return false;
}

bool checkWin(const char board[BOARD_SIZE][BOARD_SIZE], char symbol) {
    // Check rows and columns
    for (int i = 0; i < BOARD_SIZE; i++) {
        // Check row
        if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) {
            return true;
        }
        // Check column
        if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) {
            return true;
        }
    }
    
    // Check diagonals
    if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
        return true;
    }
    if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) {
        return true;
    }
    
    return false;
}

bool isBoardFull(const char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

void resetBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

void computerMove(char board[BOARD_SIZE][BOARD_SIZE], DifficultyLevel difficulty) {
    // Easy: Random move
    if (difficulty == EASY) {
        vector<pair<int, int>> emptyCells;
        
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == ' ') {
                    emptyCells.push_back({i, j});
                }
            }
        }
        
        if (!emptyCells.empty()) {
            int index = rand() % emptyCells.size();
            board[emptyCells[index].first][emptyCells[index].second] = 'O';
        }
    }
    // Medium: Block opponent or random move
    else if (difficulty == MEDIUM) {
        // Try to win first
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'O';
                    if (checkWin(board, 'O')) {
                        return;
                    }
                    board[i][j] = ' '; // Undo move
                }
            }
        }
        
        // Try to block
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'X';
                    if (checkWin(board, 'X')) {
                        board[i][j] = 'O'; // Block player
                        return;
                    }
                    board[i][j] = ' '; // Undo move
                }
            }
        }
        
        // Try to take center
        if (board[1][1] == ' ') {
            board[1][1] = 'O';
            return;
        }
        
        // Take a corner if available
        const vector<pair<int, int>> corners = {{0,0}, {0,2}, {2,0}, {2,2}};
        vector<pair<int, int>> availableCorners;
        
        for (const auto& corner : corners) {
            if (board[corner.first][corner.second] == ' ') {
                availableCorners.push_back(corner);
            }
        }
        
        if (!availableCorners.empty()) {
            int index = rand() % availableCorners.size();
            board[availableCorners[index].first][availableCorners[index].second] = 'O';
            return;
        }
        
        // Otherwise, make a random move
        vector<pair<int, int>> emptyCells;
        
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == ' ') {
                    emptyCells.push_back({i, j});
                }
            }
        }
        
        if (!emptyCells.empty()) {
            int index = rand() % emptyCells.size();
            board[emptyCells[index].first][emptyCells[index].second] = 'O';
        }
    }
    // Hard: Minimax algorithm with alpha-beta pruning
    else {
        int bestScore = -1000;
        int bestRow = -1;
        int bestCol = -1;
        
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'O';
                    int score = minimax(board, 0, false);
                    board[i][j] = ' '; // Undo move
                    
                    if (score > bestScore) {
                        bestScore = score;
                        bestRow = i;
                        bestCol = j;
                    }
                }
            }
        }
        
        if (bestRow != -1 && bestCol != -1) {
            board[bestRow][bestCol] = 'O';
        }
    }
}

int minimax(char board[BOARD_SIZE][BOARD_SIZE], int depth, bool isMaximizing, int alpha, int beta) {
    // Terminal conditions
    if (checkWin(board, 'O')) return 10 - depth;
    if (checkWin(board, 'X')) return depth - 10;
    if (isBoardFull(board)) return 0;
    
    if (isMaximizing) {
        int bestScore = -1000;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'O';
                    int score = minimax(board, depth + 1, false, alpha, beta);
                    board[i][j] = ' '; // Undo move
                    bestScore = max(score, bestScore);
                    alpha = max(alpha, bestScore);
                    if (beta <= alpha)
                        break; // Alpha-beta pruning
                }
            }
        }
        return bestScore;
    } else {
        int bestScore = 1000;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'X';
                    int score = minimax(board, depth + 1, true, alpha, beta);
                    board[i][j] = ' '; // Undo move
                    bestScore = min(score, bestScore);
                    beta = min(beta, bestScore);
                    if (beta <= alpha)
                        break; // Alpha-beta pruning
                }
            }
        }
        return bestScore;
    }
}

void howToPlay() {
    clearScreen();
    cout << "======================================\n";
    cout << "            HOW TO PLAY              \n";
    cout << "======================================\n";
    cout << "Game Rules:\n";
    cout << "1. The game is played on a 3x3 grid.\n";
    cout << "2. Players take turns placing their symbol (X or O) in empty cells.\n";
    cout << "3. The first player to get 3 of their symbols in a row (horizontally,\n";
    cout << "   vertically, or diagonally) wins the round.\n";
    cout << "4. If all cells are filled and no player has won, the round is a tie.\n\n";
    
    cout << "Game Features:\n";
    cout << "- Two game modes: Player vs Player or Player vs Computer\n";
    cout << "- Three difficulty levels for computer opponent\n";
    cout << "- Customizable number of rounds\n";
    cout << "- Customizable win condition (how many rounds to win)\n";
    cout << "- Tied rounds do not count and will be replayed\n\n";
    
    cout << "How to Enter Moves:\n";
    cout << "- Enter the row number (0-2) followed by a space\n";
    cout << "- Then enter the column number (0-2)\n";
    cout << "- Example: '1 2' will place your symbol in the middle row, rightmost column\n\n";
    
    cout << "Computer Difficulty Levels:\n";
    cout << "- Easy: Makes random moves\n";
    cout << "- Medium: Can block your winning moves and try to win itself\n";
    cout << "- Hard: Uses an optimal strategy (minimax algorithm) - very difficult to beat!\n\n";
    
    waitForEnter();
}

void developers() {
    clearScreen();
    cout << "======================================\n";
    cout << "             DEVELOPERS              \n";
    cout << "======================================\n";
    cout << "Name: Allain\n";
    cout << "Motto: \"Balo ani bai\"\n";
    cout << "Status: It's complicated UwU\n\n";
    
    waitForEnter();
}

bool exitGame() {
    clearScreen();
    cout << "======================================\n";
    cout << "              GOODBYE!               \n";
    cout << "======================================\n";
    cout << "Thank you for playing Tic Tac Toe!\n";
    cout << "Do you want to play again? (y/n): ";
    
    char choice;
    cin >> choice;
    
    return tolower(choice) != 'y';
}

