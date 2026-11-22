#include "../zk_lib.hpp"
using namespace zk;

/*
 * Interactive showcase for zk_lib.hpp: Tic-Tac-Toe, Minesweeper, and Wordle.
 *
 * Reading guide:
 *   - main() shows the application's startup and terminal cleanup.
 *   - Shared menu configuration and game classes follow.
 * 
 *   - TicTacToe class.
 *   - Minesweeper class.
 *   - Wordle class.
 * 
 *   - run_game_collection(), at the end of the file, connects the games to menus.
 */
void run_game_collection();

int main()
{
    // Switch to the interactive terminal mode used by the library.
    init_program();

    // Build and run the menu tree after the game classes have been defined.
    run_game_collection();

    // Restore the terminal before the process exits.
    exit_program();
    return 0;
}

//
//  SHARED MENU CONFIGURATION
//

// Shared by the main menu and the game submenus.
menu_manager manager;

menu
menu_main         ("MAIN MENU"),
menu_tictactoe    ("TIC TAC TOE"),
menu_minesweeper  ("MINESWEEPER"),
menu_wordle       ("WORDLE");

// Accent color used for each menu's title and interface.
std::string
color_main        = c_lime,
color_tictactoe   = c_blue,
color_minesweeper = c_red,
color_wordle      = c_green;

// ASCII-art titles displayed above each menu and game screen.
std::vector<std::string>
title_main = {
    "                                                                       ",
    "            ▄▄▄▄▄ ▄▄ ▄▄    ▄████   ▄▄▄  ▄▄   ▄▄ ▄▄▄▄▄  ▄▄▄▄            ",
    "              ▄█▀ ██▄█▀   ██  ▄▄▄ ██▀██ ██▀▄▀██ ██▄▄  ███▄▄            ",
    "            ▄██▄▄ ██ ██    ▀███▀  ██▀██ ██   ██ ██▄▄▄ ▄▄██▀            ",
    "                                                                       "
},
title_tictactoe = {
    "                                                                       ",
    "        ██████ ▄▄  ▄▄▄▄   ██████ ▄▄▄   ▄▄▄▄   ██████ ▄▄▄  ▄▄▄▄▄        ",
    "          ██   ██ ██▀▀▀     ██  ██▀██ ██▀▀▀     ██  ██▀██ ██▄▄         ",
    "          ██   ██ ▀████     ██  ██▀██ ▀████     ██  ▀███▀ ██▄▄▄        ",
    "                                                                       "
},
title_minesweeper = {
    "                                                                       ",
    " ██▄  ▄██ ▄▄ ▄▄  ▄▄ ▄▄▄▄▄  ▄▄▄▄ ▄▄   ▄▄ ▄▄▄▄▄ ▄▄▄▄▄ ▄▄▄▄  ▄▄▄▄▄ ▄▄▄▄   ",
    " ██ ▀▀ ██ ██ ███▄██ ██▄▄  ███▄▄ ██ ▄ ██ ██▄▄  ██▄▄  ██▄█▀ ██▄▄  ██▄█▄  ",
    " ██    ██ ██ ██ ▀██ ██▄▄▄ ▄▄██▀  ▀█▀█▀  ██▄▄▄ ██▄▄▄ ██    ██▄▄▄ ██ ██  ",
    "                                                                       ",
},
title_wordle = {
    "                                                                       ",
    "                ██     ██  ▄▄▄  ▄▄▄▄  ▄▄▄▄  ▄▄    ▄▄▄▄▄                ",
    "                ██ ▄█▄ ██ ██▀██ ██▄█▄ ██▀██ ██    ██▄▄                 ",
    "                 ▀██▀██▀  ▀███▀ ██ ██ ████▀ ██▄▄▄ ██▄▄▄                ",
    "                                                                       "
};

//
//  TICTACTOE
//

class tictactoe
{
    // Variables

    int board[3][3] = {
        {0,  0,  0},
        {0,  0,  0},
        {0,  0,  0}
    };

    Vector2D cursor_pos = {1, 1};
    bool turn   = true;
    int  winner = 0;

    // Functions

