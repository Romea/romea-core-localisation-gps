# romea_core_localisation_gps

## Overview

`romea_core_localisation_gps` is a framework-independent C++ library that converts GPS/GNSS NMEA data into typed localisation observations.

It sits between `romea_core_gps`, which parses NMEA sentences and models the receiver, and `romea_core_localisation`, which defines the observations fused by localisation filters. The package validates incoming GPS data, converts WGS84 positions to a local ENU frame, builds position and course observations, and exposes diagnostic reports.

Middleware-specific nodes and message conversions are intentionally kept outside this library.

---

## Concept

The package provides two GPS localisation plugin variants:

| Plugin | GPS setup | Produced observations |
| ------ | --------- | --------------------- |
| `SingleAntennaGPSPlugin` | One GPS antenna. | `ObservationPosition` from `GGA`, and `ObservationCourse` from `RMC` when the vehicle speed is high enough. |
| `DualAntennaGPSPlugin` | Two GPS antennas or a receiver able to output true heading. | `ObservationPosition` from `GGA`, and `ObservationCourse` from `HDT`. |

Both variants share `GPSPluginBase`, which owns the `GPSReceiver`, the WGS84-to-ENU converter and the common `GGA` diagnostics.

---

## Input sentences and observations

| Input | Processing | Output |
| ----- | ---------- | ------ |
| `GGA` sentence | Checks sentence rate, completeness, fix quality, number of satellites and HDOP, then converts latitude, longitude and altitude to ENU coordinates. | `ObservationPosition` |
| `RMC` sentence | Checks sentence rate and track-angle validity, then converts the GPS track angle to a course angle using the current vehicle linear speed. | `ObservationCourse` for single-antenna GPS |
| `HDT` sentence | Checks sentence rate and heading validity, then converts true heading to a course angle. | `ObservationCourse` for dual-antenna GPS |
| `GSV` sentence | Updates the satellites-in-view information stored by `GPSReceiver`. | No observation, used for receiver state information |

Other NMEA sentences may be supported in the future if they provide useful localisation information.

---

## Position observation

`process_gga()` creates a 2D position observation:

1. Parse the `GGA` sentence with `GGAFrame`.
2. Check that the sentence is received at the expected rate and contains the required fields.
3. Check that the fix quality is at least the configured minimal fix quality.
4. Convert the geodetic position to the local ENU frame with `ENUConverter`.
5. Fill `ObservationPosition` with the ENU `x` and `y` coordinates.
6. Store the GPS antenna position in the observation `lever_arm` field.

The WGS84 anchor must be set with `set_anchor()` before processing position observations. It defines the origin of the local ENU frame used by robot-to-world localisation.

The position covariance is built from the receiver UERE associated with the `GGA` fix quality and the `HDOP` reported by the sentence:

```text
horizontal_position_std = HDOP * UERE(fix_quality)
position_covariance = I * horizontal_position_std^2
```

The UERE values are receiver and antenna dependent. They must be characterised for each fix quality used by the application.

---

## Course observation

`ObservationCourse` can be produced in two different ways:

| GPS setup | Method | Details |
| --------- | ------ | ------- |
| Single antenna | `process_rmc()` | Uses the `RMC` track angle. The vehicle linear speed must be available and above the configured minimum speed so that the track angle is meaningful. |
| Dual antenna | `process_hdt()` | Uses the true heading from `HDT`, which does not require vehicle motion. |

Both methods currently use a fixed course-angle standard deviation defined in the implementation.

---

## Diagnostics

The package provides diagnostic helpers for GPS localisation inputs:

| Diagnostic helper | Checks |
| ----------------- | ------ |
| `CheckupGGAFix` | Required `GGA` fields, minimal fix quality, HDOP threshold and number of satellites used to compute the fix. |
| `CheckupRMCTrackAngle` | Required `RMC` fields, valid fix status and sufficient vehicle speed for single-antenna course observations. |
| `CheckupHDTTrackAngle` | Required `HDT` fields and valid true-heading data for dual-antenna course observations. |

The plugin variants also monitor sentence rates and reset their diagnostic state when expected data stops arriving.

---

## Minimal usage

```cpp
#include <memory>

#include <romea_core_gps/GPSReceiver.hpp>
#include <romea_core_localisation_gps/gps_plugin.hpp>

auto gps = std::make_unique<romea::core::GPSReceiver>();

romea::core::localisation::SingleAntennaGPSPlugin plugin(
  std::move(gps),
  romea::core::FixQuality::RTK_FIX,
  minimal_speed_over_ground);

plugin.set_anchor(wgs84_anchor);

romea::core::localisation::ObservationPosition position_observation;
if (plugin.process_gga(stamp, gga_sentence, position_observation)) {
  // Send position_observation to a localisation updater.
}

plugin.process_linear_speed(stamp, vehicle_linear_speed);

romea::core::localisation::ObservationCourse course_observation;
if (plugin.process_rmc(stamp, rmc_sentence, course_observation)) {
  // Send course_observation to a localisation updater.
}

auto diagnostics = plugin.make_diagnostic_report(stamp);
```

---

## Related packages

| Package | Role |
| ------- | ---- |
| `romea_core_gps` | NMEA parsing, GPS receiver facade, fix-quality UEREs and satellites-in-view reliability. |
| `romea_core_localisation` | Localisation observations and filter assembly components. |

---

## License

This project is released under the Apache License 2.0. See the `LICENSE` file for details.

## Authors

This library was developed by **Jean Laneurit** with scientific contributions from **Christophe Debain**, **Roland Chapuis** and **Romuald Aufrere**, in the context of the Baudet Rob 2 ANR project.
