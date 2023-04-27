#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define TRUE 1
#define FALSE 0
#define SHARED_MEM_SIZE 1024
#define SHARED_MEM_NAME "name"

typedef struct game_state { //plasza stan gry
    char board[3][3];
    int turn;
} game_state; //strukt

game_state* p_game_state; 

void showboard() {
    printf("  %c | %c | %c\n",
        p_game_state->board[0][0], p_game_state->board[0][1], p_game_state->board[0][2]);
    printf(" ---+---+---\n");
    printf("  %c | %c | %c\n",
        p_game_state->board[1][0], p_game_state->board[1][1], p_game_state->board[1][2]);
    printf(" ---+---+---\n");
    printf("  %c | %c | %c\n\n",
        p_game_state->board[2][0], p_game_state->board[2][1], p_game_state->board[2][2]);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Niepoprawna liczba argumentow!\n");
        return 1;
    }
                         //readwrite    handle  nametoopen
    if (!OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEM_NAME)) {
        HANDLE h_mem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
            0, SHARED_MEM_SIZE, SHARED_MEM_NAME);


        if (h_mem == NULL) {
            printf("Nie udalo sie utworzyc pamieci wspoldzielonej!\n");
            return 1;
        }
        p_game_state = (game_state*)MapViewOfFile(h_mem, FILE_MAP_ALL_ACCESS, 0,
            0, SHARED_MEM_SIZE);
        if (p_game_state == NULL) {
            printf("Nie udalo sie zmapowac pamieci wspoldzielonej!\n");
            return 1;
        }

        memset(p_game_state->board, ' ', 9);
        p_game_state->turn = 0;
        printf("Jesteœ krzyzykiem!\n");
    }

    else {
        HANDLE h_mem = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEM_NAME);
        if (h_mem == NULL) {
            printf("Nie udalo sie otworzyc pamieci wspoldzielonej!\n");
            return 1;
        }
        p_game_state = (game_state*)MapViewOfFile(h_mem, FILE_MAP_ALL_ACCESS, 0,
            0, SHARED_MEM_SIZE);
        if (p_game_state == NULL) {
            printf("Nie udalo sie zmapowac pamieci wspoldzielonej!\n");
            return 1;
        }
        p_game_state->turn = 1;
        printf("Jesteœ kolkiem!\n");
    }

    int turn = p_game_state->turn;
    int player = turn % 2; // 0 - krzy¿yk, 1 - kó³ko

    while (TRUE) {
        //kolej synchro
        if (player == 0) {
            while(p_game_state->turn!=0){}
        }
        if(player==1) {
            while (p_game_state->turn!=1) {}
        }
        showboard();

        int x, y;
        int wybor;
        printf("Podaj wspolrzedne ruchu (1-9): ");
        scanf_s("%d", &wybor);
        while (wybor>9||wybor<1) {
            printf("Niepoprawne wspolrzedne!\n");
            printf("Podaj wspolrzedne ruchu (1-9): ");
            scanf_s("%d", &wybor);
        }
        switch (wybor) {
        case 1:
            x = 1;
            y = 1;
            break;
        case 2:
            x = 2;
            y = 1;
            break;
        case 3:
            x = 3;
            y = 1;
            break;
        case 4:
            x = 1;
            y = 2;
            break;
        case 5:
            x = 2;
            y = 2;
            break;
        case 6:
            x = 3;
            y = 2;
            break;
        case 7:
            x = 1;
            y = 3;
            break;
        case 8:
            x = 2;
            y = 3;
            break;
        case 9:
            x = 3;
            y = 3;
            break;
        }

        if (x < 1 || x > 3 || y < 1 || y > 3) {
            printf("Niepoprawne wspolrzedne!\n");
            continue;
        }

        x--; y--;
        if (p_game_state->board[y][x] != ' ') {
            printf("To pole jest juz zajete!\n");
            continue;
        }

        if (player == 0)
            p_game_state->board[y][x] = 'X';
        else
            p_game_state->board[y][x] = 'O';
        printf("Czekaj na ruch przeciwnika...\n");
        showboard();
        //wygrana
        int won = FALSE;
        for (int i = 0; i < 3; i++) {
            if (p_game_state->board[i][0] == p_game_state->board[i][1] &&
                p_game_state->board[i][1] == p_game_state->board[i][2] &&
                p_game_state->board[i][0] != ' ') {
                won = TRUE;
            }
            if (p_game_state->board[0][i] == p_game_state->board[1][i] &&
                p_game_state->board[1][i] == p_game_state->board[2][i] &&
                p_game_state->board[0][i] != ' ') {
                won = TRUE;
            }
        }
        if (p_game_state->board[0][0] == p_game_state->board[1][1] &&
            p_game_state->board[1][1] == p_game_state->board[2][2] &&
            p_game_state->board[0][0] != ' ') {
            won = TRUE;
        }
        if (p_game_state->board[2][0] == p_game_state->board[1][1] &&
            p_game_state->board[1][1] == p_game_state->board[0][2] &&
            p_game_state->board[2][0] != ' ') {
            won = TRUE;
        }
        if (won) {
            printf("Gracz %c wygral!\n", (player == 0 ? 'X' : 'O'));
            return 0;
            break;
        }

        int draw = TRUE;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (p_game_state->board[i][j] == ' ') {
                    draw = FALSE;
                }
            }
        }
        if (draw) {
            printf("Remis!\n");
            break;
        }

        if (player==0){
            p_game_state->turn = 1;
        }
        if (player == 1) {
            p_game_state->turn = 0;
        }

    }

    UnmapViewOfFile(p_game_state);

    return 0;
}