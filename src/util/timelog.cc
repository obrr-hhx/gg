/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "util/timelog.hh"

#include <sstream>

using namespace std;
using namespace std::chrono;

TimeLog::TimeLog()
  : start_( duration_cast<milliseconds>( system_clock::now().time_since_epoch() ) ),
    prev_( start_ )
{}

void TimeLog::add_point( const std::string & title )
{
  auto now = duration_cast<milliseconds>( system_clock::now().time_since_epoch() );

  points_.emplace_back( title, now - prev_ );
  prev_ = now;
}

void TimeLog::add_get( const std::string & hash, uint32_t size, std::string time )
{
  gets_.emplace_back( make_tuple(hash, size, time) );
}

void TimeLog::add_put( const std::string & hash, uint32_t size, std::string time )
{
  puts_.emplace_back( make_tuple(hash, size, time) );
}

void TimeLog::add_command(std::string command )
{
  commands_.push_back( command );
}

string TimeLog::str() const
{
  ostringstream oss;
  oss << start_.count() << endl;

  for ( const auto & point : points_ ) {
    oss << point.first << " " << point.second.count() << endl;
  }

  for ( auto  get_ : gets_ ) {
    oss << "get " << get<0>(get_) << " " << get<1>(get_) <<" "<< get<2>(get_) << endl;
  }

  for ( auto  put_ : puts_ ) {
    oss << "put " << get<0>(put_) << " " << get<1>(put_) << " " << get<2>(put_) << endl;
  }

  oss << "command ";
  for (auto & command : commands_ ) {
    oss << command <<" ";
  }
  oss << endl;
  return oss.str();
}
