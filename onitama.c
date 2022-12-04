#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gamestate.h"
#include "logging.h"

static int verbose = 0; // printing friendly game info
static int extra_checks = 0; // paranoid checks that shouldn't be required

static uint8_t player1_move(struct game_state game_in)
{
    // dummy content to make compiler happy during testing,
    // these lines will be removed by evolved code
    return game_in.max_action;
}

static uint8_t player2_move(struct game_state game_in)
{
    // dummy content to make compiler happy during testing,
    // these lines will be removed by evolved code
    return game_in.max_action;
}

/*
uint8_t pick_win_or_random_action(struct game_state game_in)
{
    if (game_in.max_action == 0)
        return 0;
    struct action action = game_in.valid_actions[0];
    struct move move = game_in.cards[action.card].moves[action.move];
    if (game_in.player == 2)
        action.card += 3;
    if (verbose)
    {
        printf("Choosing move %d, %d from card %d\n",
               move.x, move.y, action.card);
    }
    return 0;
}
*/

// simple strat for testing: pick first available action
static uint8_t pick_first_action(struct game_state game_in)
{
    if (game_in.max_action == 0)
        return 0;
    struct action action = game_in.valid_actions[0];
    struct move move = game_in.cards[action.card].moves[action.move];
    if (game_in.player == 2)
        action.card += 3;
    if (verbose)
    {
        printf("Choosing move %d, %d from card %d\n",
               move.x, move.y, action.card);
    }
    return 0;
}

// simple strat for testing: pick last available action
static uint8_t pick_last_action(struct game_state game_in)
{
    if (game_in.max_action == 0)
        return 0;
    uint8_t choice = game_in.max_action - 1;
    struct action action = game_in.valid_actions[choice];
    struct move move = game_in.cards[action.card].moves[action.move];
    if (game_in.player == 2)
        action.card += 3;
    if (verbose)
    {
        printf("Choosing move %d, %d from card %d\n",
               move.x, move.y, action.card);
    }
    return choice;
}

// simple strat for testing: pick a random action
static uint8_t pick_random_action(struct game_state game_in)
{
    if (game_in.max_action == 0)
        return 0;
    // this suffers from modulo bias but I don't think that matters
    uint8_t i = random() % game_in.max_action;
    struct action action = game_in.valid_actions[i];
    struct move move = game_in.cards[action.card].moves[action.move];
    if (game_in.player == 2)
        action.card += 3;
    if (verbose)
    {
        printf("Choosing move %d, %d from card %d\n",
               move.x, move.y, action.card);
    }
    return i;
}

// index of the card encodes if it's Red or Blue, i.e., which player should start
// if it's the extra card.
#define DECK_LEN 16
struct card deck[DECK_LEN] = {
    {.max_move = 3, { {-2, -1}, {2, -1}, {-1, 1}, {1, 1} } },
        {.max_move = 1, { {0, -2}, {0, 1}, {0, 0}, {0, 0} } },
    {.max_move = 3, { {-1, 0}, {-1, -1}, {1, 0}, {1, -1} } },
        {.max_move = 2, { {-1, -1}, {-1, 1}, {1, 0}, {0, 0} } },
    {.max_move = 2, { {0, -1}, {-1, 0}, {1, 0}, {0, 0} } },
        {.max_move = 2, { {2, 0}, {1, -1}, {-1, 1}, {0, 0} } },
    {.max_move = 2, { {-1, -1}, {1, -1}, {0, 1}, {0, 0} } },
        {.max_move = 2, { {-2, 0}, {2, 0}, {0, -1}, {0, 0} } },
    {.max_move = 3, { {1, 0}, {1, -1}, {-1, 0}, {-1, 1} } },
        {.max_move = 3, { {-1, -1}, {-1, 1}, {1, 1}, {1, -1} } },
    {.max_move = 2, { {-1, 0}, {0, 1}, {0, -1}, {0, 0} } },
        {.max_move = 2, { {1, 0}, {0, 1}, {0, -1}, {0, 0} } },
    {.max_move = 2, { {1, -1}, {1, 1}, {-1, 0}, {0, 0} } },
        {.max_move = 2, { {0, -1}, {-1, 1}, {1, 1}, {0, 0} } },
    {.max_move = 2, { {-2, 0}, {-1, -1}, {1, 1}, {0, 0} } },
        {.max_move = 3, { {-1, 0}, {-1, -1}, {1, 0}, {1, 1} } },
};
/* // kept in case I want to put names back in later
struct card deck[DECK_LEN] = {
    {.name = "Dragon",   .max_move = 3, { {-2, -1}, {2, -1}, {-1, 1}, {1, 1} } },
        {.name = "Tiger",    .max_move = 1, { {0, -2}, {0, 1}, {0, 0}, {0, 0} } },
    {.name = "Elephant", .max_move = 3, { {-1, 0}, {-1, -1}, {1, 0}, {1, -1} } },
        {.name = "Eel",      .max_move = 2, { {-1, -1}, {-1, 1}, {1, 0}, {0, 0} } },
    {.name = "Boar",     .max_move = 2, { {0, -1}, {-1, 0}, {1, 0}, {0, 0} } },
        {.name = "Rabbit",   .max_move = 2, { {2, 0}, {1, -1}, {-1, 1}, {0, 0} } },
    {.name = "Mantis",   .max_move = 2, { {-1, -1}, {1, -1}, {0, 1}, {0, 0} } },
        {.name = "Crab",     .max_move = 2, { {-2, 0}, {2, 0}, {0, -1}, {0, 0} } },
    {.name = "Rooster",  .max_move = 3, { {1, 0}, {1, -1}, {-1, 0}, {-1, 1} } },
        {.name = "Monkey",   .max_move = 3, { {-1, -1}, {-1, 1}, {1, 1}, {1, -1} } },
    {.name = "Horse",    .max_move = 2, { {-1, 0}, {0, 1}, {0, -1}, {0, 0} } },
        {.name = "Ox",       .max_move = 2, { {1, 0}, {0, 1}, {0, -1}, {0, 0} } },
    {.name = "Cobra",    .max_move = 2, { {1, -1}, {1, 1}, {-1, 0}, {0, 0} } },
        {.name = "Crane",    .max_move = 2, { {0, -1}, {-1, 1}, {1, 1}, {0, 0} } },
    {.name = "Frog",     .max_move = 2, { {-2, 0}, {-1, -1}, {1, 1}, {0, 0} } },
        {.name = "Goose",    .max_move = 3, { {-1, 0}, {-1, -1}, {1, 0}, {1, 1} } },
};
*/

