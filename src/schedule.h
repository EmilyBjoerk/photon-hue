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
std::ostream &operator<<(std::ostream &os, const schedule_point &p);

struct schedule_override {
  real start_hour;
  real end_hour;
  int light_id;
  real ct;
  real lum;
};

// Computes the scheduled colour temperature and luminosity for the current time
// of day
schedule_point schedule_now(const std::vector<schedule_point> &sched, const std::vector<schedule_override> &ovr,
                            int light);

#endif
