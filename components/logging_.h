/*
 * logging.h
 *
 *  Created on: 24.12.2018
 *      Author: eyck
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <string>
#include <sstream>
#include <systemc.h>

bool has_output();
std::string get_output();
void init_logging(unsigned level);

class Log {
public:
   Log(const char* file, int line):messageLevel(sc_core::SC_INFO), file(file), line(line){};
   Log(const Log&) = delete;
   Log& operator =(const Log&) = delete;
   virtual ~Log(){
       ::sc_core::sc_report_handler::report(messageLevel, "", os.str().c_str(), file, line );
    }
   std::ostringstream& Get(sc_core::sc_severity level = sc_core::SC_INFO){
       messageLevel = level;
       return os;
    }
protected:
   std::ostringstream os;
   sc_core::sc_severity messageLevel;
   const char* file;
   int line;
};

#define LOG(level) Log(__FILE__, __LINE__).Get(level)
#define LOG_INFO   Log(__FILE__, __LINE__).Get(sc_core::SC_INFO)
#define LOG_WARN   Log(__FILE__, __LINE__).Get(sc_core::SC_WARNING)
#define LOG_ERR   Log(__FILE__, __LINE__).Get(sc_core::SC_ERROR)
#endif /* LOGGING_H_ */
