#pragma once
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>

#include <list>
#include <cmath>
#include <ctime>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>


#ifndef PI
#define PI 3.14159265
#endif
#define PI2 (3.14159265 / 2)
#define PI4 (3.14159265 / 4)


namespace utils
{
    // --- Math 

    struct PopUpState
    {
        bool is_open        = false;
        bool should_be_open = false;
    };

    float  min(float a, float b);
    float  max(float a, float b);

    int    minmax(int a, int num, int b);       // equal to - min(max(num, a), b);
    float  minmax(float a, float num, float b);
    bool   in(int a, int num, int b);           // true if - a <= num <= b
    bool   in_f(float a, float num, float b);
    float  mod_f(float X, float Y);             // modulus for float numbers
    double randf(double start, double end, int amount); // returns random float inside (start / end) with the number of values specified in amount
    float  pif(float a, float b);               // Pythagorean theorem: a, b -> c
    float  pif(sf::Vector2i x);
    float  pif(sf::Vector2f x);
    float  distance(const sf::Vector2i& A, const sf::Vector2i& B, const sf::Vector2i& C);

    bool is_horizontal_move(const sf::Vector2i& A, const sf::Vector2i& B);
    bool is_horizontal_move(const sf::Vector2i& delta);


    bool file_exists(const std::string& path);
    std::string get_time_string();

    bool part_of_str(std::string sub_str, std::string str);
    bool part_is_in_vector(std::string part, const std::vector<std::string>& list);

    bool is_in_vector(std::string element, const std::vector<std::string>& list);

    enum class Align
    {
        Left,
        Middle,
        Right
    };
    std::string align(const std::string& text, int output_width, Align align);

    // --- ImGui

    inline namespace literals
    {
        char const* operator"" _C(const char8_t* str, std::size_t);
    }

    ImVec2 operator+ (const ImVec2& left, const ImVec2& right);
    ImVec2 operator- (const ImVec2& left, const ImVec2& right);
    bool   operator==(const ImVec2& left, const ImVec2& right);
    bool   operator!=(const ImVec2& left, const ImVec2& right);
    std::string operator*(std::string str, int mult);

    bool  block();                           // ImGUI::Sameline(); && return true;
    void  selectable_color(ImU32 color);
    float get_button_width(std::string text, ImGuiStyle& style);


    // --- SFML

    sf::Vector2f operator/(sf::Vector2i left, float right);

    enum class ButtonState
    {
        Up       = 0,
        Pressed  = 1,
        Down     = 2,
        Released = 3
    };

    extern bool ignore_input;
    extern ButtonState mouse_states [];
    extern ButtonState key_states   [];

    bool key_up       (const sf::Keyboard::Key& key);
    bool key_pressed  (const sf::Keyboard::Key& key);
    bool key_down     (const sf::Keyboard::Key& key);
    bool key_released (const sf::Keyboard::Key& key);

    bool mouse_up       (const sf::Mouse::Button& button);
    bool mouse_pressed  (const sf::Mouse::Button& button);
    bool mouse_down     (const sf::Mouse::Button& button);
    bool mouse_released (const sf::Mouse::Button& button);


    // --- OS based

//#ifdef __linux__ 
//    const char eol('\n'); // LF
//#elif _WIN32
    const char eol('\r'); // CR+LF
//#else
//    const char eol('\r'); // CR
//#endif

    void getline(std::istream& stream, std::string& string);

    // --- RWC stuff

    std::string to_map_coords(sf::Vector2i position, int scale);
} // namespace utils
