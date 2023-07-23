#include "../../Include/DataOperations/DataStorage.h"


DataStorage::DataStorage()
{
    map_data.max_loading_threads = utils::minmax(1, std::thread::hardware_concurrency() - 1, 8); // '-1' to leave 1 thread for 'load_map()'
}

DataStorage::~DataStorage()
{
    
}


int DataStorage::RailroadsData::add_Player   (Player   player  )
{
    // --- Check of existance
    if (contains_Player_with_nick(player.nickname))
        return 0;

    // --- Give ID if doesn't have
    if (player.id == 0)
        player.id = max_player_ID;

    max_player_ID = std::max(player.id + 1, max_player_ID);

    // --- Adding to the list

    Players.push_back(std::make_shared<Player>(player));

    std::sort(Players.begin(), 
              Players.end(),
              [](const std::weak_ptr<Player>& left, const std::weak_ptr<Player>& right) 
                { return left.lock()->nickname < right.lock()->nickname; });

    return player.id;
}
int DataStorage::RailroadsData::add_Railroad (Railroad railroad)
{
    // --- Check of existance
    std::string name = railroad.name;

    if (contains_Railroad_with_name(name))
        return 0;

    // --- Give ID if doesn't have
    if (railroad.id == 0)
        railroad.id = max_railroad_ID;

    max_railroad_ID = std::max(railroad.id + 1, max_railroad_ID);
    
    // --- //
    if (railroad.aka_names.empty())
        railroad.aka_names.push_back(name);

    // --- Adding to the list

    Railroads.push_back(std::make_shared<Railroad>(railroad));

    return railroad.id;
}
int DataStorage::RailroadsData::add_RR_Zone  (RR_Zone  zone    )
{
    // --- Give ID if doesn't have
    if (zone.id == 0)
        zone.id = max_zone_ID;

    max_zone_ID = std::max(zone.id + 1, max_zone_ID);

    // --- Adding to the list

    RR_Zones.push_back(std::make_shared<RR_Zone>(zone));

    return zone.id;
}
int DataStorage::RailroadsData::add_RR_Line  (RR_Line  line    )
{
    // --- Give ID if doesn't have
    if (line.id == 0)
        line.id = max_line_ID;

    max_line_ID = std::max(line.id + 1, max_line_ID);

    // --- Adding to the list
    RR_Lines.push_back(std::make_shared<RR_Line>(line));

    auto railroad = line.zone.lock()->railroad.lock();
    
    if (railroad->lines_connected == 0)
        railroad->starting_point = line.pointA;

    railroad->lines_connected++;

    return line.id;
}
int DataStorage::RailroadsData::add_RR_Point (RR_Point point   )
{
    // --- Give ID if doesn't have
    if (point.id == 0)
        point.id = max_point_ID;

    max_point_ID = std::max(point.id + 1, max_point_ID);

    // --- Adding to the list
    RR_Points.push_back(std::make_shared<RR_Point>(point));

    return point.id;
}

bool DataStorage::RailroadsData::contains_Player_with_nick   (std::string nick)
{
    transform(nick.begin(), nick.end(), nick.begin(), ::tolower);

    for (auto& player : Players)
    {
        std::string list_element = player->nickname;

        transform(list_element.begin(), list_element.end(), list_element.begin(), ::tolower);

        if (list_element == nick)
            return true;
    }

    return false;
}
bool DataStorage::RailroadsData::contains_Railroad_with_name (std::string name)
{
    for (auto& railroad_element : Railroads)
    {
        if (utils::is_in_vector(name, railroad_element->aka_names))
            return true;
    }

    return false;
}
bool DataStorage::RailroadsData::contains_Zone_with_name     (std::string name, int railroad_id)
{
    for (auto& zone_element : RR_Zones)
    {
        if (zone_element->name == name &&
            zone_element->railroad.lock()->id == railroad_id)
        {
            return true;
        }   
    }

    return false;
}

