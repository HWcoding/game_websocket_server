#ifndef SOURCE_LOGGING_LOGGER_H
#define SOURCE_LOGGING_LOGGER_H
//#include "source/logging/logger.h"

#include <iostream>
#include <errno.h>
#include <cstring>

#define IM_AN_EMPTY_STATEMENT	(void)0 //optimizes to an empty statement but doesn't throw compiler [-Wunused-value] warnings like an actual empty statement would

#define PRINT_ERROR(text, text2)        std::cerr<<text<<text2<<std::endl
#define PRINT_LOG(text,text2)           std::cout<<text<<text2<<std::endl

#define LOG_ERROR(text,text2)           PRINT_ERROR(text,text2)

#ifdef DEBUG
#define LOG_VERBOSE(text,text2)         PRINT_LOG(text,text2)
#define LOG_INFO(text,text2)            PRINT_LOG(text,text2)
#define LOG_PLAYER_INFO(text,text2)     PRINT_LOG(text,text2)
#define LOG_PLAYER_VERBOSE(text,text2)  PRINT_LOG(text,text2)
#define LOG_DEBUG(text,text2)           PRINT_LOG(text,text2)
#else
#define LOG_VERBOSE(text,text2)         IM_AN_EMPTY_STATEMENT
#define LOG_INFO(text,text2)            IM_AN_EMPTY_STATEMENT
#define LOG_PLAYER_INFO(text,text2)     IM_AN_EMPTY_STATEMENT
#define LOG_PLAYER_VERBOSE(text,text2)  IM_AN_EMPTY_STATEMENT
#define LOG_DEBUG(text,text2)           IM_AN_EMPTY_STATEMENT
#endif

#define writeError(text)	((LOG_ERROR("ERROR ","errno: "<<std::strerror(errno)<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__<<" line: "<<__LINE__<<"\n"<<text)))

#endif /* SOURCE_LOGGING_LOGGER_H */
