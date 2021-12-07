#include "utils.h"

#include <sys/types.h>
#include <sys/stat.h>


namespace {

    template <typename T>
    constexpr double cast_to_sec(const T& duration) {
        const auto a = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        return static_cast<double>(a.count()) / static_cast<double>(moonrock::NANOSEC_PER_SEC);
    }

    // https://stackoverflow.com/q/18100097/6819534
    bool dirExists(const char* const path) {
        struct stat info;

        if (stat(path, &info) != 0)
            return false;
        else if (info.st_mode & S_IFDIR)
            return true;
        else
            return false;
    }

}


namespace moonrock {

    std::string find_parent_folder_containing_folder_named(const char* const criteria) {
        std::string cur_path = ".";

        for (int i = 0; i < 10; ++i) {
            const auto fol_path = cur_path + '/' + criteria;

            if (::dirExists(fol_path.c_str())) {
                return cur_path;
            }
            else {
                cur_path += "/..";
            }
        }

        return std::string{};
    }

    double get_cur_sec() {
        const auto a = std::chrono::high_resolution_clock::now();
        return ::cast_to_sec(a.time_since_epoch());
    }

}


namespace moonrock {

    Timer::Timer() {
        this->check();
    }

    void Timer::check() {
        this->m_last_checked = std::chrono::high_resolution_clock::now();
    }

    double Timer::elapsed() const {
        return ::cast_to_sec(std::chrono::high_resolution_clock::now() - this->m_last_checked);
    }

    double Timer::check_get_elapsed() {
        const auto current_check = std::chrono::high_resolution_clock::now();
        const auto elapsed = ::cast_to_sec(current_check - this->m_last_checked);
        this->m_last_checked = current_check;
        return elapsed;
    }

}
