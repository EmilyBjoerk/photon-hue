#ifndef PHOTON_HUE_SCHEDULE_H
#define PHOTON_HUE_SCHEDULE_H

#include <iostream>
#include <vector>

typedef float real;

struct schedule_point {
  real day_hour;
  real ct;
  real lum;
};

schedule_point operator+(const schedule_point& l, const schedule_point& r);
schedule_point operator-(const schedule_point& l, const schedule_point& r);
schedule_point operator*(const schedule_point& l, real r);
std::ostream& operator<<(std::ostream& os, const schedule_point& p);

// Linearly interpolate in a 24 hour schedule defined by a vector of schedule points.
// Will handle wraparound of the schedule so it's a perfectly looping schedule
schedule_point schedule_lerp(const std::vector<schedule_point>& schedule, real t);

#endif
