#pragma once
#include "../../Include/Utils/Functions.h"

#include <nlohmann/json.hpp>


struct DataStorage;

class RR_Point;
class RR_Line;


class Player
{
public:
    Player(int id, const std::string& nickname, const std::string& discord = "");
    Player();

public:
    int id = 0;

    std::string nickname;
    std::string discord;


};

class Railroad
{
public:
    Railroad(int id, const std::string& name, const sf::Color& color);
    Railroad();

    void add_associated(std::weak_ptr<Player> player);
    bool have_associated(int player_id);

    void add_aka_name(std::string name);
    void remove_aka_name(std::string name);

    void erase_from_lists(DataStorage& data_storage);

public:
    int id = 0;
    std::string name = ""; 
    std::vector<std::string> aka_names = {};
    
    sf::Color color;
    
    std::vector<std::weak_ptr<Player>> associated;

    bool hide = false;

    int lines_connected = 0;

    std::weak_ptr<RR_Point> starting_point;
};

class RR_Zone
{
public:
    enum ZoneState
    {
        ErrState   = 0,
        Plan       = 1,
        InProgress = 2,
        Built      = 3
    } state = ZoneState::Built;

    static ZoneState ZoneState_from_int(int state);

public:
    RR_Zone(int id, const std::string& name, ZoneState state, const std::weak_ptr<Railroad>& railroad);
    RR_Zone();

    void erase_from_lists(DataStorage& data_storage);

public:
    int id = 0;
    std::string name = "";

    std::weak_ptr<Railroad> railroad;
};

class RR_Line
{
public:
    RR_Line(int id, const std::weak_ptr<RR_Point>& pointA, const std::weak_ptr<RR_Point>& pointB, 
            const std::weak_ptr<RR_Zone>& zone);
    RR_Line();

    void erase_from_lists(DataStorage& data_storage, bool erase_from_points = true);

public:
    int id = 0;

    std::weak_ptr<RR_Point> pointA, pointB;
    std::weak_ptr<RR_Zone>  zone;
};

class RR_Point
{
public:
    RR_Point(int id, const sf::Vector2i& position);
    RR_Point();

    void add_connected_line(std::weak_ptr<RR_Line> line);
    void remove_connected_line(int line_id);
    void erase_from_lists(DataStorage& data_storage);

public:
    int id = 0;

    sf::Vector2i position; 
    std::vector<std::weak_ptr<RR_Line>> connected_lines;
};


void link_points_and_line(std::weak_ptr<RR_Point> pointA, std::weak_ptr<RR_Point> pointB, std::weak_ptr<RR_Line> line);


// --- JSON De/Serialization

void to_json(nlohmann::json& json, const Player& player);
void from_json(const nlohmann::json& json, Player& player);

void to_json(nlohmann::json& json, const Railroad& railroad);
void from_json(const nlohmann::json& json, Railroad& railroad, DataStorage& data_storage_);

void to_json(nlohmann::json& json, const RR_Point& point);
void from_json(const nlohmann::json& json, RR_Point& point);

void to_json(nlohmann::json& json, const RR_Zone& zone);
void from_json(const nlohmann::json& json, RR_Zone& zone, DataStorage& data_storage_);

void to_json(nlohmann::json& json, const RR_Line& line);
void from_json(const nlohmann::json& json, RR_Line& line, DataStorage& data_storage_);