// for the given piece within a set of pieces, is the given move valid?
static uint8_t is_valid_action(struct pieces *p, uint8_t p_i, struct move *move)
{
    if (p_i > p->max_piece)
        return 0;

    int8_t end_x = p->pieces[p_i].x;
    int8_t end_y = p->pieces[p_i].y;

    // check piece within board boundaries
    if (extra_checks)
    {
        if (end_x < 0 || end_x > 4)
            return 0;
        if (end_y < 0 || end_y > 4)
            return 0;
    }

    end_x += move->x;
    end_y += move->y;
    // check moved piece within board boundaries
    if (end_x < 0 || end_x > 4)
        return 0;
    if (end_y < 0 || end_y > 4)
        return 0;

    // check moved piece isn't on top of a friendly piece
    for (int i = 0; i <= p->max_piece; i++)
    {
        if (p->pieces[i].x == end_x
            && p->pieces[i].y == end_y)
        {
            return 0;
        }
    }
//    printf("Valid move: %d, %d / %d, %d\n",
//           move->x, move->y,
//           end_x, end_y);
    return 1;
}

// inspects the pieces and cards, updates both
// max_action and valid_actions to represent all
// legal moves (possibly 0).
static void update_valid_actions(struct game_state *s)
{
    // Rules for moving:
    //  - cannot move onto piece of the same colour
    //  - cannot move outside the 5x5 board
    //  - can only pass if no legal moves are available

    s->max_action = 0;

    // Game state is maintained so that cards[0] and cards[1]
    // are this player's cards this turn.
    for (int c = 0; c < 2; c++)
    {
        for (int m = 0; m <= s->cards[c].max_move; m++)
        {
            struct move *move = &(s->cards[c].moves[m]);
            for (int p = 0; p <= s->p1_pieces.max_piece; p++)
            {
                if (is_valid_action(&(s->p1_pieces), p, move))
                {
                    struct action action = {p, m, c};
                    s->valid_actions[s->max_action] = action;
                    s->max_action++;
                }
            }
        }
    }
}

