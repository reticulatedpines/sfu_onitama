#ifndef _oni_gamestate_h_
#define _oni_gamestate_h_

#include <stdint.h>

#define CARDS_LEN 5
#define MAX_ACTIONS 40 // There are 2 cards, max 4 moves per card, max 5 pieces = 40 max actions

struct move
{ // these hold relative movement, x=1, y=-1
  // is 1 east, 1 south
    int8_t x;
    int8_t y;
};

struct card
{
    // A move card may have 2, 3 or 4 options for moves on it.
//    char name[10];
    int8_t max_move;
    struct move moves[4];
};

struct piece
{ // these hold absolute position on the board.
  // top-left is 0,0
  // bottom-right is 5,5
    int8_t x;
    int8_t y;
};

struct pieces
{
    // max_piece is decremented if pieces are captured.
    // pieces[0] always holds the Master.
    // If a capture occurs the right-most piece
    // in the array is moved to replace the captured piece.
    int8_t max_piece;
    struct piece pieces[5];
};

// a request to move a piece
struct action
{
    int8_t piece; // which piece to move, an index into pieces, limited by max_piece
    int8_t move; // where to move it, an index into valid_actions
    int8_t card; // which card the move came from, an index into cards
};

struct game_state
{
    // Each player is shown the game state as if they
    // are player 1; their pieces start at the bottom
    // of the board and their card array shows their choices
    // as options 0 and 1.
    //
    // This means any AI can play both sides.
    //
    // I'm guessing recording the true player ID won't
    // interfere with that goal, and might end up being useful.
    uint8_t player;

    // The 5x5 board is implied and not needed for
    // representing game state.
    struct pieces p1_pieces;
    struct pieces p2_pieces;

    // The five possible game cards.
    // These are presented to a player with their two cards first,
    // the floating card next, then the two opponent cards
    struct card cards[CARDS_LEN];

    // To simplify the AI part, we pass in all valid actions,
    // meaning the choice of action can be expressed as an index
    // into this array.
    uint8_t max_action; // actions greater than this index are not valid this turn
    struct action valid_actions[MAX_ACTIONS];
};

#endif // _oni_gamestate_h_
