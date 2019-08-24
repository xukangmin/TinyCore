
#include "TinyTouch.h"
#include "Arduino.h"

volatile uint8_t TinyTouch::touch_postprocess_request = 0;
volatile uint8_t TinyTouch::time_to_measure_touch_flag = 0;

uint8_t TinyTouch::interrupt_cnt = 0;


ISR(ADC0_RESRDY_vect)
{
	qtm_t161x_ptc_handler_eoc();
}

ISR(RTC_CNT_vect)
{

	/* Insert your RTC Compare interrupt handling code */
	TinyTouch::touch_timer_handler();

	/* Compare interrupt flag has to be cleared manually */
	RTC.INTFLAGS = RTC_CMP_bm;
}

static void TinyTouch::touch_timer_handler(void)
{
	interrupt_cnt++;
	if (interrupt_cnt >= DEF_TOUCH_MEASUREMENT_PERIOD_MS) {
		interrupt_cnt = 0;
		/* Count complete - Measure touch sensors */
		time_to_measure_touch_flag = 1u;
		qtm_update_qtlib_timer(DEF_TOUCH_MEASUREMENT_PERIOD_MS);
	}
}

TinyTouch::TinyTouch()
{
  	while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */
	}

	// RTC.CMP = 0x0; /* Compare: 0x0 */

	// RTC.CNT = 0x0; /* Counter: 0x0 */

	RTC.CTRLA = RTC_PRESCALER_DIV1_gc   /* 1 */
	            | 1 << RTC_RTCEN_bp     /* Enable: enabled */
	            | 1 << RTC_RUNSTDBY_bp; /* Run In Standby: enabled */

	// RTC.PER = 0xffff; /* Period: 0xffff */

	// RTC.CLKSEL = RTC_CLKSEL_INT32K_gc; /* 32KHz Internal Ultra Low Power Oscillator (OSCULP32K) */

	// RTC.DBGCTRL = 0 << RTC_DBGRUN_bp; /* Run in debug: disabled */

	RTC.INTCTRL = 1 << RTC_CMP_bp    /* Compare Match Interrupt enable: enabled */
	              | 0 << RTC_OVF_bp; /* Overflow Interrupt enable: disabled */


}

TinyTouch::~TinyTouch()
{
}

void TinyTouch::begin() // default initializer 
{
  touch_init();
}

void TinyTouch::begin(uint8_t *pinList, uint16_t totalNumPins) {
	
	uint16_t    sensor_nodes;
	touch_ret_t touch_ret = TOUCH_SUCCESS;
	PORT_t* port;
	uint8_t bit_pos;
	volatile uint8_t* pin_ctrl_reg;
	// to do, init timer and pins
	timer_set_period(32);

	for(sensor_nodes = 0; sensor_nodes < totalNumPins; sensor_nodes++) {
		port = digitalPinToPortStruct(pinList[sensor_nodes]);
		bit_pos = digitalPinToBitPosition(pinList[sensor_nodes]);
		/* Calculate where pin control register is */
		pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

		*pin_ctrl_reg &= ~PORT_PULLUPEN_bm;
		*pin_ctrl_reg = (*pin_ctrl_reg & ~PORT_ISC_gm) | PORT_ISC_INPUT_DISABLE_gc;

	}
	
	time_to_measure_touch_flag = 0;

	touch_postprocess_request = 0;
	
	measurement_done_touch = 0;

	module_error_code = 0;



	ptc_qtlib_acq_gen1 = {totalNumPins, DEF_SENSOR_TYPE, DEF_PTC_CAL_AUTO_TUNE, DEF_SEL_FREQ_INIT};

	ptc_qtlib_node_stat1 = new qtm_acq_node_data_t[totalNumPins];

	ptc_seq_node_cfg1 = new qtm_acq_t161x_node_config_t[totalNumPins];

	touch_acq_signals_raw = new uint16_t[totalNumPins];

	for(sensor_nodes = 0; sensor_nodes < totalNumPins; sensor_nodes++) {
		ptc_seq_node_cfg1[sensor_nodes].node_xmask = X_NONE;
		ptc_seq_node_cfg1[sensor_nodes].node_ymask = Y(digitalPinToTouchPin(pinList[sensor_nodes]));
		ptc_seq_node_cfg1[sensor_nodes].node_csd = 0;
		ptc_seq_node_cfg1[sensor_nodes].node_rsel_prsc = PRSC_DIV_SEL_4;
		ptc_seq_node_cfg1[sensor_nodes].node_gain = NODE_GAIN(GAIN_1, GAIN_1);
		ptc_seq_node_cfg1[sensor_nodes].node_oversampling = FILTER_LEVEL_16;
	}


	qtlib_acq_set1 = {&ptc_qtlib_acq_gen1, ptc_seq_node_cfg1, ptc_qtlib_node_stat1};

	/* Init acquisition module */
	qtm_ptc_init_acquisition_module(&qtlib_acq_set1);

	
	/* Init pointers to DMA sequence memory */
	qtm_ptc_qtlib_assign_signal_memory(touch_acq_signals_raw);

	/* Initialize sensor nodes */

	for (sensor_nodes = 0u; sensor_nodes < totalNumPins; sensor_nodes++)
	{
		/* Enable each node for measurement and mark for calibration */
		qtm_enable_sensor_node(&qtlib_acq_set1, sensor_nodes);
		qtm_calibrate_sensor_node(&qtlib_acq_set1, sensor_nodes);
	}

	qtlib_key_grp_config_set1 = {totalNumPins,
								DEF_TOUCH_DET_INT,
								DEF_MAX_ON_DURATION,
								DEF_ANTI_TCH_DET_INT,
								DEF_ANTI_TCH_RECAL_THRSHLD,
								DEF_TCH_DRIFT_RATE,
								DEF_ANTI_TCH_DRIFT_RATE,
								DEF_DRIFT_HOLD_TIME,
								DEF_REBURST_MODE};
						
	qtlib_key_data_set1 = new qtm_touch_key_data_t[totalNumPins];

	qtlib_key_configs_set1 = new qtm_touch_key_config_t[totalNumPins];

	for (sensor_nodes = 0u; sensor_nodes < totalNumPins; sensor_nodes++)
	{
		qtlib_key_configs_set1[sensor_nodes].channel_threshold = 20;
		qtlib_key_configs_set1[sensor_nodes].channel_hysteresis = HYST_25;
		qtlib_key_configs_set1[sensor_nodes].channel_aks_group = NO_AKS_GROUP;
	}

	qtlib_key_set1
    = {&qtlib_key_grp_data_set1, &qtlib_key_grp_config_set1, qtlib_key_data_set1, qtlib_key_configs_set1};

		/* Enable sensor keys and assign nodes */
	for (sensor_nodes = 0u; sensor_nodes < totalNumPins; sensor_nodes++) {
		qtm_init_sensor_key(&qtlib_key_set1, sensor_nodes, &ptc_qtlib_node_stat1[sensor_nodes]);
	}


}

