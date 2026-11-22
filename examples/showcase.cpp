#include "../zk_lib.hpp"
using namespace zk;

void input_environment()
{
    std::vector<std::vector<int>> matrix(20, std::vector<int>(20, 0));
    std::string direction = "right";
    Vector2D position = {10, 10};

    clear();
    output << center(c_orange + "INPUT TESTING ENVIRONMENT" + f_clear) << "\n\n";
    output << center("Press any key to test the input") << "\n";
    output << center("Press ESC to exit") << "\n\n";
    cursor_move_to(1,6);
    while (true)
    {
        std::string current_input = get_key(false);

        if (current_input == key_right) direction = "right";
        if (current_input == key_left)  direction = "left";
        if (current_input == key_up)    direction = "up";
        if (current_input == key_down)  direction = "down";

        if (direction == "right") position.x++;
        if (direction == "left")  position.x--;
        if (direction == "up")    position.y--;
        if (direction == "down")  position.y++;
        
        wrap(position.x, 0, 19);
        wrap(position.y, 0, 19);

        matrix.assign(20, std::vector<int>(20, 0));
        matrix[position.y][position.x] = 1;

        cursor_move_to(1,6);
        clear_line();

        if (current_input == key_right) current_input = "RIGHT";
        if (current_input == key_left)  current_input = "LEFT";
        if (current_input == key_up)    current_input = "UP";
        if (current_input == key_down)  current_input = "DOWN";
        if (current_input == key_intro) current_input = "INTRO";
        
        output << center("Current Input: " + c_cyan + current_input + f_clear) << "\n\n";

        for (size_t y = 0; y < 20; y++)
        {
            std::string row;
            for (size_t x = 0; x < 20; x++)
                row += matrix[y][x] ? "● " : "○ ";
            output << center(row) << "\n";
        }
        
        output.show();

        if (current_input == key_esc)
        {
            if (confirm("Exit testing environment?"))
                break;
            continue;
        }

        sleep(33);
    }
}

