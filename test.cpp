#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 15
#define PLAYER1_SYMBOL 'X'
#define PLAYER2_SYMBOL 'O'
#define TIME_LIMIT 30.0 // 30 seconds

typedef struct {
    double timeLeft1;
    double timeLeft2;
    clock_t startTime;
} PlayerTime;

void saveResult(char board[SIZE][SIZE], const char* result) {
    FILE* fout = fopen("result.txt", "w");
    if (!fout) {
        printf("Error: Could not open result.txt for writing.\n");
        return;
    }
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col)
            fprintf(fout, "%c ", board[row][col]);
        fprintf(fout, "\n");
    }
    fprintf(fout, "%s\n", result);
    fclose(fout);
}

int isFull(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == ' ') return 0;
    return 1;
}

int checkWin(char board[SIZE][SIZE], int x, int y, char player) {
    int count = 1;
    // Check horizontal
    for (int i = 1; i <= 4 && y - i >= 0; ++i)
        if (board[x][y - i] == player) count++; else break;
    for (int i = 1; i <= 4 && y + i < SIZE; ++i)
        if (board[x][y + i] == player) count++; else break;
    if (count >= 5) return 1;

    // Check vertical
    count = 1;
    for (int i = 1; i <= 4 && x - i >= 0; ++i)
        if (board[x - i][y] == player) count++; else break;
    for (int i = 1; i <= 4 && x + i < SIZE; ++i)
        if (board[x + i][y] == player) count++; else break;
    if (count >= 5) return 1;

    // Check main diagonal (\)
    count = 1;
    for (int i = 1; i <= 4 && x - i >= 0 && y - i >= 0; ++i)
        if (board[x - i][y - i] == player) count++; else break;
    for (int i = 1; i <= 4 && x + i < SIZE && y + i < SIZE; ++i)
        if (board[x + i][y + i] == player) count++; else break;
    if (count >= 5) return 1;

    // Check anti-diagonal (/)
    count = 1;
    for (int i = 1; i <= 4 && x - i >= 0 && y + i < SIZE; ++i)
        if (board[x - i][y + i] == player) count++; else break;
    for (int i = 1; i <= 4 && x + i < SIZE && y - i >= 0; ++i)
        if (board[x + i][y - i] == player) count++; else break;
    if (count >= 5) return 1;

    return 0;
}

int isValidInput(char* input, int* x, int* y, char board[SIZE][SIZE]) {
    char* comma = strchr(input, ',');
    if (!comma) {
        printf("Input is invalid. Input must be in the format x,y (e.g., 1,2)\n");
        printf("Please re-input your coordinate: ");
        return 0;
    }

    *comma = '\0';
    char* endptr;
    *x = strtol(input, &endptr, 10);
    if (*endptr != '\0') {
        printf("Input is invalid. Coordinates must be numbers.\n");
        printf("Please re-input your coordinate: ");
        return 0;
    }
    *y = strtol(comma + 1, &endptr, 10);
    if (*endptr != '\0') {
        printf("Input is invalid. Coordinates must be numbers.\n");
        printf("Please re-input your coordinate: ");
        return 0;
    }

    if (*x < 0 || *x >= SIZE || *y < 0 || *y >= SIZE) {
        printf("Input is invalid. Coordinates must be between 0 and %d.\n", SIZE - 1);
        printf("Please re-input your coordinate: ");
        return 0;
    }
    if (board[*x][*y] != ' ') {
        printf("Cell already taken.\n");
        printf("Please re-input your coordinate: ");
        return 0;
    }
    return 1;
}

void printBoard(char board[SIZE][SIZE], int x, int y) {
    printf("  ");
    for (int col = 0; col < SIZE; col++) {
        if (col == 0) printf("    %d", col);
        else if (0 < col && col <= 10) printf("     %d", col);
        else printf("    %d", col);
    }
    printf("\n");

    printf("   +");
    for (int i = 0; i < SIZE - 1; i++) {
        printf("-----+");
    }
    printf("-----+\n");

    for (int row = 0; row < SIZE; row++) {
        if (row < 10) printf(" %d ", row);
        else printf("%d ", row);

        for (int col = 0; col < SIZE; col++) {
            if (row == x && col == y) {
                printf("| [%c] ", board[row][col]);
            } else {
                printf("|  %c  ", board[row][col]);
            }
        }
        printf("|\n");

        printf("   +");
        for (int i = 0; i < SIZE - 1; i++) {
            printf("-----+");
        }
        printf("-----+\n");
    }
}

