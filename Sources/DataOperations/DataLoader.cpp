#include "../../Include/DataOperations/DataLoader.h"


DataLoader::DataLoader(DataStorage& data_storage)
    : data_storage_(data_storage) {}
DataLoader::~DataLoader()
{
    
}


DataLoader::VersionID DataLoader::get_version_ID(std::string version_string)
{
    if (version_string == "==== Players list ====")
        return DataLoader::VersionID::v1_0;

    if (version_string == "Version:1.0")
        return DataLoader::VersionID::v1_0;

    if (version_string == "Version:2.0")
        return DataLoader::VersionID::v2_0;

    return DataLoader::VersionID::vERR;
}

DataLoader::VersionID DataLoader::latest_data_file_version()
{
    return VersionID::v2_0;
}

std::string DataLoader::to_string(VersionID v_id)
{
    switch (v_id)
    {
    case DataLoader::VersionID::v1_0: return "v1.0";
    case DataLoader::VersionID::v2_0: return "v2.0";
    default:                          return "vERR";
    }
}


int DataLoader::init()
{
    if (utils::file_exists("Data/settings.json"))
    {
        std::ifstream settings("Data/settings.json", std::ios::in);
        nlohmann::json settings_json;

        bool error_appeared = false;
        try
        {
            settings_json = nlohmann::json::parse(settings);
        }
        catch (const std::exception& err)
        {
            error_appeared = true;
            std::cout << err.what();
        }

        settings.close();

        if (error_appeared)
            return 1;

        if (settings_json.contains("maps_path"))
            settings_json["maps_path"].get_to(this->map_path_);
        else
        {
            std::cout << "Wrong settings format (can't find 'maps_path' field)" << std::endl;

            return 1;
        }
    }
    else
    {
        std::cout << "Can't find 'settings.json'!" << std::endl;

        return 1;
    }

    return 0;
}

int  DataLoader::load_railroads_data(const std::string& path)
{
    int result = 1; // 0 - Success

    if (utils::file_exists(path))
    {
        std::ifstream fin(path, std::ios::in | std::ios::binary);

        std::string line;
        utils::getline(fin, line);

        auto ver = get_version_ID(line);

        std::cout << "Data file version: " << to_string(ver);

        // --- Version defined message
        switch (ver)
        {
        case DataLoader::VersionID::v1_0:
            std::cout << " (will be converted to " << to_string(latest_data_file_version()) << " after save)" << std::endl;
            break;
        case DataLoader::VersionID::v2_0:
            std::cout << std::endl;
            break;
        default:
            std::cout << " (Error reading file)" << std::endl;
            break;
        }
        std::cout << std::endl;

        // --- Version defined loader
        switch (ver)
        {
        case DataLoader::VersionID::v1_0:
            result = load_RRs_v1_0(path);
            break;
        case DataLoader::VersionID::v2_0:
            result = load_RRs_v2_0(fin);
            break;
        default:
            break;
        }

        fin.close();
    }
    else
    {
        std::cout << "Can't find './Data/data.rr' file!" << std::endl;
    }

    return result;
}
void DataLoader::save_railroads_data(const std::string& path)
{
    std::cout << "\nSaving ... ";
    // --- Convert data to json
    nlohmann::json data {
        { "Players",   nlohmann::json{} },
        { "Railroads", nlohmann::json{} },
        { "RR_Points", nlohmann::json{} },
        { "RR_Zones",  nlohmann::json{} },
        { "RR_Lines",  nlohmann::json{} }
    };

    save_players   ( data.at("Players"  ) );
    save_railroads ( data.at("Railroads") );
    save_points    ( data.at("RR_Points") );
    save_zones     ( data.at("RR_Zones" ) );
    save_lines     ( data.at("RR_Lines" ) );

    std::vector<std::uint8_t> v_ubjson = nlohmann::json::to_ubjson(data);
    

    // --- Save to file
    std::ofstream file(path, std::ios::out | std::ios::binary);

    file << "Version:2.0\r\n";

    file.write(reinterpret_cast<const char*>(v_ubjson.data()), v_ubjson.size());

    file.close();

    std::cout << "Done!" << std::endl;
}

