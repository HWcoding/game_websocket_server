#ifndef SOURCE_LOGGING_LOGGER_H
#define SOURCE_LOGGING_LOGGER_H
//#include "source/logging/logger.h"

#include <iostream>
#include <errno.h>
#include <cstring>

#define IM_AN_EMPTY_STATEMENT	do{(void)0;}while(false)  //optimizes to an empty statement but doesn't throw compiler [-Wunused-value] warnings like an actual empty statement would

#define PRINT_ERROR(text)	((PRINT_LOG("",text<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__<<" line: "<<__LINE__)),(void)0)

#define PRINT_LOG(text,text2)           std::cout<<text<<text2<<std::endl
#define TEST_PRINT(text)                PRINT_ERROR(text)

#define LOG_VERBOSE(text,text2)         PRINT_LOG(text,text2) //IM_AN_EMPTY_STATEMENT
#define LOG_INFO(text,text2)            PRINT_LOG(text,text2) //IM_AN_EMPTY_STATEMENT
#define LOG_ERROR(text,text2)           PRINT_LOG(text,text2) //IM_AN_EMPTY_STATEMENT
#define LOG_PLAYER_INFO(text,text2)     PRINT_LOG(text,text2)
#define LOG_PLAYER_VERBOSE(text,text2)  PRINT_LOG(text,text2)
#define LOG_DEBUG(text,text2)           PRINT_LOG(text,text2)


#define writeError(text)	((PRINT_LOG("ERROR ","errno: "<<std::strerror(errno)<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__<<" line: "<<__LINE__<<std::endl<<text)))

#endif /* SOURCE_LOGGING_LOGGER_H */