static void init_game_state(struct game_state *s)
{
    // Each player is shown the game state as if they
    // are player 1; their pieces start at the bottom
    // of the board and their card array shows their choices
    // as options 0 and 1.
    //
    // This means any AI can play both sides.

    if (s == NULL)
    {
        assert(0);
        return;
    }

    // generate 5 cards from the 16 card deck
    uint8_t max_i = 15;
    uint8_t cards_i[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t cards[5] = {0};
    while (max_i > 10)
    {
        uint8_t c_i = random() % (max_i + 1);
        cards[15 - max_i] = cards_i[c_i];
        cards_i[c_i] = cards_i[max_i];
        max_i--;
    }

    // p1 is determined by card colour, cards are ordered in deck
    // such that they alternate colour.
    if (cards[2] % 2)
        s->player = 2;
    else
        s->player = 1;

    for (int i = 0; i < 5; i++)
        s->cards[i] = deck[cards[i]];

    s->p1_pieces.max_piece = 4;
    // pieces[0] is always the Master
    s->p1_pieces.pieces[0].x = 2;
    s->p1_pieces.pieces[0].y = 4;
    // 4 Pawns
    s->p1_pieces.pieces[1].x = 0;
    s->p1_pieces.pieces[1].y = 4;
    s->p1_pieces.pieces[2].x = 1;
    s->p1_pieces.pieces[2].y = 4;
    s->p1_pieces.pieces[3].x = 3;
    s->p1_pieces.pieces[3].y = 4;
    s->p1_pieces.pieces[4].x = 4;
    s->p1_pieces.pieces[4].y = 4;

    // player 2 pieces
    s->p2_pieces.max_piece = 4;
    s->p2_pieces.pieces[0].x = 2;
    s->p2_pieces.pieces[0].y = 0;
    // 4 Pawns
    s->p2_pieces.pieces[1].x = 4;
    s->p2_pieces.pieces[1].y = 0;
    s->p2_pieces.pieces[2].x = 3;
    s->p2_pieces.pieces[2].y = 0;
    s->p2_pieces.pieces[3].x = 1;
    s->p2_pieces.pieces[3].y = 0;
    s->p2_pieces.pieces[4].x = 0;
    s->p2_pieces.pieces[4].y = 0;

    update_valid_actions(s);
}

// adjust piece locations to remove a piece from play
static int remove_piece(struct game_state *s, uint8_t capture_index)
{
    struct pieces *pieces = &(s->p2_pieces); // victim pieces
    if (s->player == 2)
    {
        pieces = &(s->p1_pieces);
    }
    if (capture_index > pieces->max_piece)
        return -1;
    if (capture_index == 0)
        return -3; // no need to remove this piece, it's a win condition

    pieces->max_piece--;
    for (int i = capture_index; i <= pieces->max_piece; i++)
    {
        pieces->pieces[i] = pieces->pieces[i + 1];
    }
    return 0;
}

// returns negative for error,
// returns positive if acting player won the game,
// returns 0 otherwise.
static int8_t perform_action(struct game_state *s, struct action action)
{
    struct pieces *pieces = &(s->p1_pieces);
    struct pieces *opp_pieces = &(s->p2_pieces);
    struct move m = s->cards[action.card].moves[action.move];

    if (s->player == 2)
    {
        pieces = &(s->p2_pieces);
        opp_pieces = &(s->p1_pieces);
        // flip move, board is rotated for p2
        action.card += 3;
        m = s->cards[action.card].moves[action.move];
        m.x = 0 - m.x;
        m.y = 0 - m.y;
    }

    struct piece p = pieces->pieces[action.piece];
    if (verbose)
    {
        printf("P%d moving %d,%d by %d,%d\n", s->player,
               p.x, p.y,
               m.x, m.y);
    }
    
    struct piece *piece = NULL;
    for (int i = 0; i <= pieces->max_piece; i++)
    {
        if (pieces->pieces[i].x == p.x
            && pieces->pieces[i].y == p.y)
        {
            piece = &(pieces->pieces[i]);
            if (extra_checks)
            {
                // requesting invalid move for piece?
                if (!is_valid_action(pieces, i, &m))
                    return -10;
            }
            break;
        }
    }
    if (piece == NULL)
        return -2;
    
    // looks okay, adjust game state

    // move piece
    piece->x += m.x;
    piece->y += m.y;

    // adjust card order
    struct card tmp_card;
    tmp_card = s->cards[2]; // the extra card
    s->cards[2] = s->cards[action.card];
    s->cards[action.card] = tmp_card;

    // check for capture
    int capture_index = 0;
    for (int i = 0; i <= opp_pieces->max_piece; i++)
    {
        if (piece->x == opp_pieces->pieces[i].x
            && piece->y == opp_pieces->pieces[i].y)
        {
            if (i == 0)
            {
                if (verbose)
                    printf("P%d captured opponent Master!\n", s->player);
                return 1;
            }
            if (verbose)
                printf("P%d captured opponent pawn!\n", s->player);
            capture_index = i;
        }
    }
    if (capture_index)
    { // cannot be 0 and a capture, we return 1 immediately if this happens
        int res = remove_piece(s, capture_index);
        if (res < 0)
        {
            printf("ERROR: remove_piece() returned %d\n", res);
            return -11;
        }
    }

    // check for temple victory
    if (s->player == 2
        && piece->x == 2
        && piece->y == 4)
    {
        if (verbose)
            printf("P2 captured P1 temple!\n");
        return 1;
    }
    else if (s->player == 1
             && piece->x == 2
             && piece->y == 0)
    {
        if (verbose)
            printf("P1 captured P2 temple!\n");
        return 1;
    }

    return 0;
}

// given a game state, switch the player perspective
static void flip_player(struct game_state *s)
{
    // change piece locations, rotating the board
    struct pieces tmp_pieces = s->p1_pieces;
    for (int i = 0; i < 5; i++)
    {
        s->p1_pieces.pieces[i].x = 4 - s->p2_pieces.pieces[i].x;
        s->p1_pieces.pieces[i].y = 4 - s->p2_pieces.pieces[i].y;
        s->p2_pieces.pieces[i].x = 4 - tmp_pieces.pieces[i].x;
        s->p2_pieces.pieces[i].y = 4 - tmp_pieces.pieces[i].y;
    }
    s->p1_pieces.max_piece = s->p2_pieces.max_piece;
    s->p2_pieces.max_piece = tmp_pieces.max_piece;

    // swap cards
    struct card tmp_card = s->cards[0];
    s->cards[0] = s->cards[3];
    s->cards[3] = tmp_card;
    tmp_card = s->cards[1];
    s->cards[1] = s->cards[4];
    s->cards[4] = tmp_card;
}

int main(int argc, char **argv)
{
    // here we expect player2_move() and player1_move() to have been filled in
    // with our evolved code.  We want to run many games since the move cards
    // can vary and the players don't have to play deterministically.

    // We give the players clean copies of the data they need each time.
    // This is because we don't officially trust them (may want to make them
    // networked clients later on), and because evolved code tends to do crazy
    // stuff.
    // Consequently we avoid having pointers in the data passed to players.

    uint64_t rand_seed = 6;
    if (argc > 1)
    {
        rand_seed = strtol(argv[1], NULL, 0);
    }

    char rand_state[256];
    initstate(rand_seed, rand_state, 256);

    struct game_state game = {0};
    uint32_t p1_wins = 0;
    uint32_t p2_wins = 0;
    uint32_t draws = 0;
    for (int game_count = 0; game_count < 1000000; game_count++)
    {
        // two game states, we give each player a view with themselves
        // starting at the bottom of the board, and keep them in sync.
        // This should give the AI an easier time.
        init_game_state(&game);

        //log_actions(&game);
        if (verbose)
            log_game_state(&game);

        int i;
        for (i = 0; i < 100; i++)
        {
            // copy internal game state, with player perspective translation
            struct game_state p_game = game;
            if (game.player == 2)
            {
                flip_player(&p_game);
            }
            update_valid_actions(&p_game);
            //log_actions(&p_game);

            // Copy the data we'll want after passing p_game
            // to the AI.  We don't want to use data the AI code
            // may have modified.
            uint8_t max_action = p_game.max_action;
            struct action valid_actions[MAX_ACTIONS];
            memcpy(valid_actions, p_game.valid_actions, sizeof(struct action) * MAX_ACTIONS);

            // call the player AI
            uint8_t action_i;
            if (game.player == 1)
                action_i = pick_random_action(p_game);
            else
                action_i = pick_random_action(p_game);
            // From this point, we can't trust p_game data
            // (well, we can if I make a bug free AI...),

            int8_t win_condition;
            if (max_action > 0)
            {
                struct action action = valid_actions[action_i % max_action];
                // This action is correct for P1 of p_game, which
                // may be flipped into P2's perspective.
                // 
                // perform_action() determine's which perspective
                // it should apply the move from.
                win_condition = perform_action(&game, action);
                // 0 == no winner, -ve == error, +ve == current player won
            }
            else
            {
                if (verbose)
                    printf("P%d has no valid moves, passing\n", game.player);
                win_condition = 0;
            }

            int last_player = game.player;
            if (game.player == 2)
                game.player = 1;
            else
                game.player = 2;
            if (verbose)
                log_game_state(&game);

            if (win_condition < 0)
            { // negative is error
                // Because we only allow players to choose an index into
                // valid actions, this should never occur.  We want to know
                // about it if it does, it's a serious bug.
                printf("ERROR: perform_action() returned %d\n", win_condition);
                exit(-1);
            }
            if (win_condition > 0)
            {
                if (verbose)
                    printf("WINNER: P%d\n", last_player);
                if (last_player == 1)
                    p1_wins++;
                else
                    p2_wins++;
                break;
            }
        }
        if (i == 100)
        {
            draws++;
            if (verbose)
                printf("DRAW (turn limit hit)\n");
        }
    }
    printf("P1, P2, draw: %d, %d, %d\n", p1_wins, p2_wins, draws);

}
