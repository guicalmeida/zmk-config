#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#include "os_status.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct os_status_state {
    bool is_macos;
};

static struct os_status_state get_state(const zmk_event_t *_eh) {
    return (struct os_status_state) {
        .is_macos = zmk_keymap_layer_active(4) || zmk_keymap_layer_active(5) // LOWER_MACOS or RAISE_MACOS
    };
}

static void set_status_symbol(lv_obj_t *label, struct os_status_state state) {
    if (state.is_macos) {
        lv_label_set_text(label, "MAC");
        lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    } else {
        lv_label_set_text(label, "LNX");
        lv_obj_set_style_text_color(label, lv_color_make(0x00, 0xB4, 0xD8), LV_PART_MAIN);
    }
}

static void os_status_update_cb(struct os_status_state state) {
    struct zmk_widget_os_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_status_symbol(widget->obj, state);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_os_status, struct os_status_state,
                            os_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_os_status, zmk_layer_state_changed);

int zmk_widget_os_status_init(struct zmk_widget_os_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_set_size(widget->obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(widget->obj, LV_ALIGN_TOP_RIGHT, -2, 0);
    lv_label_set_text(widget->obj, "LNX");
    lv_obj_set_style_text_font(widget->obj, &lv_font_montserrat_10, LV_PART_MAIN);

    sys_slist_append(&widgets, &widget->node);

    widget_os_status_init();
    return 0;
}

lv_obj_t *zmk_widget_os_status_obj(struct zmk_widget_os_status *widget) {
    return widget->obj;
}
