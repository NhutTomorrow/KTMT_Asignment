#include <iostream>
#include <ostream>
#include <string>
#include <fstream>
#include <chrono>
using namespace std;

const int SIZE = 15;
const char PLAYER1_SYMBOL = 'X';
const char PLAYER2_SYMBOL = 'O';
const double TIME_LIMIT = 30.0; // 2 minutes in seconds

struct PlayerTime {
    double timeLeft1 = TIME_LIMIT;
    double timeLeft2 = TIME_LIMIT;
    chrono::steady_clock::time_point startTime;
};

void saveResult(char board[SIZE][SIZE], const string& result) {
    ofstream fout("result.txt");
    if (!fout.is_open()) {
        cout << "Error: Could not open result.txt for writing.\n";
        return;
    }
    fout << "  ";
    for (int col = 0; col < SIZE; col++) {
        if (col == 0) fout << "    " << col;
        else if (0 < col && col <= 10) fout << "     " << col;
        else fout << "    " << col;
    }
    fout << endl;

    fout << "   +";
    for (int i = 0; i < SIZE - 1; i++) {
        fout << "-----+";
    }
    fout << "-----+" << endl;

    for (int row = 0; row < SIZE; row++) {
        if (row < 10) fout << " " << row << " ";
        else fout << row << " ";

        for (int col = 0; col < SIZE; col++) {
               fout << "|  " << board[row][col] << "  ";
        }
        fout << "|" << endl;

        fout << "   +";
        for (int i = 0; i < SIZE - 1; i++) {
            fout << "-----+";
        }
        fout << "-----+" << endl;
    }
    fout << result << endl;
    fout.close();
}

bool isFull(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == ' ') return false;
    return true;
}

bool checkWin(char board[SIZE][SIZE], int x, int y, char player) {
    int count = 1;
    // Check horizontal
    for (int i = 1; i <= 4 && y - i >= 0; ++i)
        if (board[x][y - i] == player) count++; else break;
    for (int i = 1; i <= 4 && y + i < SIZE; ++i)
        if (board[x][y + i] == player) count++; else break;
    if (count >= 5) return true;

    // Check vertical
    count = 1;
    for (int i = 1; i <= 4 && x - i >= 0; ++i)
        if (board[x - i][y] == player) count++; else break;
        for (int i = 1; i <= 4 && x + i < SIZE; ++i)
            if (board[x + i][y] == player) count++; else break;
    if (count >= 5) return true;

    // Check main diagonal (\)
    count = 1;
    for (int i = 1; i <= 4 && x - i >= 0 && y - i >= 0; ++i)
        if (board[x - i][y - i] == player) count++; else break;
    for (int i = 1; i <= 4 && x + i < SIZE && y + i < SIZE; ++i)
        if (board[x + i][y + i] == player) count++; else break;
    if (count >= 5) return true;

    // Check anti-diagonal (/)
    count = 1;
    for (int i = 1; i <= 4 && x - i >= 0 && y + i < SIZE; ++i)
        if (board[x - i][y + i] == player) count++; else break;
    for (int i = 1; i <= 4 && x + i < SIZE && y - i >= 0; ++i)
        if (board[x + i][y - i] == player) count++; else break;
    if (count >= 5) return true;


    return false;
}

bool isValidInput(string input, int& x, int& y, char board[SIZE][SIZE]) {
    size_t comma = input.find(',');
    if (comma == string::npos) {
        cout << "Input is invalid. Input must be in the format x,y (e.g., 1,2)\n";
        cout << "Please re-input your coordinate: ";
        return false;
    }

    try {
        x = stoi(input.substr(0, comma));
        y = stoi(input.substr(comma + 1));
    } catch (...) {
        cout << "Input is invalid. Coordinates must be numbers.\n";
        cout << "Please re-input your coordinate: ";
        return false;
    }

    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        cout << "Input is invalid. Coordinates must be between 0 and " << SIZE - 1 << ".\n";
        cout << "Please re-input your coordinate: ";
        return false;
    }
    if (board[x][y] != ' ') {
        cout << "Cell already taken.\n";
        cout << "Please re-input your coordinate: ";
        return false;
    }
    return true;
}

void printBoard(char board[SIZE][SIZE], int x, int y) {
    cout << "  ";
    for (int col = 0; col < SIZE; col++) {
        if (col == 0) cout << "    " << col;
        else if (0 < col && col <= 10) cout << "     " << col;
        else cout << "    " << col;
    }
    cout << endl;

    cout << "   +";
    for (int i = 0; i < SIZE - 1; i++) {
        cout << "-----+";
    }
    cout << "-----+" << endl;

    for (int row = 0; row < SIZE; row++) {
        if (row < 10) cout << " " << row << " ";
        else cout << row << " ";

        for (int col = 0; col < SIZE; col++) {
            if (row == x && col == y) {
                cout << "| [" << board[row][col] << "] ";
            } else {
                cout << "|  " << board[row][col] << "  ";
            }
        }
        cout << "|" << endl;

        cout << "   +";
        for (int i = 0; i < SIZE - 1; i++) {
            cout << "-----+";
        }
        cout << "-----+" << endl;
    }
}