void DataLoader::load_map(sf::Vector2i from, sf::Vector2i to)
{
    from.x = std::max(0, from.x);
    from.y = std::max(0, from.y);

    to.x = std::min(to.x, SPRITES_AMOUNT_HORIZONTAL - 1);
    to.y = std::min(to.y, SPRITES_AMOUNT_VERTICAL   - 1);

    auto& map_data = data_storage_.map_data;

    for (int vertical = from.y; vertical <= to.y; ++vertical)
    {
        for (int horizontal = from.x; horizontal <= to.x; ++horizontal)
        {
            int index = vertical * SPRITES_AMOUNT_HORIZONTAL + horizontal;

            if (map_data.map_images[index].container_state.load() != ImageContainer::EMPTY)
                continue;

            // --- Wait in loading queue
            while (map_data.threads_counter.load() >= map_data.max_loading_threads) {}

            // --- Submit loading
            ++map_data.threads_counter;

            map_data.map_images[index].container_state.store(ImageContainer::LOADING);

            // --- Prepare memory
            map_data.map_images[index].current_map_tex = { std::make_shared<sf::Texture>() };
            map_data.map_images[index].current_map_spr = { std::make_shared<sf::Sprite> () };

            map_data.map_images[index].difference_map_tex = { std::make_shared<sf::Texture>() };
            map_data.map_images[index].difference_map_spr = { std::make_shared<sf::Sprite> () };

            map_data.map_images[index].old_map_tex = { std::make_shared<sf::Texture>() };
            map_data.map_images[index].old_map_spr = { std::make_shared<sf::Sprite> () };

            // --- Launch thread
            std::thread load_image_thread(&DataLoader::load_image, this, sf::Vector2i{horizontal, vertical});
            load_image_thread.detach();
        }
    }

    while (data_storage_.map_data.threads_counter > 0) {}

    map_data.loading_images = false;
}
void DataLoader::load_image(sf::Vector2i tile)
{
    int index = tile.y * SPRITES_AMOUNT_HORIZONTAL + tile.x;

    auto path_to_current    = CTP_current_map    (tile.x, 9 - tile.y, map_path_);
    auto path_to_difference = CTP_difference_map (tile.x, 9 - tile.y, map_path_);
    auto path_to_old        = CTP_old_map        (tile.x, 9 - tile.y, map_path_);

    // --- //

    if (utils::file_exists(path_to_current))
    {
        data_storage_.map_data.map_images[index].current_map_tex->loadFromFile(path_to_current);
        data_storage_.map_data.map_images[index].current_map_spr->setTexture(
            *data_storage_.map_data.map_images[index].current_map_tex, true
        );
    }
    else
        std::cout << ("File doesn't exist! '" + path_to_current + "'") << std::endl;

    if (utils::file_exists(path_to_difference))
    {
        data_storage_.map_data.map_images[index].difference_map_tex->loadFromFile(path_to_difference);
        data_storage_.map_data.map_images[index].difference_map_spr->setTexture(
            *data_storage_.map_data.map_images[index].difference_map_tex, true
        );
    }
    else
        std::cout << ("File doesn't exist! '" + path_to_difference + "'") << std::endl;

    if (utils::file_exists(path_to_old))
    {
        data_storage_.map_data.map_images[index].old_map_tex->loadFromFile(path_to_old);
        data_storage_.map_data.map_images[index].old_map_spr->setTexture(
            *data_storage_.map_data.map_images[index].old_map_tex, true
        );
    }
    else
        std::cout << ("File doesn't exist! '" + path_to_old + "'") << std::endl;

    // --- //

    data_storage_.map_data.map_images[index].container_state = ImageContainer::LOADED;

    --data_storage_.map_data.threads_counter;
}


