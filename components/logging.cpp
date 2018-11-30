/*
 * logging.cpp
 *
 *  Created on: 24.12.2018
 *      Author: eyck
 */


#include "logging.h"
#include <systemc>
#include <deque>
#include <array>
#include <sstream>
#include <iomanip>

using namespace sc_core;

enum log_level {NONE, FATAL, ERROR, WARNING, INFO, DEBUG, TRACE};

namespace {

static std::deque<std::string> msg_buf;

inline log_level verbosity2log(int verb) {
    if (verb >= sc_core::SC_FULL) return TRACE;
    if (verb >= sc_core::SC_HIGH) return DEBUG;
    return INFO;
}

std::string time2string(const sc_core::sc_time &t) {
    const std::array<const char *, 6> time_units{"fs", "ps", "ns", "us", "ms", "s "};
    const std::array<uint64_t, 6> multiplier{1ULL,
                                             1000ULL,
                                             1000ULL * 1000,
                                             1000ULL * 1000 * 1000,
                                             1000ULL * 1000 * 1000 * 1000,
                                             1000ULL * 1000 * 1000 * 1000 * 1000};
    std::ostringstream oss;
    const sc_core::sc_time_tuple tt{t};
    const auto val = tt.value();
    if (!val) {
        oss << "0 s";
    } else {
        const unsigned scale = tt.unit();
        const auto fs_val = val * multiplier[scale];
        for (int j = multiplier.size() - 1; j >= scale; --j) {
            if (fs_val > multiplier[j]) {
                const auto i = val / multiplier[j - scale];
                const auto f = val % multiplier[j - scale];
                oss << i << '.' << std::setw(3 * (j - scale)) << std::setfill('0') << std::left << f << ' '
                    << time_units[j];
                break;
            }
        }
    }
    return oss.str();
}

const std::string compose_message(const sc_report &rep) {
    std::stringstream os;
    os << "[" << std::setw(20) << time2string(sc_core::sc_time_stamp()) << "] ";
    if (rep.get_id() >= 0)
        os << "("
           << "IWEF"[rep.get_severity()] << rep.get_id() << ") ";
    os << rep.get_msg_type();
    if (*rep.get_msg()) os << ": " << rep.get_msg();
    if (rep.get_severity() > SC_INFO) {
        std::array<char, 16> line_number_str;
        os << " [FILE:" << rep.get_file_name() << ":" << rep.get_line_number() << "]";
        sc_simcontext *simc = sc_get_curr_simcontext();
        if (simc && sc_is_running()) {
            const char *proc_name = rep.get_process_name();
            if (proc_name) os << "[PROCESS:" << proc_name << "]";
        }
    }
    return os.str();
}

void report_handler(const sc_report &rep, const sc_actions &actions) {
    std::array<const log_level, 4> map = {{INFO, WARNING, ERROR, FATAL}};
    if (actions & SC_DISPLAY) {
        auto level = rep.get_severity() > sc_core::SC_INFO ? map[rep.get_severity()] : verbosity2log(rep.get_verbosity());
        msg_buf.push_back(compose_message(rep));
    }
    if (actions & SC_STOP) sc_stop();
    if (actions & SC_ABORT) abort();
    if (actions & SC_THROW) throw rep;
}
}

bool has_output(){
    return !msg_buf.empty();
}

std::string get_output(){
    std::string ret = msg_buf.front();
    msg_buf.pop_front();
    return ret;
}

void init_logging(unsigned level) {
    const std::array<int, 8> verbosity = {SC_NONE,   // Logging::NONE
                                          SC_LOW,    // Logging::FATAL
                                          SC_LOW,    // Logging::ERROR
                                          SC_LOW,    // Logging::WARNING
                                          SC_MEDIUM, // Logging::INFO
                                          SC_HIGH,   // logging::DEBUG
                                          SC_FULL,   // logging::TRACE
                                          SC_DEBUG}; // logging::TRACE+1
    sc_report_handler::set_verbosity_level(verbosity[level]);
    sc_report_handler::set_handler(report_handler);
}



