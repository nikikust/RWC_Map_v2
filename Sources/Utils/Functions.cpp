#include "../../Include/Utils/Functions.h"


namespace utils
{
    // --- Math

    float  min(float a, float b) { return (a > b) ? b : a; }
    float  max(float a, float b) { return (a > b) ? a : b; }

    int    minmax(int a, int num, int b) 
    { 
        return (a > num) ? a : (b < num) ? b : num; 
    }
    float  minmax(float a, float num, float b) 
    { 
        return (a > num) ? a : (b < num) ? b : num; 
    }
    bool   in(int a, int num, int b) 
    { 
        return (num >= a && num <= b); 
    }
    bool   in_f(float a, float num, float b) 
    { 
        return (num >= a - 0.01 && num <= b + 0.01); 
    }
    float  mod_f(float X, float Y)
    {
        if (X > 0)
            return fmod(X, Y);
        else
            return fmod(Y - fmod(-X, Y), Y);
    }
    double randf(double start, double end, int amount)
    {
        return start + (rand() % amount) / double(amount) * (end - start);
    }
    float  pif(float a, float b) 
    { 
        return sqrt(a * a + b * b); 
    }
    float  pif(sf::Vector2i x)
    {
        return sqrt((float)x.x * x.x + (float)x.y * x.y);
    }
    float  pif(sf::Vector2f x)
    {
        return sqrt(x.x * x.x + x.y * x.y);
    }
    float  distance(const sf::Vector2i& P1, const sf::Vector2i& P2, const sf::Vector2i& X)
    { 
        return abs((long long)(P2.y - P1.y) * X.x - (long long)(P2.x - P1.x) * X.y + (long long)P2.x * P1.y - (long long)P2.y * P1.x) 
                   / pif((float)P2.x - P1.x, (float)P2.y - P1.y);
    }

    bool is_horizontal_move(const sf::Vector2i& A, const sf::Vector2i& B)
    {
        return (abs(B.x - A.x) >= abs(B.y - A.y));
    }
    bool is_horizontal_move(const sf::Vector2i& delta)
    {
        return (abs(delta.x) >= abs(delta.y));
    }


    bool file_exists(const std::string& path)
    {
        try
        {
            std::ifstream test(path);
            if (test.is_open())
            {
                test.close();
                return true;
            }
        }
        catch (const std::exception& err)
        {
            std::cout << "file_exists(): " << err.what() << std::endl;
        }

        return false;
    }
    std::string get_time_string()
    {
        // --- //

        std::ostringstream oss;

#ifdef WIN32
        tm local_time;
        time_t time = std::time(nullptr);

        localtime_s(&local_time, &time);

        oss << std::put_time(&local_time, "%d-%m-%Y_%H-%M-%S");
#else
        struct tm* local_time;
        time_t t = time(NULL);

        local_time = localtime(&t);

        oss << std::put_time(local_time, "%d-%m-%Y_%H-%M-%S");
#endif

        return oss.str();
    }

    bool part_of_str(std::string sub_str, std::string str)
    {
        transform(sub_str.begin(), sub_str.end(), sub_str.begin(), ::tolower);
        transform(str    .begin(), str    .end(), str    .begin(), ::tolower);

        if (str.find(sub_str) != std::string::npos)
            return true;

        return false;
    }
    bool part_is_in_vector(std::string part, const std::vector<std::string>& vector)
    {
        transform(part.begin(), part.end(), part.begin(), ::tolower);

        for (auto& vector_element_it : vector)
        {
            std::string vector_element = vector_element_it;

            transform(vector_element.begin(), vector_element.end(), vector_element.begin(), ::tolower);

            if (vector_element.find(part) != std::string::npos)
                return true;
        }
        return false;
    }

    bool is_in_vector(std::string element, const std::vector<std::string>& vector)
    {
        transform(element.begin(), element.end(), element.begin(), ::tolower);

        for (auto& vector_element_it : vector)
        {
            std::string vector_element = vector_element_it;

            transform(vector_element.begin(), vector_element.end(), vector_element.begin(), ::tolower);

            if (vector_element == element)
                return true;
        }
        return false;
    }

    std::string align(const std::string& text, int output_width, Align align)
    {
        if (text.length() >= output_width)
        {
            return text;
        }

        std::string output = "";

        switch (align)
        {
        case utils::Align::Left:
            output = text + std::string(" ") * (output_width - (int)text.length());
            break;
        case utils::Align::Middle: {
            int from_left = (output_width - (int)text.length()) / 2;
            int from_right = output_width - (int)text.length() - from_left;
            output = 
                std::string(" ") * from_left + text + std::string(" ") * from_right;
        }   break;
        case utils::Align::Right:
            output = std::string(" ") * (output_width - (int)text.length()) + text;
            break;
        default:
            break;
        }

        return output;
    }

