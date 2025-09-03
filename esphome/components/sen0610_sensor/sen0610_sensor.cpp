#include "sen0610_sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sen0610_sensor {
// Helper to update detection thresholds from number entities
static const char *TAG = "sen0610_sensor.sensor";

void Sen0610Sensor::update_delay_thresholds() {
  if (busy_ && millis() < busy_until_) {
    ESP_LOGW(TAG, "Sensor busy, skipping command");
    return;
  }
  ESP_LOGI(TAG, "Updating delay thresholds function called");
  bool delay_changed = this->detection_delay_ != nullptr && this->detection_delay_->has_state();
  bool keepdelay_changed = this->detection_keepdelay_ != nullptr && this->detection_keepdelay_->has_state();

  int delay = radar_.getTrigDelay();
  int keepdelay = radar_.getKeepTimerout();

  ESP_LOGI(TAG, "Current radar delay: %d, keepdelay: %d", delay, keepdelay);

  if (delay_changed) {
    delay = static_cast<int>(this->detection_delay_->state);
  }
  if (keepdelay_changed) {
    keepdelay = static_cast<int>(this->detection_keepdelay_->state);
  }
  ESP_LOGI(TAG, "Attempting to set delay: %d, keepdelay: %d", delay, keepdelay);
  if (delay_changed || keepdelay_changed) {
    if (radar_.setDelay(delay, keepdelay)) {
      ESP_LOGI(TAG, "Detection delays successfully set to: delay: %d, keepdelay: %d", delay, keepdelay);
      // Immediately read back and log the values to verify
      busy_ = true;
      busy_until_ = millis() + SENSOR_BUSY_MS;
    } else {
      ESP_LOGW(TAG, "Failed to set detection delays: delay: %d, keepdelay: %d", delay, keepdelay);
    }
  }
}

void Sen0610Sensor::update_detection_thresholds() {
  if (busy_ && millis() < busy_until_) {
    ESP_LOGW(TAG, "Sensor busy, skipping command");
    return;
  }
  // TODO: something going wrong here, seems to overwrite delay thresholds
  // perhaps the DFRobot class isnt working properly and need to write to I2C directly
  bool min_changed = this->detection_range_min_ != nullptr && this->detection_range_min_->has_state();
  bool max_changed = this->detection_range_max_ != nullptr && this->detection_range_max_->has_state();
  bool thres_changed = this->detection_range_threshold_ != nullptr && this->detection_range_threshold_->has_state();

  int min_range = radar_.getMinRange();
  int max_range = radar_.getMaxRange();
  int thres_range = radar_.getTrigRange();

  if (min_changed) {
    min_range = static_cast<int>(this->detection_range_min_->state);
  }
  if (max_changed) {
    max_range = static_cast<int>(this->detection_range_max_->state);
  }
  if (thres_changed) {
    thres_range = static_cast<int>(this->detection_range_threshold_->state);
  }
  if (min_changed || max_changed || thres_changed) {
    if (radar_.setDetectThres(min_range, max_range, thres_range)) {
      ESP_LOGI(TAG, "Detection thresholds successfully set to: min: %d, max: %d, thres: %d", min_range, max_range,
               thres_range);
      busy_ = true;
      busy_until_ = millis() + SENSOR_BUSY_MS;

    } else {
      ESP_LOGW(TAG, "Failed to set detection thresholds: min: %d, max: %d, thres: %d", min_range, max_range,
               thres_range);
    }
  }
}

void Sen0610Sensor::setup_sensor() {
  // radar_.setSensor(eRecoverSen);
  // radar_.setFrettingDetection(eON);
  // radar_.setSensorMode(eSpeedMode);
  // radar_.setSensor(eResetSen);
}

