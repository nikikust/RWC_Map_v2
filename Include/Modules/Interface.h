#pragma once
#include "../../Include/DataOperations/DataStorage.h"
#include "../../Include/DataOperations/DataLoader.h"

#include "Window.h"


class Interface
{
public:
    Interface(DataStorage& data_storage, DataLoader& data_loader, Window& window, DataStorage& data_storage_diff);
    Interface() = delete;
    Interface(Interface&) = delete;
    Interface(Interface&&) = delete;

    ~Interface();

    // --- //

    void update();

private:
    void init();
    void shutdown();

    void check_autosave();

    void show_main();
    void show_help();
    void show_lines_edit();
    void show_railroads();
    void show_players();
    void show_settings();
    void show_debug();

    void show_railroad_info();
    void show_player_info();
    void show_length_top();

    void show_cursor_message();

    void show_exit_popup();


    // --- Utils

    void create_message(const std::string& message, int timeout = 5);
    std::string form_length_top() const;

    // --- Data

    DataStorage& data_storage_;
    DataLoader&  data_loader_;
    Window&      window_;


    // --- Previous Data

    DataStorage& data_storage_diff_;
};