    void update_screen()
    {
        // Figures

        std::string figure_cursor[] = {
            c_green + "┌     ┐" + f_clear,
            c_green + "       " + f_clear,
            c_green + "└     ┘" + f_clear
        };

        std::string figure_x[] = {
            c_blue + "  ╲ ╱  " + f_clear,
            c_blue + "   ╳   " + f_clear,
            c_blue + "  ╱ ╲  " + f_clear
        };

        std::string figure_x_selected[] = {
            c_red + "┌" + c_blue + " ╲ ╱ " + c_red + "┐" + f_clear,
            c_red + " " + c_blue + "  ╳  " + c_red + " " + f_clear,
            c_red + "└" + c_blue + " ╱ ╲ " + c_red + "┘" + f_clear
        };

        std::string figure_o[] = {
            c_red + "  ┌─┐  " + f_clear,
            c_red + "  │ │  " + f_clear,
            c_red + "  └─┘  " + f_clear
        };

        std::string figure_o_selected[] = {
            c_red + "┌ ┌─┐ ┐" + f_clear,
            c_red + "  │ │  " + f_clear,
            c_red + "└ └─┘ ┘" + f_clear
        };

        // Print title
        print_title(title_tictactoe, color_tictactoe);

        // Print header
        std::string string_header = (turn ? c_blue + "X'S TURN" + f_clear : c_red + "O'S TURN" + f_clear);

        if (winner == 1 or winner == 2)
            string_header = bold + underline + c_green + "WE HAVE A WINNER!" + f_clear;

        if (winner == 3)
            string_header = bold + underline + c_green + "NO SPACES LEFT!" + f_clear;
        
        std::cout << center(string_header) << "\n\n";

        // Prerender board
        std::string board_render[11] = { "","","","","","","","","","","" };
        size_t current_line = 0;
        for (size_t y = 0; y < 3; y++)
        {
            for (size_t i = 0; i < 3; i++)
            {
                for (size_t x = 0; x < 3; x++)
                {
                    // Cursor position
                    bool is_cursor = (cursor_pos.x == x and cursor_pos.y == y);

                    if (!is_cursor)
                    {
                        if (board[y][x] == 0) board_render[current_line] += "       ";
                        if (board[y][x] == 1) board_render[current_line] += figure_x[i];
                        if (board[y][x] == 2) board_render[current_line] += figure_o[i];
                    }
                    else
                    {
                        if (board[y][x] == 0) board_render[current_line] += figure_cursor[i];
                        if (board[y][x] == 1) board_render[current_line] += figure_x_selected[i];
                        if (board[y][x] == 2) board_render[current_line] += figure_o_selected[i];
                    }

                    // Vertical separator
                    if (x < 2) board_render[current_line] += "┃";
                }
                
                current_line++;
            }

            // Horizontal separator
            if (y < 2)
            {
                board_render[current_line] = "━━━━━━━╋━━━━━━━╋━━━━━━━";
                current_line++;
            }
        }
        
        // Print board
        for (auto &&line : board_render)
            std::cout << center(line) << "\n";

        // Print footer
        std::string footer = c_blue + "← ↑ → ↓" + c_gray + " | " + c_green + "↲" + f_clear +
                        "\n" + c_orange + "⟳ R" + c_gray + " | " + c_red + "✖ Q" + f_clear;
        print_footer(footer);
        
        // std::flush
        std::cout << std::flush;
    }

    int verify_winner(bool minimax_mode = false)
    {
        size_t calculated_winner = 0;

        // Evaluates X then O
        for (size_t p = 1; p<= 2; p++)
        {
            for (size_t i = 0; i < 3; i++)
            {
                // Check rows & columns
                if (board[i][0] == p and board[i][1] == p and board[i][2] == p) { calculated_winner = p; goto winner_found; }
                if (board[0][i] == p and board[1][i] == p and board[2][i] == p) { calculated_winner = p; goto winner_found; }
            }
            
            // Check diagonals
            if (board[0][0] == p and board[1][1] == p and board[2][2] == p) { calculated_winner = p; goto winner_found; }
            if (board[0][2] == p and board[1][1] == p and board[2][0] == p) { calculated_winner = p; goto winner_found; }
        }

        winner_found:

        // Search zeros for draw
        bool filled = true;
        for (size_t y = 0; y < 3; y++)
            for (size_t x = 0; x < 3; x++)
                if (board[y][x] == 0) { filled = false; break; }
        
        if (filled and calculated_winner == 0) calculated_winner = 3;

        // Return calculated
        if (minimax_mode)
        {
            if (calculated_winner == 0) return  -1;
            if (calculated_winner == 1) return -10;
            if (calculated_winner == 2) return +10;
            if (calculated_winner == 3) return   0;
        }
        
        // Not minimax
        winner = calculated_winner; return 0;
    }

    int minimax(int _board[3][3], bool is_machine)
    {
        // Verify winner
        int score = verify_winner(true);
        if (score != -1) return score;

        // Machine turn
        if (is_machine)
        {
            int best_score = -1000;

            for (size_t y = 0; y < 3; y++)
                for (size_t x = 0; x < 3; x++)
                    if (_board[y][x] == 0)
                    {
                        _board[y][x] = 2;
                        best_score = std::max(best_score, minimax(_board, false));
                        _board[y][x] = 0;
                    }
            
            return best_score;
        }
        
        // Simulated human turn
        else
        {
            int best_score = +1000;

            for (size_t y = 0; y < 3; y++)
                for (size_t x = 0; x < 3; x++)
                    if (_board[y][x] == 0)
                    {
                        _board[y][x] = 1;
                        best_score = std::min(best_score, minimax(_board, true));
                        _board[y][x] = 0;
                    }
            
            return best_score;
        }
    }

    Vector2D get_best_move(int _board[3][3])
    {
        int best_score = -1000;
        Vector2D best_move = {-1, -1};

        for (size_t y = 0; y < 3; y++)
            for (size_t x = 0; x < 3; x++)
                if (_board[y][x] == 0)
                {
                    _board[y][x] = 2;
                    int score = minimax(_board, false);
                    _board[y][x] = 0;

                    if (score > best_score)
                    {
                        best_score = score;
                        best_move = {int(x), int(y)};
                    }
                }
                
        return best_move;
    }

    Vector2D get_rand_move(int _board[3][3])
    {
        Vector2D rand_move = {-1, -1};

        // Get random move
        while (true)
        {
            rand_move = {irandom(0,2), irandom(0,2)};
            if (_board[rand_move.y][rand_move.x] == 0) break;
        }
        
        return rand_move;
    }

    Vector2D get_some_move(int _board[3][3])
    {
        Vector2D some_move = {-1, -1};

        if (irandom(1, 100) <= 80)
            some_move = get_best_move(_board);
        else
            some_move = get_rand_move(_board);

        return some_move;
    }