void Sen0610Sensor::setup() {
  // TODO: replace all this with DFRobot_C4001 class usage

  // I2C device initialization is typically done here.
  // Note that a number of read/write methods are available in the I2CDevice
  // class. See "i2c/i2c.h" for details.

  // Add callback for immediate slider changes
  if (this->radar_trig_sensitivity_ != nullptr) {
    this->radar_trig_sensitivity_->add_on_state_callback([this](int value) {
      if (busy_ && millis() < busy_until_) {
        ESP_LOGW(TAG, "Sensor busy, skipping command");
        return;
      }
      if (radar_.setTrigSensitivity(value)) {
        ESP_LOGI(TAG, "Trigger sensitivity successfully set to: %d (callback)", value);
        busy_ = true;
        busy_until_ = millis() + SENSOR_BUSY_MS;

      } else {
        ESP_LOGW(TAG, "Failed to set trigger sensitivity: %d (callback)", value);
      }
    });
  }
  if (this->radar_keep_sensitivity_ != nullptr) {
    this->radar_keep_sensitivity_->add_on_state_callback([this](int value) {
      if (busy_ && millis() < busy_until_) {
        ESP_LOGW(TAG, "Sensor busy, skipping command");
        return;
      }
      if (radar_.setKeepSensitivity(value)) {
        ESP_LOGI(TAG, "Keep sensitivity successfully set to: %d (callback)", value);
        busy_ = true;
        busy_until_ = millis() + SENSOR_BUSY_MS;

      } else {
        ESP_LOGW(TAG, "Failed to set keep sensitivity: %d (callback)", value);
      }
    });
  }

  if (this->detection_range_min_ != nullptr) {
    this->detection_range_min_->add_on_state_callback([this](int value) { this->update_detection_thresholds(); });
  }
  if (this->detection_range_max_ != nullptr) {
    this->detection_range_max_->add_on_state_callback([this](int value) { this->update_detection_thresholds(); });
  }
  if (this->detection_range_threshold_ != nullptr) {
    this->detection_range_threshold_->add_on_state_callback([this](int value) { this->update_detection_thresholds(); });
  }
  // TODO: something here isnt working, the functions dont get called to set the config:
  if (this->detection_delay_ != nullptr) {
    this->detection_delay_->add_on_state_callback([this](int value) {
      ESP_LOGI(TAG, "detection_delay callback triggered with value: %d", value);
      this->update_delay_thresholds();
    });
  }
  if (this->detection_keepdelay_ != nullptr) {
    this->detection_keepdelay_->add_on_state_callback([this](int value) {
      ESP_LOGI(TAG, "detection_keepdelay callback triggered with value: %d", value);
      this->update_delay_thresholds();
    });
  }

  // Register reset button callback
  if (this->reset_sensor_button_ != nullptr) {
    this->reset_sensor_button_->add_on_press_callback([this]() { this->reset_sensor(); });
  }

  if (this->recover_sensor_button_ != nullptr) {
    this->recover_sensor_button_->add_on_press_callback([this]() { this->recover_sensor(); });
  }
  if (this->save_config_button_ != nullptr) {
    this->save_config_button_->add_on_press_callback([this]() { this->save_config(); });
  }
  if (this->start_sensor_button_ != nullptr) {
    this->start_sensor_button_->add_on_press_callback([this]() { this->start_sensor(); });
  }

  if (this->stop_sensor_button_ != nullptr) {
    this->stop_sensor_button_->add_on_press_callback([this]() { this->stop_sensor(); });
  }
}
void Sen0610Sensor::reset_sensor() {
  if (busy_ && millis() < busy_until_) {
    ESP_LOGW(TAG, "Sensor busy, skipping command");
    return;
  }
  ESP_LOGI(TAG, "Resetting sensor...");
  radar_.setSensor(eResetSen);  // Or your actual reset logic
  busy_ = true;
  busy_until_ = millis() + SENSOR_BUSY_MS;
}

void Sen0610Sensor::recover_sensor() {
  if (busy_ && millis() < busy_until_) {
    ESP_LOGW(TAG, "Sensor busy, skipping command");
    return;
  }
  ESP_LOGI(TAG, "Recovering sensor...");
  radar_.setSensor(eRecoverSen);  // Or your actual recover logic
  busy_ = true;
  busy_until_ = millis() + SENSOR_BUSY_MS;
}

void Sen0610Sensor::start_sensor() {
  if (busy_ && millis() < busy_until_) {
    ESP_LOGW(TAG, "Sensor busy, skipping command");
    return;
  }
  ESP_LOGI(TAG, "Starting sensor...");
  radar_.setSensor(eStartSen);  // Or your actual start logic
  busy_ = true;
  busy_until_ = millis() + SENSOR_BUSY_MS;
}