void TinyTouch::timer_set_period(const uint8_t val)
{
	while (RTC.STATUS & RTC_PERBUSY_bm) /* wait for RTC synchronization */
		;
	RTC.PER = val;
}

static void TinyTouch::qtm_measure_complete_callback(void)
{
	touch_postprocess_request = 1u;
}

void TinyTouch::qtm_error_callback(uint8_t error)
{
	module_error_code = error + 1u;

}

void TinyTouch::touchHandle() {
	touch_ret_t touch_ret;

	/* check the time_to_measure_touch_flag flag for Touch Acquisition */
	if (time_to_measure_touch_flag == 1u) {
		/* Do the acquisition */
		
		touch_ret = qtm_ptc_start_measurement_seq(&qtlib_acq_set1, TinyTouch::qtm_measure_complete_callback);

		/* if the Acquistion request was successful then clear the request flag */
		if (TOUCH_SUCCESS == touch_ret) {
			/* Clear the Measure request flag */
			time_to_measure_touch_flag = 0u;
		}
	}

	/* check the flag for node level post processing */
	if (touch_postprocess_request == 1u) {
		/* Reset the flags for node_level_post_processing */
		touch_postprocess_request = 0u;

		/* Run Acquisition module level post processing*/
		touch_ret = qtm_acquisition_process();

		/* Check the return value */
		if (TOUCH_SUCCESS == touch_ret) {
			/* Returned with success: Start module level post processing */
			touch_ret = qtm_key_sensors_process(&qtlib_key_set1);
			if (TOUCH_SUCCESS != touch_ret) {
				qtm_error_callback(1);
			}
		} else {
			/* Acq module Eror Detected: Issue an Acq module common error code 0x80 */
			qtm_error_callback(0);
		}

		if ((0u != (qtlib_key_set1.qtm_touch_key_group_data->qtm_keys_status & 0x80u))) {
			time_to_measure_touch_flag = 1u;
		} else {
			measurement_done_touch = 1u;
		}

	}
}

void TinyTouch::end() {
	// clean up
	delete ptc_qtlib_node_stat1;
	delete ptc_seq_node_cfg1;
	delete touch_acq_signals_raw;

	delete qtlib_key_data_set1;

	delete qtlib_key_configs_set1;
}

uint16_t TinyTouch::getValue(uint8_t sensor_node) {
  return (ptc_qtlib_node_stat1[sensor_node].node_acq_signals);
}