    int player_turn()
    {
        // Get key
        std::string key = get_key();

        if (key == key_up    and cursor_pos.y > 0) cursor_pos.y--;
        if (key == key_down  and cursor_pos.y < 2) cursor_pos.y++;
        if (key == key_right and cursor_pos.x < 2) cursor_pos.x++;
        if (key == key_left  and cursor_pos.x > 0) cursor_pos.x--;
        
        if (key == "q" or key == "Q") return -1;
        if (key == "r" or key == "R") { play(); return -1; }

        if (key == key_intro)
        {
            // Put turn
            if (board[cursor_pos.y][cursor_pos.x] == 0)
            {
                board[cursor_pos.y][cursor_pos.x] = (turn ? 1 : 2);
                turn = !turn;
            }
        }

        return 0;
    }

    int announce_winner()
    {
        cursor_pos = {-1, -1};
        update_screen();
        sleep(2000);

        std::string announce;
        if (winner == 1) announce = "X has won" + c_blue + "!" + f_clear;
        if (winner == 2) announce = "O has won" + c_red + "!" + f_clear;
        if (winner == 3) announce = "It's a " + c_green + "Draw" + f_clear + "!";

        int option = dialog(announce, {"Play again","View board","Back"});

        switch (option)
        {
            case 0: update_screen(); play(); return -1; break;
            case 1: update_screen(); wait(); return -1; break;
            case 2: return -1; break;
        }

        return -1;
    }

    void PvP()
    {
        // Game loop
        while (true)
        {
            update_screen();
            if (player_turn() == -1) break;

            // If there's a winner
            verify_winner();
            if (winner != 0)
                if (announce_winner() == -1) break;
        }
    }

    void PvE()
    {
        while (true)
        {
            // Human turn
            if (turn and winner == 0)
            {
                update_screen();

                if (player_turn() == -1) break;

                verify_winner();
            }

            // Machine turn
            if (!turn and winner == 0)
            {
                // Verify if the board is empty for a random placemente
                bool empty = true;
                for (size_t y = 0; y < 3; y++)
                    for (size_t x = 0; x < 3; x++)
                        if (board[y][x] != 0) { empty = false; break; }
                
                update_screen();

                Vector2D pos;

                // Get move by difficulty
                if (!empty)
                {
                    dialog("Thinking", {}, true, 2);

                    switch (difficulty)
                    {
                        case 0:  pos = get_rand_move(board); break;
                        case 1:  pos = get_some_move(board); break;
                        case 2:  pos = get_best_move(board); break;
                        default: pos = get_best_move(board); break;
                    }
                }
                else pos = get_rand_move(board);

                // Apply move
                board[pos.y][pos.x] = 2;
                turn = true;

                verify_winner();
            }
            
            // If there's a winner
            if (winner != 0)
                if (announce_winner() == -1) break;
        }
    }

public:

    int game_mode  = 0;
    int difficulty = 0;

    void play()
    {
        // Restart variables
        for (size_t y = 0; y < 3; y++)
            for (size_t x = 0; x < 3; x++)
                board[y][x] = 0;
        
        cursor_pos  = {1,1};
        winner      = 0;

        // Decide who starts
        turn = (irandom(1, 100) <= 50);

        // Announce who starts
        std::string announce = turn ? "You will start" + c_blue + f_clear : "The " + c_red + "MACHINE" + f_clear + " will start";
        switch (difficulty)
        {
            case 0:  announce += "\non " + c_green  + "easy"     + f_clear + " mode"; break;
            case 1:  announce += "\non " + c_orange + "hard"     + f_clear + " mode"; break;
            case 2:  announce += "\non " + c_red    + "impossible" + f_clear + " mode"; break;
            default: announce += "\non " + c_red    + "impossible" + f_clear + " mode"; break;
        }

        // Select game mode
        if (game_mode == 0) { turn = true;              PvP(); }
        if (game_mode == 1) { dialog(announce); PvE(); }
    }
};

class minesweeper
{
    // Variables

    std::vector<std::vector<int>> board;
    size_t board_size;

    std::vector<Vector2D> revealed_cells;
    std::vector<Vector2D> mine_cells;
    std::vector<Vector2D> flag_cells;

    size_t remaining_flags = 0;

    Vector2D cursor_pos = {0,0};

    int win = 0;

    enum class GameResult
    {
        quit,
        restart,
        finished
    };

    const int directions[8][2] = {
        {-1,-1}, { 0,-1}, {+1,-1},
        {-1, 0},          {+1, 0},
        {-1,+1}, { 0,+1}, {+1,+1},
    };

    // Functions

    void fill_board(Vector2D first_click)
    {
        // Fill with zeros
        std::vector<int> row(board_size, 0);
        for (size_t i = 0; i < board_size; i++) board.push_back(row);

        // Fill with mines
        int max_mines_possible = (board_size * board_size) - 9;
        int actual_mines_count = std::min(board_mines_count, max_mines_possible);

        size_t mines_placed = 0;
        while (mines_placed < actual_mines_count)
        {
            Vector2D pos = {irandom(0,board_size-1),irandom(0,board_size-1)};

            // Avoid place the mine in the fist click
            if (abs(pos.x - first_click.x) <= 1 and abs(pos.y - first_click.y) <= 1) continue;
            
            // Place mine
            if (board[pos.y][pos.x] == 0)
            {
                board[pos.y][pos.x] = 9;
                mine_cells.push_back(pos);
                mines_placed++;
            }
            
        }
        remaining_flags = mines_placed;

        // Calculate numbers
        for (size_t y = 0; y < board_size; y++)
            for (size_t x = 0; x < board_size; x++)
            {
                if (board[y][x] != 0) continue;
                
                int mine_count = 0;

                // Check neighbours
                for (const auto &dir : directions)
                {
                    int offset_x = x + dir[0];
                    int offset_y = y + dir[1];

                    if (offset_x < 0 or offset_x >= board_size or offset_y < 0 or offset_y >= board_size) continue;

                    if (board[offset_y][offset_x] == 9) mine_count++;
                }

                board[y][x] = mine_count;
            }
    }

