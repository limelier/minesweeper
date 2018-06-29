#include "minesweeper.h"

int main() {

    MinesweeperBoard board;
    int input_x, input_y;
    char input_x_chr, input_y_chr;
    char input_action;

    HANDLE console;
    console = GetStdHandle(STD_OUTPUT_HANDLE);
    srand (time(NULL));

    InputValues(board);
    InitArrays(board);
    PlaceMines(board);
    board.tiles_remaining = board.width * board.height - board.mines;

    system("cls");

    // main game loop
    while (board.is_game_over == 0) {
        PrintBoard(board, console);

        cout << "Pick a tile (vertical and horizontal coordinates) and an action. Actions: F (flag), C (click)." << endl;
        cout << "For example: \"4 B F\" would flag tile (4, B)." << endl << endl;
        cout << "> ";
        cin >> input_x_chr >> input_y_chr >> input_action;
        input_x = ConvertFromChr(input_x_chr);
        input_y = ConvertFromChr(input_y_chr);
        if (input_action == 'C' || input_action == 'c')
            ClickTile(board, input_x, input_y);
        else if (input_action == 'F' || input_action == 'f')
            FlagTile(board, input_x, input_y);
        system("cls");
    }
    EndScreen(board, console);
    system("pause");

    return 0;
}