std::shared_ptr<Player>   DataStorage::RailroadsData::get_Player_by_ID   (int player_id  )
{
    for (auto& player : Players)
        if (player->id == player_id)
            return player;

    return nullptr;
}
std::shared_ptr<Railroad> DataStorage::RailroadsData::get_Railroad_by_ID (int railroad_id)
{
    for (auto& railroad : Railroads)
        if (railroad->id == railroad_id)
            return railroad;

    return nullptr;
}
std::shared_ptr<RR_Zone>  DataStorage::RailroadsData::get_RR_Zone_by_ID  (int rr_zone_id )
{
    for (auto& zone : RR_Zones)
        if (zone->id == rr_zone_id)
            return zone;

    return nullptr;
}
std::shared_ptr<RR_Line>  DataStorage::RailroadsData::get_RR_Line_by_ID  (int rr_line_id )
{
    for (auto& line : RR_Lines)
        if (line->id == rr_line_id)
            return line;

    return nullptr;
}
std::shared_ptr<RR_Point> DataStorage::RailroadsData::get_RR_Point_by_ID (int rr_point_id)
{
    for (auto& point : RR_Points)
        if (point->id == rr_point_id)
            return point;

    return nullptr;
}

std::shared_ptr<Player>   DataStorage::RailroadsData::get_Player_by_nick (const std::string& nickname)
{
    for (auto& player : Players)
        if (player->nickname == nickname)
            return player;

    return nullptr;
}

void DataStorage::RailroadsData::erase_Player_by_ID   (int player_id  )
{
    for(auto it = Players.begin(); it != Players.end(); ++it)
    {
        if ((*it)->id == player_id)
        {
            it = Players.erase(it);
            return;
        }
    }
}
void DataStorage::RailroadsData::erase_Railroad_by_ID (int railroad_id)
{
    for (auto it = Railroads.begin(); it != Railroads.end(); ++it)
    {
        if ((*it)->id == railroad_id)
        {
            it = Railroads.erase(it);
            return;
        }
    }
}
void DataStorage::RailroadsData::erase_RR_Zone_by_ID  (int rr_zone_id )
{
    for (auto it = RR_Zones.begin(); it != RR_Zones.end(); ++it)
    {
        if ((*it)->id == rr_zone_id)
        {
            it = RR_Zones.erase(it);
            return;
        }
    }
}
void DataStorage::RailroadsData::erase_RR_Line_by_ID  (int rr_line_id )
{
    for (auto it = RR_Lines.begin(); it != RR_Lines.end(); ++it)
    {
        if ((*it)->id == rr_line_id)
        {
            (*it)->zone.lock()->railroad.lock()->lines_connected--;

            it = RR_Lines.erase(it);
            return;
        }
    }
}
void DataStorage::RailroadsData::erase_RR_Point_by_ID (int rr_point_id)
{
    for (auto it = RR_Points.begin(); it != RR_Points.end(); ++it)
    {
        if ((*it)->id == rr_point_id)
        {
            it = RR_Points.erase(it);
            return;
        }
    }
}

void DataStorage::MapData::reset_images_states()
{
    while (loading_images.load() == true) {}

    for (int vertical = 0; vertical < SPRITES_AMOUNT_VERTICAL; ++vertical)
    {
        for (int horizontal = 0; horizontal < SPRITES_AMOUNT_HORIZONTAL; ++horizontal)
        {
            int index = vertical * SPRITES_AMOUNT_HORIZONTAL + horizontal;

            map_images[index].current_map_spr.reset();
            map_images[index].current_map_tex.reset();

            map_images[index].difference_map_spr.reset();
            map_images[index].difference_map_tex.reset();

            map_images[index].old_map_spr.reset();
            map_images[index].old_map_tex.reset();

            map_images[index].container_state = ImageContainer::EMPTY;
        }
    }
}
