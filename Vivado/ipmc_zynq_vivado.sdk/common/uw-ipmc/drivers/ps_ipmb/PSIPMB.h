/*
 * PSIPMBAB.h
 *
 *  Created on: Oct 24, 2017
 *      Author: jtikalsky
 */

#ifndef SRC_COMMON_UW_IPMC_DRIVERS_PS_IPMB_AB_PSIPMB_H_
#define SRC_COMMON_UW_IPMC_DRIVERS_PS_IPMB_AB_PSIPMB_H_

#include <functional>
#include "xiicps.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <IPMC.h>
#include <libs/RingBuffer.h>
#include <libs/ThreadingPrimitives.h>
#include <libs/StatCounter.h>
#include <drivers/generics/IPMB.h>
#include <services/ipmi/IPMI_MSG.h>
#include <services/ipmi/ipmbsvc/IPMBSvc.h>

/**
 * An interrupt-based driver for the PS I2C, specialized for IPMB functionality.
 */
class PS_IPMB : public IPMB {
public:
	PS_IPMB(u16 DeviceId, u32 IntrId, u8 SlaveAddr);
	virtual ~PS_IPMB();
	void _HandleInterrupt(u32 StatusEvent); ///< \protected Internal.

	/**
	 * This function will send a message out on the IPMB in a blocking manner.
	 *
	 * \param msg  The IPMI_MSG to deliver.
	 * \return     true if message was delivered else false
	 */
	virtual bool send_message(IPMI_MSG &msg);

	StatCounter messages_received;         ///< The number of messages received on this IPMB.
	StatCounter invalid_messages_received; ///< The number of received messages on this IPMB that are discarded as invalid.
	StatCounter incoming_messages_missed;  ///< The number of received messages on this IPMB that are discarded for lack of space or readiness.
	StatCounter unexpected_send_result_interrupts; ///< The number of unexpected send result interrupts we have received.
protected:
	static const int i2c_bufsize = 40; ///< The buffer size for I2C interactions. (Must be 1 greater than needed.)
	bool master;                       ///< Identify whether the IPMB is currently in a master or slave mode.
	XIicPs IicInst;                    ///< The I2C driver instance handle.
	u8 SlaveAddr;                      ///< The local IPMB slave address.
	u8 i2c_inbuf[i2c_bufsize];         ///< The buffer for incoming I2C data.
	u32 i2c_result;                    ///< The result of the I2C operation.
	SemaphoreHandle_t mutex;           ///< A mutex serializing IPMB message requests.
	u32 IntrId;                        ///< Interrupt ID, used to disable interrupts in the destructor.
	QueueHandle_t sendresult_q;        ///< A queue to transfer the Send result from ISR land back to the send() function.

	void setup_slave();
	void setup_master();
};

#endif /* SRC_COMMON_UW_IPMC_DRIVERS_PS_IPMB_AB_PSIPMB_H_ */