    void update_screen()
    {
        // Print title
        print_title(title_minesweeper, color_minesweeper);

        // Print header
        std::string header = "Remaining: " + c_red + std::to_string(remaining_flags) + " ⚑" + f_clear;
        if (revealed_cells.empty()) header = " ";
        if (win == 2) header = color_rgb(237, 92, 121, true) + c_yellow + "BOOM!" + f_clear;
        if (win == 1) header = bold + underline + c_green + "YOU WON!" + f_clear;
        
        std::cout << center(header) << "\n";

        // Prerender board
        std::vector<std::string> board_render(board_size, "");
        for (size_t y = 0; y < board_size; y++)
            for (size_t x = 0; x < board_size; x++)
            {
                Vector2D pos = {int(x), int(y)};

                // Cursor selection
                if (cursor_pos.x == x and cursor_pos.y == y) board_render[y] += color_rgb(100,100,100,true); 
                
                // Flag at position
                if (exist(flag_cells, pos))
                { 
                    board_render[y] += c_red + "⚑" + f_clear;
                    if (x < board_size - 1) { board_render[y] += " "; }
                    continue;
                }
                
                // Not revealed
                if (!exist(revealed_cells, pos))
                { 
                    board_render[y] += "■" + f_clear;
                    if (x < board_size - 1) { board_render[y] += " "; }
                    continue;
                }
                
                // Number
                switch (board[y][x])
                {
                    case 0: board_render[y] +=          + " " + f_clear; break;
                    case 1: board_render[y] += c_blue   + "1" + f_clear; break;
                    case 2: board_render[y] += c_green  + "2" + f_clear; break;
                    case 3: board_render[y] += c_yellow + "3" + f_clear; break;
                    case 4: board_render[y] += c_orange + "4" + f_clear; break;
                    case 5: board_render[y] += c_red    + "5" + f_clear; break;
                    case 6: board_render[y] += c_pink   + "6" + f_clear; break;
                    case 7: board_render[y] += c_purple + "7" + f_clear; break;
                    case 8: board_render[y] += c_white  + "8" + f_clear; break;

                    case 9: board_render[y] += c_red    + "✹" + f_clear; break;
                }

                // Add space
                if (x < board_size - 1) { board_render[y] += " "; }
            }
        for (auto &&line : board_render) { line = "┃ " + line + " ┃"; }
        board_render.insert(board_render.begin(), "┏" + string_repeat("━", board_size * 2 + 1) + "┓");
        board_render.push_back("┗" + string_repeat("━", board_size * 2 + 1) + "┛");
        
        // Print board
        for (auto &&line : board_render)
            std::cout << center(line) << "\n";

        // Print footer
        std::string footer = c_blue + "← ↑ → ↓" + c_gray + " | " + c_green + "↲" + c_gray + " | " + c_red + "⚑ F" +
                        "\n" + c_orange + "⟳ R" + c_gray + " | " + c_red + "✖ Q" + f_clear;
        print_footer(footer);

        // std::flush
        std::cout << std::flush;
    }

    void reveal_zone()
    {
        std::stack<Vector2D> to_reveal;
        to_reveal.push(cursor_pos);

        while (!to_reveal.empty())
        {
            // Actual position
            int x = to_reveal.top().x;
            int y = to_reveal.top().y;

            // Pop
            to_reveal.pop();

            // Check if is already revealed
            if (exist(revealed_cells, Vector2D{x,y})) continue;
            
            // Reveal cell
            revealed_cells.push_back({x,y});

            // Check neighbours
            for (const auto &dir : directions)
            {
                int offset_x = x + dir[0];
                int offset_y = y + dir[1];

                if (offset_x < 0 or offset_x >= board_size or offset_y < 0 or offset_y >= board_size) continue;
                
                if (board[offset_y][offset_x] == 0) to_reveal.push({offset_x, offset_y});
                else revealed_cells.push_back({offset_x, offset_y});
            }
        }
    }

    int explode()
    {
        cursor_pos = {-1,-1};

        // Reveal all mines
        for (auto &&mine : mine_cells)
        {
            revealed_cells.push_back(mine);
            update_screen();
            sleep(int(2000/mine_cells.size()));
        }

        sleep(1000);
        
        return dialog(bold + c_red + "You stepped on a mine!" + f_clear, {"Play again","View board","Back"});
    }

    int win_game()
    {
        cursor_pos = {-1,-1};
        update_screen();
        
        sleep(1000);

        return dialog(bold + c_green + "You found all the mines!" + f_clear, {"Play again","View board","Back"});
    }

