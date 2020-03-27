#include <chrono>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <vector>

#include "Hue.h"
#include "LinHttpHandler.h"
#include "schedule.h"

using namespace std::chrono_literals;

constexpr auto c_max_brightness = 254;

// For reference
// 2000K just before/after sun-rise/set
// 3000-3500K sunset on horizon
// 4500-5000K early morning/late afternoon
// 5600K default daylight

// May in Munich:
std::vector<schedule_point> schedule = {
    {1.0, 2000.0, 0.1},   // Solar Midnight
    {6.0, 2000.0, 0.5},   // Dawn
    {8.0, 3500.0, 0.7},   // Sunrise
    {9.0, 5000.0, 0.9},   // Morning
    {13.0, 6500.0, 1.0},  // Solar Noon
    {20.0, 4500.0, 0.8},  // Evening
    {22.0, 3000.0, 0.6},  // Sunset
    {23.0, 2000.0, 0.1},  // Dusk
};

auto find_bridge_id(HueFinder& finder) {
  const auto bridge_mac = getenv("HUE_MAC");

  while (true) {
    auto bridges = finder.FindBridges();
    for (auto& b : bridges) {
      if (bridge_mac) {
        if (b.mac == HueFinder::NormalizeMac(bridge_mac)) {
          std::cout << "Found specified bridge with MAC: " << bridge_mac << std::endl;
          return b;
        }
      } else {
        std::cout << "HUE_MAC unspecified, chosing: " << b.mac << std::endl;
        return b;
      }
    }

    if (bridge_mac) {
      std::cout << "Didn't find specified bridge with MAC: " << bridge_mac << std::endl;
    } else {
      std::cout << "No bridges found!" << std::endl;
    }
    std::this_thread::sleep_for(10s);
  }
}

auto find_bridge() {
  const auto bridge_user = getenv("HUE_APIKEY");
  auto finder = HueFinder{std::make_shared<LinHttpHandler>()};
  auto bridge_id = find_bridge_id(finder);

  if (bridge_user) {
    std::cout << "HUE_APIKEY specified, using API key..." << std::endl;
    finder.AddUsername(bridge_id.mac, bridge_user);
  } else {
    std::cout << "HUE_APIKEY not specified, requesting new API key..." << std::endl;
  }
  return finder.GetBridge(bridge_id);
}

int main() {
  auto bridge = find_bridge();

  while (true) {
    auto t = std::time(nullptr);
    auto* tm = std::localtime(&t);
    auto day_hour = tm->tm_hour + tm->tm_min / real{60};
    auto point = schedule_lerp(schedule, day_hour);

    for (auto& light_ref : bridge.getAllLights()) {
      auto& light = light_ref.get();
      if (light.isOn()) {
        light.setBrightness(c_max_brightness * point.lum);
        light.setColorTemperature(light.KelvinToMired(point.ct));
      }
    }
    std::this_thread::sleep_for(5s);
  }
}
