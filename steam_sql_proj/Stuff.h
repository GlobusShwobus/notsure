#pragma once

#include <iostream>
#include <string>
#include <span>
#include <chrono>
#include <format>


namespace badSQL
{
    const std::string ll_to_unix_time(std::int64_t unix_timestamp) {
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(unix_timestamp);
        return std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(tp));
    }

    //confirmed functions

   // void console_wait() {
   //     if (std::cin.rdbuf()->in_avail() > 0)
   //         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
   //     std::cout << "Press Enter to continue...";
   //     std::cin.get();
   // }

    void console_clear() {
        system("cls");
    }


    constexpr std::string_view console_title() noexcept
    {
        return R"( _             _ ___  ___  _    
| |__  __ _ __| / __|/ _ \| |   
| '_ \/ _` / _` \__ \ (_) | |__ 
|_.__/\__,_\__,_|___/\__\_\____|
 )";
    }
    std::string input() {
        std::string str;
        std::getline(std::cin, str);
        return str;
    }
}