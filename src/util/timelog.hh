/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef UTIL_TIMELOG_HH
#define UTIL_TIMELOG_HH

#include <vector>
#include <chrono>
#include <string>
#include <ctime>

#include "thunk/thunk.hh"

class TimeLog
{
private:
  std::chrono::milliseconds start_;
  std::chrono::milliseconds prev_;
  std::vector<std::pair<std::string, std::chrono::milliseconds>> points_ {};
  std::vector<std::pair<std::string, std::size_t> > points_rw {};
  std::vector<std::pair<std::string, off_t> > points_size {};
  std::vector<std::pair<std::string, off_t> > get_hash {};
  std::vector<std::pair<std::string, off_t> > upload_hash {};
  std::string execute_info;
public:
  TimeLog();
  void add_point( const std::string & title );
  void add_point_rw( const std::string & title, std::size_t cnt);
  void add_point_size( const std::string & title, off_t size);
  void add_point_hash( const std::string & title, std::string hash_value, off_t size);
  void add_execute(gg::thunk::Function &function);
  std::string str() const;
};

#endif /* UTIL_TIMELOG_HH */
