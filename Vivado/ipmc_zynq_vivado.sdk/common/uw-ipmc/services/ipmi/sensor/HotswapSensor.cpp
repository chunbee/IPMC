/*
 * HotswapSensor.cpp
 *
 *  Created on: Oct 30, 2018
 *      Author: jtikalsky
 */

#include <services/ipmi/sensor/HotswapSensor.h>
#include <FreeRTOS.h>
#include <semphr.h>

HotswapSensor::HotswapSensor(const std::vector<uint8_t> &sdr_key, LogTree &log)
	: Sensor(sdr_key, log), mstate(1), previous_mstate(0),
	  last_transition_reason(TRANS_NORMAL) {
	configASSERT(this->mutex = xSemaphoreCreateMutex());
}

HotswapSensor::~HotswapSensor() {
	vSemaphoreDelete(this->mutex);
}

/**
 * Update the current M-state and send the appropriate events.
 * @param new_state The new M-state
 * @param reason The reason for the state transition
 */
void HotswapSensor::transition(uint8_t new_state, enum StateTransitionReason reason) {
	configASSERT(new_state < 8); // Check in range.
	std::vector<uint8_t> data;
	xSemaphoreTake(this->mutex, portMAX_DELAY);
	data.push_back(0xA|new_state);
	data.push_back((static_cast<uint8_t>(reason)<<4)|this->mstate);
	data.push_back(0 /* FRU Device ID */);
	this->previous_mstate = this->mstate;
	this->mstate = new_state;
	this->last_transition_reason = reason;
	xSemaphoreGive(this->mutex);
	this->send_event(Sensor::EVENT_ASSERTION, data);
}

std::vector<uint8_t> HotswapSensor::get_sensor_reading() {
	std::vector<uint8_t> out{0/*IPMI::Completion::Success*/,0,0,0};
	if (this->all_events_disabled())
		out[2] |= 0x80;
	if (this->sensor_scanning_disabled())
		out[2] |= 0x40;
	out[3] = this->mstate;
	return out;
}

void HotswapSensor::rearm() {
	/* It is unclear what previous state or reason code I am expected to send in
	 * the event of a rearm of the hotswap sensor, but I presume we should
	 * resend the last event.
	 */
	std::vector<uint8_t> data;
	xSemaphoreTake(this->mutex, portMAX_DELAY);
	data.push_back(0xA|this->mstate);
	data.push_back((static_cast<uint8_t>(this->last_transition_reason)<<4)|this->previous_mstate);
	data.push_back(0 /* FRU Device ID */);
	xSemaphoreGive(this->mutex);
	this->send_event(Sensor::EVENT_ASSERTION, data);
}