    GameResult game_loop()
    {
        while (win == 0)
        {
            update_screen();

            // Get key
            std::string key = get_key();
            
            // Move cursor
            if (key == key_up    and cursor_pos.y > 0)              cursor_pos.y--;
            if (key == key_down  and cursor_pos.y < board_size - 1) cursor_pos.y++;
            if (key == key_right and cursor_pos.x < board_size - 1) cursor_pos.x++;
            if (key == key_left  and cursor_pos.x > 0)              cursor_pos.x--;

            // Game
            if (key == "q" or key == "Q") return GameResult::quit;
            if (key == "r" or key == "R") return GameResult::restart;
            
            // Reveal cell
            if (key == key_intro)
            {
                // Do nothing if there's a flag
                if (exist(flag_cells, cursor_pos)) continue;
                
                // Reveal initial zone
                if (revealed_cells.empty()) fill_board(cursor_pos);
                
                // Reveal zone
                if (board[cursor_pos.y][cursor_pos.x] == 0) reveal_zone();

                // Reveal individual
                if (in_range(board[cursor_pos.y][cursor_pos.x], 1, 8)) revealed_cells.push_back(cursor_pos);
                
                // Stepped on a mine
                if (board[cursor_pos.y][cursor_pos.x] == 9) { revealed_cells.push_back(cursor_pos); win = 2; }
            }
            
            // Toggle flag
            if (key == "f" or key == "F")
            {
                // If the board is still full ignore
                if (revealed_cells.empty()) continue;

                // If is already revealed
                if (exist(revealed_cells, cursor_pos)) continue;
                
                // Toggle flag off
                if (exist(flag_cells, cursor_pos))
                {
                    wipe(flag_cells, cursor_pos);
                    remaining_flags++;
                    continue;
                }
                
                // Toggle flag on
                if (remaining_flags > 0)
                {
                    flag_cells.push_back(cursor_pos);
                    remaining_flags--;
                }
                
                // Check win
                bool all_mines_flagged = true;
                for (auto &&mine : mine_cells)
                    if (!exist(flag_cells, mine)) { all_mines_flagged = false; break; }
                if (all_mines_flagged) win = 1;
            }
        }

        return GameResult::finished;
    }

public:

    int board_size_option =  0;
    int board_mines_count = 12;

    void play()
    {
        while (true)
        {
            board.clear();
            revealed_cells.clear();
            mine_cells.clear();
            flag_cells.clear();
            remaining_flags = 0;
            win = 0;

            switch (board_size_option)
            {
                case 0:  board_size =  8; break;
                case 1:  board_size = 10; break;
                case 2:  board_size = 12; break;
                default: board_size =  8; break;
            }

            cursor_pos = {int(board_size) / 2, int(board_size) / 2};

            const GameResult result = game_loop();
            if (result == GameResult::quit) return;
            if (result == GameResult::restart) continue;

            const int option = win == 2 ? explode() : win_game();
            switch (option)
            {
                case 0: continue;
                case 1: update_screen(); wait(); return;
                case 2: return;
                default: return;
            }
        }
    }
};

class wordle
{
    // Words

    std::vector<std::string> words_easy = {
        "able","acid","aged","also","area","army","away","baby","back","ball","band","bank","base","bath","bear",
        "beat","been","bell","best","bill","bird","blow","blue","boat","body","book","born","both","busy","call",
        "came","camp","card","care","case","cash","cast","cave","chat","city","club","coat","cold","come","cook",
        "cool","copy","cost","crew","dark","data","dawn","days","dead","deal","deep","deer","desk","dial","diet",
        "dirt","disk","does","done","door","down","draw","drop","drug","dust","each","east","easy","edge","else",
        "even","ever","face","fact","fair","fall","farm","fast","fate","fear","feel","feet","fell","felt","file",
        "fill","film","find","fire","fish","five","flat","flow","food","foot","form","four","free","from","fuel",
        "full","fund","game","gate","gave","gear","gift","girl","give","glad","goal","goes","gold","golf","gone",
        "good","gray","grew","grow","hair","half","hall","hand","hang","hard","have","head","hear","heat","held",
        "help","here","hero","high","hill","hint","hold","hole","home","hope","host","hour","huge","hung","hunt",
        "idea","into","iron","item","jack","join","jump","just","keen","keep","kept","kick","kind","king","knee",
        "knew","know","lake","land","lane","last","late","lead","left","less","life","lift","like","line","link",
        "list","live","load","lock","long","look","lost","love","luck","made","mail","main","make","many","mark",
        "meet","milk","mind","mine","miss","moon","more","most","move","much","must","name","near","neck","need",
        "news","next","nice","nine","none","nose","note","okay","once","only","open","oven","over","own","page",
        "paid","pair","park","part","pass","past","path","pick","pink","plan","play","plus","pool","poor","port",
        "post","pull","pure","push","race","rain","rare","read","real","rest","rice","rich","ride","ring","rise",
        "road","rock","role","room","rose","rule","safe","said","same","sang","save","seat","seed","seek","seem",
        "seen","self","sell","send","sent","ship","shoe","shop","shot","show","shut","sick","side","sign","sing",
        "site","size","skin","slow","snow","soft","soil","sold","some","song","soon","sort","star","stay","step",
        "stop","such","sure","take","tale","talk","tall","tank","tape","task","team","tell","tend","term","test",
        "text","than","that","them","then","they","thin","this","thus","time","tiny","told","took","tool","tour",
        "town","tree","trip","true","tune","turn","type","upon","used","user","very","view","vote","wait","wake",
        "walk","wall","want","warm","wash","wave","ways","weak","wear","week","well","went","were","west","what",
        "when","wide","wife","wild","will","wind","wine","wing","wise","wish","with","wood","word","wore","work",
        "yard","yeah","year","your","zero","zone"
    };

