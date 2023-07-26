#include "../../Include/Modules/Interface.h"


Interface::Interface(DataStorage& data_storage, DataLoader& data_loader, Window& window)
    : data_storage_(data_storage),
      data_loader_ (data_loader),
      window_      (window)
{
    init();
}
Interface::~Interface()
{
    shutdown();
}

void Interface::update()
{
    check_autosave      ();

    show_main           ();
    show_help           ();
    show_lines_edit     ();
    show_railroads      ();
    show_players        ();
    show_settings       ();
    show_debug          ();

    show_railroad_info  ();
    show_player_info    ();
    show_length_top     ();

    show_cursor_message ();

    show_exit_popup     ();
}


void Interface::init()
{
    data_storage_.deltaClock.restart();
}
void Interface::shutdown()
{
    ImGui::SFML::Shutdown();
}

void Interface::check_autosave()
{
    if (data_storage_.settings.autosave == false)
        return;

    if (data_storage_.status.start_time_for_autosave + data_storage_.settings.autosave_interval_in_minutes * 60 <= time(0))
    {
        data_loader_.save_railroads_data();

        data_storage_.status.start_time_for_autosave = time(0);
    }
}


void Interface::show_main()
{
    if (data_storage_.menus.Main == false)
        return;

    ImGui::Begin("Main menu");

    auto& menus = data_storage_.menus;

    ImGui::Checkbox("Help menu (F1)"      , &menus.Help      );
    ImGui::Checkbox("Lines edit menu (F2)", &menus.LinesEdit );
    ImGui::Checkbox("Settings menu (F3)"  , &menus.Settings  );
    ImGui::Checkbox("Railroads list (F4)" , &menus.Railroads );
    ImGui::Checkbox("Players list (F5)"   , &menus.Players   );
    ImGui::Checkbox("Length top (F6)"     , &menus.LengthTop );
    ImGui::Checkbox("Debug menu (F10)"    , &menus.Debug     );

    auto  cursor_pos = window_.get_cursor_position();
    auto &camera     = data_storage_.camera;

    ImGui::Text("Cursor Coord - X: %.1f | Z: %.1f",
        (camera.position.x + (cursor_pos.x - int(data_storage_.screen_size.x) / 2.0) / camera.scale_modifier_as_pow2) * 2,
        (camera.position.y + (cursor_pos.y - int(data_storage_.screen_size.y) / 2.0) / camera.scale_modifier_as_pow2) * 2);
    
    ImGui::Text("Scale: 1:%.2f", camera.scale_modifier_as_pow2);

    ImGui::Separator();

    if (ImGui::Button("Reload map"))
        data_storage_.map_data.reset_images_states();

    ImGui::End();
}
void Interface::show_help()
{
    if (data_storage_.menus.Help == false)
        return;

    ImGui::Begin("Help");

    ImGui::TextWrapped("1 pixel of Map = 2 blocks (at default scale)");
    ImGui::TextWrapped("Menus can be moved and docked inside each other");

    ImGui::Separator();
    ImGui::TextWrapped("To move Point, hover it and pull. Press Escape when pulled to return it to original position.");
    ImGui::TextWrapped("To select Railroad, click on line on map or on name at 'Railroads list' menu.");
    ImGui::TextWrapped("To select Zone, first select railroad and then click on name at 'Railroad info' which appeared when you selected railroad.");

    ImGui::Separator();
    ImGui::TextWrapped("To add line you need to select railroad and zone. Then just click on any point - you are now in 'Line Edit' mode. Press Escape to exit it.");
    ImGui::TextWrapped("At 'Line edit' menu you can turn on/off point alignment at creation and movement.");
    ImGui::TextWrapped("When in 'Line Edit' you have these ways to add line:");
    ImGui::TextWrapped("- Click on free space to put point there and connect line to it.");
    ImGui::TextWrapped("- Click on the any point to connect line to it (can avoid alignment).");
    ImGui::TextWrapped("- Click on the any line to split it and add point there.");

    ImGui::Separator();
    ImGui::Text("\nShortcuts table");

    ImGuiTableFlags table_flags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_ScrollX |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_NoSavedSettings |
        ImGuiTableFlags_Borders;

    if (ImGui::BeginTable("##shortcuts_table", 2, table_flags, {-FLT_MIN, 300.f}))
    {
        ImGui::TableSetupColumn("Key");
        ImGui::TableSetupColumn("Description");

        ImGui::TableHeadersRow();

        /* Add point */
        ImGui::TableNextColumn();
        ImGui::Text("A");

        ImGui::TableNextColumn();
        ImGui::Text("Add point (will split line if hovered)");

        /* Delete */
        ImGui::TableNextColumn();
        ImGui::Text("D/Delete");

        ImGui::TableNextColumn();
        ImGui::Text("Delete hovered point or line");

        /* Escape */
        ImGui::TableNextColumn();
        ImGui::Text("Escape");

        ImGui::TableNextColumn();
        ImGui::Text("Exit app or disable Tool");

        /* Hide railroad */
        ImGui::TableNextColumn();
        ImGui::Text("H");

        ImGui::TableNextColumn();
        ImGui::Text("Hide selected railroad");

        /* Livemap */
        ImGui::TableNextColumn();
        ImGui::Text("M");

        ImGui::TableNextColumn();
        ImGui::Text("Open current location on RWC livemap");

        /* Zoom */
        ImGui::TableNextColumn();
        ImGui::Text("Numpad -,+ or [,]");

        ImGui::TableNextColumn();
        ImGui::Text("Zoom");

        /* Screenshot */
        ImGui::TableNextColumn();
        ImGui::Text("P");

        ImGui::TableNextColumn();
        ImGui::Text("Make a screenshot");

        /* Menus */
        ImGui::TableNextColumn();
        ImGui::Text("F1-6,10");

        ImGui::TableNextColumn();
        ImGui::Text("Open menus");

        /* Save */
        ImGui::TableNextColumn();
        ImGui::Text("Ctrl+S");

        ImGui::TableNextColumn();
        ImGui::Text("Save data");

        /* LMB */
        ImGui::TableNextColumn();
        ImGui::Text("LMB");

        ImGui::TableNextColumn();
        ImGui::Text("Move camera (on free space).\nMove point (when hovered)\nSelect railroad (when clicked)");

        /* RMB */
        ImGui::TableNextColumn();
        ImGui::Text("RMB");

        ImGui::TableNextColumn();
        ImGui::Text("Move camera (when in 'Line Edit' mode");

        ImGui::EndTable();
    }

    ImGui::Separator();

    if (ImGui::Button("Close##help_menu"))
        data_storage_.menus.Help = false;

    ImGui::End();
}
void Interface::show_lines_edit()
{
    if (data_storage_.menus.LinesEdit == false)
        return;

    bool railroad_selected = !data_storage_.selection_info.railroad.expired();
    bool zone_selected     = !data_storage_.selection_info.zone    .expired();

    auto& fields = data_storage_.menus.fields.lines_edit;

    if (!railroad_selected)
    {
        fields.new_RR_name    = "";
        fields.saved_RR_color = sf::Color::Black;

        fields.new_RR_color_3f[0] = 0.f;
        fields.new_RR_color_3f[1] = 0.f;
        fields.new_RR_color_3f[2] = 0.f;

        fields.initialized = false;
    }
    else if (!fields.initialized)
    {
        auto railroad = data_storage_.selection_info.railroad.lock();

        fields.new_RR_name    = railroad->name;
        fields.saved_RR_color = railroad->color;

        fields.new_RR_color_3f[0] = railroad->color.r / 255.f;
        fields.new_RR_color_3f[1] = railroad->color.g / 255.f;
        fields.new_RR_color_3f[2] = railroad->color.b / 255.f;

        fields.initialized = true;
    }

    ImGui::Begin("Line edit");

    if (!railroad_selected)
        ImGui::BeginDisabled();

    ImGui::Text("RR name: "); ImGui::SameLine();
    ImGui::InputText("##new_RR_name", &fields.new_RR_name);


    if (ImGui::Button("Change name") && railroad_selected) {
        auto railroad = data_storage_.selection_info.railroad.lock();

        if (!data_storage_.railroads_data.contains_Railroad_with_name(fields.new_RR_name))
        {
            railroad->remove_aka_name(railroad->name);

            railroad->name = fields.new_RR_name;

            railroad->add_aka_name(railroad->name);
        }

        fields.new_RR_name = railroad->name;
    }

    if (ImGui::ColorEdit3("Line color", fields.new_RR_color_3f) && railroad_selected)
    {
        auto railroad = data_storage_.selection_info.railroad.lock();

        railroad->color = {
            static_cast<sf::Uint8>(fields.new_RR_color_3f[0] * 255),
            static_cast<sf::Uint8>(fields.new_RR_color_3f[1] * 255),
            static_cast<sf::Uint8>(fields.new_RR_color_3f[2] * 255),
                                                               255
        };
    }
    if (ImGui::Button("Revert color") && railroad_selected)
    {
        auto railroad = data_storage_.selection_info.railroad.lock();

        railroad->color = fields.saved_RR_color;
    }

    if (!railroad_selected)
        ImGui::EndDisabled();

    ImGui::Separator();

    ImGui::TextWrapped("Align points (Horizontal/Vertical movement)");
    ImGui::Checkbox("For creation", &fields.align_at_creation);
    ImGui::Checkbox("For movement", &fields.align_at_movement);
    ImGui::Separator();

    ImGui::Checkbox("Continue line after point set", &fields.auto_line_repeat);
    ImGui::Separator();

    ImGui::Checkbox("Show lines",  &fields.show_lines);
    ImGui::Checkbox("Show points", &fields.show_points);
    ImGui::Separator();

    ImGui::Checkbox("Show Planned",     &fields.show_Plans     );
    ImGui::Checkbox("Show In progress", &fields.show_InProgress);
    ImGui::Checkbox("Show Built",       &fields.show_Built     );

    ImGui::End();
}
void Interface::show_railroads()
{
    if (data_storage_.menus.Railroads == false)
        return;

    auto& fields = data_storage_.menus.fields.railroads;

    ImGui::Begin("Railroads list");

    ImGui::PushItemWidth(150);
    ImGui::InputText("##add_new_railroad", &fields.add_new_railroad_name);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (ImGui::Button("Add railroad"))
    {
        if (!data_storage_.railroads_data.contains_Railroad_with_name(fields.add_new_railroad_name))
        {
            Railroad new_railroad{
                0, fields.add_new_railroad_name, sf::Color::White
            }; // 'id = 0' for auto set in .add_Railroad()

            data_storage_.railroads_data.add_Railroad(new_railroad);
        }
    }

    ImGui::Separator();

    if (ImGui::Button("X##clear_railroads_search"))
        fields.railroads_search = "";

    ImGui::SameLine();

    ImGui::InputText("Search##railroads_list", &fields.railroads_search);

    if (ImGui::BeginListBox("RRs list"))
    {
        for (auto& railroad : data_storage_.railroads_data.Railroads)
        {
            if (railroad->name == "World Border")
                continue;

            std::string railroad_name = railroad->name;

            if (fields.railroads_search.empty() ||
                utils::part_is_in_vector(fields.railroads_search, railroad->aka_names))
            {
                bool selected = false;

                if (!data_storage_.selection_info.railroad.expired())
                    selected = data_storage_.selection_info.railroad.lock()->id == railroad->id;

                if (ImGui::Selectable(railroad_name.c_str(), selected))
                {
                    data_storage_.selection_info.railroad = railroad;
                    data_storage_.selection_info.zone.reset();
                    
                    fields.add_new_railroad_name = railroad_name;


                    data_storage_.menus.RailroadInfo = true;
                    data_storage_.menus.fields.railroad_info.initialized = false;
                    data_storage_.menus.fields.lines_edit   .initialized = false;
                }
            }
        }

        ImGui::EndListBox();
    }


    ImGui::Separator();

    bool railroad_is_selected = !data_storage_.selection_info.railroad.expired();

    if (!railroad_is_selected)
        ImGui::BeginDisabled();

    if (ImGui::Button("Show the beginning of the RR##railroads_list"))
    {
        if (!data_storage_.selection_info.railroad.lock()->starting_point.expired())
            data_storage_.camera.position = data_storage_.selection_info.railroad.lock()->starting_point.lock()->position;
        else
        {
            data_storage_.menus.fields.cursor_message.elapse_at = time(0) + 5;
            data_storage_.menus.fields.cursor_message.message   = "Railroad doesn't have lines to show!";
            data_storage_.menus.CursorMessage = true;
        }
    }

    if (!railroad_is_selected)
        ImGui::EndDisabled();

    ImGui::End();
}
void Interface::show_players()
{
    if (data_storage_.menus.Players == false)
        return;

    auto& fields = data_storage_.menus.fields.players;

    ImGui::Begin("Players list");

    ImGui::PushItemWidth(150);
    ImGui::InputText("##add_new_player", &fields.add_new_player_nick);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (ImGui::Button("Add player"))
    {
        if (!data_storage_.railroads_data.contains_Player_with_nick(fields.add_new_player_nick))
        {
            Player new_player{
                0, fields.add_new_player_nick
            }; // 'id = 0' for auto set in .add_Player()

            data_storage_.railroads_data.add_Player(new_player);
        }
    }

    ImGui::Separator();

    if (ImGui::Button("X##clear_players_search"))
        fields.players_search = "";

    ImGui::SameLine();

    ImGui::InputText("Search##players_search", &fields.players_search);


    if (ImGui::BeginListBox("##players_list"))
    {
        for (auto& player : data_storage_.railroads_data.Players)
        {
            if (fields.players_search.empty() ||
                utils::part_of_str(fields.players_search, player->nickname) ||
                utils::part_of_str(fields.players_search, player->discord))
            {
                bool selected = false;

                if (!data_storage_.selection_info.player.expired())
                    selected = data_storage_.selection_info.player.lock()->nickname == player->nickname;

                if (ImGui::Selectable(player->nickname.c_str(), selected))
                {
                    data_storage_.selection_info.player = player;

                    fields.add_new_player_nick = player->nickname;
                }
            }
        }

        ImGui::EndListBox();
    }

    ImGui::Separator();

    bool delete_disabled = data_storage_.selection_info.player.expired();
    if (delete_disabled)
        ImGui::BeginDisabled();

    if (ImGui::Button("Delete player"))
    {
        data_storage_.railroads_data.erase_Player_by_ID(data_storage_.selection_info.player.lock()->id);

        data_storage_.selection_info.player.reset();
    }

    ImGui::SameLine();

    if (ImGui::Button("Show info"))
    {
        data_storage_.menus.PlayerInfo = true;

        data_storage_.menus.fields.player_info.initialized = false;
    }

    if (delete_disabled)
        ImGui::EndDisabled();

    ImGui::End();
}
void Interface::show_settings()
{
    if (data_storage_.menus.Settings == false)
        return;

    ImGui::Begin("Settings");

    if(ImGui::Checkbox("AutoSave", &data_storage_.settings.autosave))
        data_storage_.status.start_time_for_autosave = time(0);

    ImGui::SameLine();

    ImGui::Text("Every"); ImGui::SameLine();

    ImGui::PushItemWidth(150);
    if (ImGui::InputInt("##autosave_interval", &data_storage_.settings.autosave_interval_in_minutes))
    {
        data_storage_.settings.autosave_interval_in_minutes = utils::minmax(1, data_storage_.settings.autosave_interval_in_minutes, 60);

        data_storage_.status.start_time_for_autosave = time(0);
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();

    ImGui::Text("minute%s", (data_storage_.settings.autosave_interval_in_minutes > 1) ? "s" : "");

    if (ImGui::Button("Save"))
        data_loader_.save_railroads_data();


    ImGui::Separator();

    ImGui::PushItemWidth(150);
    ImGui::Text("Go to position:"); ImGui::SameLine();
    ImGui::InputInt2("(X/Z) ", data_storage_.menus.fields.settings.goto_2f);

    ImGui::SameLine();

    if (ImGui::Button("Go"))
    {
        data_storage_.camera.position = sf::Vector2i{
            data_storage_.menus.fields.settings.goto_2f[0] / 2,
            data_storage_.menus.fields.settings.goto_2f[1] / 2 
        };
    }

    ImGui::PopItemWidth();

    ImGui::Separator();

    ImGui::Checkbox("Show hidden RRs", &data_storage_.settings.show_hidden_RRs);

    ImGui::Checkbox("Hide map", &data_storage_.settings.hide_map);

    ImGui::Text("Show:");
    if (ImGui::RadioButton("Current map",    data_storage_.settings.map_view == DataStorage::Settings::MapView::CURRENT   ))
    {
        data_storage_.settings.map_view = DataStorage::Settings::MapView::CURRENT;
    }
    if (ImGui::RadioButton("Difference map", data_storage_.settings.map_view == DataStorage::Settings::MapView::DIFFERENCE))
    {
        data_storage_.settings.map_view = DataStorage::Settings::MapView::DIFFERENCE;
    }
    if (ImGui::RadioButton("Old map",        data_storage_.settings.map_view == DataStorage::Settings::MapView::OLD       ))
    {    
        data_storage_.settings.map_view = DataStorage::Settings::MapView::OLD;
    }

    ImGui::Separator();

    ImGui::Checkbox("Unlimited scaling", &data_storage_.settings.super_scale);


    if (!data_storage_.settings.super_scale)
        ImGui::BeginDisabled();

    ImGui::SameLine();

    if (ImGui::Button("Map to screen"))
    {
        data_storage_.camera.scale_modifier_as_pow2 = (float)data_storage_.screen_size.y / (SPRITES_AMOUNT_VERTICAL * data_storage_.map_data.sprite_size.y);

        data_storage_.camera.scale_modifier = log2f(data_storage_.camera.scale_modifier_as_pow2);

        data_storage_.camera.position = { -14000, 400 };

        data_storage_.map_data.reset_images_states();
    }

    if (!data_storage_.settings.super_scale)
        ImGui::EndDisabled();

    ImGui::End();
}
void Interface::show_debug()
{
    if (data_storage_.menus.Debug == false)
        return;

    ImGui::Begin("Debug");

    if (ImGui::ColorEdit3("Background color", data_storage_.settings.background_color_3f)) {
        data_storage_.settings.background_color.r = static_cast<sf::Uint8>(data_storage_.settings.background_color_3f[0] * 255.f);
        data_storage_.settings.background_color.g = static_cast<sf::Uint8>(data_storage_.settings.background_color_3f[1] * 255.f);
        data_storage_.settings.background_color.b = static_cast<sf::Uint8>(data_storage_.settings.background_color_3f[2] * 255.f);
    }


    auto  cursor_position = window_.get_cursor_position();
    auto& camera_position = data_storage_.camera.position;

    ImGui::Text("Mouse X: %i | Mouse Y: %i", cursor_position.x, cursor_position.y);

    ImGui::Text("Sector X: %i | Sector Y: %i", 
        data_storage_.map_data.central_view_sector.x, 
        data_storage_.map_data.central_view_sector.y);
    
    ImGui::Text("Pos X: %i | Pos Y: %i", camera_position.x, camera_position.y);

    ImGui::Text("Num lines: %i | Num points: %i", 
        data_storage_.railroads_data.RR_Lines.size(),
        data_storage_.railroads_data.RR_Points.size());

    ImGui::Text("Railroads amount: %i", data_storage_.railroads_data.Railroads.size());


    ImGui::Text("Selected Railroad ID: %i", data_storage_.selection_info.railroad.expired() ? 0 : data_storage_.selection_info.railroad.lock()->id);
    ImGui::Text("Selected Zone ID: %i",     data_storage_.selection_info.zone    .expired() ? 0 : data_storage_.selection_info.zone    .lock()->id);

    ImGui::Text("Movement mode: %i", static_cast<int>(data_storage_.status.movement_mode));
    ImGui::Text("Edit mode: %i",     static_cast<int>(data_storage_.status.edit_mode    ));

    ImGui::Text("Overlapped - line: %i, point: %i",
        !data_storage_.status.overlapped_line .expired(),
        !data_storage_.status.overlapped_point.expired()
    );

    ImGui::Text("Want capture mouse: %i",    ImGui::GetIO().WantCaptureMouse   );
    ImGui::Text("Want capture keyboard: %i", ImGui::GetIO().WantCaptureKeyboard);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Separator();

    // ImGui::Checkbox("Screen mode", &screenMode);

    ImGui::End();
}

void Interface::show_railroad_info()
{
    if (data_storage_.menus.RailroadInfo == false)
        return;

    auto& fields = data_storage_.menus.fields.railroad_info;

    if (data_storage_.selection_info.railroad.expired())
    {
        data_storage_.menus.RailroadInfo = false;

        fields.initialized = false;

        return;
    }

    // --- Preparation

    if (!fields.initialized)
    {
        fields.selected_aka_name = "";

        fields.all_players_search        = "";
        fields.associated_players_search = "";

        fields.AKA_names_add_name = "";
        fields.zone_add_name      = "";

        fields.initialized = true;
    }

    auto railroad = data_storage_.selection_info.railroad.lock();

    ImGui::Begin("RR Info");

    ImGui::Text("RR name: %s", railroad->name.c_str());

    ImGui::Separator();

    
    // --- Associated players edit

    ImGui::Columns(2);
    ImGui::Text("All players:");

    ImGui::InputText("Search##railroad_info_all_players_search", &data_storage_.menus.fields.railroad_info.all_players_search);

    if (ImGui::BeginListBox("##railroad_info_all_players"))
    {
        for (auto& player : data_storage_.railroads_data.Players)
        {
            if (fields.all_players_search.empty() ||
                utils::part_of_str(fields.all_players_search, player->nickname))
            {
                if (!railroad->have_associated(player->id))
                    if (ImGui::Selectable(player->nickname.c_str(), false))
                        railroad->add_associated(player);
            }
        }

        ImGui::EndListBox();
    }
    ImGui::Text("Click to add.");

    ImGui::NextColumn();
    ImGui::Text("Associated with RR:");

    ImGui::InputText("Search##railroad_info_associated_players_search", &data_storage_.menus.fields.railroad_info.associated_players_search);

    if (ImGui::BeginListBox("##railroad_info_associated_players"))
    {
        auto it = railroad->associated.begin();
        while (it != railroad->associated.end())
        {
            std::string nickname = it->lock()->nickname;

            if (fields.associated_players_search.empty() ||
                utils::part_of_str(fields.associated_players_search, nickname))
            {
                if (ImGui::Selectable(nickname.c_str(), false))
                {
                    it = railroad->associated.erase(it);

                    continue;
                }
            }
            
            ++it;
        }
        ImGui::EndListBox();
    }
    ImGui::Text("Click to delete.");


    // --- 'Also Known As' edit

    ImGui::Columns(1);
    
    ImGui::Separator();
    
    ImGui::Text("AKA names:");

    ImGui::PushItemWidth(150);
    ImGui::InputText("##railroad_info_aka_name_add", &data_storage_.menus.fields.railroad_info.AKA_names_add_name); ImGui::SameLine();
    ImGui::PopItemWidth();

    if (ImGui::Button("Add name##railroad_info_aka_name_add") &&
        !data_storage_.menus.fields.railroad_info.AKA_names_add_name.empty())
    {
        std::string aka_name = data_storage_.menus.fields.railroad_info.AKA_names_add_name;

        if (!utils::is_in_vector(aka_name, railroad->aka_names))
        {
            railroad->aka_names.push_back(fields.AKA_names_add_name);

            fields.AKA_names_add_name = "";
        }
    }

    if (ImGui::BeginListBox("##railroad_info_aka_names_list"))
    {
        for (auto& aka_name : railroad->aka_names)
        {
            if (aka_name != railroad->name)
            {
                bool selected = fields.selected_aka_name == aka_name;

                if (ImGui::Selectable(aka_name.c_str(), selected))
                {
                    fields.selected_aka_name = aka_name;
                    
                    fields.AKA_names_add_name = aka_name;
                }
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Delete##AKAName"))
    {
        auto it = railroad->aka_names.begin();

        while (it != railroad->aka_names.end())
        {
            if (*it == fields.selected_aka_name)
            {
                railroad->aka_names.erase(it);
                break;
            }

            ++it;
        }
    }

    ImGui::Separator();


    // --- Zones edit

    ImGui::Text("Zones:");

    ImGui::PushItemWidth(150);
    ImGui::InputText("##railroad_info_zone_add", &fields.zone_add_name); ImGui::SameLine();
    ImGui::PopItemWidth();

    if (ImGui::Button("Add##railroad_info_zone_add") &&
        !fields.zone_add_name.empty())
    {
        if (!data_storage_.railroads_data.contains_Zone_with_name(fields.zone_add_name, railroad->id))
        {
            RR_Zone zone{ 0, fields.zone_add_name, RR_Zone::Built, railroad };

            data_storage_.railroads_data.add_RR_Zone(zone);
        }
    }

    ImGui::Columns(2);
    if (ImGui::BeginListBox("##railroad_info_zones_list"))
    {
        for (auto& zone : data_storage_.railroads_data.RR_Zones)
        {
            if (zone->railroad.lock()->id == railroad->id)
            {
                bool selected = false;

                if (!data_storage_.selection_info.zone.expired())
                    selected = data_storage_.selection_info.zone.lock()->id == zone->id;

                if (ImGui::Selectable(zone->name.c_str(), selected))
                {
                    data_storage_.selection_info.zone = zone;

                    fields.rename_zone = zone->name;
                }
            }
        }

        ImGui::EndListBox();
    }

    ImGui::NextColumn();

    if (!data_storage_.selection_info.zone.expired())
    {
        auto zone = data_storage_.selection_info.zone.lock();

        ImGui::PushItemWidth(150);
        ImGui::InputText("##rename_zone", &fields.rename_zone);
        ImGui::PopItemWidth();

        ImGui::SameLine();

        if (ImGui::Button("Rename##zone"))
        {
            if (!data_storage_.railroads_data.contains_Zone_with_name(fields.rename_zone, railroad->id))
            {
                zone->name = fields.rename_zone;
            }

            fields.rename_zone = zone->name;
        }

        ImGui::Text("Zone type:");

        if (ImGui::RadioButton("Plan",        zone->state == RR_Zone::Plan      )) { zone->state = RR_Zone::Plan      ; }
        if (ImGui::RadioButton("In progress", zone->state == RR_Zone::InProgress)) { zone->state = RR_Zone::InProgress; }
        if (ImGui::RadioButton("Built",       zone->state == RR_Zone::Built     )) { zone->state = RR_Zone::Built     ; }

        

        if (ImGui::Button("Delete##railroad_info_zone_delete")) 
        { 
            fields.delete_zone = true;
        }

        if (fields.delete_zone == true)
        {
            ImGui::SameLine();

            if (ImGui::Button("Confirm##railroad_info_zone_delete"))
            {
                zone->erase_from_lists(data_storage_);

                data_storage_.selection_info.zone.reset();

                fields.delete_zone = false;
            }
        }
    }

    ImGui::Columns(1);

    if (ImGui::Button("Deselect zone##railroad_info_zone_deselect")) 
    {
        data_storage_.selection_info.zone.reset();
    }

    if (ImGui::CollapsingHeader("Move Zone to another Railroad"))
    {
        if (ImGui::Button("X##clear_railroads_search_for_zones"))
            fields.railroads_search = "";

        ImGui::SameLine();

        ImGui::InputText("Search##move_zone_to_another_RR", &fields.railroads_search);

        if (ImGui::BeginListBox("RRs list##move_zone_to_another_RR"))
        {
            for (auto& railroad : data_storage_.railroads_data.Railroads)
            {
                if (railroad->name == "World Border")
                    continue;

                std::string railroad_name = railroad->name;

                if (fields.railroads_search.empty() ||
                    utils::part_is_in_vector(fields.railroads_search, railroad->aka_names))
                {
                    bool selected = false;

                    if (!fields.selected_railroad_for_zone_move.expired())
                        selected = fields.selected_railroad_for_zone_move.lock()->id == railroad->id;

                    if (ImGui::Selectable(railroad_name.c_str(), selected))
                    {
                        fields.selected_railroad_for_zone_move = railroad;
                    }
                }
            }

            ImGui::EndListBox();
        }

        if (fields.selected_railroad_for_zone_move.expired() || data_storage_.selection_info.zone.expired())
            ImGui::BeginDisabled();

        if (ImGui::Button("Move to selected RR"))
        {
            if (data_storage_.railroads_data.contains_Zone_with_name(
                data_storage_.selection_info.zone     .lock()->name,
                fields.selected_railroad_for_zone_move.lock()->id)  ==  false)
            {
                data_storage_.selection_info.zone.lock()->railroad = fields.selected_railroad_for_zone_move;

                data_storage_.selection_info.zone.reset();
            }
            else
            {
                data_storage_.menus.CursorMessage = true;
                data_storage_.menus.fields.cursor_message.elapse_at = time(0) + 5;
                data_storage_.menus.fields.cursor_message.message   = "Target RR have Zone with same name";
            }
        }

        if (fields.selected_railroad_for_zone_move.expired() || data_storage_.selection_info.zone.expired())
        {
            ImGui::EndDisabled();

            ImGui::SameLine();
            ImGui::Text("Select railroad and zone!");
        }
    }
    
    ImGui::Separator();


    // --- //

    ImGui::Checkbox("Hide Railroad", &railroad->hide);

    ImGui::Text("Amount of connected lines: %i", railroad->lines_connected);
    ImGui::End();
}
void Interface::show_player_info()
{
    if (data_storage_.menus.PlayerInfo == false)
        return;

    auto& fields = data_storage_.menus.fields.player_info;

    if (!fields.initialized &&
        !data_storage_.selection_info.player.expired())
    {
        fields.selected_player = data_storage_.selection_info.player;

        fields.new_nickname = data_storage_.selection_info.player.lock()->nickname;
        fields.new_discord  = data_storage_.selection_info.player.lock()->discord;

        fields.initialized = true;
    }
    else if (fields.selected_player.expired())
    {
        data_storage_.menus.PlayerInfo = false;

        fields.initialized = false;

        return;
    }


    auto player = data_storage_.menus.fields.player_info.selected_player.lock();

    ImGui::Begin("Player Info");


    // --- Change nickname

    ImGui::Text("Ingame nick: %s", player->nickname.c_str());

    ImGui::PushItemWidth(150);
    ImGui::InputText("##player_info_change_nickname", &data_storage_.menus.fields.player_info.new_nickname);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (ImGui::Button("Change##player_info_change_nickname"))
    {
        player->nickname = data_storage_.menus.fields.player_info.new_nickname;
    }


    // --- Change Discord nick

    ImGui::Text("Discord nick: %s", player->discord.c_str());

    ImGui::PushItemWidth(150);
    ImGui::InputText("##player_info_change_discord", &data_storage_.menus.fields.player_info.new_discord);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (ImGui::Button("Change##player_info_change_discord"))
    {
        player->discord = data_storage_.menus.fields.player_info.new_discord;
    }

    ImGui::Separator();


    // --- Associated with railroads

    bool railroad_is_selected = !data_storage_.selection_info.railroad.expired();

    ImGui::Text("Associated with:");

    ImGui::PushItemWidth(200);


    if (ImGui::BeginListBox("##player_info_associated_list"))
    {
        auto it = data_storage_.railroads_data.Railroads.begin();
        while (it != data_storage_.railroads_data.Railroads.end())
        {
            if ((*it)->have_associated(player->id))
            {
                bool selected = false;

                if (railroad_is_selected)
                    selected = (*it)->id == data_storage_.selection_info.railroad.lock()->id;

                if (ImGui::Selectable((*it)->name.c_str(), selected))
                {
                    data_storage_.selection_info.railroad = *it;

                    data_storage_.menus.fields.lines_edit   .initialized = false;
                    data_storage_.menus.fields.railroad_info.initialized = false;
                }
            }
            
            ++it;
        }

        ImGui::EndListBox();
    }
    ImGui::PopItemWidth();
    
    ImGui::SameLine();

    if (ImGui::Button("Show the\nbeginning\nof the RR##player_info") && railroad_is_selected)
    {
        if (!data_storage_.selection_info.railroad.lock()->starting_point.expired())
            data_storage_.camera.position = data_storage_.selection_info.railroad.lock()->starting_point.lock()->position;
        else
        {
            data_storage_.menus.fields.cursor_message.elapse_at = time(0) + 5;
            data_storage_.menus.fields.cursor_message.message = "Railroad doesn't have lines to show!";
            data_storage_.menus.CursorMessage = true;
        }
    }

    ImGui::Separator();


    // --- //

    if (ImGui::Button("Close##player_info"))
    {
        data_storage_.menus.PlayerInfo = false;
    }

    ImGui::End();
}
void Interface::show_length_top()
{
    if (data_storage_.menus.LengthTop == false)
        return;

    auto& fields = data_storage_.menus.fields.length_top;


    ImGui::Begin("Length top");

    ImGui::Text("Railroad Search: ");
    ImGui::InputText("##length_top_railroad_search", &fields.search_name);

    ImGui::Separator();

    if (ImGui::Button("Update") || !fields.initialized)
    {
        fields.total_length = 0;

        fields.railroad_keys.clear();
        fields.entries      .clear();

        for (auto& line : data_storage_.railroads_data.RR_Lines)
        {
            auto zone = line->zone.lock();

            switch (zone->state)
            {
            case RR_Zone::Built      : if (!fields.with_built       ) continue; break;
            case RR_Zone::InProgress : if (!fields.with_in_progress ) continue; break;
            case RR_Zone::Plan       : if (!fields.with_plans       ) continue; break;
            default:                                                  continue;
            }

            // --- //

            auto  railroad    = zone->railroad.lock();

            if (railroad->name == "World Border")
                continue;

            float line_length = utils::pif(line->pointA.lock()->position - line->pointB.lock()->position) * 2;

            if (fields.railroad_keys.contains(railroad->id))
                fields.entries.at(fields.railroad_keys.at(railroad->id)).length += line_length;
            else
            {
                fields.railroad_keys.insert({ railroad->id, (int)fields.entries.size() });
                fields.entries.push_back({ railroad, line_length });
            }

            fields.total_length += line_length;
        }

        fields.railroad_keys.clear();

        std::sort(fields.entries.begin(), fields.entries.end(),
                  [](const DataStorage::Menus::Fields::LengthTop::LengthTopEntry& left, 
                     const DataStorage::Menus::Fields::LengthTop::LengthTopEntry& right)
                    { return left.length > right.length; });

        fields.initialized = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Copy"))
    {
        sf::Clipboard clipboard;

        std::string as_string = "Total length: " + std::to_string((int)ceil(fields.total_length)) + "\n";
        int counter = 1;

        for (auto& entry : fields.entries)
            as_string += std::to_string(counter++) + ") " + entry.railroad.lock()->name + " - " + std::to_string((int)ceil(entry.length)) + "\n";

        clipboard.setString(as_string);
    }

    ImGui::SameLine();
    if (ImGui::Button("Close"))
    {
        data_storage_.menus.LengthTop = false;

        fields.initialized = false;
    }


    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::SameLine();
    if (ImGui::Checkbox("Built##length_top"      , &fields.with_built      )) fields.initialized = false;
    ImGui::SameLine();
    if (ImGui::Checkbox("In progress##length_top", &fields.with_in_progress)) fields.initialized = false;
    ImGui::SameLine();
    if (ImGui::Checkbox("Plan##length_top"       , &fields.with_plans      )) fields.initialized = false;
    
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical); 

    ImGui::SameLine();
    ImGui::Text("Total length: %i", (int)ceil(fields.total_length));

    if (ImGui::BeginChildFrame(ImGui::GetID("length_top_child"), { -FLT_MIN, -FLT_MIN }))
    {
        int counter = 0;

        ImGuiTableFlags table_flags =
            ImGuiTableFlags_RowBg           |
            ImGuiTableFlags_SizingFixedFit  |
            ImGuiTableFlags_ScrollX         |
            ImGuiTableFlags_ScrollY         |
            ImGuiTableFlags_NoSavedSettings |
            ImGuiTableFlags_Borders;

        if (ImGui::BeginTable("##length_top_table", 3, table_flags))
        {
            ImGui::TableSetupColumn("Position");
            ImGui::TableSetupColumn("Railroad Name");
            ImGui::TableSetupColumn("Length");

            ImGui::TableHeadersRow();

            for (auto& entry : fields.entries)
            {
                auto railroad = entry.railroad.lock();

                counter++;

                if (!fields.search_name.empty() && !utils::part_is_in_vector(fields.search_name, railroad->aka_names))
                    continue;

                ImGui::TableNextColumn();
                ImGui::Text("%i", counter);

                ImGui::TableNextColumn();
                ImGui::Text("%s", railroad->name.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%i", (int)ceil(entry.length));
            }

            ImGui::EndTable();
        }

        ImGui::EndChildFrame();
    }

    ImGui::End();
}

void Interface::show_cursor_message()
{
    if (data_storage_.menus.CursorMessage == false)
        return;

    auto& fields = data_storage_.menus.fields.cursor_message;

    if (fields.elapse_at <= time(0))
    {
        fields.message = "";

        data_storage_.menus.CursorMessage = false;
    }

    ImVec2 position{
        (float)window_.get_cursor_position().x + 4,
        (float)window_.get_cursor_position().y + 4
    };

    ImGui::SetNextWindowPos(  position,  ImGuiCond_Always);
    ImGui::SetNextWindowSize({ 250, 0 }, ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar      |
        ImGuiWindowFlags_NoResize        |
        ImGuiWindowFlags_NoMove          |
        ImGuiWindowFlags_NoCollapse      |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoDecoration;

    ImGui::Begin("##Message", NULL, flags);

    ImGui::TextWrapped(fields.message.c_str());

    ImGui::End();
}

void Interface::show_exit_popup()
{
    if (data_storage_.menus.ConfirmExit.should_be_open == true &&
        data_storage_.menus.ConfirmExit.is_open        == false)
    {
        ImGui::OpenPopup("Exit", ImGuiPopupFlags_NoOpenOverExistingPopup);

        data_storage_.menus.ConfirmExit.is_open = true;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Exit", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%s", "Are you sure?");
        ImGui::NewLine();
        ImGui::Separator();

        if (ImGui::Button("Quit and Save", ImVec2(120, 0)) || utils::key_pressed(sf::Keyboard::Enter))
        {
            data_storage_.menus.ConfirmExit.should_be_open = false;
            data_storage_.menus.ConfirmExit.is_open        = false;

            data_loader_.save_railroads_data();

            ImGui::CloseCurrentPopup();

            window_.close();
        }

        ImGui::SameLine();

        if (ImGui::Button("Quit and Don't Save", ImVec2(180, 0)))
        {
            data_storage_.menus.ConfirmExit.should_be_open = false;
            data_storage_.menus.ConfirmExit.is_open        = false;

            ImGui::CloseCurrentPopup();

            window_.close();
        }

        ImGui::SameLine();

        if (ImGui::Button("Stay", ImVec2(120, 0)) ||
            data_storage_.menus.ConfirmExit.should_be_open == false)
        {
            data_storage_.menus.ConfirmExit.should_be_open = false;
            data_storage_.menus.ConfirmExit.is_open = false;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
