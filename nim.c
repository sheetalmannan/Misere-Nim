#include "nim.h"
#include <stdio.h>
#include <stdlib.h>

int sum_board(int board_size, int *board){
    int sum = 0;
    for (int i = 0; i< board_size; i++){
        sum+=board[i];
    }
    return sum;
}

int compute_nimsum( int board_size, int *board ){
    int negate = 1;
    int nimsum = 0;
    int sum = 0;
    for(int i = 0; i<board_size; i++){
        if (board[i] > 1){
            negate = 0;
        }
        nimsum ^= board[i];
        sum += board[i];
    }
    // special case for when we have only 1 match
    if (sum <= 1) {
        return 0;
    }
    return negate ? ~nimsum : nimsum;
}

int *new_board(int board_size){
    int *board = (int*)malloc(board_size * sizeof(int));
    // check to see if malloc fails or not
    if (board == NULL){
        fprintf( stderr, "Could not allocate memory for board");
        return NULL;
    }
    return board;
}

struct node *mk_nim_hash( int max_hash, int board_size, int *start_board ){
    struct node *hashs = malloc(max_hash * sizeof(struct node));
    if (hashs == NULL){
        fprintf( stderr, "Could not allocate memory for nim hash");
        return NULL;
    }
    for (int i = 0; i<max_hash; i++){
        hashs[i].nimsum = -1;
        hashs[i].moves = -1;	
        hashs[i].move = NULL;
    }
    return hashs;
}
void free_board( int *board ){
    free(board);
}

void free_nim_hash( int max_hash, struct node *nim_hash ){
    for(int i = 0; i < max_hash; i++){
        free(nim_hash[i].move);
    }
    free(nim_hash); 
}

int *board_from_argv( int board_size, char **argv ){
    int* board = new_board(board_size);
    for (int i = 0; i< board_size; i++){
        board[i] = atoi(argv[i]);
    }
    return board;
}

int *copy_board( int board_size, int *board ){
    int* nboard = new_board(board_size);
    for (int i = 0; i< board_size; i++){
        nboard[i] = board[i];
    }
    return nboard;
}

int game_over( int board_size, int *board ){
    
    return sum_board(board_size, board) <= 1;
}

void join_graph( struct node *nim_hash, int hash, int board_size, int *start_board ){
    struct node n = nim_hash[hash];
    if (n.moves != -1){
        return;
    }
    int* current_board = hash2board(board_size, start_board, hash);
    n.nimsum = compute_nimsum(board_size, current_board);
    int number_moves = sum_board(board_size, current_board);
    n.moves = number_moves;
    n.move = malloc(number_moves * sizeof(struct move));
    int index = 0;
    for (int i = 0; i< board_size; i++){
        for (int j = 1; j<=current_board[i]; j++){
            struct move m;
            m.row = i;
            m.matches = j;
            // get new board and calculate its hash
            int *board = copy_board(board_size, current_board);
            board[m.row] = board[m.row] - m.matches;
            m.hash = board2hash(board_size, start_board, board);
            n.move[index] = m;
            index++;
            join_graph(nim_hash, m.hash, board_size, start_board);
        }
    }
    nim_hash[hash] = n;
}
