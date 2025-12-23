module;
#include <string>
#include <sstream>
#include <iostream>

export module console;

std::string s_log, s_prev_text, s_prev_command;

//template<typename T>
//concept StringLike = std::is_convertible_v<T, std::string_view>;

export namespace console {
    void log() {}
    template <typename First, typename... Rest> //requires StringLike<First> && (StringLike<Rest> && ...)    
    void log(First first, Rest... rest) {
        std::ostringstream oss;
        oss << first;
        const std::string text = oss.str().c_str();    // c_str or prints address for other than int
        std::cout << oss.str().c_str();
        if (text.back() != '\n') {
            s_prev_text += text;
        } else {
            s_log.append(s_prev_text + text);
            s_prev_text.clear();
            /*++s_current_line_num;
            if (s_current_line_num > 14) {
                s_current_line_num = 0;
            }*/
        }
        log(rest...);
    }
    void warning() {}
    template <typename First, typename... Rest>
    void warning(First first, Rest... rest) {
        log(first, rest...);
    }
    void error() {}
    template <typename First, typename... Rest>
    void error(First first, Rest... rest) {
        log(first, rest...);
    }
}