    // --- ImGui

    inline namespace literals
    {
        char const* operator"" _C(const char8_t* str, std::size_t)
        {
            return reinterpret_cast<const char*>(str);
        }
    }

    ImVec2 operator+(const ImVec2& left, const ImVec2& right)
    {
        return ImVec2(left.x + right.x, left.y + right.y);
    }
    ImVec2 operator-(const ImVec2& left, const ImVec2& right)
    {
        return ImVec2(left.x - right.x, left.y - right.y);
    }
    bool operator==(const ImVec2& left, const ImVec2& right)
    {
        return left.x == right.x && left.y == right.y;
    }
    bool operator!=(const ImVec2& left, const ImVec2& right)
    {
        return left.x != right.x || left.y != right.y;
    }
    std::string operator*(std::string str, int mult)
    {
        std::string buf = "";
        while (mult--)
            buf += str;
        return buf;
    }

    bool block()
    {
        ImGui::SameLine();
        return true;
    }
    void selectable_color(ImU32 color)
    {
        ImVec2 p_min = ImGui::GetItemRectMin();
        ImVec2 p_max = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
    }
    float get_button_width(std::string text, ImGuiStyle& style)
    {
        return ImGui::CalcTextSize(text.c_str()).x + style.FramePadding.x * 2 + style.ItemSpacing.x;
    }

    // --- SFML

    sf::Vector2f operator/(sf::Vector2i left, float right)
    {
        return sf::Vector2f((float)left.x / right, (float)left.y / right);
    }

    bool ignore_input = false;
    ButtonState mouse_states [sf::Mouse::ButtonCount] = { ButtonState::Up };
    ButtonState key_states   [sf::Keyboard::KeyCount] = { ButtonState::Up };

    bool key_up       (const sf::Keyboard::Key& key)
    {
        if (ignore_input)
            return false;
        return !sf::Keyboard::isKeyPressed(key);
    }
    bool key_pressed  (const sf::Keyboard::Key& key)
    {
        if (key_states[key] == ButtonState::Pressed) {
            key_states[key]  = ButtonState::Down;
            if (!ignore_input)
                return true;
        }
        return false;
    }
    bool key_down     (const sf::Keyboard::Key& key)
    {
        if (ignore_input)
            return false;
        return sf::Keyboard::isKeyPressed(key);
    }
    bool key_released (const sf::Keyboard::Key& key)
    {
        if (key_states[key] == ButtonState::Released) {
            key_states[key]  = ButtonState::Up;
            if (!ignore_input)
                return true;
        }
        return false;
    }

    bool mouse_up       (const sf::Mouse::Button& button)
    {
        if (ignore_input)
            return false;
        return !sf::Mouse::isButtonPressed(button);
    }
    bool mouse_pressed  (const sf::Mouse::Button& button)
    {
        if (mouse_states[button] == ButtonState::Pressed) {
            mouse_states[button]  = ButtonState::Down;
            if (!ignore_input)
                return true;
        }
        return false;
    }
    bool mouse_down     (const sf::Mouse::Button& button)
    {
        if (ignore_input)
            return false;
        return sf::Mouse::isButtonPressed(button);
    }
    bool mouse_released (const sf::Mouse::Button& button)
    {
        if (mouse_states[button] == ButtonState::Released) {
            mouse_states[button]  = ButtonState::Up;
            if (!ignore_input)
                return true;
        }
        return false;
    }
    

    // --- RWC Stuff

    void getline(std::istream& stream, std::string& string)
    {
        std::getline(stream, string, eol);

//#ifdef _WIN32
        stream.get(); // skip LF after CR
//#endif
    }

    std::string to_map_coords(sf::Vector2i position, int scale)
    {
        // http://198.50.173.174:19256/?worldname=world&mapname=flat&zoom=2&x=14&y=64&z=67
        std::string base_link = "http://198.50.173.174:19256/?worldname=world&mapname=flat&zoom=";

        base_link += std::to_string(scale) + "&x=";
        base_link += std::to_string(position.x * 2) + "&y=64&z=";
        base_link += std::to_string(position.y * 2 + 50);

        return base_link;
    }
} // namespace utils