void Sen0610Sensor::stop_sensor() {
  if (busy_ && millis() < busy_until_) {
    ESP_LOGW(TAG, "Sensor busy, skipping command");
    return;
  }
  ESP_LOGI(TAG, "Stopping sensor...");
  radar_.setSensor(eStopSen);  // Or your actual stop logic
  busy_ = true;
  busy_until_ = millis() + SENSOR_BUSY_MS;
}

void Sen0610Sensor::save_config() {
  if (busy_ && millis() < busy_until_) {
    ESP_LOGW(TAG, "Sensor busy, skipping command");
    return;
  }
  ESP_LOGI(TAG, "Saving sensor configuration...");
  radar_.setSensor(eSaveParams);  // Or your actual save config logic
  busy_ = true;
  busy_until_ = millis() + SENSOR_BUSY_MS;
}

void Sen0610Sensor::update() {
  if (busy_ && millis() >= busy_until_) {
    busy_ = false;
  }
  if (busy_ && millis() < busy_until_) {
    ESP_LOGW(TAG, "Sensor busy, skipping command");
    return;
  }
  // TODO: replace all this with DFRobot_C4001 class usage
  if (!radar_initialized_) {
    if (radar_.begin()) {
      ESP_LOGI(TAG, "Radar initialization successful");
      radar_initialized_ = true;
      this->setup_sensor();
      // Apply initial settings from number entities
      this->update_detection_thresholds();
      this->update_delay_thresholds();
    } else {
      ESP_LOGE(TAG, "Radar initialization failed");
      return;  // Exit the update function if initialization fails
    }
  }
  // Read radar data
  if (radar_initialized_) {
    ESP_LOGI(TAG, "status: %d", radar_.getStatus().workStatus);
    ESP_LOGI(TAG, "energy: %u", static_cast<uint32_t>(radar_.getTargetEnergy()));
    ESP_LOGI(TAG, "targets: %u", radar_.getTargetNumber());
    ESP_LOGI(TAG, "speed: %f", radar_.getTargetSpeed());
    ESP_LOGI(TAG, "motion detection: %d", static_cast<bool>(radar_.motionDetection()));

    ESP_LOGI("TAG",
             "config: trigger sensitivity: %u keep sensitivity: %u min range: %u max range: %u trigrange: %u keeptime: "
             "%u trigdelay: %u ",
             radar_.getTrigSensitivity(), radar_.getKeepSensitivity(), radar_.getMinRange(), radar_.getMaxRange(),
             radar_.getTrigRange(), radar_.getKeepTimerout(), radar_.getTrigDelay());

    // Publish the response as a sensor state
    this->publish_state(radar_.getTargetNumber());
    if (this->target_energy_ != nullptr) {
      this->target_energy_->publish_state(static_cast<float>(radar_.getTargetEnergy()));
    }
    if (this->target_speed_ != nullptr) {
      this->target_speed_->publish_state(static_cast<float>(radar_.getTargetSpeed()));
    }
    if (this->target_range_ != nullptr) {
      this->target_range_->publish_state(static_cast<float>(radar_.getTargetRange()));
    }
  }
}
void Sen0610Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "sen0610 sensor");
  ESP_LOGCONFIG("  energy", "%p", this->target_energy_);
  ESP_LOGCONFIG("  speed", "%p", this->target_speed_);
  ESP_LOGCONFIG("  range", "%p", this->target_range_);
  ESP_LOGCONFIG("  radar_trig_sensitivity", "%p", this->radar_trig_sensitivity_);
  ESP_LOGCONFIG("  detection_range_min", "%p", this->detection_range_min_);
  ESP_LOGCONFIG("  detection_range_max", "%p", this->detection_range_max_);
  ESP_LOGCONFIG("  detection_range_threshold", "%p", this->detection_range_threshold_);
  ESP_LOGCONFIG("  detection_delay", "%p", this->detection_delay_);
  ESP_LOGCONFIG("  detection_keepdelay", "%p", this->detection_keepdelay_);
  ESP_LOGCONFIG("  radar_keep_sensitivity", "%p", this->radar_keep_sensitivity_);
}

}  // namespace sen0610_sensor
}  // namespace esphome
