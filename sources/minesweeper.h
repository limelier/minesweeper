#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

using namespace std;

int adjacent_tile_offsets[8][2] = {
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, -1},
    {0, 1},
    {1, -1},
    {1, 0},
    {1, 1}
};

char char_coords[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

struct MinesweeperBoard {
    char** data;
    bool** revealed;
    bool** flagged;
    int height;
    int width;
    int mines;
    int tiles_remaining;
    bool is_game_over = 0;
    bool is_game_won = 0;
    int loss_mine_x;
    int loss_mine_y;
};

int ConvertFromChr(char coord_chr) {
    if (coord_chr > '0' && coord_chr <= '9') {
        return coord_chr - '0';
    }
    else if (coord_chr >= 'a' && coord_chr <= 'z')
        return coord_chr - 'a' + 10;
    else if (coord_chr >= 'A' && coord_chr <= 'Z')
        return coord_chr - 'A' + 10;
    else
        return 0;
}

void InvalidInput() {
    system("cls");
    cout << "Invalid input! Retrying..." << endl;
    Sleep(2000);
}

void InputValues(MinesweeperBoard &board) {
    int difficulty = 0;
    do {
        cout << "Please select your difficulty by typing in its number:" << endl << endl;
        cout << "1. Easy (9x9 field, 10 mines)" << endl;
        cout << "2. Medium (16x16 board, 40 mines)" << endl;
        cout << "3. Hard (16x30 board, 99 mines)" << endl;
        cout << "4. Custom..." << endl << endl;
        cin >> difficulty;

        switch (difficulty) {
            case 1: {
                board.width = 9;
                board.height = 9;
                board.mines = 10;
                break;
            }
            case 2: {
                board.width = 16;
                board.height = 16;
                board.mines = 40;
                break;
            }
            case 3: {
                board.width = 30;
                board.height = 16;
                board.mines = 99;
                break;
            }
            case 4: {
                bool ok = 1;
                do {
                    system("cls");
                    cout << "This screen will allow you to select a custom difficulty." << endl << endl;
                    cout << "Board width (between 1 and 35) - ";
                    cin >> board.width;
                    cout << endl << "Board height (between 1 and 19) - ";
                    cin >> board.height;
                    cout << endl << "Number of mines (between 1 and the total number of available squares) - ";
                    cin >> board.mines;
                    if (board.width < 1 || board.height < 1 || board.mines < 1 ||
                        board.width > 35 || board.height > 19 || board.mines > board.width * board.height) {
                        InvalidInput();
                        ok = 0;
                    }
                } while (ok == 0);
                break;
            }
            default: {
                InvalidInput();

            }
        }
        system("cls");
    } while (difficulty < 1 || difficulty > 4);
}

void InitArrays(MinesweeperBoard &board) {
    // create arrays
    board.data = new char*[board.height+2];
    board.revealed = new bool*[board.height+2];
    board.flagged = new bool*[board.height+2];
    for (int row = 0; row <= board.height+1; row++) {
        board.data[row] = new char[board.width+2];
        board.revealed[row] = new bool[board.width+2];
        board.flagged[row] = new bool[board.width+2];
    }

    // init arrays
    for (int coord_x = 0; coord_x <= board.height + 1; coord_x++)
        for (int coord_y = 0; coord_y <= board.width + 1; coord_y++) {
            board.data[coord_x][coord_y] = '.';
            board.revealed[coord_x][coord_y] = 0;
            board.flagged[coord_x][coord_y] = 0;
        }

    // surround the data
    for (int coord_x = 0; coord_x <= board.height + 1; coord_x++) {
        board.data[coord_x][0] = '#';
        board.data[coord_x][board.width+1] = '#';
    }

    for (int coord_y = 1; coord_y <= board.width; coord_y++) {
        board.data[0][coord_y] = '#';
        board.data[board.height+1][0] = '#';
    }
}

void PlaceMines(MinesweeperBoard &board) {
    for (int mine_num = 1; mine_num <= board.mines; mine_num++) {
        int coord_x, coord_y;

        do {
            coord_x = rand() % board.height + 1;
            coord_y = rand() % board.width + 1;
        } while (board.data[coord_x][coord_y] == '*');

        board.data[coord_x][coord_y] = '*';

        for (int i = 0; i < 8; i++) {
            int final_x = coord_x + adjacent_tile_offsets[i][0];
            int final_y = coord_y + adjacent_tile_offsets[i][1];
            if (board.data[final_x][final_y] == '.')
                board.data[final_x][final_y] = '1';
            else if (board.data[final_x][final_y] != '*')
                board.data[final_x][final_y]++;
        }
    }
}

void PrintBoardHorizontalCoordinates(MinesweeperBoard board, HANDLE console) {
    SetConsoleTextAttribute(console, 120);
    cout << ' ';
    for (int coord_y = 1; coord_y <= board.width; coord_y++) {
        cout << ' ' << char_coords[coord_y];
    }
    cout << ' ' << endl;
    SetConsoleTextAttribute(console, 15);
}

void PrintBoardVerticalCoordinates(MinesweeperBoard board, HANDLE console, int coord_x) {
    SetConsoleTextAttribute(console, 120);
    cout << char_coords[coord_x];
    SetConsoleTextAttribute(console, 15);
}

void PrintTile(MinesweeperBoard board, HANDLE console, int coord_x, int coord_y) {
    if (board.revealed[coord_x][coord_y] == 1) {
        if (board.data[coord_x][coord_y] != '*') {
            SetConsoleTextAttribute(console, 135); // 135 is light grey on dark grey
            cout << ' ' << board.data[coord_x][coord_y];
        }
        else {
            SetConsoleTextAttribute(console, 192); // 192 is black on red
            cout << " *";
        }
    }
    else {
        if (board.data[coord_x][coord_y] == '*') {
            if (board.flagged[coord_x][coord_y] == 0) {
                if (board.is_game_over == 0) {
                    SetConsoleTextAttribute(console, 127); // 127 is white on light grey
                    cout << "[]";
                }
                else if (board.is_game_won == 0) {
                    SetConsoleTextAttribute(console, 128); // 112 is black on dark
                    cout << " *";
                }
                else {
                    SetConsoleTextAttribute(console, 124); // 124 is red on light grey
                    cout << '<';
                    SetConsoleTextAttribute(console, 112); // 112 is black on light grey
                    cout << '|';
                }
            }
            else {
                SetConsoleTextAttribute(console, 124); // 124 is red on light grey
                cout << '<';
                SetConsoleTextAttribute(console, 112); // 112 is black on light grey
                cout << '|';
            }
        }
        else {
            if (board.flagged[coord_x][coord_y] == 1) {
                if (board.is_game_over == 0 || board.is_game_won == 1) {
                    SetConsoleTextAttribute(console, 124); // 124 is red on light grey
                    cout << '<';
                    SetConsoleTextAttribute(console, 112); // 112 is black on light grey
                    cout << '|';
                }
                else {
                    SetConsoleTextAttribute(console, 124); // 124 is red on light grey
                    cout << "><";
                }
            }
            else {
                SetConsoleTextAttribute(console, 127); // 127 is white on light grey
                cout << "[]";
            }
        }
    }
}

void PrintBoard(MinesweeperBoard board, HANDLE console) {
    PrintBoardHorizontalCoordinates(board, console);

    for (int coord_x = 1; coord_x <= board.height; coord_x++) {
        PrintBoardVerticalCoordinates(board, console, coord_x);
        for (int coord_y = 1; coord_y <= board.width; coord_y++) {
            PrintTile(board, console, coord_x, coord_y);
        }
        SetConsoleTextAttribute(console, 15);
        PrintBoardVerticalCoordinates(board, console, coord_x);
        cout << endl;
    }

    PrintBoardHorizontalCoordinates(board, console);
}


void WinGame(MinesweeperBoard &board) {
    board.is_game_over = 1;
    board.is_game_won = 1;
}

void RevealTile(MinesweeperBoard &board, int coord_x, int coord_y) {
    if (board.flagged[coord_x][coord_y] == 1)
        return;
    if (coord_x < 1 || coord_x > board.height || coord_y < 1 || coord_y > board.width)
        return;
    board.revealed[coord_x][coord_y] = 1;
    board.tiles_remaining--;
    if (board.data[coord_x][coord_y] == '.')
        for (int i = 0; i < 8; i++) {
            int final_x = coord_x + adjacent_tile_offsets[i][0];
            int final_y = coord_y + adjacent_tile_offsets[i][1];
            if (board.data[final_x][final_y] != '*' && board.revealed[final_x][final_y] == 0)
                RevealTile(board, final_x, final_y);
        }
    if (board.tiles_remaining == 0)
        WinGame(board);
}

void ClickTile(MinesweeperBoard &board, int coord_x, int coord_y) {
    if (coord_x < 1 || coord_x > board.height || coord_y < 1 || coord_y > board.width)
        return;

    if (board.flagged[coord_x][coord_y] == 0 && board.revealed[coord_x][coord_y] == 0) {
        if (board.data[coord_x][coord_y] == '*') {
            board.revealed[coord_x][coord_y] = 1;
            board.is_game_over = 1;
        }
        else
            RevealTile(board, coord_x, coord_y);
    }
}

void FlagTile(MinesweeperBoard &board, int coord_x, int coord_y) {
    if (coord_x < 1 || coord_x > board.height || coord_y < 1 || coord_y > board.width)
        return;
    if (board.revealed[coord_x][coord_y] == 0) {
        if (board.flagged[coord_x][coord_y] == 1)
            board.flagged[coord_x][coord_y] = 0;
        else
            board.flagged[coord_x][coord_y] = 1;
    }
}

void EndScreen(MinesweeperBoard board, HANDLE console) {
    PrintBoard(board, console);
    cout << endl;
    if (board.is_game_won == 1) {
        SetConsoleTextAttribute(console, 47);
        cout << "You win!" << endl;
        SetConsoleTextAttribute(console, 15);
    }
    else {
        SetConsoleTextAttribute(console, 79);
        cout << "You lose!" << endl;
        SetConsoleTextAttribute(console, 15);
    }
}
