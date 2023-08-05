#pragma once
#include "../../Include/Utils/Functions.h"
#include "../../Include/Modules/RailroadBricks.h"

#include "ImageContainer.h"

#include <thread>


#define SPRITES_AMOUNT_HORIZONTAL 10
#define SPRITES_AMOUNT_VERTICAL   10


struct DataStorage
{
    DataStorage();
    DataStorage(DataStorage&) = delete;
    DataStorage(DataStorage&&) = delete;

    ~DataStorage();

    // --- //

    sf::Vector2i screen_size;

    sf::Clock deltaClock;

    // --- Interface

    struct SelectionInfo
    {
        std::weak_ptr<Railroad> railroad;
        std::weak_ptr<RR_Zone>  zone;

        std::weak_ptr<Player>   player;

        std::weak_ptr<RR_Point> point;
    } selection_info;

    struct Menus 
    {
        bool Main          = true ;
        bool Help          = false;
        bool LinesEdit     = true ;
        bool Railroads     = true ;
        bool Players       = true ;
        bool Settings      = true ;
        bool Debug         = false;
        
        bool RailroadInfo  = false;
        bool PlayerInfo    = false;
        bool LengthTop     = false;

        bool CursorMessage = false;

        struct Fields
        {
            struct LinesEdit
            {
                bool initialized = false;

                std::string new_RR_name = "";

                float new_RR_color_3f[3] = { 1.f, 1.f, 1.f };
                sf::Color saved_RR_color;

                // - Editing Settings
                bool align_at_creation = true;
                bool align_at_movement = true;
                bool auto_line_repeat  = true;

                bool show_lines  = true;
                bool show_points = true;

                bool show_Plans      = false;
                bool show_InProgress = false;
                bool show_Built      = true ;

            } lines_edit;

            struct Railroads
            {
                std::string add_new_railroad_name = "";

                std::string railroads_search = "";
            } railroads;
            struct Players
            {
                std::string add_new_player_nick = "";

                std::string players_search = "";
            } players;

            struct Settings
            {
                int goto_2f[2] = { 0 };
            } settings;

            struct RailroadInfo
            {
                bool initialized = false;

                std::string selected_aka_name = "";

                std::string all_players_search        = "";
                std::string associated_players_search = "";

                std::string AKA_names_add_name = "";
                std::string zone_add_name      = "";
                std::string rename_zone        = "";

                bool delete_zone     = false;
                bool delete_railroad = false;

                std::string railroads_search   = "";
                std::weak_ptr<Railroad> selected_railroad_for_zone_move;
            } railroad_info;
            struct PlayerInfo
            {
                bool initialized = false;

                std::weak_ptr<Player> selected_player;

                std::string new_nickname = "";
                std::string new_discord  = "";
            } player_info;
            struct LengthTop 
            {
                bool initialized = false;

                struct LengthTopEntry
                {
                    std::weak_ptr<Railroad> railroad;

                    float length = 0.f;

                    size_t top_position = 0;

                    bool existed_before = false; // for new entries in DB because missed before
                };

                struct LengthsData 
                {
                    std::vector        <LengthTopEntry> entries;       // vector_pos -> entry
                    std::unordered_map <int, size_t>    railroad_keys; // rr_id      -> vector_pos

                    float total_length = 0;
                };

                LengthsData current_data;
                LengthsData old_data;

                std::string search_name = "";

                bool with_built       = true ;
                bool with_in_progress = false;
                bool with_plans       = false;
            } length_top;

            struct CursorMessage
            {
                std::string message = "";

                time_t elapse_at = time(0);
            } cursor_message;
        } fields;

        utils::PopUpState ConfirmExit;
    } menus;

    struct Settings 
    { 
        bool autosave = false; 
        int  autosave_interval_in_minutes  = 3;

        bool show_hidden_RRs = false;
        bool super_scale     = false; 
        bool hide_map        = false; 

        bool disable_interface_on_screenshot = false;

        enum class MapView
        {
            CURRENT,
            DIFFERENCE,
            OLD
        } map_view = MapView::CURRENT;

        float background_color_3f[3] = { 0 };
        sf::Color background_color   = { 0, 0 ,0 ,255 };