int main()
{
    init_program();

    //
    //  Variables
    //

    std::vector<std::string> ascii_title = {
        "    ▄▄▄▄▄ ▄▄ ▄▄   ▄▄    ▄▄ ▄▄▄▄     ",
        "      ▄█▀ ██▄█▀   ██    ██ ██▄██    ",
        "    ▄██▄▄ ██ ██   ██▄▄▄ ██ ██▄█▀    ",
    };

    int selector  = 0;
    int slider    = 5;
    bool checkbox = false;
    std::string textfield;

    int elapsed_frames = 0;
    int progress = 0;

    int dialog_option = 0;
    bool dialog_confirmation = false;

    int R = 18, G = 25, B = 8;
    std::string accent_color = color_rgb(R * 10, G * 10, B * 10);

    int rainbow_index = 0;

    std::string number;
    int rand_num = 0;
    int rand_max = 10;

    //
    //  Menu declaration
    //

    menu_manager manager;
    menu menu_main("Main Menu");
    menu menu_widgets("Widgets showcase");
    menu menu_inputs("Dialogs & Inputs");
    menu menu_colors("Colors & Strings");
    menu menu_math("Math Utils");

    //
    //  Main menu
    //

    menu_main.set_color(accent_color);
    menu_main.set_ascii_title(ascii_title);

    menu_main.add_button("Widgets Showcase", [&](){
        manager.submenu_goto(menu_widgets);    
    });
    menu_main.add_button("Dialogs & Inputs", [&](){
        manager.submenu_goto(menu_inputs);
    });
    menu_main.add_button("Colors & Strings", [&](){
        manager.submenu_goto(menu_colors);
    });
    menu_main.add_button("Math Utils", [&](){
        manager.submenu_goto(menu_math);
    });
    menu_main.add_br();
    menu_main.add_separator();
    menu_main.add_button("Exit", [&](){
        if (exit_program_confirm())
            manager.submenu_return();
    });

    //
    //  Widgets menu
    //

    menu_widgets.set_color(accent_color);
    menu_widgets.set_ascii_title(ascii_title);

    menu_widgets.add_label(underline + "Interactive Widgets", true);
    menu_widgets.add_br();
    menu_widgets.add_button("Button", [&](){
        dialog(c_green + bold + "Button pressed!" + f_clear);
    });
    menu_widgets.add_selector("Selector", {"Option 1", "Option 2", "Option 3"}, selector);
    menu_widgets.add_slider("Slider ", slider, 0, 10);
    menu_widgets.add_checkbox("Checkbox", checkbox);
    menu_widgets.add_textfield("Textfield", textfield, false, 32);
    menu_widgets.add_textfield("Textfield (pass)", textfield, true, 32);

    menu_widgets.add_br();
    menu_widgets.add_label(underline + "Informative Widgets", true);
    menu_widgets.add_br();
    menu_widgets.add_label([&](){
        return 
        "Dynamic Label: "+
        menu_widgets.get_color() +
        std::to_string(elapsed_frames) + 
        " elapsed frames" + f_clear;
    });
    menu_widgets.add_progress("Progress bar", progress, 100);

    menu_widgets.add_br();
    menu_widgets.add_separator();
    menu_widgets.add_button("Back", [&](){
        manager.submenu_return();
    });

    menu_widgets.on_loop([&](){
        elapsed_frames++;
        progress++;
        wrap(progress, 0, 100);
    });

    //
    //  Inputs menu
    //

    menu_inputs.set_color(accent_color);
    menu_inputs.set_ascii_title(ascii_title);

    menu_inputs.add_label(underline + "Dialogs", true);
    menu_inputs.add_br();
    menu_inputs.add_button("Show dialog", [&](){
        dialog_option = dialog(c_orange + bold + "Select an option" + f_clear, {
            "Option 0",
            "Option 1",
            "Option 2",
            "Option 3",
            "Option 4"
        });
    });
    menu_inputs.add_label([&](){
        return "Selected index: " + menu_inputs.get_color() + std::to_string(dialog_option) + f_clear;
    });
    menu_inputs.add_br();
    menu_inputs.add_button("Show confirmation", [&](){
        dialog_confirmation = confirm("Are you sure?");
    });
    menu_inputs.add_label([&](){
        return "Status: " +
        (dialog_confirmation ?
        c_green + "True" + f_clear : c_red + "False" + f_clear);
    });

    menu_inputs.add_br();
    menu_inputs.add_separator();
    menu_inputs.add_br();

    menu_inputs.add_button("Test input", [&](){
        if (!confirm("Do you want to enter to the\ninput testing environment?")) return;
        input_environment();
    });

    menu_inputs.add_br();
    menu_inputs.add_separator();
    menu_inputs.add_button("Back", [&](){
        manager.submenu_return();
    });

    //
    //  Colors menu
    //

    menu_colors.set_color(accent_color);
    menu_colors.set_ascii_title(ascii_title);

    menu_colors.add_label(underline + "Accent Color", true);
    menu_colors.add_br();
    menu_colors.add_slider("R", R, 0, 25);
    menu_colors.add_slider("G", G, 0, 25);
    menu_colors.add_slider("B", B, 0, 25);

    menu_colors.add_separator();

    menu_colors.add_label(
        c_red    + "RED "    +
        c_orange + "ORANGE " +
        c_yellow + "YELLOW " +
        c_lime   + "LIME "   +
        c_green  + "GREEN "  +
        c_cyan   + "CYAN "   +
        c_blue   + "BLUE "   +
        c_purple + "PURPLE " +
        c_pink   + "PINK "   +
        c_white  + "WHITE "  +
        c_gray   + "GRAY "   +
        c_black  + "BLACK"
    );

    menu_colors.add_label(
        c_bg_red    + "RED"    + f_clear + " " +
        c_bg_orange + "ORANGE" + f_clear + " " +
        c_bg_yellow + "YELLOW" + f_clear + " " +
        c_bg_lime   + "LIME"   + f_clear + " " +
        c_bg_green  + "GREEN"  + f_clear + " " +
        c_bg_cyan   + "CYAN"   + f_clear + " " +
        c_bg_blue   + "BLUE"   + f_clear + " " +
        c_bg_purple + "PURPLE" + f_clear + " " +
        c_bg_pink   + "PINK"   + f_clear + " " +
        c_bg_white  + "WHITE"  + f_clear + " " +
        c_bg_gray   + "GRAY"   + f_clear + " " +
        c_bg_black  + "BLACK"
    );

    menu_colors.add_label([&](){
        std::string col;

        switch (rainbow_index)
        {
            case 0: col = c_red;    break;
            case 1: col = c_orange; break;
            case 2: col = c_yellow; break;
            case 3: col = c_lime;   break;
            case 4: col = c_green;  break;
            case 5: col = c_cyan;   break;
            case 6: col = c_blue;   break;
            case 7: col = c_purple; break;
            case 8: col = c_pink;   break;

            default: rainbow_index = 0; break;
        }

        rainbow_index++;
        wrap(rainbow_index, 0, 9);

        return col + bold + "RAINBOW" + f_clear;
    });

    menu_colors.add_separator();
    menu_colors.add_br();

    menu_colors.add_textfield("String", textfield, false, 32);
    menu_colors.add_button("Show string info", [&](){
        dialog(
            c_green + bold + "String Info" + f_clear + "\n" +
            "String: " + c_lime + "\"" + textfield + "\"" + f_clear + "\n" +
            "Lower:  " + c_pink + string_lower(textfield) + f_clear + "\n" +
            "Upper:  " + c_pink + string_upper(textfield) + f_clear + "\n" +
            "Length: " + c_cyan + std::to_string(string_length(textfield))
        );
    });

    menu_colors.add_br();
    menu_colors.add_separator();
    menu_colors.add_button("Back", [&](){
        manager.submenu_return();
    });

    menu_colors.on_loop([&](){
        accent_color = color_rgb(R * 10, G * 10, B * 10);

        if (accent_color != menu_colors.get_color())
        {
            menu_main.set_color(accent_color);
            menu_widgets.set_color(accent_color);
            menu_inputs.set_color(accent_color);
            menu_colors.set_color(accent_color);
            menu_math.set_color(accent_color);
        }
    });

    //
    //  Menu Math
    //

    menu_math.set_color(accent_color);
    menu_math.set_ascii_title(ascii_title);

    menu_math.add_label(underline + "Check Valid Number", true);
    menu_math.add_br();
    menu_math.add_textfield("Number", number, false, 20, 10);
    menu_math.add_label([&](){
        return "Valid: " +
        (is_valid_number(number) ?
        c_green + "True" + f_clear : c_red + "False" + f_clear);
    });
    menu_math.add_label([&](){
        if (!is_valid_number(number)) return "Converted to: " + c_gray + "-" + f_clear;
        
        return "Converted to: " + c_cyan + std::to_string(std::stoll(number)) + f_clear;
    });
    menu_math.add_label([&](){
        if (!is_valid_number(number)) return "Is even: " + c_gray + "-" + f_clear;
        
        return "Is even: " +
        (is_even(std::stoll(number)) ?
        c_green + "True" + f_clear : c_red + "False" + f_clear);
    });
    menu_math.add_label([&](){
        if (!is_valid_number(number)) return "Is prime: " + c_gray + "-" + f_clear;
        
        return "Is prime: " +
        (is_prime(std::stoll(number)) ?
        c_green + "True" + f_clear : c_red + "False" + f_clear);
    });

    menu_math.add_br();

    menu_math.add_label(underline + "Random", true);
    menu_math.add_br();
    menu_math.add_label([&](){
        return "Random int: " + c_pink + std::to_string(rand_num) + f_clear;
    });
    menu_math.add_slider("Max random", rand_max, 0, 100);
    menu_math.add_button("Generate random", [&](){
        rand_num = irandom(0, rand_max);
    });

    menu_math.add_br();
    menu_math.add_separator();
    menu_math.add_button("Back", [&](){
        manager.submenu_return();
    });

    //
    //  Menu system execution
    //

    manager.exec(menu_main);

    exit_program();
}