    std::vector<std::string> words_medium = {
        "account","achieve","address","airport","ancient","another","anxious","anybody","apology","approve",
        "arrange","article","attempt","attract","balance","balloon","battery","because","bedroom","between",
        "bicycle","blanket","breathe","brother","builder","button","camera","capital","captain","careful",
        "ceiling","century","certain","channel","chapter","charity","chicken","classic","climate","closely",
        "collect","comfort","command","company","compare","complex","concert","connect","contact","control",
        "cookie","corner","correct","cottage","country","courage","curtain","customer","dancing","danger",
        "decided","deliver","density","deposit","deserve","diamond","digital","dinner","disease","display",
        "distant","divided","drawing","eastern","economy","educate","emperor","employ","evening","examine",
        "example","excited","explore","factory","failure","fashion","feature","feeling","fiction","finally",
        "finance","finding","finger","flavor","flight","focused","foreign","forever","fortune","forward",
        "freedom","friendly","further","gallery","general","gesture","getting","glasses","grocery","growing",
        "healthy","hearing","helpful","highway","history","holiday","hundred","husband","illegal","imagine",
        "improve","include","initial","inquiry","instead","journey","justice","kitchen","leading","library",
        "limited","machine","manager","married","massive","meaning","measure","medical","meeting","mention",
        "message","million","mixture","monitor","morning","musical","mystery","natural","neither","nervous",
        "nothing","nuclear","numbers","obvious","officer","opinion","outside","package","painted","parking",
        "patient","pattern","payment","pending","percent","perfect","perhaps","picture","plastic","pleased",
        "popular","portion","poverty","present","prevent","primary","printer","problem","produce","product",
        "project","promise","protect","provide","purpose","quality","quarter","quickly","reading","reality",
        "receive","regular","related","release","remains","removed","replace","request","respect","respond",
        "routine","running","science","section","serious","service","several","shelter","silence","similar",
        "sitting","someone","station","storage","strange","student","subject","success","suggest","support",
        "suppose","surface","teacher","terrible","testing","theater","thought","through","together","tonight",
        "traffic","trouble","uniform","unknown","usually","various","vehicle","version","village","western",
        "whether","writing"
    };

    std::vector<std::string> words_hard = {
        "abandoned","abundance","accidental","advantage","afternoon","agreement","ambitious","amusement","animation",
        "apartment","apparatus","appliance","argument","arrangement","assistant","attention","authority","beautiful",
        "beginning","biography","blueprint","breathing","brilliant","candidate","celebrate","character","chemistry",
        "childhood","chocolate","classroom","colleague","community","complaint","component","conclusion","condition",
        "confidence","confusion","consequence","construction","continent","convention","conversation","corporate",
        "craftsman","dangerous","decoration","definition","delicious","democracy","departure","dependent","desperate",
        "development","dictionary","different","direction","discovery","discussion","distribute","education","effective",
        "electricity","emergency","emotional","emphasis","encounter","engineer","entertain","equipment","excellent",
        "exception","excitement","expensive","experiment","explosion","expression","fantastic","furniture","generosity",
        "government","graduation","happiness","important","incredible","independent","individual","influential","invention",
        "knowledge","landscape","leadership","lightning","literature","machinery","maintenance","measurement","mysterious",
        "necessary","neighborhood","operation","opportunity","organization","originality","passionate","performance",
        "photograph","physician","politician","population","possibility","practical","preparation","prescription",
        "president","privilege","profession","programming","pronunciation","psychology","reflection","relationship",
        "remarkable","reputation","restaurant","scientific","sensitive","significant","spectacular","strategic",
        "substantial","technology","temperature","traditional","tremendous","understanding","vegetarian","volunteer"
    };

    std::vector<std::string> words_impossible = {
        "acknowledgement","anthropologically","antidisestablishmentarianism","characteristically","circumstantiality",
        "compartmentalization","comprehensibility","contemporaneously","counterintelligence","counterproductive",
        "decriminalization","deinstitutionalize","disproportionately","electrocardiographic","electroencephalogram",
        "environmentalism","extraordinarily","extraterrestrial","fundamentalistically","heterogeneousness",
        "hypercommunication","incompatibility","incomprehensibility","indistinguishable","institutionalization",
        "intellectualization","intercontinental","interdisciplinary","intergovernmental","internationalization",
        "irreconcilability","mischaracterization","misinterpretation","misrepresentation","multidimensionality",
        "neuropsychological","nonconfrontational","noninstitutional","notwithstanding","overcompensation",
        "overgeneralization","overintellectualize","paleontological","parallelopipedon","pharmaceutical",
        "philosophically","photographically","photosynthesis","preauthorization","professionalization",
        "psychophysiology","radiotelegraphy","reconceptualization","representational","responsibilities",
        "semiautobiographical","telecommunication","thermodynamically","transformational","transubstantiation",
        "uncharacteristically","unconstitutionally","uncontrollability","unquestioningly","unsuccessfully"
    };

    // Variables

    std::string word;

    bool win;

    std::vector<std::string> attempts;
    int current_attempt;

    struct apparition
    {
        char letter;
        int  count;

        bool operator==(const apparition &other) const
        {
            return (letter == other.letter);
        }
    };

    std::vector<apparition> apparitions;

    struct char_status
    {
        char letter;
        int  status;

        bool operator==(const char_status &other) const
        {
            return (letter == other.letter);
        }
    };

    std::vector<std::vector<char_status>> attempts_status;

    // Functions

    void count_apparitions()
    {
        apparitions.clear();

        for (auto &&chr : word)
        {
            size_t i = exist_at(apparitions, apparition{chr});
            if (i != SIZE_MAX) apparitions[i].count++;
            else apparitions.push_back(apparition{chr, 1});
        }
    }

