#include "../../Include/Modules/RailroadBricks.h"
#include "../../Include/DataOperations/DataStorage.h"


Player::Player(int id, const std::string& nickname, const std::string& discord)
    : id(id), nickname(nickname), discord(discord) {}
Player::Player() {}

Railroad::Railroad(int id, const std::string& name, const sf::Color& color)
    : id(id), name(name), color(color) {}
Railroad::Railroad() {}

RR_Zone::RR_Zone(int id, const std::string& name, ZoneState state, const std::weak_ptr<Railroad>& railroad)
    : id(id), name(name), state(state), railroad(railroad) {}
RR_Zone::RR_Zone() {}

RR_Line::RR_Line(int id, const std::weak_ptr<RR_Point>& pointA, const std::weak_ptr<RR_Point>& pointB, 
                 const std::weak_ptr<RR_Zone>& zone)
    : id(id), pointA(pointA), pointB(pointB), zone(zone) {}
RR_Line::RR_Line() {}

RR_Point::RR_Point(int id, const sf::Vector2i& position)
    : id(id), position(position) {}
RR_Point::RR_Point() {}

// --- //

void Railroad::add_associated(std::weak_ptr<Player> player)
{
    associated.push_back(player);

    std::sort(associated.begin(),
              associated.end(),
              [](const std::weak_ptr<Player>& left, const std::weak_ptr<Player>& right) 
                { return left.lock()->nickname < right.lock()->nickname; });
}
bool Railroad::have_associated(int player_id)
{
    for (auto& player : this->associated)
        if (!player.expired() && player.lock()->id == player_id)
            return true;

    return false;
}

void Railroad::add_aka_name(std::string name)
{
    aka_names.push_back(name);

    std::sort(aka_names.begin(),
              aka_names.end(),
              [](const std::string& left, const std::string& right) 
                { return left < right; });
}
void Railroad::remove_aka_name(std::string name)
{
    for (auto it = aka_names.begin(); it != aka_names.end(); ++it)
    {
        if (*it == name)
        {
            it = aka_names.erase(it);
            break;
        }
    }

    std::sort(aka_names.begin(),
              aka_names.end(),
              [](const std::string& left, const std::string& right) 
                { return left < right; });
}

void Railroad::erase_from_lists(DataStorage& data_storage)
{
    std::vector<std::shared_ptr<RR_Zone>> zones_to_erase;

    for (auto& zone : data_storage.railroads_data.RR_Zones)
        if (zone->railroad.lock()->id == this->id)
            zones_to_erase.push_back(zone);

    for (auto& zone : zones_to_erase)
        zone->erase_from_lists(data_storage);

    zones_to_erase.clear();

    data_storage.railroads_data.erase_Railroad_by_ID(this->id);
}

RR_Zone::ZoneState RR_Zone::ZoneState_from_int(int state)
{
    switch (state)
    {
    case 1:  return RR_Zone::ZoneState::Plan       ;
    case 2:  return RR_Zone::ZoneState::InProgress ;
    case 3:  return RR_Zone::ZoneState::Built      ;
    default: return RR_Zone::ZoneState::ErrState   ;
    }
}

void RR_Zone::erase_from_lists(DataStorage& data_storage)
{
    auto it_points = data_storage.railroads_data.RR_Points.begin();

    while (it_points != data_storage.railroads_data.RR_Points.end())
    {
        auto& point = *it_points;

        auto it_lines = point->connected_lines.begin();

        while (it_lines != point->connected_lines.end())
        {
            if (it_lines->expired())
            {
                it_lines = point->connected_lines.erase(it_lines);

                continue;
            }

            auto line = it_lines->lock();

            if (line->zone.lock()->id == this->id)
            {
                line->erase_from_lists(data_storage, false);

                it_lines = point->connected_lines.erase(it_lines);

                continue;
            }

            ++it_lines;
        }

        if (point->connected_lines.empty())
        {
            it_points = data_storage.railroads_data.RR_Points.erase(it_points);

            continue;
        }

        ++it_points;
    }

    data_storage.railroads_data.erase_RR_Zone_by_ID(this->id);
}

void RR_Line::erase_from_lists(DataStorage& data_storage, bool erase_from_points)
{
    if (erase_from_points)
    {
        if (!this->pointA.expired()) this->pointA.lock()->remove_connected_line(this->id);
        if (!this->pointB.expired()) this->pointB.lock()->remove_connected_line(this->id);
    }

    data_storage.railroads_data.erase_RR_Line_by_ID(this->id);
}