bool playAgain(char board[SIZE][SIZE], PlayerTime& timers) {
    cout << "Do you want to play again? (y/n): ";
    char playAgain;
    cin >> playAgain;
    if (playAgain != 'y' && playAgain != 'Y') return true;

    // Reset board
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';

    // Reset timers
    timers.timeLeft1 = TIME_LIMIT;
    timers.timeLeft2 = TIME_LIMIT;

    return false;
}

void updateTimer(PlayerTime& timers, int currentPlayer) {
    auto endTime = chrono::steady_clock::now();
    double elapsed = chrono::duration<double>(endTime - timers.startTime).count();
    if (currentPlayer == 1) {
        timers.timeLeft1 -= elapsed;
    } else {
        timers.timeLeft2 -= elapsed;
    }
}

bool checkTimeOut(PlayerTime& timers, int currentPlayer, char board[SIZE][SIZE]) {
    if (currentPlayer == 1 && timers.timeLeft1 <= 0 && timers.timeLeft2 > 0) {
        printBoard(board, -1, -1);
        string result = "Player 2 wins due to Player 1 timeout";
        cout << result << "!\n";
        saveResult(board, result);
        return true;
    } else if (currentPlayer == 2 && timers.timeLeft2 <= 0 && timers.timeLeft1 > 0) {
        printBoard(board, -1, -1);
        string result = "Player 1 wins due to Player 2 timeout";
        cout << result << "!\n";
        saveResult(board, result);
        return true;
    }
    return false;
}

int main() {
    cout << "Convention:" << endl;
    cout << "  - Use X for black stones" << endl;
    cout << "  - Use O for white stones" << endl;
    cout << "  - Each player has " << TIME_LIMIT << " seconds total time" << endl;
    cout << "  - Player who runs out of time first loses" << endl;
    cout << "The Player 1 with X" << endl;
    cout << "The Player 2 with O" << endl;

    char board[SIZE][SIZE];
    PlayerTime timers;

    // Initialize board
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';

    cout << "Please choose the first Player (1 or 2): ";
    int currentPlayer;
    cin >> currentPlayer;
    if (currentPlayer != 1 && currentPlayer != 2) {
        cout << "Invalid player. Defaulting to Player 1.\n";
        currentPlayer = 1;
    }

    bool finished = false;
    printBoard(board, -1, -1);

    while (!finished) {
        // Display remaining time
        cout << "Player 1 time left: " << fixed << timers.timeLeft1 << " seconds\n";
        cout << "Player 2 time left: " << timers.timeLeft2 << " seconds\n";
       
        // Check for timeout before move
       
        cout << "Player " << currentPlayer << ", please input coordinates (x,y): ";
        timers.startTime = chrono::steady_clock::now();
        string input;
        getline(cin >> ws, input);
        int x, y;
        while (!isValidInput(input, x, y, board)) {
            getline(cin >> ws, input);
        }
        
        // Update time after move
        updateTimer(timers, currentPlayer);
        if (checkTimeOut(timers, currentPlayer, board)) {
            finished = playAgain(board, timers);
            if (!finished) {
                cout << "Please choose the first Player (1 or 2): ";
                cin >> currentPlayer;
                if (currentPlayer != 1 && currentPlayer != 2) currentPlayer = 1;
                printBoard(board, -1, -1);
            }
            continue;
        }

        board[x][y] = currentPlayer == 1 ? PLAYER1_SYMBOL : PLAYER2_SYMBOL;
        printBoard(board, x, y);

        if (checkWin(board, x, y, board[x][y])) {
            string result = "Player " + to_string(currentPlayer) + " wins";
            cout << result << "!\n";
            saveResult(board, result);
            finished = playAgain(board, timers);
            if (!finished) {
                cout << "Please choose the first Player (1 or 2): ";
                cin >> currentPlayer;
                if (currentPlayer != 1 && currentPlayer != 2) currentPlayer = 1;
                printBoard(board, -1, -1);
            }
        } else if (isFull(board)) {
            string result = "Tie";
            cout << result << "!\n";
            saveResult(board, result);
            finished = playAgain(board, timers);
            if (!finished) {
                cout << "Please choose the first Player (1 or 2): ";
                cin >> currentPlayer;
                if (currentPlayer != 1 && currentPlayer != 2) currentPlayer = 1;
                printBoard(board, -1, -1);
            }
        } else {
            currentPlayer = 3 - currentPlayer;
        }
    }

    return 0;
}

