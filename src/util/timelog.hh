/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_TIMELOG_HH
#define UTIL_TIMELOG_HH

#include <vector>
#include <chrono>
#include <string>
#include <ctime>

class TimeLog
{
private:
  std::chrono::milliseconds start_;
  std::chrono::milliseconds prev_;
  std::vector<std::pair<std::string, std::chrono::milliseconds>> points_ {};
  std::vector<std::pair<std::string, std::size_t> > points_rw {};
  std::vector<std::pair<std::string, off_t> > points_size {};
public:
  TimeLog();
  void add_point( const std::string & title );
  void add_point_rw( const std::string & title, std::size_t cnt);
  void add_point_size( const std::string & title, off_t size);
  std::string str() const;
};

#endif /* UTIL_TIMELOG_HH */
