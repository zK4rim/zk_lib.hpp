#include "../zk_lib.hpp"
using namespace zk;

int main()
{
    // Setup terminal for UTF-8 support, raw terminal mode and alternate screen buffer
    init_program();

    // Menu declaration
    menu main_menu("Basic Menu");

    std::vector<std::string> ascii_title = {
        "    ▄▄▄▄▄ ▄▄ ▄▄   ▄▄    ▄▄ ▄▄▄▄     ",
        "      ▄█▀ ██▄█▀   ██    ██ ██▄██    ",
        "    ▄██▄▄ ██ ██   ██▄▄▄ ██ ██▄█▀    ",
    };

    std::string name;

    main_menu.set_color(c_cyan);
    main_menu.set_ascii_title(ascii_title);
    main_menu.add_textfield("Name", name);
    main_menu.add_button("Say Hello", [&](){
        dialog("Hello " + name + "!");
    });
    main_menu.add_br();
    main_menu.add_button("Exit", [&](){
        main_menu.close();
    });

    // Start the menu execution
    main_menu.show();

    // Restore original terminal state
    exit_program();

    return 0;
}