    void process_attempt()
    {
        std::vector<apparition> apparitions_left = apparitions;

        std::vector<char_status> attempt_status(
            word.size(),
            {' ', 0}
        );

        // First pass: green
        for (size_t i = 0; i < word.size(); ++i)
        {
            char chr = attempts[current_attempt][i];

            if (chr == word[i])
            {
                attempt_status[i] = {chr, 2};

                size_t j = exist_at(apparitions_left, apparition{chr});
                if (j != SIZE_MAX)
                    apparitions_left[j].count--;
            }
        }

        // Second pass: yellow / gray
        for (size_t i = 0; i < word.size(); ++i)
        {
            // Already green
            if (attempt_status[i].status == 2)
                continue;

            char chr = attempts[current_attempt][i];

            size_t j = exist_at(apparitions_left, apparition{chr});

            if (j != SIZE_MAX && apparitions_left[j].count > 0)
            {
                attempt_status[i] = {chr, 1};
                apparitions_left[j].count--;
            }
            else
            {
                attempt_status[i] = {chr, 0};
            }
        }

        attempts_status[current_attempt] = attempt_status;

        // Check win
        if (attempts[current_attempt] == word) win = true;
    }

    void update_screen()
    {
        // Title
        print_title(title_wordle, color_wordle);

        // Prerender
        std::vector<std::string> attempts_renders = { "","","","","","" };

        for (size_t line = 0; line < attempts.size(); ++line)
        {
            const std::vector<char_status>& current_attempt_status = attempts_status[line];
            std::string row_render = " ";

            for (auto &&cell : current_attempt_status)
            {
                // Cell empty
                if (cell.letter == ' ')
                {
                    row_render += c_gray + underline + " " + f_clear + " ";
                    continue;
                }

                std::string color_code = "";
                if (cell.status == 0) color_code = c_gray;
                if (cell.status == 1) color_code = c_yellow;
                if (cell.status == 2) color_code = c_green;

                row_render += color_code + underline + string_upper(std::string(1, cell.letter)) + f_clear + " ";
            }

            attempts_renders[line] = row_render;
        }
        
        // Draw attempts
        for (auto &&attempt : attempts_renders)
            std::cout << center(attempt) << "\n\n";
        cursor_move_up(1);
        
        // Print footer
        std::string footer = c_green + "■ ✔" + c_gray + " | " + c_yellow + "■ ⬌" + c_gray + " | " + c_gray + "■ ✖" + f_clear;
        print_footer(footer);
        
        std::cout << std::flush;
    }

    int losse_game()
    {
        update_screen();
        sleep(1000);

        std::string message = bold + c_red + "You ran out of attempts!" + f_clear +
                        "\nThe word was: " + c_green + string_upper(word) + f_clear;

        return dialog(message, {"Play again","View board","Back"});
    }

    int win_game()
    {
        update_screen();
        sleep(1000);

        std::string message = bold + c_green + "You guessed the word!" + f_clear;

        return dialog(message, {"Play again","View board","Back"});
    }

    void game_loop()
    {
        while (current_attempt < 6 and win == false)
        {
            // Get input
            while (true)
            {
                update_screen();

                std::string user_input = input(
                    "Attempt " + std::to_string(current_attempt + 1) + ": ",
                    c_gray, c_green,
                    string_length(word),
                    terminal_width,
                    false, true, ""
                );

                // To lower
                user_input = string_lower(user_input);

                // Avoid empty input
                if (string_length(user_input) != string_length(word))
                {
                    std::string message = c_red + bold + "ERROR" + f_clear +
                                    "\nEnter a complete word";

                    dialog(message);
                    continue;
                }

                // Avoid non-alphabetic chars
                bool valid = true;
                for (auto &&chr : user_input)
                {
                    switch (chr)
                    {
                        case 'a': case 'b': case 'c': case 'd':
                        case 'e': case 'f': case 'g': case 'h':
                        case 'i': case 'j': case 'k': case 'l':
                        case 'm': case 'n': case 'o': case 'p':
                        case 'q': case 'r': case 's': case 't':
                        case 'u': case 'v': case 'w': case 'x':
                        case 'y': case 'z': break;

                        default: valid = false; break;
                    }
                }
                if (!valid)
                {
                    std::string message = c_red + bold + "ERROR" + f_clear +
                                    "\nUse only letters from the " + c_orange + "English alphabet" + c_red +
                                    "\n(no spaces, numbers, accents, etc.)" + f_clear;

                    dialog(message);
                    continue;
                }

                attempts[current_attempt] = user_input;
                break;
            }

            process_attempt();

            current_attempt++;
        }
    }
    
public:
    int game_mode  = 0;
    int difficulty = 0;
    std::string custom_word = "";

