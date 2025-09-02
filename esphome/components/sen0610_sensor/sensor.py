import esphome.codegen as cg
from esphome.components import button, i2c, number, sensor
import esphome.config_validation as cv
from esphome.const import ICON_HEART_PULSE, UNIT_EMPTY

DEPENDENCIES = ["i2c"]

sen0610_sensor_ns = cg.esphome_ns.namespace("sen0610_sensor")
Sen0610Sensor = sen0610_sensor_ns.class_(
    "Sen0610Sensor", cg.PollingComponent, i2c.I2CDevice
)
CONF_TARGET_SPEED = "target_speed"
CONF_TARGET_ENERGY = "target_energy"
CONF_TARGET_RANGE = "target_range"
CONF_RADAR_TRIG_SENSITIVITY = "radar_trig_sensitivity"
CONF_DETECTION_RANGE_MIN = "detection_range_min"
CONF_DETECTION_RANGE_MAX = "detection_range_max"
CONF_DETECTION_RANGE_THRESHOLD = "detection_range_threshold"
CONF_DETECTION_DELAY = "detection_delay"
CONF_DETECTION_KEEPDELAY = "detection_keepdelay"
CONF_RADAR_KEEP_SENSITIVITY = "radar_keep_sensitivity"
CONF_RESET_SENSOR_BUTTON = "reset_sensor_button"
CONF_RECOVER_SENSOR_BUTTON = "recover_sensor_button"
CONF_SAVE_CONFIG_BUTTON = "save_config_button"

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        Sen0610Sensor,
        unit_of_measurement=UNIT_EMPTY,
        icon=ICON_HEART_PULSE,
        accuracy_decimals=1,
    )
    .extend(
        {
            cv.Optional(CONF_TARGET_SPEED): sensor.sensor_schema(
                unit_of_measurement="m/s",
                icon="mdi:speedometer",
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_TARGET_ENERGY): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                icon="mdi:flash",
                accuracy_decimals=0,
            ),
            cv.Optional(CONF_TARGET_RANGE): sensor.sensor_schema(
                unit_of_measurement="m",
                icon="mdi:ruler",
                accuracy_decimals=2,
            ),
            cv.Optional(CONF_RADAR_TRIG_SENSITIVITY): cv.use_id(number.Number),
            cv.Optional(CONF_DETECTION_RANGE_MIN): cv.use_id(number.Number),
            cv.Optional(CONF_DETECTION_RANGE_MAX): cv.use_id(number.Number),
            cv.Optional(CONF_DETECTION_RANGE_THRESHOLD): cv.use_id(number.Number),
            cv.Optional(CONF_DETECTION_DELAY): cv.use_id(number.Number),
            cv.Optional(CONF_DETECTION_KEEPDELAY): cv.use_id(number.Number),
            cv.Optional(CONF_RADAR_KEEP_SENSITIVITY): cv.use_id(number.Number),
            cv.Optional(CONF_RESET_SENSOR_BUTTON): cv.use_id(button.Button),
            cv.Optional(CONF_RECOVER_SENSOR_BUTTON): cv.use_id(button.Button),
            cv.Optional(CONF_SAVE_CONFIG_BUTTON): cv.use_id(button.Button),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(i2c.i2c_device_schema(0x2A))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_TARGET_SPEED in config:
        speed_sensor = await sensor.new_sensor(config[CONF_TARGET_SPEED])
        cg.add(var.set_target_speed_sensor(speed_sensor))

    if CONF_TARGET_ENERGY in config:
        energy_sensor = await sensor.new_sensor(config[CONF_TARGET_ENERGY])
        cg.add(var.set_target_energy_sensor(energy_sensor))

    if CONF_RADAR_TRIG_SENSITIVITY in config:
        trig_num = await cg.get_variable(config[CONF_RADAR_TRIG_SENSITIVITY])
        cg.add(var.set_radar_trig_sensitivity_number(trig_num))

    if CONF_TARGET_RANGE in config:
        range_sensor = await sensor.new_sensor(config[CONF_TARGET_RANGE])
        cg.add(var.set_target_range_sensor(range_sensor))

    if CONF_DETECTION_RANGE_MIN in config:
        min_num = await cg.get_variable(config[CONF_DETECTION_RANGE_MIN])
        cg.add(var.set_detection_range_min_number(min_num))

    if CONF_DETECTION_RANGE_MAX in config:
        max_num = await cg.get_variable(config[CONF_DETECTION_RANGE_MAX])
        cg.add(var.set_detection_range_max_number(max_num))

    if CONF_DETECTION_RANGE_THRESHOLD in config:
        thres_num = await cg.get_variable(config[CONF_DETECTION_RANGE_THRESHOLD])
        cg.add(var.set_detection_range_threshold_number(thres_num))

    if CONF_DETECTION_DELAY in config:
        delay_num = await cg.get_variable(config[CONF_DETECTION_DELAY])
        cg.add(var.set_detection_delay_number(delay_num))

    if CONF_DETECTION_KEEPDELAY in config:
        keep_num = await cg.get_variable(config[CONF_DETECTION_KEEPDELAY])
        cg.add(var.set_detection_keepdelay_number(keep_num))

    if CONF_RADAR_KEEP_SENSITIVITY in config:
        keep_sen = await cg.get_variable(config[CONF_RADAR_KEEP_SENSITIVITY])
        cg.add(var.set_radar_keep_sensitivity_number(keep_sen))

    if CONF_RESET_SENSOR_BUTTON in config:
        reset_btn = await cg.get_variable(config[CONF_RESET_SENSOR_BUTTON])
        cg.add(var.set_reset_sensor_button(reset_btn))

    if CONF_RECOVER_SENSOR_BUTTON in config:
        recover_btn = await cg.get_variable(config[CONF_RECOVER_SENSOR_BUTTON])
        cg.add(var.set_recover_sensor_button(recover_btn))

    if CONF_SAVE_CONFIG_BUTTON in config:
        save_btn = await cg.get_variable(config[CONF_SAVE_CONFIG_BUTTON])
        cg.add(var.set_save_config_button(save_btn))
