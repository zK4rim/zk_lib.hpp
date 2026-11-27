#pragma once

#pragma region Multiplatform

//
//  LIBRARIES & MULTIPLATFORM SETTINGS
//

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>
#include <ctime>
#include <vector>
#include <stack>
#include <chrono>

namespace zk
{

class out_stream
{
    std::ostringstream frame_current, frame_previous;

public:
    /*
    @brief Appends a value to the frame being built
    */
    template<typename T>
    out_stream& operator<<(const T& value)
    {
        frame_current << value;
        return *this;
    }

    out_stream& operator<<(std::ostream& (*manipulator)(std::ostream&))
    {
        manipulator(frame_current);
        return *this;
    }

    out_stream& operator<<(std::ios_base& (*manipulator)(std::ios_base&))
    {
        manipulator(frame_current);
        return *this;
    }

    /*
    @brief Appends text to the frame being built
    */
    template<typename T>
    void print(const T& text) { frame_current << text; }

    /*
    @brief Returns the stream used to build the current frame
    */
    std::ostream& stream() { return frame_current; }

    /*
    @brief Discards the frame being built without changing the last displayed frame
    */
    void clear_current()
    {
        frame_current.str("");
        frame_current.clear();
    }

    /*
    @brief Displays the current frame and optionally remembers it for later restoration
    */
    void show(bool remember = true)
    {
        const std::string frame = frame_current.str();
        std::cout << frame << std::flush;
        if (remember) frame_previous.str(frame);
        clear_current();
    }

    /*
    @brief Reprints the last remembered frame
    */
    void show_previous()
    {
        std::cout << frame_previous.str() << std::flush;
    }

    /*
    @brief Clears both the current and remembered frames
    */
    void clear()
    {
        clear_current();
        frame_previous.str("");
        frame_previous.clear();
    }
};

inline out_stream out;

/*
@brief A simple timer class that checks if a specified duration has elapsed since its creation or last reset
*/
class timer
{
private:
    std::chrono::milliseconds duration;
    std::chrono::steady_clock::time_point start;

public:
    /*
    @brief Constructs a timer with a specified duration
    @param ms The duration in milliseconds
    */
    timer(int ms)
    {
        this->duration = std::chrono::milliseconds(ms);
        start = std::chrono::steady_clock::now();
    }

    /*
    @brief Checks if the specified duration has elapsed since the timer was created or last reset
    */
    bool time_reached()
    {
        auto now = std::chrono::steady_clock::now();

        if ((now - start) >= duration)
        {
            start += duration;
            return true;
        }
        
        return false;
    }

    /*
    @brief Resets the timer
    */
    void reset()
    {
        start = std::chrono::steady_clock::now();
    }
};

}

