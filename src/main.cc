#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>

#include "Hue.h"
#include "LinHttpHandler.h"

using namespace std::chrono_literals;
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

// 2000K just before/after sun-rise/set
// 3000-3500K sunset on horizon
// 4500-5000K early morning/late afternoon
// 5600K default daylight
// May in Munich:
//
// "Night time" = 22 - 7
// "Morning" = 7 - 10
// "Day" = 10 - 16
// "Evening" = 16 - 20
std::vector<gradient_point> schedule = {
    {1.0, 2000.0, 0.1},   // Solar Midnight
    {6.0, 2000.0, 0.5},   // Dawn
    {8.0, 3500.0, 0.7},   // Sunrise
    {9.0, 5000.0, 0.9},   // Morning
    {13.0, 6500.0, 1.0},  // Solar Noon
    {20.0, 4500.0, 0.8},  // Evening
    {22.0, 3000.0, 0.6},  // Sunset
    {23.0, 2000.0, 0.1},  // Dusk
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

Hue getBridge() {
  auto http_handler = std::make_shared<LinHttpHandler>();
  HueFinder finder = {http_handler};
  std::vector<HueFinder::HueIdentification> bridges = finder.FindBridges();
  if (bridges.empty()) {
    std::cout << "No bridges found!" << std::endl;
    exit(1);
  }


  return finder.GetBridge(bridges[0]);
}

int main() {
  auto bridge = getBridge();

  while (true) {
    std::time_t t = std::time(nullptr);
    auto* tm = std::localtime(&t);
    real day_hour = tm->tm_hour + tm->tm_min / 60.0;
    auto s = lerp(day_hour);

    for (auto& light_ref : bridge.getAllLights()) {
      auto& light = light_ref.get();
      if (light.isOn()) {
        light.setBrightness(254 * s.lum);
        light.setColorTemperature(light.KelvinToMired(s.ct));
      }
    }
    std::this_thread::sleep_for(5s);
  }
}
