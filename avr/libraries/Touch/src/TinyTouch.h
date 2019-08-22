#ifndef TINY_TOUCH_H
#define TINY_TOUCH_H

#include "pins_arduino.h"
#include "Arduino.h"
#include "QTouch/touch.h"

class TinyTouch{
  private:
    qtm_acq_t161x_node_config_t *ptc_seq_node_cfg1;
    qtm_acq_node_data_t *ptc_qtlib_node_stat1;
    qtm_acq_node_group_config_t ptc_qtlib_acq_gen1;
    qtm_acquisition_control_t qtlib_acq_set1;
    uint16_t *touch_acq_signals_raw;

    qtm_touch_key_group_config_t qtlib_key_grp_config_set1;
    qtm_touch_key_group_data_t qtlib_key_grp_data_set1;
    qtm_touch_key_data_t *qtlib_key_data_set1;
    qtm_touch_key_config_t *qtlib_key_configs_set1;
    qtm_touch_key_control_t qtlib_key_set1;

    /* Flag to indicate time for touch measurement */
    static volatile uint8_t time_to_measure_touch_flag;

    /* postporcess request flag */
    static volatile uint8_t touch_postprocess_request;

    /* Measurement Done Touch Flag  */
    static volatile uint8_t measurement_done_touch;

    /* Error Handling */
    static uint8_t module_error_code;

    static void qtm_measure_complete_callback(void);
    static void qtm_error_callback(uint8_t error);

  public:
    TinyTouch();
    ~TinyTouch();

    void begin();
    void begin(uint8_t *pinList, uint16_t totalNumPins);
    void end();
    void touchHandle();

    uint16_t getValue(uint8_t sensor_node);
};


#endif