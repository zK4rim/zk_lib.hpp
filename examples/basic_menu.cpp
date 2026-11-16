#include "../zk_lib.hpp"
using namespace zk;

int main()
{
    //
    // Setup terminal for UTF-8 support, raw terminal mode and alternate screen buffer
    //

    init_program();

    //
    // Menu declaration
    //

    menuManager manager;
    menu main_menu("Main Menu");
    menu widgets_menu("Widgets Showcase");

    std::vector<std::string> ascii_title = {
        "                                    ",
        "    ▄▄▄▄▄ ▄▄ ▄▄   ▄▄    ▄▄ ▄▄▄▄     ",
        "      ▄█▀ ██▄█▀   ██    ██ ██▄██    ",
        "    ▄██▄▄ ██ ██   ██▄▄▄ ██ ██▄█▀    ",
        "                                    ",
    };

    //
    // Main menu setup
    //

    main_menu.set_color(c_cyan);
    main_menu.set_ascii_title(ascii_title);

    main_menu.add_button("Widgets", [&](){
        manager.submenu_goto(widgets_menu);
    });
    main_menu.add_separator();
    main_menu.add_button("Exit", [&](){
        if (exit_program_confirm()) manager.submenu_return();
    });

    //
    // Widgets menu setup
    //

    widgets_menu.set_color(c_lime);
    widgets_menu.set_ascii_title(ascii_title);

    int selector_option = 0;
    int slider_value    = 50;
    bool checkbox_state = false;
    std::string text    = "Hello World!";
    std::string pass    = "Secret";

    widgets_menu.add_button("Button", [&](){
        dialog("Button pressed!");
    });
    widgets_menu.add_selector("Selector", {"Option 1", "Option 2", "Option 3"}, selector_option);
    widgets_menu.add_slider("Slider", slider_value, 0, 100);
    widgets_menu.add_checkbox("Checkbox", checkbox_state);
    widgets_menu.add_textfield("Textfield", text, false);
    widgets_menu.add_textfield("Pass mode", pass, true);

    widgets_menu.add_br();
    widgets_menu.add_separator();
    widgets_menu.add_button("Back", [&](){
        manager.submenu_return();
    });

    //
    // Menu system execution
    //

    manager.exec(main_menu);

    //
    // Restore original terminal state
    //

    exit_program();

    return 0;
}