int DataLoader::load_RRs_v1_0(const std::string& path)
{
    std::ifstream file(path, std::ios::in);
    std::string str;


    // --- Players --- //

    getline(file, str); // Skip area name

    int player_id = 1;
    while (true) {
        getline(file, str);
        
        if (str == "") 
            break;
        
        size_t next = str.find("|");

        std::string nickname { str.substr(0, next)  };
        std::string discord  { str.substr(next + 1) };

        data_storage_.railroads_data.Players.push_back(
            std::make_shared<Player>(player_id++, nickname, discord)
        );

        data_storage_.railroads_data.max_player_ID = player_id;
    }

    std::sort(data_storage_.railroads_data.Players.begin(), 
              data_storage_.railroads_data.Players.end(),
              [](const std::weak_ptr<Player>& left, const std::weak_ptr<Player>& right) 
                { return left.lock()->nickname < right.lock()->nickname; });

    std::cout << "Players loaded: " << data_storage_.railroads_data.Players.size() << std::endl;


    // --- Railroads --- //

    getline(file, str); // Skip area name


    while (true) {
        getline(file, str);

        if (str == "")
            break;

        size_t prev = 0;
        size_t next;

        std::string b[4];
        int jk = 0;

        while ((next = str.find("|", prev)) != std::string::npos) {
            b[jk] = str.substr(prev, next - prev);
            prev = next + 1;
            jk++;
        }
        
        int railroad_id           { stoi(b[0]) };
        std::string railroad_name { "" };
        sf::Color railroad_color  { (sf::Uint8)stoi(b[1]), (sf::Uint8)stoi(b[2]), (sf::Uint8)stoi(b[3]), 255 };


        data_storage_.railroads_data.Railroads.push_back(
            std::make_shared<Railroad>(railroad_id, railroad_name, railroad_color)
        );

        data_storage_.railroads_data.max_railroad_ID = std::max(railroad_id + 1, data_storage_.railroads_data.max_railroad_ID);
    }

    std::cout << "Railroads loaded: " << data_storage_.railroads_data.Railroads.size() << std::endl;


    // ---- RR Lines (preparation - 1/2) ---- //

    getline(file, str); // Skip area name

    std::vector<std::string> rr_lines;

    while (true) {
        getline(file, str);

        if (str == "")
            break;

        rr_lines.push_back(str);
    }

    std::cout << "Lines prepared (From v1.0 layout)\n";


    // ---- RR Points ---- //

    getline(file, str); // Skip area name

    while (true) {
        getline(file, str);

        if (str == "")
            break;

        size_t prev = 0;
        size_t next;

        std::string b[5];
        int jk = 0;

        while ((next = str.find("|", prev)) != std::string::npos) {
            b[jk] = str.substr(prev, next - prev);
            prev = next + 1;
            jk++;
        }

        int point_id = stoi(b[4]);

        sf::Vector2i point_position { stoi(b[0]), stoi(b[1]) + 25 }; // Fix Y error of MapV1

        data_storage_.railroads_data.RR_Points.push_back(
            std::make_shared<RR_Point>(point_id, point_position)
        );

        data_storage_.railroads_data.max_point_ID = std::max(point_id + 1, data_storage_.railroads_data.max_point_ID);
    }

    std::cout << "Points loaded: " << data_storage_.railroads_data.RR_Points.size() << std::endl;


    // ---- Railroads AKA Names ---- //

    getline(file, str); // Skip area name

    while (true) {
        getline(file, str);

        if (str == "")
            break;

        size_t next = str.find("|", 0);
        size_t prev = next + 1;

        int railroad_id = stoi(str.substr(0, next));

        next = str.find("|", prev);

        std::string railroad_name = str.substr(prev, next - prev);

        prev = next + 1;

        auto railroad = data_storage_.railroads_data.get_Railroad_by_ID(railroad_id);

        railroad->name = railroad_name;
        railroad->aka_names.push_back(railroad_name);


        while ((next = str.find("|", prev)) != std::string::npos) {
            railroad->aka_names.push_back(str.substr(prev, next - prev));
            prev = next + 1;
        }
        
        std::sort(railroad->aka_names.begin(),
                  railroad->aka_names.end(),
                  [](const std::string& left, const std::string& right) 
                    { return left < right; });
    }

    std::cout << "Aka Names loaded\n";


    // ---- Players association ---- //

    getline(file, str); // Skip area name

    while (true) {
        getline(file, str);

        if (str == "")
            break;

        size_t next = str.find("|", 0);
        size_t prev = next + 1;

        int railroad_id = stoi(str.substr(0, str.find("|")));

        auto railroad = data_storage_.railroads_data.get_Railroad_by_ID(railroad_id);

        while ((next = str.find("|", prev)) != std::string::npos) {
            auto player = data_storage_.railroads_data.get_Player_by_nick(str.substr(prev, next - prev));

            railroad->associated.push_back(player);

            prev = next + 1;
        }

        std::sort(railroad->associated.begin(),
                  railroad->associated.end(),
                  [](const std::weak_ptr<Player>& left, const std::weak_ptr<Player>& right) { return left.lock()->nickname < right.lock()->nickname; });
    }

    std::cout << "Associated players loaded\n";


    // ---- Railroads zones ---- //

    getline(file, str); // Skip area name

    while (true) {
        getline(file, str);

        if (str == "")
            break;

        size_t prev = 0;
        size_t next;

        std::string b[4];
        int jk = 0;

        while ((next = str.find("|", prev)) != std::string::npos) {
            b[jk] = str.substr(prev, next - prev);
            prev = next + 1;
            jk++;
        }


        int zone_id { stoi(b[0]) };
        std::string zone_name = b[2];

        auto zone_state = RR_Zone::ZoneState_from_int(stoi(b[3]));

        int  railroad_id { stoi(b[1]) };
        auto railroad = data_storage_.railroads_data.get_Railroad_by_ID(railroad_id);

        if (zone_name.empty() || railroad == nullptr) // Skip errors in file
            continue;


        data_storage_.railroads_data.RR_Zones.push_back(
            std::make_shared<RR_Zone>(zone_id, zone_name, zone_state, railroad)
        );

        data_storage_.railroads_data.max_zone_ID = std::max(zone_id + 1, data_storage_.railroads_data.max_zone_ID);
    }

    std::cout << "Zones loaded: " << data_storage_.railroads_data.RR_Zones.size() << std::endl;


    // ---- RR Lines (filling - 2/2) ---- //

    for (auto& str : rr_lines) {

        if (str == "")
            break;

        size_t prev = 0;
        size_t next;

        std::string b[5];
        int jk = 0;

        while ((next = str.find("|", prev)) != std::string::npos) {
            b[jk] = str.substr(prev, next - prev);
            prev = next + 1;
            jk++;
        }


        auto pointA = data_storage_.railroads_data.get_RR_Point_by_ID(stoi(b[0]));
        auto pointB = data_storage_.railroads_data.get_RR_Point_by_ID(stoi(b[1]));

        auto railroad = data_storage_.railroads_data.get_Railroad_by_ID(stoi(b[2]));

        int line_id { stoi(b[3]) };

        auto zone = data_storage_.railroads_data.get_RR_Zone_by_ID(stoi(b[4]));


        data_storage_.railroads_data.RR_Lines.push_back(
            std::make_shared<RR_Line>(line_id, pointA, pointB, zone)
        );

        data_storage_.railroads_data.max_line_ID = std::max(line_id + 1, data_storage_.railroads_data.max_line_ID);
        
        if (railroad->lines_connected == 0)
            railroad->starting_point = pointA;

        railroad->lines_connected++;

        auto& line = data_storage_.railroads_data.RR_Lines.back();
        pointA->connected_lines.push_back(line);
        pointB->connected_lines.push_back(line);
    }

    std::cout << "Lines loaded: " << data_storage_.railroads_data.RR_Lines.size() << std::endl;

    return 0;
}
int DataLoader::load_RRs_v2_0(std::ifstream& file)
{
    std::vector<uint8_t> v_ubjson((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    nlohmann::json data{ nlohmann::json::from_ubjson(v_ubjson) };
    
    load_players   (data.at("Players"  ));
    load_railroads (data.at("Railroads"));
    load_points    (data.at("RR_Points"));
    load_zones     (data.at("RR_Zones" ));
    load_lines     (data.at("RR_Lines" ));

    return 0;
}

void DataLoader::save_players   (nlohmann::json& data)
{
    for (auto& player : data_storage_.railroads_data.Players)
    {
        nlohmann::json player_json;

        to_json(player_json, *player);

        data.push_back(player_json);
    }
}
void DataLoader::save_railroads (nlohmann::json& data)
{
    for (auto& railroad : data_storage_.railroads_data.Railroads)
    {
        nlohmann::json railroad_json;

        to_json(railroad_json, *railroad);

        data.push_back(railroad_json);
    }
}
void DataLoader::save_points    (nlohmann::json& data)
{
    for (auto& point : data_storage_.railroads_data.RR_Points)
    {
        nlohmann::json point_json;

        to_json(point_json, *point);

        data.push_back(point_json);
    }
}
void DataLoader::save_zones     (nlohmann::json& data)
{
    for (auto& zone : data_storage_.railroads_data.RR_Zones)
    {
        nlohmann::json zone_json;

        to_json(zone_json, *zone);

        data.push_back(zone_json);
    }
}
void DataLoader::save_lines     (nlohmann::json& data)
{
    for (auto& line : data_storage_.railroads_data.RR_Lines)
    {
        nlohmann::json line_json;

        to_json(line_json, *line);

        data.push_back(line_json);
    }
}

void DataLoader::load_players   (nlohmann::json& data)
{
    auto& railroads_data = data_storage_.railroads_data;

    for (auto& player_json : data)
    {
        std::shared_ptr<Player> player { std::make_shared<Player>() };

        from_json(player_json, *player);

        railroads_data.Players.push_back(player);

        railroads_data.max_player_ID = std::max(railroads_data.max_player_ID, player->id + 1);
    }

    std::sort(data_storage_.railroads_data.Players.begin(),
              data_storage_.railroads_data.Players.end(),
              [](const std::weak_ptr<Player>& left, const std::weak_ptr<Player>& right)
                { return left.lock()->nickname < right.lock()->nickname; });

    std::cout << "Players loaded: " << data_storage_.railroads_data.Players.size() << std::endl;
}
void DataLoader::load_railroads (nlohmann::json& data)
{
    auto& railroads_data = data_storage_.railroads_data;

    for (auto& railroad_json : data)
    {
        std::shared_ptr<Railroad> railroad { std::make_shared<Railroad>() };

        from_json(railroad_json, *railroad, data_storage_);

        railroads_data.Railroads.push_back(railroad);

        railroads_data.max_railroad_ID = std::max(railroads_data.max_railroad_ID, railroad->id + 1);
    }

    std::cout << "Railroads loaded: " << data_storage_.railroads_data.Railroads.size() << std::endl;
}
void DataLoader::load_points    (nlohmann::json& data)
{
    auto& railroads_data = data_storage_.railroads_data;

    for (auto& point_json : data)
    {
        std::shared_ptr<RR_Point> point { std::make_shared<RR_Point>() };

        from_json(point_json, *point);

        railroads_data.RR_Points.push_back(point);

        railroads_data.max_point_ID = std::max(railroads_data.max_point_ID, point->id + 1);
    }

    std::cout << "Points loaded: " << data_storage_.railroads_data.RR_Points.size() << std::endl;
}
void DataLoader::load_zones     (nlohmann::json& data)
{
    auto& railroads_data = data_storage_.railroads_data;

    for (auto& zone_json : data)
    {
        std::shared_ptr<RR_Zone> zone { std::make_shared<RR_Zone>() };

        from_json(zone_json, *zone, data_storage_);

        railroads_data.RR_Zones.push_back(zone);

        railroads_data.max_zone_ID = std::max(railroads_data.max_zone_ID, zone->id + 1);
    }

    std::cout << "Zones loaded: " << data_storage_.railroads_data.RR_Zones.size() << std::endl;
}
void DataLoader::load_lines     (nlohmann::json& data)
{
    auto& railroads_data = data_storage_.railroads_data;

    for (auto& line_json : data)
    {
        std::shared_ptr<RR_Line> line { std::make_shared<RR_Line>() };

        from_json(line_json, *line, data_storage_);

        auto railroad = line->zone.lock()->railroad.lock();
        auto pointA   = line->pointA               .lock();
        auto pointB   = line->pointB               .lock();

        if (railroad->lines_connected == 0)
            railroad->starting_point = pointA;

        railroad->lines_connected++;

        pointA->connected_lines.push_back(line);
        pointB->connected_lines.push_back(line);

        railroads_data.RR_Lines.push_back(line);

        railroads_data.max_line_ID = std::max(railroads_data.max_line_ID, line->id + 1);
    }

    std::cout << "Lines loaded: " << data_storage_.railroads_data.RR_Lines.size() << std::endl;
}

// - Coordinates To Path _ x
std::string DataLoader::CTP_current_map    (int x, int y, const std::string& dir)
{
    std::string full_path = dir;

    full_path += "/Map";
    full_path += "/pos_" + std::to_string(x) + "x" + std::to_string(y);
    full_path += MIP_map_scale_path();
    full_path += ".png";

    return full_path;
}
std::string DataLoader::CTP_difference_map (int x, int y, const std::string& dir)
{
    std::string full_path = dir;

    full_path += "/Map_difference";
    full_path += "/pos_" + std::to_string(x) + "x" + std::to_string(y);
    full_path += MIP_map_scale_path();
    full_path += ".png";

    return full_path;
}
std::string DataLoader::CTP_old_map        (int x, int y, const std::string& dir)
{
    std::string full_path = dir;

    full_path += "/Map_old";
    full_path += "/pos_" + std::to_string(x) + "x" + std::to_string(y);
    full_path += MIP_map_scale_path();
    full_path += ".png";

    return full_path;
}

std::string DataLoader::MIP_map_scale_path()
{
    using namespace utils;

    int scale = (int)ceil(-data_storage_.camera.scale_modifier);

    if (scale >= 5)
    {
        return "_xxxxx";
    }
    else if (scale <= 0)
    {
        return "";
    }
    else
    {
        std::string x = "x";
        return "_" + x * scale;
    }
}
