//
// Created by lml on 2018/7/2.
//

#ifndef PROJECT_WF_LOG_H
#define PROJECT_WF_LOG_H

#include <iostream>
#include <fstream>

namespace wf{

    typedef enum log_rank {
        INFO = 1,
        WARNING,
        ERROR,
    }log_rank_t;

//    void initLogger(const std::string&info_log_filename,
//                    const std::string&warn_log_filename,
//                    const std::string&error_log_filename);

    class wf_logger {

        friend void initLogger(const std::string& info_log_filename,
                               const std::string& warn_log_filename,
                               const std::string& erro_log_filename);

    public:
        //构造函数
        wf_logger(log_rank_t log_rank) : m_log_rank(log_rank) {};

        ~wf_logger();
        ///
        /// \brief 写入日志信息之前先写入的源代码文件名, 行号, 函数名
        /// \param log_rank 日志的等级
        /// \param line 日志发生的行号
        /// \param function 日志发生的函数
        static std::ostream& start(log_rank_t log_rank,
                                   const int line,
                                   const std::string& function);

    private:
        ///
        /// \brief 根据等级获取相应的日志输出流
        ///
        static std::ostream& getStream(log_rank_t log_rank);

        static std::ofstream m_info_log_file;                   ///< 信息日子的输出流
        static std::ofstream m_warn_log_file;                  ///< 警告信息的输出流
        static std::ofstream m_error_log_file;                  ///< 错误信息的输出流
        log_rank_t m_log_rank;                             ///< 日志的信息的等级

        static std::string get_time();                    //获取当前时间
        static std::string log_level(log_rank_t);                   //日志等级（枚举转字符串）
    };
}

#define wflog(log_rank)   \
wf_logger(log_rank).start(log_rank, __LINE__,__FUNCTION__)

#define wf_info  \
wf_logger(INFO).start(INFO, __LINE__,__FILE__)

#define wf_warn \
wf_logger(WARNING).start(WARNING, __LINE__,__FILE__)

#define wf_error \
wf_logger(ERROR).start(ERROR, __LINE__,__FILE__)

#endif //PROJECT_WF_LOG_H
