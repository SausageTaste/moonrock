#include "utils.h"


namespace {

    template <typename T>
    constexpr double cast_to_sec(const T& duration) {
        const auto a = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        return static_cast<double>(a.count()) / static_cast<double>(moonrock::NANOSEC_PER_SEC);
    }

}


namespace moonrock {

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
