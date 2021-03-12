#include "schedule.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

schedule_point operator+(const schedule_point& l, const schedule_point& r) {
  return {l.day_hour + r.day_hour, l.ct + r.ct, l.lum + r.lum};
}

schedule_point operator-(const schedule_point& l, const schedule_point& r) {
  return {l.day_hour - r.day_hour, l.ct - r.ct, l.lum - r.lum};
}

schedule_point operator*(const schedule_point& l, real r) { return {l.day_hour * r, l.ct * r, l.lum * r}; }

std::ostream& operator<<(std::ostream& os, const schedule_point& p) {
  os << p.day_hour << ", " << p.ct << ", " << p.lum;
  return os;
}

schedule_point schedule_lerp(const std::vector<schedule_point>& schedule, real t) {
  auto t0 = std::fmod(t, 24);
  auto it = schedule.begin();
  while (it != schedule.end() && t0 > it->day_hour) {
    it++;
  }

  schedule_point next, prev;
  if (it == schedule.end()) {
    // Past the end of the schedule until 24:00
    it = schedule.begin();
    next = *it;
    prev = *(it - 1);
    next.day_hour += 24;
  } else if (it == schedule.begin()) {
    // Before the start of the schedule from 00:00
    next = *it;
    prev = *(schedule.end() - 1);
    next.day_hour += 24;
    t0 += 24;
  } else {
    // Within schedule
    next = *it;
    prev = *(it - 1);
  }

  auto dt = (t0 - prev.day_hour) / (next.day_hour - prev.day_hour);
  auto ans = prev + (next - prev) * dt;
  ans.day_hour = std::fmod(ans.day_hour, 24);
  return ans;
}

schedule_point schedule_now(const std::vector<schedule_point>& schedule, const std::vector<schedule_override>& ovr,
                            int light) {
  const auto t = std::time(nullptr);
  const auto* tm = std::localtime(&t);
  const auto day_hour = tm->tm_hour + tm->tm_min / real{60};

  for (auto& o : ovr) {
    if (o.light_id == light) {
      if (o.end_hour < o.start_hour) {
        // The time range straddles the 24 hour mark
        if (day_hour > o.start_hour || day_hour < o.end_hour) {
          return schedule_point{day_hour, o.ct, o.lum};
        }
      } else {
        // Normal time spec
        if (day_hour > o.start_hour && day_hour < o.end_hour) {
          return schedule_point{day_hour, o.ct, o.lum};
        }
      }
    }
  }

  return schedule_lerp(schedule, day_hour);
}
