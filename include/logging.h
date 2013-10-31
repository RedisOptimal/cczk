/**
 * @file   logging.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */

#ifndef _XCS_LOGGING_H_
#define _XCS_LOGGING_H_

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <glog/raw_logging.h>

namespace xcs {

/* Logging with inner mutex  */
// Debug mode, not present in release build
#define XCS_DEBUG_INFO DLOG(INFO) << "[DEBUG]  "
// Debug / Release mode
#define XCS_INFO  LOG(INFO)
#define XCS_WARN  LOG(WARNING)
#define XCS_ERROR LOG(ERROR)
#define XCS_FATAL LOG(FATAL)            // Will core-dump when use this

/* Logging raw type, lock-free */
#define XCS_RAW_INFO(...)  RAW_LOG_INFO(__VA_ARGS__)
#define XCS_RAW_WARN(...)  RAW_LOG_WARNING(__VA_ARGS__)
#define XCS_RAW_ERROR(...) RAW_LOG_ERROR(__VA_ARGS__)
#define XCS_RAW_FATAL(...) RAW_LOG_FATAL(__VA_ARGS__) // Will core-dump

/** 
 * If used in XOA2, this does not need to be set
 * 
 * @param log_name 
 * 
 */
static void set_xcs_glog_file(const char* log_name) {
  google::InitGoogleLogging(log_name);
}

}

#endif /* _XCS_LOGGING_H_ */
