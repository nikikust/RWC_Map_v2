#pragma once
#include "DataStorage.h"

#include <nlohmann/json.hpp>


class DataLoader
{
public:
    DataLoader(DataStorage& data_storage);
    DataLoader() = delete;
    DataLoader(DataLoader&) = delete;
    DataLoader(DataLoader&&) = delete;
    ~DataLoader();

    // --- //
    int  init();

    int  load_railroads_data(const std::string& path = "Data\\data.rr");
    void save_railroads_data(const std::string& path = "Data\\data.rr");

    void load_map(sf::Vector2i from, sf::Vector2i to);

private:
    void load_image(sf::Vector2i tile);

private:
    enum class VersionID
    {
        vERR = 0,
        v1_0,
        v2_0
    };

    static VersionID get_version_ID(std::string version_string);
    static VersionID latest_data_file_version();

private:
    // --- Load/Save Railroads data
    std::string to_string(VersionID v_id);

    int load_RRs_v1_0(const std::string& path);
    int load_RRs_v2_0(std::ifstream& file);

    void save_players   (nlohmann::json& data);
    void save_railroads (nlohmann::json& data);
    void save_points    (nlohmann::json& data);
    void save_zones     (nlohmann::json& data);
    void save_lines     (nlohmann::json& data);

    void load_players   (nlohmann::json& data);
    void load_railroads (nlohmann::json& data);
    void load_points    (nlohmann::json& data);
    void load_zones     (nlohmann::json& data);
    void load_lines     (nlohmann::json& data);

    // --- Load Map

    // - Coordinates To Path _ x
    std::string CTP_current_map    (int x, int y, const std::string& dir = "Data\\Map");
    std::string CTP_difference_map (int x, int y, const std::string& dir = "Data\\Map");
    std::string CTP_old_map        (int x, int y, const std::string& dir = "Data\\Map");

    std::string MIP_map_scale_path();

    
    // --- //
    DataStorage& data_storage_;

    std::string map_path_ = "C:/Users/Nikita/source/RWC_Maps";
};
