/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_TIMELOG_HH
#define UTIL_TIMELOG_HH

#include <vector>
#include <utility>
#include <tuple>
#include <chrono>
#include <string>
#include <ctime>

class TimeLog
{
private:
  std::chrono::milliseconds start_;
  std::chrono::milliseconds prev_;
  std::vector<std::pair<std::string, std::chrono::milliseconds>> points_ {};
  std::vector<std::tuple<std::string, uint32_t, std::string> > gets_ {};
  std::vector<std::tuple<std::string, uint32_t, std::string> > puts_ {};
  std::vector<std::string> commands_ {};

public:
  TimeLog();
  void add_point( const std::string & title );
  void add_get( const std::string & hash, uint32_t size, std::string time );
  void add_put( const std::string & hash, uint32_t size , std::string time);
  void add_command(std::string command );

  std::string str() const;
};

#endif /* UTIL_TIMELOG_HH */
