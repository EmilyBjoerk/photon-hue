#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>

typedef float real;

struct gradient_point {
  real day_hour;
  real ct;
  real lum;
};

gradient_point operator+(const gradient_point& l, const gradient_point& r) {
  return {l.day_hour + r.day_hour, l.ct + r.ct, l.lum + r.lum};
}

gradient_point operator-(const gradient_point& l, const gradient_point& r) {
  return {l.day_hour - r.day_hour, l.ct - r.ct, l.lum - r.lum};
}

gradient_point operator*(const gradient_point& l, real r) {
  return {l.day_hour * r, l.ct * r, l.lum * r};
}

std::ostream& operator<<(std::ostream& os, const gradient_point& p) {
  os << p.day_hour << ", " << p.ct << ", " << p.lum;
  return os;
}

std::vector<gradient_point> schedule = {
    {8.0, 2500.0, 0.1},
    {12.0, 6500.0, 1.0},
    {18.0, 6500.0, 0.8},
    {20.0, 1500.0, 0.4},
};

gradient_point lerp(real t) {
  auto t0 = std::fmod(t, 24.0);
  auto it = schedule.begin();
  while (it != schedule.end() && t0 > it->day_hour) {
    it++;
  }

  gradient_point next, prev;
  if (it == schedule.end()) {
    // Past the end of the schedule until 24:00
    it = schedule.begin();
    next = *it;
    prev = *(it - 1);
    next.day_hour += 24.0;
  } else if (it == schedule.begin()) {
    // Before the start of the schedule from 00:00
    next = *it;
    prev = *(schedule.end() - 1);
    next.day_hour += 24.0;
    t0 += 24.0;
  } else {
    // Within schedule
    next = *it;
    prev = *(it - 1);
  }

  auto dt = (t0 - prev.day_hour) / (next.day_hour - prev.day_hour);
  auto ans = prev + (next - prev) * dt;
  ans.day_hour = std::fmod(ans.day_hour, 24.0);
  return ans;
}

int main() { /*
   for (real t = 0; t < 30; t += 0.1) {
     std::cout << lerp(t) << std::endl;
   }*/

  std::time_t t = std::time(nullptr);

  auto* tm = std::localtime(&t);
  real day_hour = tm->tm_hour + tm->tm_min / 60.0;
  std::cout << lerp(day_hour) << std::endl;
}
