#ifndef SOURCE_LOGGING_LOGGER_H
#define SOURCE_LOGGING_LOGGER_H
//#include "source/logging/logger.h"

#include <iostream>
#include <errno.h>
#include <cstring>

#define IM_AN_EMPTY_STATEMENT	(void)0 //optimizes to an empty statement but doesn't throw compiler [-Wunused-value] warnings like an actual empty statement would


#define PRINT_ERROR(text, text2)        std::cerr<<text<<text2<<std::endl
#define PRINT_LOG(text,text2)           std::cerr<<text<<text2<<std::endl

#define LOG_LOCATION                    " in "<<"\033[35m "<<__PRETTY_FUNCTION__<<"\033[0m"<<" in file "<<"\033[36m"<<__FILE__<<"\033[0m"<<" line: "<<__LINE__
#define LOG_FUNCTION					" in "<<"\033[35m "<<__PRETTY_FUNCTION__<<"\033[0m"<<" line: "<<__LINE__

#define LOG_EXCEPTION(text)             PRINT_ERROR("[\033[1;37;41mEXCEPTION\033[0m]: ",text<<LOG_FUNCTION)
#define LOG_ERROR(text)                 PRINT_ERROR("\033[31m[ERROR]:\033[0m ",text)

#ifdef DEBUG
#define LOG_VERBOSE(text)               PRINT_LOG("VERBOSE: ",text)
#define LOG_INFO(text)                  PRINT_LOG("\033[33m[INFO]:\033[0m ",text)
#define LOG_PLAYER_INFO(text)           PRINT_LOG("PLAYER INFO: ",text)
#define LOG_PLAYER_VERBOSE(text)        PRINT_LOG("PLAYER VERBOSE: ",text)
#define LOG_DEBUG(text)                 PRINT_LOG("DEBUG: ",text)

#else
#define LOG_VERBOSE(text)               IM_AN_EMPTY_STATEMENT
#define LOG_INFO(text)                  IM_AN_EMPTY_STATEMENT
#define LOG_PLAYER_INFO(text)           IM_AN_EMPTY_STATEMENT
#define LOG_PLAYER_VERBOSE(text)        IM_AN_EMPTY_STATEMENT
#define LOG_DEBUG(text)                 IM_AN_EMPTY_STATEMENT
#endif

#define writeError(text)	((LOG_ERROR("errno: "<<std::strerror(errno))))

#endif /* SOURCE_LOGGING_LOGGER_H */
