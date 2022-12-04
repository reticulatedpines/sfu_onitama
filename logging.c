#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "logging.h"

// adds cards to the board visualisation
static void add_cards(char rows[9][140], struct game_state *s)
{
    int x_off = 15;
    int y_off = 2;

    for (int i = 0; i < 5; i++)
    {
        for (int m = 0; m <= s->cards[i].max_move; m++)
        {
            struct move move = s->cards[i].moves[m];
            rows[move.y + y_off + 2][move.x + x_off + 2] = 'o';
        }
        x_off += 12;
    }
}

// adds pieces to the board visualisation
static void add_pieces(char rows[9][140], struct game_state *s)
{
    uint8_t edge_offset = 3; // 3-byte unicode char ║
    if (s->player == 2)
        rows[0][3] = '2';

    // Add the pawns to the board.
    for (int i = 0; i <= s->p1_pieces.max_piece; i++)
    {
        char player_colour = 'r'; // r or b, R or B for Master
        struct piece piece = s->p1_pieces.pieces[i];
        if (i == 0)
        { // capitalise the Master "colour"
            rows[piece.y + 2][piece.x + edge_offset] = player_colour ^ 0x20;
        }
        else
        {
            rows[piece.y + 2][piece.x + edge_offset] = player_colour;
        }
    }
    for (int i = 0; i <= s->p2_pieces.max_piece; i++)
    {
        char player_colour = 'b'; // r or b, R or B for Master
        struct piece piece = s->p2_pieces.pieces[i];
        if (i == 0)
        { // capitalise the Master "colour"
            rows[piece.y + 2][piece.x + edge_offset] = player_colour ^ 0x20;
        }
        else
        {
            rows[piece.y + 2][piece.x + edge_offset] = player_colour;
        }
    }
}

static void get_game_state_template(char rows[9][140])
{
    strcpy(rows[0], "- P1 -    P1      P1     Extra    P2      P2   \0");
    strcpy(rows[1], "╔═════╗ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐\0");
    strcpy(rows[2], "║     ║ │     │ │     │ │     │ │     │ │     │\0");
    strcpy(rows[3], "║     ║ │     │ │     │ │     │ │     │ │     │\0");
    strcpy(rows[4], "║     ║ │  x  │ │  x  │ │  x  │ │  x  │ │  x  │\0");
    strcpy(rows[5], "║     ║ │     │ │     │ │     │ │     │ │     │\0");
    strcpy(rows[6], "║     ║ │     │ │     │ │     │ │     │ │     │\0");
    strcpy(rows[7], "╚═════╝ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘\0");
    strcpy(rows[8], "                                               \0");
}

// player must be 1 or 2
void log_game_state(struct game_state *s)
{
    static char rows[9][140] = {0};

    if ((s->player != 1) && (s->player != 2))
    {
        printf("ERROR: invalid player number: %d\n", s->player);
        assert(0);
        return;
    }

    get_game_state_template(rows);
    add_pieces(rows, s);
    add_cards(rows, s);

    // output the prepared game state
    printf("\n");
    for (int i = 0; i < 9; i++)
        printf("%s\n", rows[i]);
};

void log_actions(struct game_state *s)
{
    printf("Actions:\n");
    for (int a = 0; a < s->max_action; a++)
    {
        printf("piece, move, card: %d, %d, %d\n",
               s->valid_actions[a].piece,
               s->valid_actions[a].move,
               s->valid_actions[a].card);
    }
}
