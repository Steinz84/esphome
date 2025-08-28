#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "DFRobot_C4001.h"
#include "esphome/components/number/number.h"
#include "esphome/components/button/button.h"
#define I2C_COMMUNICATION

namespace esphome {
namespace sen0610_sensor {

class Sen0610Sensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  void set_target_speed_sensor(sensor::Sensor *sensor) { target_speed_ = sensor; }
  void set_target_energy_sensor(sensor::Sensor *sensor) { target_energy_ = sensor; }
  void set_target_range_sensor(sensor::Sensor *sensor) { target_range_ = sensor; }
  void set_radar_trig_sensitivity_number(number::Number *number) { radar_trig_sensitivity_ = number; }
  void set_radar_keep_sensitivity_number(number::Number *number) { radar_keep_sensitivity_ = number; }
  void set_detection_range_min_number(number::Number *number) { detection_range_min_ = number; }
  void set_detection_range_max_number(number::Number *number) { detection_range_max_ = number; }
  void set_detection_range_threshold_number(number::Number *number) { detection_range_threshold_ = number; }

  void update_detection_thresholds();
  void update_delay_thresholds();
  void setup_sensor();
  void set_detection_delay_number(number::Number *number) { detection_delay_ = number; }
  void set_detection_keepdelay_number(number::Number *number) { detection_keepdelay_ = number; }
  void set_reset_sensor_button(button::Button *button) { reset_sensor_button_ = button; }
  void reset_sensor();
  void set_recover_sensor_button(button::Button *button) { recover_sensor_button_ = button; }
  void recover_sensor();
  bool busy_ = false;
  uint32_t busy_until_ = 0;
  static constexpr uint32_t SENSOR_BUSY_MS = 3000;

 protected:
  bool radar_initialized_{false};
  DFRobot_C4001_I2C radar_;  // Radar object lives as long as the component
  sensor::Sensor *target_energy_{nullptr};
  sensor::Sensor *target_speed_{nullptr};
  sensor::Sensor *target_range_{nullptr};
  number::Number *radar_trig_sensitivity_{nullptr};
  number::Number *detection_range_min_{nullptr};
  number::Number *detection_range_max_{nullptr};
  number::Number *detection_range_threshold_{nullptr};
  number::Number *detection_delay_{nullptr};
  number::Number *detection_keepdelay_{nullptr};
  number::Number *radar_keep_sensitivity_{nullptr};
  button::Button *reset_sensor_button_{nullptr};
  button::Button *recover_sensor_button_{nullptr};
};

}  // namespace sen0610_sensor
}  // namespace esphome
