#include <chrono>
#include <thread>
#include <unordered_set>
#include <vector>

#include "hueplusplus/Bridge.h"
#include "hueplusplus/ColorUnits.h"
#include "hueplusplus/LinHttpHandler.h"
#include "schedule.h"

using namespace std::chrono_literals;

using hueplusplus::BridgeFinder;
using hueplusplus::kelvinToMired;
using hueplusplus::LinHttpHandler;

constexpr auto c_max_brightness = 254;

// For reference
// 2000K just before/after sun-rise/set
// 3000-3500K sunset on horizon
// 4500-5000K early morning/late afternoon
// 5600K default daylight
const std::vector<schedule_point> schedule = {
    // {day_hour, colour_temperature, brightness}
    {1.0, 2000.0, 0.1},   // Solar Midnight
    {6.0, 2000.0, 0.5},   // Dawn
    {8.0, 3500.0, 0.7},   // Sunrise
    {9.0, 5000.0, 0.9},   // Morning
    {13.0, 6500.0, 1.0},  // Solar Noon
    {20.0, 4500.0, 0.8},  // Evening
    {22.0, 3000.0, 0.6},  // Sunset
    {23.0, 2000.0, 0.1},  // Dusk
};

const std::vector<schedule_override> sched_ovr = {
    // {day_hour_from, day_hour_to, light_id, colour_temperature, brightness}
    {19, 6, 4, 2700, 1.0}};

auto find_bridge_id(BridgeFinder& finder) {
  const auto bridge_mac = getenv("HUE_MAC");

  while (true) {
    auto bridges = finder.findBridges();
    for (auto& b : bridges) {
      if (bridge_mac) {
        if (b.mac == BridgeFinder::normalizeMac(bridge_mac)) {
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
  auto finder = BridgeFinder{std::make_shared<LinHttpHandler>()};
  auto bridge_id = find_bridge_id(finder);

  if (bridge_user) {
    std::cout << "HUE_APIKEY specified, using API key..." << std::endl;
    finder.addUsername(bridge_id.mac, bridge_user);
  } else {
    std::cout << "HUE_APIKEY not specified, requesting new API key..." << std::endl;
  }
  return finder.getBridge(bridge_id);
}

int main() {
  auto bridge = find_bridge();
  std::cout << "Bridge connected!" << std::endl;
  auto& lights = bridge.lights();
  lights.setRefreshDuration(hueplusplus::c_refreshNever);

  constexpr auto c_scan_delay = 500ms;
  constexpr auto c_schedule_delay = 30s;
  auto last_schedule_update = std::chrono::system_clock::now();

  std::unordered_set<int> enabled_lights;

  while (true) {
    try {
      std::this_thread::sleep_for(c_scan_delay);

      const auto now = std::chrono::system_clock::now();
      lights.refresh();

      for (auto& light : lights.getAll()) {
        if (light.isOn()) {
          if (enabled_lights.count(light.getId()) == 0 ||    // Freshly turned on
              now - last_schedule_update > c_schedule_delay  // Or schedule update
          ) {
            auto point = schedule_now(schedule, sched_ovr, light.getId());
            light
                .transaction()                                 //
                .setBrightness(c_max_brightness * point.lum)   //
                .setColorTemperature(kelvinToMired(point.ct))  //
                .commit();
            enabled_lights.insert(light.getId());
          }
        } else {
          enabled_lights.erase(light.getId());
        }
      }
      if (now - last_schedule_update > c_schedule_delay) {
        last_schedule_update = now;
      }
    } catch (const std::system_error& err) {
      std::cout << "Caught std::system_error, what(): " << err.what() << std::endl;
    }
  }
}
