//
// Created by lml on 2018/7/2.
//

#include "wf_log.h"

using namespace wf;

std::ofstream wf_logger::m_error_log_file;
std::ofstream wf_logger::m_info_log_file;
std::ofstream wf_logger::m_warn_log_file;

void initLogger(const std::string&info_log_filename,
                const std::string&warn_log_filename,
                const std::string&error_log_filename){
//    wf_logger::m_info_log_file.open(info_log_filename.c_str());
//    wf_logger::m_warn_log_file.open(warn_log_filename.c_str());
//    wf_logger::m_error_log_file.open(error_log_filename.c_str());
}

std::ostream& wf_logger::getStream(log_rank_t log_rank){
    return (INFO == log_rank)? std::cout : std::cerr;
//    return (INFO == log_rank) ?
//           (m_info_log_file.is_open() ?m_info_log_file : std::cout) :
//           (WARNING == log_rank ?
//            (m_warn_log_file.is_open()? m_warn_log_file : std::cerr) :
//            (m_error_log_file.is_open()? m_error_log_file : std::cerr));
}

std::string wf_logger::get_time()
{
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}

std::string wf_logger::log_level(log_rank_t log_rank){
    switch (log_rank){
        case INFO:
            return "INFO";
        case WARNING:
            return "WARN";
        case ERROR:
            return "ERRO";
        default:
            return "UNKNOW";
    }
}

std::ostream& wf_logger::start(log_rank_t log_rank,
                            const int line,
                            const std::string&function)
{
//    return getStream(log_rank) << "[" << get_time() << "]"
//                               << "function (" << function << ")"
//                               << "line " << line <<" "
//                               << std::flush;

    return getStream(log_rank) << "[" << get_time() << "]"
                               << "[" << log_level(log_rank) << "] "
                               << std::flush;
}

wf_logger::~wf_logger(){
    getStream(m_log_rank) << std::flush;

//    if (FATAL == m_log_rank) {
//        m_info_log_file.close();
//        m_info_log_file.close();
//        m_info_log_file.close();
//        abort();
//    }
}
