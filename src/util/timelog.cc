/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "util/timelog.hh"

#include <sstream>

using namespace std;
using namespace std::chrono;

TimeLog::TimeLog()
  : start_( duration_cast<milliseconds>( system_clock::now().time_since_epoch() ) ),
    prev_( start_ )
{}

void TimeLog::add_point_size(const std::string & title, off_t size)
{
  points_size.emplace_back(title, size);
}

void TimeLog::add_point_rw( const std::string & title, std::size_t cnt)
{
  points_rw.emplace_back(title, cnt);
}

void TimeLog::add_point( const std::string & title )
{
  auto now = duration_cast<milliseconds>( system_clock::now().time_since_epoch() );

  points_.emplace_back( title, now - prev_ );
  prev_ = now;
}

string TimeLog::str() const
{
  ostringstream oss;
  oss << start_.count() << endl;

  for ( const auto & point : points_ ) {
    oss << point.first << " " << point.second.count() << endl;
  }
  for( const auto & point : points_rw ) {
    oss << point.first << " " << point.second << endl;
  }
  for( const auto & point : points_size ) {
    oss << point.first << " " << point.second << endl;
  }

  return oss.str();
}