    void play()
    {
        // Restart variables
        win = false;

        if (game_mode == 0)
        {
            switch (difficulty)
            {
                case 0:  word =       words_easy[irandom(0, words_easy.size()-1      )]; break;
                case 1:  word =     words_medium[irandom(0, words_medium.size()-1    )]; break;
                case 2:  word =       words_hard[irandom(0, words_hard.size()-1      )]; break;
                case 3:  word = words_impossible[irandom(0, words_impossible.size()-1)]; break;
                default: word = words_impossible[irandom(0, words_impossible.size()-1)]; break;
            }
        }
        if (game_mode == 1)
        {
            // Avoid empty input
            if (custom_word == "")
            {
                std::string message = c_red + bold + "ERROR" + f_clear +
                                "\nEnter a word to play";

                dialog(message);
                return;
            }

            // Avoid non-alphabetic chars
            bool valid = true;
            for (auto &&chr : custom_word)
            {
                switch (chr)
                {
                    case 'a': case 'b': case 'c': case 'd':
                    case 'e': case 'f': case 'g': case 'h':
                    case 'i': case 'j': case 'k': case 'l':
                    case 'm': case 'n': case 'o': case 'p':
                    case 'q': case 'r': case 's': case 't':
                    case 'u': case 'v': case 'w': case 'x':
                    case 'y': case 'z': break;

                    default: valid = false; break;
                }
            }
            if (!valid)
            {
                std::string message = c_red + bold + "ERROR" + f_clear +
                                "\nUse only letters from the " + c_orange + "English alphabet" + c_red +
                                "\n(no spaces, numbers, accents, etc.)" + f_clear;

                dialog(message);
                return;
            }

            // Avoid long input
            if (string_length(custom_word) > 30)
            {
                std::string message = c_red + bold + "ERROR" + f_clear +
                                "\nMaximum " + c_orange + "30" + f_clear + " characters";

                dialog(message);
                return;
            }

            // Proceed
            word = custom_word;
        }

        attempts = {
            string_repeat(" ", string_length(word)),
            string_repeat(" ", string_length(word)),
            string_repeat(" ", string_length(word)),
            string_repeat(" ", string_length(word)),
            string_repeat(" ", string_length(word)),
            string_repeat(" ", string_length(word))
        };

        current_attempt = 0;

        char_status blank = {' ', 0};
        std::vector<char_status> attempt_status;
        for (size_t i = 0; i < string_length(word); i++) attempt_status.push_back(blank);
        attempts_status = {
            attempt_status,
            attempt_status,
            attempt_status,
            attempt_status,
            attempt_status,
            attempt_status
        };

        count_apparitions();

        // Game loop
        game_loop();

        int option;

        if (win == false) option = losse_game();
        if (win == true ) option = win_game();
        
        switch (option)
        {
            case 0: play(); return; break;
            case 1: update_screen(); wait(); return; break;
            case 2: return; break;
        }
    }
};

// Create the game objects, configure their menus, and start the menu manager.
void run_game_collection()
{
    // Keep each game's settings and state alive while its menu is running.
    tictactoe game_tictactoe;
    minesweeper game_minesweeper;
    wordle game_wordle;

    // Main menu: open a game submenu or exit the application.
    menu_main.set_ascii_title(title_main);
    menu_main.set_color(color_main);
    menu_main.add_button("Tic-Tac-Toe", [&]() {
        manager.submenu_goto(menu_tictactoe);
    });
    menu_main.add_button("Minesweeper", [&]() {
        manager.submenu_goto(menu_minesweeper);
    });
    menu_main.add_button("Wordle", [&]() {
        manager.submenu_goto(menu_wordle);
    });
    menu_main.add_br();
    menu_main.add_separator();
    menu_main.add_button("Exit", [&]() {
        if (exit_program_confirm())
            manager.submenu_return();
    });

    // Tic-Tac-Toe: choose a mode and, when playing against the machine, a difficulty.
    menu_tictactoe.set_ascii_title(title_tictactoe);
    menu_tictactoe.set_color(color_tictactoe);
    menu_tictactoe.add_button("Play", [&]() {
        game_tictactoe.play();
    });
    menu_tictactoe.add_br();
    menu_tictactoe.add_separator();
    menu_tictactoe.add_label("Settings", true);
    menu_tictactoe.add_br();
    menu_tictactoe.add_selector(
        "Mode",
        {"Player vs Player", "Player vs Machine"},
        game_tictactoe.game_mode
    );
    menu_tictactoe.add_selector(
        "Difficulty",
        {"Easy", "Hard", "Impossible"},
        game_tictactoe.difficulty
    );
    menu_tictactoe.add_br();
    menu_tictactoe.add_separator();
    menu_tictactoe.add_button("Back", [&]() {
        manager.submenu_return();
    });

    // Minesweeper: configure the board dimensions and number of mines.
    menu_minesweeper.set_ascii_title(title_minesweeper);
    menu_minesweeper.set_color(color_minesweeper);
    menu_minesweeper.add_button("Play", [&]() {
        game_minesweeper.play();
    });
    menu_minesweeper.add_br();
    menu_minesweeper.add_separator();
    menu_minesweeper.add_label("Settings", true);
    menu_minesweeper.add_br();
    menu_minesweeper.add_selector(
        "Size",
        {"8x8", "10x10", "12x12"},
        game_minesweeper.board_size_option
    );
    menu_minesweeper.add_slider(
        "Mines",
        game_minesweeper.board_mines_count,
        1,
        50
    );
    menu_minesweeper.add_br();
    menu_minesweeper.add_separator();
    menu_minesweeper.add_button("Back", [&]() {
        manager.submenu_return();
    });

    // Wordle: select a random or custom word and a difficulty.
    menu_wordle.set_ascii_title(title_wordle);
    menu_wordle.set_color(color_wordle);
    menu_wordle.add_button("Play", [&]() {
        game_wordle.play();
    });
    menu_wordle.add_br();
    menu_wordle.add_separator();
    menu_wordle.add_label("Settings", true);
    menu_wordle.add_br();
    menu_wordle.add_selector(
        "Mode",
        {"Random word", "Custom word"},
        game_wordle.game_mode
    );
    menu_wordle.add_selector(
        "Difficulty",
        {"Easy", "Medium", "Hard", "Impossible"},
        game_wordle.difficulty
    );
    menu_wordle.add_textfield("Custom word", game_wordle.custom_word, true);
    menu_wordle.add_br();
    menu_wordle.add_separator();
    menu_wordle.add_button("Back", [&]() {
        manager.submenu_return();
    });

    // Run the menu tree; main() restores the terminal after it returns.
    manager.exec(menu_main);
}