#ifndef TINY_TOUCH_H
#define TINY_TOUCH_H

#include "pins_arduino.h"
#include "Arduino.h"
#include "QTouch/include/touch_api_ptc.h"
#include "avr/interrupt.h"

#define DEF_TOUCH_MEASUREMENT_PERIOD_MS 20

/* Defines the Type of sensor
 * Default value: NODE_MUTUAL.
 */
#define DEF_SENSOR_TYPE NODE_SELFCAP

/* Set sensor calibration mode for charge share delay ,Prescaler or series resistor.
 * Range: CAL_AUTO_TUNE_NONE / CAL_AUTO_TUNE_RSEL / CAL_AUTO_TUNE_PRSC / CAL_AUTO_TUNE_CSD
 * Default value: CAL_AUTO_TUNE_NONE.
 */
#define DEF_PTC_CAL_OPTION CAL_AUTO_TUNE_NONE

/* Defines the interrupt priority for the PTC. Set low priority to PTC interrupt for applications having interrupt time
 * constraints. Range: 0 to 2 Default: 2 (Lowest Priority)
 */
#define DEF_PTC_INTERRUPT_PRIORITY None

/* Calibration option to ensure full charge transfer */
/* Bits 7:0 = XX | TT SELECT_TAU | X | CAL_OPTION */
#define DEF_PTC_TAU_TARGET CAL_CHRG_5TAU
#define DEF_PTC_CAL_AUTO_TUNE (uint8_t)((DEF_PTC_TAU_TARGET << CAL_CHRG_TIME_POS) | DEF_PTC_CAL_OPTION)

/* Set default bootup acquisition frequency.
 * Range: FREQ_SEL_0 - FREQ_SEL_15 , FREQ_SEL_SPREAD
 * Default value: FREQ_SEL_0.
 */
#define DEF_SEL_FREQ_INIT FREQ_SEL_0

#define DEF_TOUCH_DET_INT 4

/* De-bounce counter for additional measurements to confirm away from touch signal
 * to initiate Away from touch re-calibration.
 * Range: 0 to 255.
 * Default value: 5.
 */
#define DEF_ANTI_TCH_DET_INT 5

/* Threshold beyond with automatic sensor recalibration is initiated.
 * Range: RECAL_100/ RECAL_50 / RECAL_25 / RECAL_12_5 / RECAL_6_25 / MAX_RECAL
 * Default value: RECAL_100.
 */
#define DEF_ANTI_TCH_RECAL_THRSHLD RECAL_100

/* Rate at which sensor reference value is adjusted towards sensor signal value
 * when signal value is greater than reference.
 * Units: 200ms
 * Range: 0-255
 * Default value: 20u = 4 seconds.
 */
#define DEF_TCH_DRIFT_RATE 20

/* Rate at which sensor reference value is adjusted towards sensor signal value
 * when signal value is less than reference.
 * Units: 200ms
 * Range: 0-255
 * Default value: 5u = 1 second.
 */
#define DEF_ANTI_TCH_DRIFT_RATE 5

/* Time to restrict drift on all sensor when one or more sensors are activated.
 * Units: 200ms
 * Range: 0-255
 * Default value: 20u = 4 seconds.
 */
#define DEF_DRIFT_HOLD_TIME 20

/* Set mode for additional sensor measurements based on touch activity.
 * Range: REBURST_NONE / REBURST_UNRESOLVED / REBURST_ALL
 * Default value: REBURST_UNRESOLVED
 */
#define DEF_REBURST_MODE REBURST_UNRESOLVED

/* Sensor maximum ON duration upon touch.
 * Range: 0-255
 * Default value: 0
 */
#define DEF_MAX_ON_DURATION 0

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
    volatile uint8_t measurement_done_touch;

    /* Error Handling */
    uint8_t module_error_code;

    static uint8_t interrupt_cnt;
    void qtm_error_callback(uint8_t error);
    void timer_set_period(const uint8_t val);
    
  public:
    
    static void qtm_measure_complete_callback(void);
    static void touch_timer_handler();
    
    TinyTouch();
    ~TinyTouch();

    void begin();
    void begin(uint8_t *pinList, uint16_t totalNumPins);
    void end();
    void touchHandle();

    uint16_t getValue(uint8_t sensor_node);
};


#endif