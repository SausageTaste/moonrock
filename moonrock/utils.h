#pragma once

#include <string>
#include <chrono>


namespace moonrock {

    constexpr int NANOSEC_PER_SEC = 1'000'000'000;


    std::string find_parent_folder_containing_folder_named(const char* const criteria);

    double get_cur_sec();


    class Timer {

    private:
        std::chrono::high_resolution_clock::time_point m_last_checked;

    public:
        Timer();

        void check();

        double elapsed() const;

        double check_get_elapsed();

    };

}