// Multiplatform
#ifdef _WIN32

    // Windows

    #include <windows.h>
    #include <conio.h>

    namespace zk {

    /*
    @brief Initializes the terminal for raw input mode and sets up the environment for the program
    */
    void init_program()
    {
        // Enter alternate screen mode
        out << "\x1B[?1049h";

        // Hide cursor
        out << "\x1B[?25l";

        // Allow Unicode
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        out.show();

        // Set random seed
        srand(time(0));
    }

    /*
    @brief Exits the program and restores the terminal to its original state
    */
    void exit_program()
    {
        // Exit alternate screen mode
        out << "\x1B[?1049l";

        // Show cursor
        out << "\x1B[?25h";
        out.show();
    }

    /*
    @brief Pauses the execution of the program for a specified number of milliseconds
    @param ms The number of milliseconds to sleep
    */
    void sleep(int ms) { Sleep(ms); }

    /*
    @brief Gets the width of the terminal in characters
    @return The width of the terminal
    */
    size_t get_terminal_width()
    {
        // Get terminal data
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

        // Return terminal width
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }

    /*
    @brief Gets the height of the terminal in characters
    @return The height of the terminal
    */
    size_t get_terminal_height()
    {
        // Get terminal data
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

        // Return terminal height
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

    // Keys

    inline const std::string key_up    = "\xE0\x48";
    inline const std::string key_down  = "\xE0\x50";
    inline const std::string key_right = "\xE0\x4D";
    inline const std::string key_left  = "\xE0\x4B";

    inline const std::string key_intro = "\r";
    inline const std::string key_esc   = "\x1B";

    /*
    @brief Gets a key press from the user
    @param blocking Whether to block until a key is pressed
    @return The key pressed, or an empty string if no key was pressed
    */
    std::string get_key(bool blocking = true)
    {
        // Non-blocking mode
        if (!blocking && !_kbhit()) return "";

        int chr = _getch();

        // Escape sequence detected
        if (chr == 0 || chr == 224)
        {
            std::string seq;
            seq += char(chr);
            seq += char(_getch());

            return seq;
        }
        
        // Return char
        return std::string(1, char(chr));
    }

    }

#else

    // Linux / MacOS

    #include <sys/select.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
    #include <thread>

    namespace zk {

    inline struct termios orig_termios;
    inline bool raw_mode_active = false;

    /*
    @brief Initializes the terminal for raw input mode and sets up the environment for the program
    */
    void init_program()
    {
        // Save terminal config and deactivate eco buffer
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        raw_mode_active = true;

        // Enter alternate screen mode
        out << "\x1B[?1049h";

        // Hide cursor
        out << "\x1B[?25l";
        out.show();

        // Set random seed
        srand(time(0));
    }

    /*
    @brief Exits the program and restores the terminal to its original state
    */
    void exit_program()
    {
        // Restore terminal config
        if (raw_mode_active)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
            raw_mode_active = false;
        }

        // Exit alternate screen mode
        out << "\x1B[?1049l";

        // Show cursor
        out << "\x1B[?25h";
        out.show();
    }

    /*
    @brief Pauses the execution of the program for a specified number of milliseconds
    @param ms The number of milliseconds to sleep
    */
    void sleep(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

    /*
    @brief Gets the width of the terminal in characters
    @return The width of the terminal
    */
    size_t get_terminal_width()
    {
        // Get terminal data
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        // Return terminal width
        return w.ws_col;
    }

    /*
    @brief Gets the height of the terminal in characters
    @return The height of the terminal
    */
    size_t get_terminal_height()
    {
        // Get terminal data
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        // Return terminal height
        return w.ws_row;
    }

    /*
    @brief Checks if there is input available on the standard input
    @param timeout_ms The timeout in milliseconds
    @return True if input is available, false otherwise
    */
    bool has_input(int timeout_ms = 0)
    {
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
    }

    /*
    @brief Gets a single character from the standard input
    @return The character read, or -1 if an error occurred
    */
    int _getch()
    {
        if (raw_mode_active)
        {
            unsigned char c;
            if (read(STDIN_FILENO, &c, 1) > 0) return c;
            return -1;
        }

        // Fallback
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        unsigned char c;
        int res = read(STDIN_FILENO, &c, 1);

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return (res > 0) ? c : -1;
    }

    // Keys

    inline const std::string key_up    = "\x1B[A";
    inline const std::string key_down  = "\x1B[B";
    inline const std::string key_right = "\x1B[C";
    inline const std::string key_left  = "\x1B[D";

    inline const std::string key_intro = "\n";
    inline const std::string key_esc   = "\x1B";

    /*
    @brief Gets a key press from the user
    @param blocking Whether to block until a key is pressed
    @return The key pressed, or an empty string if no key was pressed
    */
    std::string get_key(bool blocking = true)
    {
        if (!blocking && !has_input(0)) return "";

        int chr = _getch();
        if (chr == -1) return "";

        // Escape sequence
        if (chr == 27)
        {
            std::string seq;
            seq += char(chr);

            // Wait 10 ms for second byte
            if (has_input(10))
            {
                seq += char(_getch());
                // Wait 10 ms for third byte
                if (has_input(10))
                {
                    seq += char(_getch());
                }
            }
            return seq;
        }

        return std::string(1, char(chr));
    }

    }

#endif

#pragma endregion Multiplatform

namespace zk
{

#pragma region Utilities

//
//  FUNCTIONS & VARIABLES
//

/*
@brief Represents a 2D vector with integer coordinates
*/
struct Vector2D
{
    int x, y;

    bool operator==(const Vector2D& other) const
    {
        return (x == other.x && y == other.y);
    }
};

inline size_t terminal_width  = get_terminal_width();
inline size_t terminal_height = get_terminal_height();

// Erases the screen
void clear() { out << "\x1B[2J\x1B[H"; }

// Clear the current line and move the cursor to the beginning of the line
void clear_line() { out << "\x1B[2K\r"; }

// Save the cursor position on the screen
void cursor_save_pos() { out << "\x1B[s"; }

// Move the cursor to the saved position
void cursor_load_pos() { out << "\x1B[u"; }

// Move the cursor x lines up
void cursor_move_up(size_t lines) { out << "\x1B[" << std::to_string(lines) << "A"; }

// Move the cursor x lines down
void cursor_move_down(size_t lines) { out << "\x1B[" << std::to_string(lines) << "B"; }

// Move the cursor x columns to the right
void cursor_move_right(size_t columns) { out << "\x1B[" << std::to_string(columns) << "C"; }

// Move the cursor x columns to the left
void cursor_move_left(size_t columns) { out << "\x1B[" << std::to_string(columns) << "D"; }

// Move the cursor to a specific position
void cursor_move_to(size_t x, size_t y) { out << "\x1B[" << std::to_string(y) << ";" << std::to_string(x) << "H"; }

// Move the cursor to the beginning of the line
void cursor_to_begin_line() { out << "\r"; }

// Makes the cursor invisible
void hide_cursor() { out << "\x1B[?25l"; }

// Makes the cursor visible
void show_cursor() { out << "\x1B[?25h"; }

/*
@brief Limit the value within the min-max range

It doesn't return nothing, instead, it changes the variable directly

@param value Variable to clamp
@param min Minimum value possible
@param max Maximum value possible
*/
void clamp(int &value, int min, int max)
{
    if (value < min) { value = min; }
    if (value > max) { value = max; }
}

/*
@brief Limit the value withing the min-max range but the value comes out the other side

It doesn't return nothing, instead, it changes the variable directly

@param value Variable to clamp
@param min Minimum value possible
@param max Maximum value possible
*/
void wrap(int &value, int min, int max)
{
    int range = max - min + 1;
    value = min + ((value - min) % range + range) % range;
}

/*
@brief Checks if the value is in between min and max

@tparam T Any numeric type (int, float, size_t, etc.)
@param value Variable to check
@param min Minimum value acceptable
@param max Maximum value acceptable

@return true if the value is in range or false if is not
*/
template<typename T>
bool in_range(const T &value, T min, T max) { return (value >= min && value <= max); }

/*
@brief Generates a random integer between a and b, inclusive

@param a Minimum value possible
@param b Maximum value possible

@return Random integer
*/
int irandom(int a, int b)
{
    int range = b - a + 1;
    return a + (rand() % range);
}

/*
@brief Set a custom color for text

@param R Amount of red (0-255)
@param G Amount of green (0-255)
@param B Amount of blue (0-255)
@param bg Apply the color to the background (Default: false)

@return String with the ANSI code of that color
*/
std::string color_rgb(int R, int G, int B, bool bg = false)
{
    // Clamp values
    clamp(R, 0, 255); clamp(G, 0, 255); clamp(B, 0, 255);

    // Format color
    std::string formatted_color = bg ? "\x1B[48;2;" : "\x1B[38;2;";
    formatted_color += std::to_string(R) + ";";
    formatted_color += std::to_string(G) + ";";
    formatted_color += std::to_string(B) + "m";

    // Return format
    return formatted_color;
}

inline const std::string f_clear     = "\x1B[0m";

inline const std::string bold        = "\x1B[1m";
inline const std::string underline   = "\x1B[4m";

inline const std::string c_white     = color_rgb(255, 255, 255);
inline const std::string c_gray      = color_rgb(100, 100, 100);
inline const std::string c_black     = color_rgb(  0,   0,   0);

inline const std::string c_green     = color_rgb( 92, 237, 124);
inline const std::string c_cyan      = color_rgb( 92, 237, 210);
inline const std::string c_blue      = color_rgb( 92, 152, 237);
inline const std::string c_purple    = color_rgb(121,  92, 237);
inline const std::string c_pink      = color_rgb(230,  92, 237);
inline const std::string c_red       = color_rgb(237,  92, 121);
inline const std::string c_orange    = color_rgb(237, 133,  92);
inline const std::string c_yellow    = color_rgb(237, 215,  92);
inline const std::string c_lime      = color_rgb(191, 237,  92);

inline const std::string c_bg_white  = color_rgb(255, 255, 255, true);
inline const std::string c_bg_gray   = color_rgb(100, 100, 100, true);
inline const std::string c_bg_black  = color_rgb(  0,   0,   0, true);

inline const std::string c_bg_green  = color_rgb( 92, 237, 124, true);
inline const std::string c_bg_cyan   = color_rgb( 92, 237, 210, true);
inline const std::string c_bg_blue   = color_rgb( 92, 152, 237, true);
inline const std::string c_bg_purple = color_rgb(121,  92, 237, true);
inline const std::string c_bg_pink   = color_rgb(230,  92, 237, true);
inline const std::string c_bg_red    = color_rgb(237,  92, 121, true);
inline const std::string c_bg_orange = color_rgb(237, 133,  92, true);
inline const std::string c_bg_yellow = color_rgb(237, 215,  92, true);
inline const std::string c_bg_lime   = color_rgb(191, 237,  92, true);

/*
@brief Get the number of characters that contains a string (supporting UTF-8)

@param s String to count
@param ignore_ansi Ignore color codes (Default: true)

@return Number of chars in the string
*/
size_t string_length(const std::string &s, bool ignore_ansi = true)
{
    size_t count = 0;
    bool in_ansi = false;

    for (unsigned char c : s)
    {
        // Ignore ANSI sequences
        if (ignore_ansi)
        {
            // Escape sequence detected
            if (c == 0x1B)
            {
                in_ansi = true;
                continue;
            }
            
            // We are inside the ANSI
            if (in_ansi)
            {
                // Found ANSI end (m)
                if (c == 'm') in_ansi = false;

                // Ignore char
                continue;
            }
        }

        // Increase char count
        if ((c & 0xC0) != 0x80) ++count;
    }

    return count;
}

/*
@brief Splits a string using the specified delimiter

@param text String to split
@param delimiter Char used to separate the string

@return A vector containing the substrings
*/
std::vector<std::string> string_split(const std::string &text, const char delimiter)
{
    std::vector<std::string> data;
    std::string current_buffer = "";

    // Iterate through all chars in the string
    for (const char& chr : text)
    {
        // If the delimiter was found, append buffer to the list and clear the buffer 
        if (chr == delimiter)
        {
            // Skip if there are no chars
            if (current_buffer == "") continue;

            data.push_back(current_buffer);
            current_buffer = "";
            continue;
        }
        
        // Append chat to buffer
        current_buffer += chr;
    }

    // Add spare string
    if (!current_buffer.empty()) data.push_back(current_buffer);
    
    // Return list
    return data;
}

/*
@brief Repeats a substring a specified number of times

@param substring String to repeat
@param n Number of repetitions

@return A new string containing the repeated substring
*/
std::string string_repeat(std::string substring, size_t n)
{
    std::string new_string = "";
    for (size_t i = 0; i < n; i++) new_string += substring;
    return new_string;
}

/*
@brief Centers a string with a specified width

ANSI sequences are ignored 

@param text String to center
@param width Target width (Default: current terminal width)

@return The centered string
*/
std::string center(std::string text, size_t width = terminal_width)
{
    // Check lenght
    if (string_length(text) >= width) return text;

    // Calculate spaces
    size_t space_count = (width / 2) - (string_length(text) / 2);
    
    // Return space + text
    return string_repeat(" ", space_count) + text;
}

/*
@brief Checks if a char exist in a string

@param chr Char to search
@param in String to search within

@return true if the char is found, otherwise false
*/
bool char_in(char chr, std::string in)
{
    bool contains = false;

    // Iterate throught each char in "in" looking for "chr"
    for (auto &&c : in)
        if (c == chr) { contains = true; break; }
    
    return contains;
}

/*
@brief Clears the screen and prints a formatted title

@param lines Title lines to display
@param color ANSI color to apply
@param centered If the title should be centered in the terminal
@param draw_separator If should draw a separator below the title
*/
void print_title(std::vector<std::string> &lines, std::string color = "", bool centered = true, bool draw_separator = true)
{
    clear();

    std::string separator = string_repeat("─", string_length(lines[0]));

    for (auto &&line : lines)
        out << color << (centered ? center(line) : line) << f_clear << '\n';
    
    if (draw_separator)
        out << (centered ? center(separator) : separator) << "\n\n";
    
    out.show();
}

/*
@brief Prints a centered footer at the bottom of the current output

Supports multiple lines separated by \\n

@param str Footer text
*/
void print_footer(std::string str)
{
    std::vector<std::string> lines = string_split(str, '\n');

    std::string separator = string_repeat("─", string_length(str) + 6);
    out << "\n" << center(separator) << "\n";

    for (auto &&line : lines)
        out << center(line) << "\n";
    
    out.show();
}

/*
@brief Converts a number into seven-segment Unicode

@tparam T Any numeric type (int, float, size_t, etc.)
@param n Number to convert

@return A string containing the stylized number
*/
template<typename T>
std::string fancy_number(T n)
{
    std::string n_string = std::to_string(n);
    std::string fancy_n  = "";
    
    for (auto &&chr : n_string)
        switch (chr)
        {
            case '0': fancy_n += "🯰"; break;
            case '1': fancy_n += "🯱"; break;
            case '2': fancy_n += "🯲"; break;
            case '3': fancy_n += "🯳"; break;
            case '4': fancy_n += "🯴"; break;
            case '5': fancy_n += "🯵"; break;
            case '6': fancy_n += "🯶"; break;
            case '7': fancy_n += "🯷"; break;
            case '8': fancy_n += "🯸"; break;
            case '9': fancy_n += "🯹"; break;
            case '.': fancy_n += "."; break;
            case '-': fancy_n += "-"; break;
            default : fancy_n += chr; break;
        }
    
    return fancy_n;
}

/*
@brief Converts a string to lowercase

@param str String to convert
@return A lowercase copy of the input string
*/
std::string string_lower(const std::string &str)
{
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    return lower_str;
}

/*
@brief Converts a string to uppercase

@param str String to convert

@return A uppercase copy of the input string
*/
std::string string_upper(const std::string &str)
{
    std::string upper_str = str;
    std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);
    return upper_str;
}

/*
@brief Returns the last UTF-8 character in a string

@param s UTF-8 string

@return The last UTF-8 char or an empty string if the input is empty
*/
static std::string utf8_last_char(const std::string &s)
{
    if (s.empty()) return "";
    int i = (int)s.length() - 1;
    while (i > 0 && ((unsigned char)s[i] & 0xC0) == 0x80) --i;
    return s.substr(i);
}

/*
@brief Removes the last UTF-8 char from a string

@param s UTF-8 string to modify
*/
static void utf8_pop_back(std::string &s)
{
    if (s.empty()) return;
    int i = (int)s.length() - 1;
    while (i > 0 && ((unsigned char)s[i] & 0xC0) == 0x80) --i;
    s.erase(i);
}

/*
@brief Removes and returns the first UTF-8 char from a string

@param s UTF-8 string to modify

@return The removed UTF-8 char, or an empty string if the input is empty
*/
static std::string utf8_pop_front(std::string &s)
{
    if (s.empty()) return "";
    unsigned char c = (unsigned char)s[0];
    size_t len = 1;
    if ((c & 0x80) == 0x00) len = 1;
    else if ((c & 0xE0) == 0xC0) len = 2;
    else if ((c & 0xF0) == 0xE0) len = 3;
    else if ((c & 0xF8) == 0xF0) len = 4;
    if (len > s.size()) { std::string t = s; s.clear(); return t; }
    std::string first = s.substr(0, len);
    s.erase(0, len);
    return first;
}

/*
@brief Returns a substring of a UTF-8 string, starting at a specified character index and with a specified length
@param text UTF-8 string to slice
@param start Starting character index (0-based)
@param length Number of characters to include in the slice
@return A new string containing the specified slice of the input string
*/
static std::string utf8_slice(std::string text, size_t start, size_t length)
{
    for (size_t i = 0; i < start && !text.empty(); i++) utf8_pop_front(text);

    std::string result;
    for (size_t i = 0; i < length && !text.empty(); i++)
        result += utf8_pop_front(text);

    return result;
}

/*
@brief Waits for the user to press any key

Displays a centered message at the bottom of the terminal and
blocks the execution until a key is pressed

@param message Message displayed to the user
*/
void wait(std::string message = "Press any key to continue")
{
    cursor_move_to(1, terminal_height);

    int remaining = std::max(0, int(terminal_width) - int(string_length(message)));
    int left = remaining / 2;
    int right = remaining - left;

    out << color_rgb(255, 255, 255, true) << c_black << bold
        << string_repeat(" ", left)
        << message
        << string_repeat(" ", right)
        << f_clear;
    out.show();

    _getch();
    return;
}

/*
@brief Draws a dialog frame with the specified content and options
@param lines Lines of text to display inside the dialog
@param options List of selectable options to display at the bottom of the dialog
@param dialog_width Width of the dialog in characters
@param selected_option Index of the currently selected option
@param loading_mode If true, displays a loading animation instead of options
@param loading_figure The character to use for the loading animation
@param show_input_cursor If true, shows a cursor for text input
@param input_cursor_offset The offset of the input cursor from the left edge of the dialog
*/
static void draw_dialog_frame(
    const std::vector<std::string> &lines,
    const std::vector<std::string> &options,
    size_t dialog_width,
    int selected_option,
    bool loading_mode = false,
    const std::string &loading_figure = "",
    bool show_input_cursor = false,
    size_t input_cursor_offset = 0
)
{
    size_t dialog_height = loading_mode
        ? lines.size()
        : lines.size() + options.size() + 1;

    std::string draw_top = string_repeat("━", dialog_width + 2); draw_top = " ┏" + draw_top + "┓  ";
    std::string draw_mid = string_repeat(" ", dialog_width + 2); draw_mid = " ┃" + draw_mid + "┃" + c_gray + "░ " + f_clear;
    std::string draw_bot = string_repeat("━", dialog_width + 2); draw_bot = " ┗" + draw_bot + "┛" + c_gray + "░ " + f_clear;
    std::string draw_shw = string_repeat("░", dialog_width + 4); draw_shw = c_gray + "  " + draw_shw + " " + f_clear;

    cursor_move_to((terminal_width / 2) - (dialog_width / 2) - 2, (terminal_height / 2) - (dialog_height / 2) - 3);
    cursor_save_pos();

    out << draw_top << '\n';

    for (size_t i = 0; i < dialog_height; i++)
    {
        cursor_load_pos(); cursor_move_down(i + 1);
        out << draw_mid << '\n';
    }

    cursor_load_pos(); cursor_move_down(dialog_height + 1);
    out << draw_bot << '\n';
    cursor_load_pos(); cursor_move_down(dialog_height + 2);
    out << draw_shw << '\n';

    cursor_load_pos();
    for (auto &&line : lines)
    {
        cursor_move_down(1); cursor_to_begin_line();
        cursor_move_right((terminal_width / 2) - (dialog_width / 2));

        if (loading_mode)
            out << c_white << loading_figure << " " << c_gray;

        out << line << (loading_mode ? f_clear : "");
    }

    if (!loading_mode)
    {
        cursor_move_down(1);
        size_t i = 0;
        for (auto &&option : options)
        {
            cursor_move_down(1); cursor_to_begin_line();

            size_t option_width  = string_length(option);
            size_t dialog_start  = (terminal_width / 2) - (dialog_width / 2);
            size_t center_offset = (dialog_width - option_width) / 2;

            cursor_move_right(dialog_start + center_offset);
            out << (selected_option == int(i) ? ("\x1B[4m" + option + f_clear) : option);

            i++;
        }
    }

    if (show_input_cursor)
    {
        cursor_load_pos();
        cursor_move_down(1); cursor_to_begin_line();
        cursor_move_right((terminal_width / 2) - (dialog_width / 2));
        cursor_move_right(input_cursor_offset);
    }
}

/*
@brief Displays an interactive dialog box

Can operate as a selection dialog or a loading indicator

@param message Message displayed inside the dialog. Accepts multiple lines with '\\n'
@param options List of selectable options
@param loading_mode If should display a loading animation instead of options (DECORATIVE ONLY)
@param loading_sec Duration of the loading animation in seconds

@return The index of the selected option. Returns 0 in loading mode
*/
int dialog(
    std::string message,
    std::vector<std::string> options = {"Ok"},
    bool loading_mode = false,
    size_t loading_sec = 0,
    bool esc_can_close = false
) {
    // Update terminal size

    terminal_height = get_terminal_height();
    terminal_width  = get_terminal_width();

    // Loading mode

    const int SPEED_MS = 150;
    size_t elapsed_ms  = 0;
    int current_figure = 0;
    std::vector<std::string> figures;
    figures.push_back("𜹥");
    figures.push_back("𜹗");
    figures.push_back("𜹛");
    figures.push_back("𜹺");
    figures.push_back("𜺈");
    figures.push_back("𜺄");

    // Calculate dialog window width

    std::vector<std::string> lines = string_split(message, '\n');

    size_t dialog_width = 0;
    for (auto &&line : lines)     if (string_length(line)   > dialog_width) dialog_width = string_length(line);
    for (auto &&option : options) if (string_length(option) > dialog_width) dialog_width = string_length(option);

    if (loading_mode) dialog_width += 2;

    int selected_option = 0;

    // Render loop

    while (true)
    {
        draw_dialog_frame(lines, options, dialog_width, selected_option, loading_mode, figures[current_figure]);

        // Loading mode

        if (loading_mode)
        {
            // Check time

            if (elapsed_ms >= (loading_sec * 1000))
            {
                out.show_previous();
                return 0;
            }

            // Flush

            out.show(false);
            
            // Wait time

            sleep(SPEED_MS);

            // Increase time

            elapsed_ms += SPEED_MS;

            // Change figure

            current_figure++;
            wrap(current_figure, 0, 5);

            // Continue to the next iteration

            continue;
        }
        
        out.show(false);

        // Get key

        std::string key = get_key();

        // Change option

        if (key == key_down) selected_option++;
        if (key == key_up)   selected_option--;
        wrap(selected_option, 0, int(options.size() - 1));

        // ESC pressed

        if (options.size() == 1) esc_can_close = true;
        if (key == key_esc && esc_can_close)
        {
            out.show_previous();
            return -1;
        }

        // Return option

        if (key == key_intro)
        {
            out.show_previous();
            return selected_option;
        }
    }
}

/*
@brief Displays a confirmation dialog

Shows a Yes/No prompt and returns the user's choice

@param message Confirmation message

@return true if the user selects "Yes", otherwise false
*/
bool confirm(std::string message)
{
    std::vector<std::string> options;
    options.push_back(c_lime + "Yes" + f_clear);
    options.push_back(c_red + "No" + f_clear);

    return !dialog(c_green + bold + "Confirm\n" + f_clear + message, options, false, 0, true);
}

/*
@brief Displays a confirmation dialog asking if the user wants to exit the program

@return true if the user confirms exit, otherwise false
*/
bool exit_program_confirm()
{
    return confirm("Exit the program?");
}

/*
@brief Repeatedly executes some code until the user chooses to stop

After each execution the user is asked if wants to repeat the operation

@param code Function/Lambda to execute
*/
void exec_repeat(const std::function<void()> code)
{
    do code(); while (confirm("Would you like to repeat?"));
}

/*
@brief Checks if a container contains a specific element

@tparam Container Container type
@tparam T Element type
@param container Container to search
@param element Element to find

@return true if the element exist, otherwise false
*/
template<typename Container, typename T>
bool exist(const Container &container, const T element)
{
    return std::find(container.begin(), container.end(), element) != container.end();
}

/*
@brief Returns the index of an element in a container

@tparam Container Container type
@tparam T Element type
@param container Container to search
@param element Element to find

@return Zero-based index of the element, or SIZE_MAX if not found
*/
template<typename Container, typename T>
size_t exist_at(const Container &container, const T element)
{
    auto it = std::find(container.begin(), container.end(), element);
    if (it != container.end())
        return std::distance(container.begin(), it);
    else
        return -1;
}

/*
@brief Removes the fist occurrence of an element from a container

@tparam Container Container type
@tparam T Element type
@param container Container to modify
@param element Element to remove
*/
template<typename Container, typename T>
void wipe(Container &container, const T element)
{
    container.erase(std::find(container.begin(), container.end(), element));
}

/*
@brief Reverses the order of the elements in a std::vector

@tparam T Element type
@param container Vector to reverse
*/
template<typename T>
void reverse(std::vector<T> &container)
{
    std::vector<T> reversed;
    std::stack<T> elements_stack;

    for (auto &&element : container) elements_stack.push(element);
    while (elements_stack.size() > 0)
    {
        auto element = elements_stack.top();
        reversed.push_back(element);
        elements_stack.pop();
    }

    container = reversed;
}

/*
@brief Reads editable text from the terminal

Supports cursor navigation, UTF-8 input, pasword masking, default text and centered rendering

@param prompt Prompt displayed before the input
@param promt_color ANSI color applied to the prompt
@param input_color ANSI color applied to the user input
@param max_length Maximum allowed input length
@param centered Width used to horizontally center the input line
@param password_mode Whether the typed chars should be hidden
@param menu_mode Whether the input is displayed inside a centered dialog
@param default_text Initial text shown in the input field
@param dialog_width Width of the input dialog; 0 selects it automatically

@return The entered text
*/
std::string input(   
    std::string prompt       = "",
    std::string prompt_color = "",
    std::string input_color  = "",
    size_t max_length        = 64,
    size_t centered          = 0,
    bool password_mode       = false,
    bool menu_mode           = false,
    std::string default_text = "",
    size_t dialog_width      = 0
) {
    // Variables

    std::string key;

    std::string text_left   = "";
    std::string text_right  = "";

    // Default text

    if (default_text != "") text_left = default_text;

    // Save start position

    if (menu_mode) cursor_move_to(1, terminal_height);
    cursor_save_pos();

    // Show cursor

    show_cursor();

    while (true)
    {
        // Format input text

        std::string plain_text = text_left + text_right;
        std::string pass_text  = string_repeat("•", string_length(plain_text));
        
        std::string current_input = password_mode ? pass_text : plain_text;

        if (menu_mode)
        {
            terminal_height = get_terminal_height();
            terminal_width  = get_terminal_width();

            size_t prompt_length = string_length(prompt);
            size_t input_capacity = std::max(size_t(1), max_length);
            size_t requested_width = dialog_width > 0
                ? std::max(dialog_width, prompt_length + 1)
                : std::max(size_t(24), prompt_length + input_capacity);
            size_t max_dialog_width = std::max(size_t(1), terminal_width > 8 ? terminal_width - 8 : terminal_width);
            size_t actual_dialog_width = std::min(requested_width, max_dialog_width);
            size_t visible_width = actual_dialog_width > prompt_length ? actual_dialog_width - prompt_length : 1;

            size_t cursor_position = string_length(text_left);
            size_t visible_start = cursor_position >= visible_width
                ? cursor_position - visible_width + 1
                : 0;
            std::string visible_input = utf8_slice(current_input, visible_start, visible_width);

            std::vector<std::string> lines;
            lines.push_back(prompt_color + prompt + f_clear + input_color + visible_input + f_clear);

            size_t input_cursor_offset = prompt_length + cursor_position - visible_start;
            draw_dialog_frame(lines, {}, actual_dialog_width, 0, false, "", true, input_cursor_offset);
            out.show(false);
        }
        else
        {
            std::string full_line = prompt + current_input;

            // Calculate offset for centering

            size_t leading_spaces = 0;
            if (centered > 0 && centered > string_length(full_line)) {
                leading_spaces = (centered / 2) - (string_length(full_line) / 2);
            }

            // Load saved position

            cursor_load_pos();

            // Clear current line

            clear_line();

            // Print spaces for centering

            for (size_t i = 0; i < leading_spaces; i++) out << ' ';

            // Print prompt

            out << prompt_color << prompt << "\x1B[0m";

            // Print user input

            out << input_color << current_input;

            // Load saved position

            cursor_load_pos();

            // Calculate cursor offset

            size_t cursor_offset = leading_spaces + string_length(prompt) + string_length(text_left);

            // Move cursor i times to the right

            for (size_t i = 0; i < cursor_offset; i++) cursor_move_right(1);

            out.show();
        }

        // Get key

        key = get_key();

        // Move to the left

        if (key == key_left)
        {
            if (string_length(text_left) > 0)
            {
                std::string last = utf8_last_char(text_left);
                utf8_pop_back(text_left);
                text_right = last + text_right;
            }
            continue;
        }

        // Move to the right

        if (key == key_right)
        {
            if (string_length(text_right) > 0)
            {
                std::string first = utf8_pop_front(text_right);
                text_left += first;
            }
            continue;
        }
        
        // Backspace pressed

        if (key.size() == 1 && (key[0] == 8 || key[0] == 127))
        {
            if (string_length(text_left) > 0) utf8_pop_back(text_left);
            continue;
        }

        // Intro pressed

        if (key == key_intro) break;

        // ESC pressed

        if (!menu_mode && key == key_esc) continue;

        if (menu_mode && key == key_esc)
        {
            text_left = default_text;
            text_right = "";
            break;
        }

        // TAB pressed (ignore)

        if (key == "\t") continue;

        // Ignore up and down

        if (key == key_up || key == key_down) continue;
        
        // Append char to the buffer

        if (string_length(text_left + text_right) < max_length) text_left += key;
    }
    
    // Hide cursor

    hide_cursor();

    // Flush

    if (!menu_mode)
    {
        out << "\n";
        out.show();
    }
    else
    {
        out.show(false);
        out.show_previous();
    }

    // Return input

    return (text_left + text_right);
}

/*
@brief Checks whether a string represents a valid number

Supports signed integers, floats, and scientific notation using 'e' or 'E'

@param str String to validate
@param allow_exponent If true, allows scientific notation (default: true)
@param allow_decimals If true, allows decimal points (default: true)
@param allow_signs If true, allows '+' and '-' signs (default: true)

@return true if the string is a valid numeric representation, otherwise false
*/
bool is_valid_number(
    std::string str,
    bool allow_exponent = true,
    bool allow_decimals = true,
    bool allow_signs = true
)
{
    // Init flags

    bool can_use_exp = true;
    bool can_use_dot = true;

    bool has_digits     = false;
    bool exp_has_digits = false;

    size_t i     = 0;
    size_t i_exp = 0;

    // Iterate through string

    for (auto &&chr : str)
    {
        // If there is an exponent but the allow_exponent flag is false, return false

        if (!allow_exponent && char_in(chr, "eE")) return false;

        // If there is a dot but the allow_decimals flag is false, return false

        if (!allow_decimals && chr == '.') return false;

        // If there is a sign but the allow_signs flag is false, return false

        if (!allow_signs && char_in(chr, "-+")) return false;

        // If there is something that isn't a math char return, false
        
        if (!char_in(chr, "0123456789.-+eE")) return false;

        // If there is - or + anywhere other than the beginning of the expression or the exponent, return false

        if (char_in(chr, "-+") && i != 0 && i != i_exp) return false;

        // If the dot has already been used, return false

        if (chr == '.' && (!can_use_dot || !can_use_exp)) return false;

        // If there is more than one exponent, return false

        if (char_in(chr, "eE") && !can_use_exp) return false;

        // If there are no digits in the expression and the exponent has already started, return false

        if (!has_digits && char_in(chr, "eE")) return false;
        
        // Turn off exponent flag at the beggining of the exponent

        if (char_in(chr, "eE")) { can_use_exp = false; i_exp = i + 1; }

        // Turn off dot flag if a dot is detected

        if (chr == '.') can_use_dot = false;
        
        // If there are at least one digit turn on has_digits flag

        if (char_in(chr, "0123456789")) has_digits = true;

        // If there are at least one digit at the exponent turn on exp_has_digits flag

        if (char_in(chr, "0123456789") && !can_use_exp) exp_has_digits = true;
        
        // Increase position counter

        i++;
    }

    // If there are no digits, return false

    if (!has_digits) return false;

    // If there are an exponent but not digits in there, return false

    if (!exp_has_digits && !can_use_exp) return false;
    
    // If this point is reached, it means there are no issues, return true

    return true;
}

#define SPECIALIZE_TO_NUMBER(Type, Func) \
    template<> inline Type to_number<Type>(const std::string &str) { return Func(str); }

/*
@brief Converts a string to a numeric value

@tparam T Target numeric type
@param str String to convert

@return The converted numeric value
*/
template<typename T>
T to_number(const std::string &str);

SPECIALIZE_TO_NUMBER(int, std::stoi)
SPECIALIZE_TO_NUMBER(long, std::stol)
SPECIALIZE_TO_NUMBER(long long, std::stoll)
SPECIALIZE_TO_NUMBER(unsigned long, std::stoul)
SPECIALIZE_TO_NUMBER(unsigned long long, std::stoull)
SPECIALIZE_TO_NUMBER(float, std::stof)
SPECIALIZE_TO_NUMBER(double, std::stod)

#undef SPECIALIZE_TO_NUMBER

/*
@brief Checks if a number is even

@param n Number to check

@return true if is even, false is odd
*/
bool is_even(int n) { return !(n % 2); }

/*
@brief Checks if a number is a prime number

@param n Number to check

@return true if is prime, otherwise false
*/
bool is_prime(int n)
{
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    for (int i = 3; i * i <= n; i++)
        if (n % i == 0) return false;
        
    return true;
}

#pragma endregion Utilities

#pragma region Menus

//
//  MENUS
//

class widget
{
    protected:
        std::string label;

    public:
        widget(std::string label) : label(label) {}
        virtual ~widget() = default;

        virtual bool is_focusable() const { return true; }
        virtual void draw(bool selected, std::string color, size_t width, std::ostream &out = zk::out.stream()) = 0;
        virtual void on_left() {}
        virtual void on_right() {}
        virtual void on_enter() {}
};

class button : public widget
{
    std::function<void()> call;

    public:
        button(std::string text, std::function<void()> call) : widget(text), call(call) {}

        void draw(bool selected, std::string color, size_t width, std::ostream &out) override
        {
            std::string text = selected ? "▶▶ " + label + " ◀◀" : label;
            out << (selected ? color : "") << center(text, width) << "\x1B[0m\x1B[K\n";
        }

        void on_enter() override { call(); }
};

class selector : public widget
{
    std::vector<std::string> options;
    int &selected_option;

    public:
        selector(std::string text, std::vector<std::string> options, int &selected_option) : widget(text), options(options), selected_option(selected_option) {}

        void draw(bool selected, std::string color, size_t width, std::ostream &out) override
        {
            std::string text = selected ? "◀ " + label + ": < " + options[selected_option] + " > ▶" : label + ": < " + options[selected_option] + " >";
            out << (selected ? color : "") << center(text, width) << "\x1B[0m\x1B[K\n";
        }

        void on_left() override
        {
            selected_option--;
            wrap(selected_option, 0, options.size() - 1);
        }

        void on_right() override
        {
            selected_option++;
            wrap(selected_option, 0, options.size() - 1);
        }
};

class slider : public widget
{
    int &value;
    int min, max;

    public:
        slider(std::string text, int &value, int min, int max) : widget(text), value(value), min(min), max(max) {}

        void draw(bool selected, std::string color, size_t width, std::ostream &out) override
        {
            std::string bar;

            size_t filled = value * 10 / max;

            for (size_t i = 0; i < 10; i++) bar += (i < filled ? "■" : "┄");

            std::string text = label + " |" + bar + "| " + std::to_string(value);
            if (selected) text = "◀ " + text + " ▶";
            out << (selected ? color : "") << center(text, width) << "\x1B[0m\x1B[K\n";
        }

        void on_left() override
        {
            if (value > min) value--;
        }

        void on_right() override
        {
            if (value < max) value++;
        }
};

class checkbox : public widget
{
    bool &checked;

    public:
        checkbox(std::string text, bool &checked) : widget(text), checked(checked) {}

        void draw(bool selected, std::string color, size_t width, std::ostream &out) override
        {
            std::string text = std::string(checked ? "[✓]" : "[ ]") + " " + label;
            if (selected) text = "▶ " + text + " ◀";
            out << (selected ? color : "") << center(text, width) << "\x1B[0m\x1B[K\n";
        }

        void on_enter() override
        {
            checked = !checked;
        }
};

class textfield : public widget
{
    std::string &value;
    std::string col;
    bool pass_mode;
    size_t dialog_width;
    size_t max_length;

    public:
        textfield(std::string text, std::string &value, bool pass_mode = false, size_t dialog_width = 0, size_t max_length = 64)
            : widget(text), value(value), pass_mode(pass_mode), dialog_width(dialog_width), max_length(max_length) {}

        void draw(bool selected, std::string color, size_t width, std::ostream &out) override
        {
            col = color;

            std::string text;

            if (pass_mode)
                text = label + ": [ " + string_repeat("•", string_length(value)) + " ]";
            else
                text = label + ": [ " + value + " ]";
            
            if (selected) text = "✎ " + text + " ✎";
            out << (selected ? color : "") << center(text, width) << "\x1B[0m\x1B[K\n";
        }
        
        void on_enter() override
        {
            value = input(label + ": ", c_gray, col, max_length, terminal_width, pass_mode, true, value, dialog_width);
        }
};

class text_label : public widget
{
    std::function<std::string()> dynamic_text;
    std::string custom_color;
    bool use_accent;

    public:
        // Static constructor
        text_label(std::string text, std::string color = "", bool use_accent = false)
            : widget(text), dynamic_text(nullptr), custom_color(color), use_accent(use_accent) {}

        // Dynamic constructor (updates every frame)
        text_label(std::function<std::string()> text_fn, std::string color = "", bool use_accent = false)
            : widget(""), dynamic_text(text_fn), custom_color(color), use_accent(use_accent) {}

        bool is_focusable() const override { return false; }

        void draw(bool selected, std::string color, size_t width, std::ostream &out = zk::out.stream()) override
        {
            std::string current_text = dynamic_text ? dynamic_text() : label;
            std::string active_color = use_accent ? color : custom_color;
            out << active_color << center(current_text, width) << f_clear << "\x1B[K\n";
        }
};

class spacer : public widget
{
    size_t lines;

    public:
        spacer(size_t lines = 1) : widget(""), lines(lines) {}

        bool is_focusable() const override { return false; }

        void draw(bool selected, std::string color, size_t width, std::ostream &out) override
        {
            for (size_t i = 0; i < lines; i++) out << "\n";
        }
};

class separator : public widget
{
    size_t line_width;
    std::string pattern;
    std::string custom_color;

    public:
        separator(size_t line_width = 20, std::string pattern = "─", std::string color = c_gray)
            : line_width(line_width), widget(""), pattern(pattern), custom_color(color) {}

        bool is_focusable() const override { return false; }

        void draw(bool selected, std::string color, size_t width, std::ostream &out) override
        {
            std::string line = string_repeat(pattern, line_width);
            out << custom_color << center(line, width) << f_clear << "\n";
        }
};

class progress_bar : public widget
{
    int &value;
    int max;

    public:
        progress_bar(std::string text, int &value, int max)
            : widget(text), value(value), max(max) {}

        bool is_focusable() const override { return false; }

        void draw(bool selected, std::string color, size_t width, std::ostream &out) override
        {
            int clamped = value;
            clamp(clamped, 0, max);

            size_t filled = (max > 0) ? (clamped * 10 / max) : 0;
            std::string bar;
            for (size_t i = 0; i < 10; i++) bar += (i < filled ? "━━" : "┄┄");

            std::string text = label + " " + color + "[" + bar + "]" + f_clear + " " + std::to_string(clamped) + "/" + std::to_string(max);
            out << center(text, width) << "\x1B[0m\x1B[K\n";
        }
};

class menu
{
public:
    enum class title_animation
    {
        none,
        scanline_h,
        scanline_v,
        rainbow,
        glitch
    };

private:
    bool executing = true;
    std::vector<std::string> ascii_title;
    title_animation current_animation = title_animation::none;
    int animation_i = 0;
    std::string menu_title;
    std::string accent_color;
    std::vector<std::unique_ptr<widget>> widgets;
    int selected_option = 0;
    size_t menu_width;

    std::function<void()> on_escape = nullptr;
    std::function<void()> on_update = nullptr;

    template<typename T, typename... Args>
    void add(Args&&... args) { widgets.push_back(std::make_unique<T>(std::forward<Args>(args)...)); }

    bool has_focusable_widgets() const
    {
        for (const auto &w : widgets)
            if (w->is_focusable()) return true;
        return false;
    }

    void move_selection(int direction)
    {
        if (!has_focusable_widgets()) return;

        do {
            selected_option += direction;
            wrap(selected_option, 0, int(widgets.size() - 1));
        } while (!widgets[selected_option]->is_focusable());
    }

    void render_ascii_title()
    {
        switch (current_animation)
        {
            default:
            case title_animation::none:
                for (const auto &line : ascii_title)
                    out << accent_color << center(line) << "\x1B[0m\x1B[K\n";
                break;
            
            case title_animation::scanline_h:
            {
                size_t max_length = 0;
                for (const auto &line : ascii_title)
                    max_length = std::max(max_length, string_length(line));
                
                if (max_length > 0)
                    wrap(animation_i, 0, int(max_length) - 1);
                
                for (const auto &line : ascii_title)
                {
                    size_t line_length = string_length(line);
                    std::string new_line = line;

                    if (size_t(animation_i) < line_length)
                        new_line = utf8_slice(line, 0, animation_i)
                                 + " "
                                 + utf8_slice(line, animation_i + 1, line_length - animation_i - 1);
                    
                    out << accent_color << center(new_line) << "\x1B[0m\x1B[K\n";
                }

                break;
            }
            
            case title_animation::scanline_v:
            {
                if (!ascii_title.empty())
                    wrap(animation_i, 0, int(ascii_title.size() - 1));
                
                for (size_t row = 0; row < ascii_title.size(); row++)
                {
                    const std::string &line = ascii_title[row];
                    out << accent_color
                        << center(int(row) == animation_i ? string_repeat(" ", string_length(line)) : line)
                        << "\x1B[0m\x1B[K\n";
                }

                break;
            }

            case title_animation::rainbow:
            {
                static const std::string rainbow_colors[] = {
                    c_red, c_orange, c_yellow, c_lime, c_green, c_cyan, c_blue, c_purple, c_pink
                };
                const int color_count = int(sizeof(rainbow_colors) / sizeof(rainbow_colors[0]));
                wrap(animation_i, 0, color_count - 1);

                for (const auto &line : ascii_title)
                {
                    std::string remaining = line;
                    std::string colored_line;
                    size_t character_i = 0;

                    while (!remaining.empty())
                    {
                        std::string character = utf8_pop_front(remaining);
                        int color_i = (animation_i + int(character_i)) % color_count;
                        colored_line += rainbow_colors[color_i] + character;
                        character_i++;
                    }
                    
                    out << center(colored_line) << "\x1B[0m\x1B[K\n";
                }
                
                break;
            }

            case title_animation::glitch:
            {
                static const std::string glitch_chars[] = { "#", "%", "&", "@", "?", "/", "\\", "|", "_" };
                const int glitch_count = int(sizeof(glitch_chars) / sizeof(glitch_chars[0]));

                for (const auto &line : ascii_title)
                {
                    std::string remaining = line;
                    std::string glitched_line;

                    while (!remaining.empty())
                    {
                        std::string character = utf8_pop_front(remaining);
                        if (character != " " && irandom(1, 100) <= 8)
                            glitched_line += color_rgb(255, irandom(40, 120), irandom(160, 255))
                                           + glitch_chars[irandom(0, glitch_count - 1)]
                                           + accent_color;
                        else
                            glitched_line += accent_color + character;
                    }

                    out << center(glitched_line) << "\x1B[0m\x1B[K\n";
                }
                break;
            }
        }
    }

    void render_frame()
    {
        out << "\x1B[H";

        render_ascii_title();

        // Header
        out << center(string_repeat("─", menu_width)) << "\x1B[K\n\x1B[K\n";
        out << center(menu_title) << "\x1B[K\n\x1B[K\n";

        // Widgets
        for (size_t i = 0; i < widgets.size(); i++)
            widgets[i]->draw(int(i) == selected_option, accent_color, terminal_width);
        
        // Footer
        std::string footer_str = "Navigation: " + accent_color + "← ↑ → ↓" + c_gray + " | " + accent_color + "↲" + f_clear;
        std::string footer_sep = string_repeat("─", string_length(footer_str) + 6);
        out << "\x1B[K\n" << center(footer_sep) << "\x1B[K\n" << center(footer_str) << "\x1B[K\n";

        // Clear trailing garbage & flush
        out << "\x1B[J";
        out.show();
    }

    void handle_input()
    {
        std::string key;
        while (!(key = get_key(false)).empty())
        {
            if (has_focusable_widgets())
            {
                if (key == key_intro) { widgets[selected_option]->on_enter(); clear(); return; }
                if (key == key_left)  { widgets[selected_option]->on_left(); }
                if (key == key_right) { widgets[selected_option]->on_right(); }
            }
            
            if (key == key_up)   { move_selection(-1); }
            if (key == key_down) { move_selection(1); }
            
            if (key == key_esc)
            {
                if (on_escape)
                {
                    on_escape();
                    clear();
                    return;
                }
                if (exit_program_confirm())
                {
                    clear();
                    close();
                    return;
                }
                clear();
            }
        }
    }

    public:
        /*
        @brief Menu constructor

        @param title Title to be displayed in the menu
        */
        menu(std::string title) : menu_title(std::move(title)), menu_width(string_length(menu_title)) {}

        /*
        @brief Adds a button to the menu, it can be activated with intro
        
        @param text Text to display in the button
        @param call Function/s to execute at intro
        */
        void add_button(std::string text, std::function<void()> call) { add<button>(text, call); }

        /*
        @brief Adds a selector to the menu, it can be changed with left/right

        @param text Text to display in the selector
        @param options List of selectable options
        @param selected_option Reference to the variable that will store the selected option index
        */
        void add_selector(std::string text, std::vector<std::string> options, int &selected_option) { add<selector>(text, options, selected_option); }

        /*
        @brief Adds a slider to the menu, it can be changed with left/right

        @param text Text to display in the slider
        @param value Reference to the variable that will store the slider value
        @param min Minimum value of the slider
        @param max Maximum value of the slider
        */
        void add_slider(std::string text, int &value, int min, int max) { add<slider>(text, value, min, max); }

        /*
        @brief Adds a checkbox to the menu, it can be toggled with intro

        @param text Text to display in the checkbox
        @param checked Reference to the variable that will store the checkbox state
        */
        void add_checkbox(std::string text, bool &checked) { add<checkbox>(text, checked); }

        /*
        @brief Adds a textfield to the menu, it can be edited with keyboard input

        @param text Text to display in the textfield
        @param value Reference to the variable that will store the textfield value
        @param pass_mode Whether the textfield should display characters as asterisks
        @param dialog_width Width of the input dialog; 0 chooses the width automatically
        @param max_length Maximum number of UTF-8 characters accepted
        */
        void add_textfield(std::string text, std::string &value, bool pass_mode = false, size_t dialog_width = 0, size_t max_length = 64)
        {
            add<textfield>(text, value, pass_mode, dialog_width, max_length);
        }

        /*
        @brief Adds a label to the menu

        @param text Text to display in the label
        @param use_accent Whether to use the accent color
        @param color Color code in ANSI format
        */
        void add_label(std::string text, bool use_accent = false, std::string color = "") { add<text_label>(text, color, use_accent); }

        /*
        @brief Adds a label to the menu

        @param text_fn Function that returns the text to display in the label
        @param use_accent Whether to use the accent color
        @param color Color code in ANSI format
        */
        void add_label(std::function<std::string()> text_fn, bool use_accent = false, std::string color = "") { add<text_label>(text_fn, color, use_accent); }

        /*
        @brief Adds a blank line to the menu

        @param lines Number of blank lines to add
        */
        void add_br(size_t lines = 1) { add<spacer>(lines); }

        /*
        @brief Adds a separator to the menu

        @param line_width Width of the separator line
        @param pattern Pattern to use for the separator
        @param color Color code in ANSI format
        */
        void add_separator(size_t line_width = 20, std::string pattern = "─", std::string color = c_gray) { add<separator>(line_width, pattern, color); }

        /*
        @brief Adds a progress bar to the menu

        @param text Text to display next to the progress bar
        @param value Reference to the variable that will store the progress value
        @param max Maximum value of the progress bar
        */
        void add_progress(std::string text, int &value, int max) { add<progress_bar>(text, value, max); }

        /*
        @brief Set a decorative ASCII art title to the menu

        @param title Vector of ASCII art lines
        */
        void set_ascii_title(std::vector<std::string> title) { ascii_title = title; menu_width = title.empty() ? 0 : string_length(title[0]); }

        /*
        @brief Set an animation to the ASCII art title

        @param animation Animation to be displayed
        */
        void set_ascii_title_animation(title_animation animation) { current_animation = animation; }

        /*
        @brief Set the function to be called when the escape key is pressed

        @param call Function to be called
        */
        void set_on_escape(std::function<void()> call) { on_escape = call; }

        /*
        @brief Sets the function to be called on every menu update, before drawing

        @param call Function to be called
        */
        void on_loop(std::function<void()> call) { on_update = call; }

        /*
        @brief Set an accent color to the menu

        @param color Color code in ANSI format
        */
        void set_color(std::string color) { accent_color = color; }

        /*
        @brief Get the accent color of the menu in the ANSI format
        */
        std::string get_color() { return accent_color; }

        /*
        @brief Stops the execution of the menu
        */
        void close() { executing = false; }

        /*
        @brief Resets the selected option to the first focusable widget
        */
        void reset_selected() { selected_option = 0; }

        /*
        @brief Draws the menu in the screen
        */
        void show()
        {
            executing = true;
            clear();

            if (has_focusable_widgets() && !widgets[selected_option]->is_focusable())
                move_selection(1);
            
            timer anim_timer(100);
            animation_i = 0;

            while (executing)
            {
                if (on_update) on_update();
                if (!executing) break;

                terminal_width  = get_terminal_width();
                terminal_height = get_terminal_height();

                if (current_animation != title_animation::none &&
                    current_animation != title_animation::glitch &&
                    anim_timer.time_reached())

                    animation_i++;
                
                render_frame();
                handle_input();
                sleep(33);
            }
            
        }
};

class menu_manager
{
    private:
        // Submenu stack

        std::stack<menu*> menu_stack; 

    public:
        /*
        @brief Pushes a submenu onto the stack and starts its execution

        @param submenu Reference to the submenu to execute
        */
        void submenu_goto(menu& submenu)
        {
            // Update terminal size
            terminal_height = get_terminal_height();
            terminal_width  = get_terminal_width();

            if (!menu_stack.empty()) menu_stack.top()->close();

            submenu.set_on_escape([this](){
                if (menu_stack.size() > 1)
                {
                    submenu_return();
                }
                else if (exit_program_confirm())
                {
                    submenu_return();
                }
            });

            menu_stack.push(&submenu);
        }

        /*
        @brief Returns to the previous menu in the stack or finishes the execution of the menu system if there are no more menus in the stack
        */
        void submenu_return()
        {
            // Update terminal size
            terminal_height = get_terminal_height();
            terminal_width  = get_terminal_width();

            if (!menu_stack.empty())
            {
                menu_stack.top()->reset_selected();
                menu_stack.top()->close();
                menu_stack.pop();
            }
        }

        /*
        @brief Starts the execution of the menu system. At least one menu has to be in the stack

        @param starting_menu Reference to the first menu to execute
        */
        void exec(menu& starting_menu)
        {
            submenu_goto(starting_menu);

            while (menu_stack.size() > 0) { menu_stack.top()->show(); }
            clear();
            out.show();
        }
};

#pragma endregion Menus

}