        // --- Debug

        bool length_top_manipulations = false;
    } settings;

    struct Status
    {
        time_t start_time_for_autosave = time(0);

        bool mouse_captured    = false;
        bool keyboard_captured = false;

        enum class MovementMode
        {
            IDLE              = 0,
            MovingCamera      = 1,
            MovingPoint       = 2,
            RailroadSelection = 3
        } movement_mode = MovementMode::IDLE;

        enum class EditMode
        {
            IDLE     = 0,
            AddPoint = 1,
            AddLine  = 2
        } edit_mode = EditMode::IDLE;

        std::weak_ptr<RR_Line>  overlapped_line;
        std::weak_ptr<RR_Point> overlapped_point;

        sf::Vector2i mouse_position_on_click  = { 0, 0 };
        sf::Vector2i camera_position_on_click = { 0, 0 };
        sf::Vector2i point_position_on_click  = { 0, 0 };

        int saving_counter { 0 };
        int saving_total   { 0 };

        bool make_screenshot         = false;
        int  screenshot_frames_delay = 0;
    } status;

    struct Camera
    {
        sf::Vector2i position{ 0, 0 };

        float scale_modifier = 0.0;
        float scale_modifier_as_pow2 = 1;
        std::string Scales[6] = { "0.25x", "0.5x", "1x", "2x", "4x", "8x" };
    } camera;


    // --- Renderer

    class MapData
    {
    public:
        // - Images
        const sf::Vector2i sprite_size          { 3584u, 3584u };
        const sf::Vector2i sprite_0_0_pos_shift {  512u, 3488u };
        const sf::Vector2i sprite_with_0_0_pos  {    5 ,    4  };

        sf::Vector2i central_view_sector { sprite_with_0_0_pos };
        sf::Vector2i view_from           { sprite_with_0_0_pos };
        sf::Vector2i view_to             { sprite_with_0_0_pos };

        ImageContainer map_images[SPRITES_AMOUNT_VERTICAL * SPRITES_AMOUNT_HORIZONTAL];

        // - Threads
        int max_loading_threads = 8;
        std::atomic<int> threads_counter{ 0 };

        std::atomic<bool> loading_images{ false };

        // --- //

        void reset_images_states();
    } map_data;

    class RailroadsData
    {
    public:
        std::vector<std::shared_ptr<Player>>   Players;
        std::vector<std::shared_ptr<Railroad>> Railroads;
        std::vector<std::shared_ptr<RR_Zone>>  RR_Zones;
        std::vector<std::shared_ptr<RR_Line>>  RR_Lines;
        std::vector<std::shared_ptr<RR_Point>> RR_Points;

        int max_player_ID   = 1;
        int max_railroad_ID = 1;
        int max_line_ID     = 1;
        int max_point_ID    = 1;
        int max_zone_ID     = 1;


        int add_Player   (Player   player  );
        int add_Railroad (Railroad railroad);
        int add_RR_Zone  (RR_Zone  zone    );
        int add_RR_Line  (RR_Line  line    );
        int add_RR_Point (RR_Point point   );

        bool contains_Player_with_nick   (std::string name);
        bool contains_Railroad_with_name (std::string name);
        bool contains_Zone_with_name     (std::string name, int railroad_id);

        std::shared_ptr<Player>   get_Player_by_ID   (int player_id  );
        std::shared_ptr<Railroad> get_Railroad_by_ID (int railroad_id);
        std::shared_ptr<RR_Zone>  get_RR_Zone_by_ID  (int rr_zone_id );
        std::shared_ptr<RR_Line>  get_RR_Line_by_ID  (int rr_line_id );
        std::shared_ptr<RR_Point> get_RR_Point_by_ID (int rr_point_id);

        std::shared_ptr<Player>   get_Player_by_nick(const std::string& nickname);

        void erase_Player_by_ID   (int player_id  );
        void erase_Railroad_by_ID (int railroad_id);
        void erase_RR_Zone_by_ID  (int rr_zone_id );
        void erase_RR_Line_by_ID  (int rr_line_id );
        void erase_RR_Point_by_ID (int rr_point_id);
    } railroads_data;
};