int playAgain(char board[SIZE][SIZE], PlayerTime* timers) {
    printf("Do you want to play again? (y/n): ");
    char playAgain;
    scanf(" %c", &playAgain);
    if (playAgain != 'y' && playAgain != 'Y') return 1;

    // Reset board
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';

    // Reset timers
    timers->timeLeft1 = TIME_LIMIT;
    timers->timeLeft2 = TIME_LIMIT;

    return 0;
}

void updateTimer(PlayerTime* timers, int currentPlayer) {
    clock_t endTime = clock();
    double elapsed = (double)(endTime - timers->startTime) / CLOCKS_PER_SEC;
    if (currentPlayer == 1) {
        timers->timeLeft1 -= elapsed;
    } else {
        timers->timeLeft2 -= elapsed;
    }
}

int checkTimeOut(PlayerTime* timers, int currentPlayer, char board[SIZE][SIZE]) {
    if (currentPlayer == 1 && timers->timeLeft1 <= 0 && timers->timeLeft2 > 0) {
        printBoard(board, -1, -1);
        char result[] = "Player 2 wins due to Player 1 timeout";
        printf("%s!\n", result);
        saveResult(board, result);
        return 1;
    } else if (currentPlayer == 2 && timers->timeLeft2 <= 0 && timers->timeLeft1 > 0) {
        printBoard(board, -1, -1);
        char result[] = "Player 1 wins due to Player 2 timeout";
        printf("%s!\n", result);
        saveResult(board, result);
        return 1;
    }
    return 0;
}

int main() {
    printf("Convention:\n");
    printf("  - Use X for black stones\n");
    printf("  - Use O for white stones\n");
    printf("  - Each player has %.1f seconds total time\n", TIME_LIMIT);
    printf("  - Player who runs out of time first loses\n");
    printf("The Player 1 with X\n");
    printf("The Player 2 with O\n");

    char board[SIZE][SIZE];
    PlayerTime timers = {TIME_LIMIT, TIME_LIMIT, 0};

    // Initialize board
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';

    printf("Please choose the first Player (1 or 2): ");
    int currentPlayer;
    scanf("%d", &currentPlayer);
    if (currentPlayer != 1 && currentPlayer != 2) {
        printf("Invalid player. Defaulting to Player 1.\n");
        currentPlayer = 1;
    }

    int finished = 0;
    printBoard(board, -1, -1);

    while (!finished) {
        // Display remaining time
        printf("Player 1 time left: %.2f seconds\n", timers.timeLeft1);
        printf("Player 2 time left: %.2f seconds\n", timers.timeLeft2);

        printf("Player %d, please input coordinates (x,y): ", currentPlayer);
        timers.startTime = clock();
        char input[20];
        scanf("%s", input);
        int x, y;
        while (!isValidInput(input, &x, &y, board)) {
            scanf("%s", input);
        }

        // Update time after move
        updateTimer(&timers, currentPlayer);
        if (checkTimeOut(&timers, currentPlayer, board)) {
            finished = playAgain(board, &timers);
            if (!finished) {
                printf("Please choose the first Player (1 or 2): ");
                scanf("%d", &currentPlayer);
                if (currentPlayer != 1 && currentPlayer != 2) currentPlayer = 1;
                printBoard(board, -1, -1);
            }
            continue;
        }

        board[x][y] = currentPlayer == 1 ? PLAYER1_SYMBOL : PLAYER2_SYMBOL;
        printBoard(board, x, y);

        if (checkWin(board, x, y, board[x][y])) {
            char result[50];
            snprintf(result, sizeof(result), "Player %d wins", currentPlayer);
            printf("%s!\n", result);
            saveResult(board, result);
            finished = playAgain(board, &timers);
            if (!finished) {
                printf("Please choose the first Player (1 or 2): ");
                scanf("%d", &currentPlayer);
                if (currentPlayer != 1 && currentPlayer != 2) currentPlayer = 1;
                printBoard(board, -1, -1);
            }
        } else if (isFull(board)) {
            char result[] = "Tie";
            printf("%s!\n", result);
            saveResult(board, result);
            finished = playAgain(board, &timers);
            if (!finished) {
                printf("Please choose the first Player (1 or 2): ");
                scanf("%d", &currentPlayer);
                if (currentPlayer != 1 && currentPlayer != 2) currentPlayer = 1;
                printBoard(board, -1, -1);
            }
        } else {
            currentPlayer = 3 - currentPlayer;
        }
    }

    return 0;
}