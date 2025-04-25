#include<iostream>
#include<ostream>
#include<string>
#include <fstream>
using namespace std;
const int SIZE = 15;
const char PLAYER1_SYMBOL = 'X';
const char PLAYER2_SYMBOL = 'O';
void saveResult(char board[SIZE][SIZE], const string& result){
    ofstream fout("result.txt");
    if (!fout.is_open()) {
        cout << "Error: Could not open result.txt for writing.\n";
        return;
    }
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col)
            fout << board[row][col] << ' ';
        fout << '\n';
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
    if (comma == string::npos){
        cout <<"Input is invalid" << endl;
        cout << "Please re-input your coordinate: "; 
        return false;
    } 

    try {
        x = stoi(input.substr(0, comma));
        y = stoi(input.substr(comma + 1));
    } catch (...) {
        return false;
    }

    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        cout <<"Input is invalid. Input must be in the format x,y (e.g., 1,2)" << endl;
        cout << "Please re-input your coordinate: "; 
        return false;
    }
    if (board[x][y] != ' ') {
        cout << "Cell already taken. \n";
        cout << "Please re-input your coordinate: ";
        return false;
    }    
    return true;
}
void printBoard(char board[SIZE][SIZE], int x, int y) {
    // In chỉ số cột
    cout << "  ";
    for (int col = 0; col < SIZE; col++) {
        if(col == 0) cout << "    " << col;
        else if (0 < col && col <= 10) cout << "     " << col ;
        else  cout << "    " <<col;
      
    }
    cout << endl;

    // In dòng trên cùng
    cout << "   +";
    for (int i = 0; i < SIZE - 1; i++) {
        cout << "-----+";
    }
    cout << "-----+" << endl;

    // In thân bàn cờ
    for (int row = 0; row < SIZE; row++) {
        // In chỉ số hàng
        if (row < 10) cout << " " << row << " ";
        else cout << row << " ";

        // In nội dung hàng
        for (int col = 0; col < SIZE; col++) {
            if(row == x && col == y){
                cout << "| [" << board[row][col] <<"] ";
            } else cout << "|  " << board[row][col] << "  ";
            
        }
        cout << "|" << endl;

        // In dòng phân cách
        cout << "   +";
        for (int i = 0; i < SIZE - 1; i++) {
            cout << "-----+";
        }
        cout << "-----+" << endl;
    }
}
void Gomoku(int currentPlayer, char board[SIZE][SIZE], bool finished){
    if(finished) return;
    cout << "Player " << currentPlayer << ", please input your coordinates: " ;
    string input; 
    getline(cin >> ws, input);
    int x, y;
    while(!isValidInput(input, x, y, board)){
        getline(cin >> ws, input);
    }

    board[x][y] = currentPlayer == 1 ? PLAYER1_SYMBOL : PLAYER2_SYMBOL;    
    if (checkWin(board, x, y, board[x][y])) {
        printBoard(board, x, y);
        string result = "Player " + to_string(currentPlayer) + " wins";
        cout << result << "!\n";
        saveResult(board, result);
        finished  = true;
    }else if (isFull(board)) {
        printBoard(board,x ,y);
        string result = "Tie";
        cout << result << "!\n";
        saveResult(board, result);
        finished  = true;
    }
    currentPlayer = 3 - currentPlayer;
    Gomoku(currentPlayer, board, finished);
}
bool playAgain( char board[SIZE][SIZE]){
    cout << "Do you want to play again? (y/n): ";
        char playAgain;
        cin >> playAgain;
        if (playAgain != 'y' && playAgain != 'Y') return true;
        // Reset board and game state
        else{
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                board[i][j] = ' ';
         }
         return false;
}
int main() {
    cout << "Convention:" <<endl;
    cout << "  - Use X for black stones " <<endl;
    cout << "  - Use O for white stones " << endl;
    cout << "The Player 1 with X" << endl;
    cout << "The Player 2 with O" << endl;
    char board[SIZE][SIZE];
    // Khởi tạo tất cả các ô là dấu cách (chưa có quân)
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';
     cout << "Plese choose the Player first: " ;
     int currentPlayer ;
     cin >> currentPlayer ;
    bool finished = false;
    printBoard(board, -1, -1);
    //start
    while(!finished){
        cout << "Player " << currentPlayer << ", please input coordinates (x,y): ";
     string input; 
    getline(cin >> ws, input);
    int x, y;
    while(!isValidInput(input, x, y, board)){
        getline(cin >> ws, input);
    }

    board[x][y] = currentPlayer == 1 ? 'X' : 'O';
    printBoard(board, x, y);
    if (checkWin(board, x, y, board[x][y])) {
        printBoard(board, x, y);
        string result = "Player " + to_string(currentPlayer) + " wins";
        cout << result << "!\n";
        saveResult(board, result);
        finished = playAgain(board);
    }
    if (isFull(board)) {
        printBoard(board,x ,y);
        string result = "Tie";
        cout << result << "!\n";
        saveResult(board, result);
        finished = playAgain(board);
    }
    currentPlayer = 3 - currentPlayer;
    }

    
    return 0;
}
