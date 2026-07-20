/*
 * Runtime CPI (DPI) adjust for the PMW3610 trackball.
 *
 * The badjeff pmw3610 driver registers a sensor_driver_api whose attr_set
 * routes PMW3610_ATTR_CPI to a runtime write of the RES_STEP register (the
 * write path works on this sensor batch — only the *read* of 0x85 is broken,
 * see config/west.yml). The driver does not export its headers to other
 * modules, so the attr value and CPI range are mirrored here.
 *
 * The trackball lives on the split central (right half) and behaviors run on
 * the central, so a plain local device call is enough. The peripheral build
 * compiles a no-op (its &trackball node stays disabled).
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_cpi_adjust

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define TRACKBALL_NODE DT_NODELABEL(trackball)

/* Mirrored from the driver's src/pmw3610.h (enum pmw3610_attribute: CPI is
 * the first entry). */
#define PMW3610_ATTR_CPI 0
#define PMW3610_MIN_CPI 200
#define PMW3610_MAX_CPI 3200

struct behavior_cpi_adjust_config {
    int step;
};

#if DT_NODE_HAS_STATUS(TRACKBALL_NODE, okay)

/* Boot value comes from the devicetree; runtime changes are RAM-only and
 * reset on power cycle — once the sweet spot is found, bake it into the
 * `cpi` property in charybdis_3610.dtsi. */
static int current_cpi = DT_PROP(TRACKBALL_NODE, cpi);

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_cpi_adjust_config *cfg = dev->config;
    const struct device *trackball = DEVICE_DT_GET(TRACKBALL_NODE);

    int cpi = CLAMP(current_cpi + cfg->step, PMW3610_MIN_CPI, PMW3610_MAX_CPI);

    struct sensor_value val = {.val1 = cpi, .val2 = 0};
    int err = sensor_attr_set(trackball, SENSOR_CHAN_ALL,
                              (enum sensor_attribute)PMW3610_ATTR_CPI, &val);
    if (err) {
        LOG_ERR("Failed to set trackball CPI to %d (%d)", cpi, err);
        return ZMK_BEHAVIOR_OPAQUE;
    }

    current_cpi = cpi;
    LOG_INF("Trackball CPI set to %d", cpi);
    return ZMK_BEHAVIOR_OPAQUE;
}

#else /* peripheral / no local trackball: behavior exists but does nothing */

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

#endif /* DT_NODE_HAS_STATUS(TRACKBALL_NODE, okay) */

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_cpi_adjust_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

#define CPI_ADJ_INST(n)                                                                            \
    static const struct behavior_cpi_adjust_config behavior_cpi_adjust_config_##n = {              \
        .step = DT_INST_PROP(n, step)};                                                            \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, &behavior_cpi_adjust_config_##n, POST_KERNEL,     \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_cpi_adjust_driver_api);

DT_INST_FOREACH_STATUS_OKAY(CPI_ADJ_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
