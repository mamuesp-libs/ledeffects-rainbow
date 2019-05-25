#include "mgos.h"
#include "led_master.h"

static void mgos_intern_rainbow_loop(mgos_rgbleds* leds)
{
    uint32_t num_rows = leds->panel_height;
    uint32_t num_cols = leds->panel_width;
    tools_rgb_data out_pix;
    leds->pix_pos = leds->pix_pos < 0 ? 255 : leds->pix_pos;

    int run = leds->internal_loops;
    run = run <= 0 ? 1 : run;

    while (run--) {
        for (int col = 0; col < num_cols; col++) {
            for (int row = 0; row < num_rows; row++) {
                int step = num_cols ? (256 / num_cols) : 0;
                int pix_pos = (leds->pix_pos + col * step) % 256;
                out_pix = tools_color_wheel(((row * 256 / num_rows) + pix_pos) & 255, 255.0);
                LOG(LL_VERBOSE_DEBUG, ("mgos_rainbow:\tR: 0x%.02X\tG: 0x%.02X\tB: 0x%.02X", out_pix.r, out_pix.g, out_pix.b));
                mgos_universal_led_plot_pixel(leds, col, row, out_pix, false);
            }
        }
        mgos_universal_led_show(leds);
        leds->pix_pos -= leds->internal_loops;
        leds->pix_pos = leds->pix_pos < 0 ? 255 : leds->pix_pos;
    }
}

void mgos_ledeffects_rainbow(void* param, mgos_rgbleds_action action)
{
    static bool do_time = false;
    static uint32_t max_time = 0;
    uint32_t time = (mgos_uptime_micros() / 1000);
    mgos_rgbleds* leds = (mgos_rgbleds*)param;

    switch (action) {
    case MGOS_RGBLEDS_ACT_INIT:
      leds->timeout = mgos_sys_config_get_ledeffects_rainbow_timeout();
      leds->dim_all = mgos_sys_config_get_ledeffects_rainbow_dim_all();
      LOG(LL_DEBUG, ("mgos_rainbow: called (init)"));
      break;
    case MGOS_RGBLEDS_ACT_EXIT:
        LOG(LL_DEBUG, ("mgos_rainbow: called (exit)"));
        break;
    case MGOS_RGBLEDS_ACT_LOOP:
        mgos_intern_rainbow_loop(leds);
        if (do_time) {
            time = (mgos_uptime_micros() /1000) - time;
            max_time = (time > max_time) ? time : max_time;
            LOG(LL_VERBOSE_DEBUG, ("Rainbow loop duration: %d milliseconds, max: %d ...", time / 1000, max_time / 1000));
        }
        break;
    }
}

bool mgos_rainbow_init(void) {
  LOG(LL_INFO, ("mgos_rainbow_init ..."));
  ledmaster_add_effect("ANIM_RAINBOW", mgos_ledeffects_rainbow);
  return true;
}
