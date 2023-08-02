#pragma once
#include "../../Include/Modules/Window.h"

#include "DataStorage.h"

#include <Urlmon.h>


class DataEditor
{
public:
    DataEditor(DataStorage& data_storage, Window& window);
    DataEditor() = delete;
    DataEditor(DataEditor&) = delete;
    DataEditor(DataEditor&&) = delete;
    ~DataEditor();

    // --- //

    void save_mouse_position_on_click();
    bool mouse_kept_position();

    void select_overlapped_point();
    void release_point();
    void restore_point_position();
    void start_moving_point();
    void move_point(sf::Vector2i delta);

    void start_railroad_selection();
    void select_railroad();
    void release_railroad();

    void save_camera_position();
    void restore_camera_position();
    void start_moving_camera();
    void stop_moving_camera();
    void move_camera(sf::Vector2i delta);

    bool mouse_io_is_released();
    bool keyboard_io_is_released();
    bool hovered_point();
    bool hovered_line();

    bool is_camera_moving_mode();
    bool is_railroad_selection_mode();
    bool is_point_moving_mode();

    bool railroad_is_selected();
    bool zone_is_selected();

    void start_add_line_mode();
    bool is_add_line_mode();
    void stop_add_line_mode();

    void create_point();
    void create_line();
    void split_line();
    void create_message(const std::string& message, int timeout = 5);

    bool message_is_opened();

    void delete_hovered_line();
    void delete_hovered_point();
    void close_message();

    void zoom_out();
    void zoom_in();

    void open_livemap();

    void screenshot_with_interface();
    void screenshot_without_interface();
    void check_screenshot_status();

    void flip_exit_popup_state();

private:
    void make_screenshot();

private:
    DataStorage& data_storage_;
    Window&      window_;

};