void RR_Point::add_connected_line(std::weak_ptr<RR_Line> line)
{
    connected_lines.push_back(line);
}
void RR_Point::remove_connected_line(int line_id)
{
    for (auto it = connected_lines.begin(); it != connected_lines.end();)
    {
        if (!it->expired() && it->lock()->id == line_id)
        {
            it = connected_lines.erase(it);
            break;
        }

        ++it;
    }
}
void RR_Point::erase_from_lists(DataStorage& data_storage)
{
    while (!this->connected_lines.empty())
    {
        auto line = this->connected_lines.begin();

        line->lock()->erase_from_lists(data_storage);
    }

    data_storage.railroads_data.erase_RR_Point_by_ID(this->id);
}

void link_points_and_line(std::weak_ptr<RR_Point> pointA, std::weak_ptr<RR_Point> pointB, std::weak_ptr<RR_Line> line)
{
    pointA.lock()->add_connected_line(line);
    pointB.lock()->add_connected_line(line);
}


// --- JSON De/Serialization

void to_json(nlohmann::json& json, const Player& player)
{
    json = {
        { "id"      , player.id       },
        { "nickname", player.nickname },
        { "discord" , player.discord  }
    };
}
void from_json(const nlohmann::json& json, Player& player)
{
    json.at("id"      ).get_to(player.id      );
    json.at("nickname").get_to(player.nickname);
    json.at("discord" ).get_to(player.discord );
}

void to_json(nlohmann::json& json, const Railroad& railroad)
{
    nlohmann::json color_json { 
        { "r", railroad.color.r },
        { "g", railroad.color.g },
        { "b", railroad.color.b }
    };

    nlohmann::json associated_json;

    for (auto& player : railroad.associated)
        associated_json.push_back(player.lock()->id);

    json = {
        { "id"        , railroad.id        },
        { "name"      , railroad.name      },
        { "aka_names" , railroad.aka_names },
        { "color"     , color_json         },
        { "associated", associated_json    }
    };
}
void from_json(const nlohmann::json& json, Railroad& railroad, DataStorage& data_storage_)
{
    nlohmann::json color_json;
    nlohmann::json associated_json;

    json.at("id"        ).get_to(railroad.id        );
    json.at("name"      ).get_to(railroad.name      );
    json.at("aka_names" ).get_to(railroad.aka_names );
    json.at("color"     ).get_to(color_json         );
    json.at("associated").get_to(associated_json    );

    color_json.at("r").get_to(railroad.color.r);
    color_json.at("g").get_to(railroad.color.g);
    color_json.at("b").get_to(railroad.color.b);

    for (auto& id_json : associated_json)
    {
        int id = id_json.get<int>();

        railroad.associated.push_back(data_storage_.railroads_data.get_Player_by_ID(id));
    }
}

void to_json(nlohmann::json& json, const RR_Point& point)
{
    nlohmann::json position_json {
        { "x", point.position.x },
        { "y", point.position.y }
    };

    json = {
        { "id"      , point.id      },
        { "position", position_json }
    };
}
void from_json(const nlohmann::json& json, RR_Point& point)
{
    nlohmann::json position_json;

    json.at("id"      ).get_to(point.id     );
    json.at("position").get_to(position_json);

    position_json.at("x").get_to(point.position.x);
    position_json.at("y").get_to(point.position.y);
}

void to_json(nlohmann::json& json, const RR_Zone& zone)
{
    json = {
        {"id"      , zone.id                  },
        {"name"    , zone.name                },
        {"railroad", zone.railroad.lock()->id },
        {"state"   , zone.state               }
    };
}
void from_json(const nlohmann::json& json, RR_Zone& zone, DataStorage& data_storage_)
{
    int railroad_id;

    json.at("id"      ).get_to(zone.id    );
    json.at("name"    ).get_to(zone.name  );
    json.at("railroad").get_to(railroad_id);
    json.at("state"   ).get_to(zone.state );

    zone.railroad = data_storage_.railroads_data.get_Railroad_by_ID(railroad_id);
}

void to_json(nlohmann::json& json, const RR_Line& line)
{
    json = {
        { "id"      , line.id                  },
        { "pointA"  , line.pointA  .lock()->id },
        { "pointB"  , line.pointB  .lock()->id },
        { "zone"    , line.zone    .lock()->id }
    };
}
void from_json(const nlohmann::json& json, RR_Line& line, DataStorage& data_storage_)
{
    int pointA_id, pointB_id, zone_id;

    json.at("id").get_to(line.id);

    json.at("pointA"  ).get_to(pointA_id  );
    json.at("pointB"  ).get_to(pointB_id  );
    json.at("zone"    ).get_to(zone_id    );

    line.pointA   = data_storage_.railroads_data.get_RR_Point_by_ID(pointA_id  );
    line.pointB   = data_storage_.railroads_data.get_RR_Point_by_ID(pointB_id  );
    line.zone     = data_storage_.railroads_data.get_RR_Zone_by_ID (zone_id    );
}
