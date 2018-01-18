/*
 * PSUART.cpp
 *
 *  Created on: Sep 15, 2017
 *      Author: jtikalsky
 */

#include "PSUART.h"
#include "IPMC.h"
#include "xscugic.h"
#include "xuartps.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#define IXR_RECV_ENABLE \
	(XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING | \
	 XUARTPS_IXR_OVER | XUARTPS_IXR_RXFULL | XUARTPS_IXR_RXOVR)

static void PS_UART_InterruptPassthrough(PS_UART *ps_uart, u32 Event, u32 EventData) {
	ps_uart->_HandleInterrupt(Event, EventData);
}

static void XUartPs_EnableInterruptMask(XUartPs *InstancePtr, u32 Mask)
{
	configASSERT(InstancePtr != NULL);

	Mask &= (u32)XUARTPS_IXR_MASK;

	/* Write the mask to the IER Register */
	XUartPs_WriteReg(InstancePtr->Config.BaseAddress,
		 XUARTPS_IER_OFFSET, Mask);
}

static void XUartPs_DisableInterruptMask(XUartPs *InstancePtr, u32 Mask)
{
	configASSERT(InstancePtr != NULL);

	Mask &= (u32)XUARTPS_IXR_MASK;

	/* Write the inverse of the Mask to the IDR register */
	XUartPs_WriteReg(InstancePtr->Config.BaseAddress,
		 XUARTPS_IDR_OFFSET, (~Mask));

}

/**
 * Instantiate a PS_UART driver.
 *
 * \note This performs hardware setup (mainly interrupt configuration).
 *
 * \param DeviceId    The DeviceId, used for XUartPs_LookupConfig(), etc
 * \param IntrId      The interrupt ID, for configuring the GIC.
 * \param ibufsize    The size of the input buffer to allocate.
 * \param obufsize    The size of the output buffer to allocate.
 * \param oblocksize  The maximum block size for output operations.  Relevant to wait times when the queue is full.
 */
PS_UART::PS_UART(u32 DeviceId, u32 IntrId, u32 ibufsize, u32 obufsize,
		u32 oblocksize) :
		error_mask(0), inbuf(RingBuffer<u8>(4096)), outbuf(
				RingBuffer<u8>(4096)), write_running(false), oblocksize(
				oblocksize), IntrId(IntrId) {

	XUartPs_Config *Config = XUartPs_LookupConfig(DeviceId);
	configASSERT(XST_SUCCESS == XUartPs_CfgInitialize(&this->UartInst, Config, Config->BaseAddress));
	XUartPs_SetInterruptMask(&this->UartInst, 0);
	XScuGic_Connect(&xInterruptController, IntrId, reinterpret_cast<Xil_InterruptHandler>(XUartPs_InterruptHandler), (void*)&this->UartInst);
	XUartPs_SetHandler(&this->UartInst, reinterpret_cast<XUartPs_Handler>(PS_UART_InterruptPassthrough), reinterpret_cast<void*>(this));
	XScuGic_Enable(&xInterruptController, IntrId);
	XUartPs_SetRecvTimeout(&this->UartInst, 8); // My example says this is u32s, the comments say its nibbles, the TRM says its baud_sample_clocks.

	u8 emptybuf;
	XUartPs_Send(&this->UartInst, &emptybuf, 0);

	u8 *dma_inbuf = NULL;
	size_t maxitems;
	this->inbuf.setup_dma_input(&dma_inbuf, &maxitems);
	XUartPs_Recv(&this->UartInst, dma_inbuf, maxitems); // Set next input

	XUartPs_EnableInterruptMask(&this->UartInst, IXR_RECV_ENABLE);
}

PS_UART::~PS_UART() {
	u8 buf;
	XUartPs_Send(&this->UartInst, &buf, 0);
	XUartPs_Recv(&this->UartInst, &buf, 0);
	XUartPs_DisableInterruptMask(&this->UartInst, IXR_RECV_ENABLE);
	XScuGic_Disconnect(&xInterruptController, this->IntrId);
	XScuGic_Disable(&xInterruptController, this->IntrId);
}

/**
 * Read from the PS_UART.
 *
 * \param buf The buffer to read into.
 * \param len The maximum number of bytes to read.
 * \param timeout The timeout for this read, in standard FreeRTOS format.
 * \param data_timeout A second timeout used to shorten `timeout` if data is present.
 *
 * \note This function is interrupt and critical safe if timeout=0.
 */
size_t PS_UART::read(u8 *buf, size_t len, TickType_t timeout, TickType_t data_timeout) {
	configASSERT(timeout == 0 || !(IN_INTERRUPT() || IN_CRITICAL()));

	AbsoluteTimeout abstimeout(timeout);
	AbsoluteTimeout abs_data_timeout(data_timeout);
	size_t bytesread = 0;
	while (bytesread < len) {
		/* We join the readwait queue now, because if we did this later we would
		 * have problems with a race condition between the read attempt and
		 * starting the wait.  We can cancel it later with a wait(timeout=0).
		 */
		WaitList::Subscription sub;
		if (!IN_INTERRUPT())
			sub = this->readwait.join();

		/* Entering a critical section to strongly pair the read and interrupt
		 * re-enable.
		 */
		if (!IN_INTERRUPT())
			taskENTER_CRITICAL();
		size_t batch_bytesread = this->inbuf.read(buf+bytesread, len-bytesread);
		if (batch_bytesread) {
			/* We have retrieved SOMETHING from the buffer.  Re-enable receive
			 * interrupts, in case they were disabled due to a full buffer.
			 */
			XUartPs_EnableInterruptMask(&this->UartInst, IXR_RECV_ENABLE);
		}
		bytesread += batch_bytesread;
		if (!IN_INTERRUPT())
			taskEXIT_CRITICAL();
		// </critical>

		if (IN_INTERRUPT())
			break; // Interrupts can't wait for more.
		if (bytesread == len)
			break;
		if (bytesread && abs_data_timeout < abstimeout)
			abstimeout = abs_data_timeout; // We have data, so if we have a data_timeout, we're now on it instead.
		if (!sub.wait(abstimeout.get_timeout()))
			break; // Timed out.
	}
	return bytesread;
}

/**
 * Write to the PS_UART.
 *
 * \param buf The buffer to write from.
 * \param len The maximum number of bytes to write.
 * \param timeout The timeout for this read, in standard FreeRTOS format.
 *
 * \note This function is interrupt and critical safe if timeout=0.
 */
size_t PS_UART::write(const u8 *buf, size_t len, TickType_t timeout) {
	configASSERT(timeout == 0 || !(IN_INTERRUPT() || IN_CRITICAL()));

	AbsoluteTimeout abstimeout(timeout);
	size_t byteswritten = 0;
	while (byteswritten < len) {
		/* We join the writewait queue now, because if we did this later we
		 * would have problems with a race condition between the write attempt
		 * and starting the wait.  We can cancel it later with a
		 * wait(timeout=0).
		 */
		WaitList::Subscription sub;
		if (!IN_INTERRUPT())
			sub = this->writewait.join();

		/* Entering a critical section to strongly pair the write and output
		 * refresh.
		 */
		if (!IN_INTERRUPT())
			taskENTER_CRITICAL();
		size_t batch_byteswritten = this->outbuf.write(buf+byteswritten, len-byteswritten);
		if (batch_byteswritten && !this->write_running) {
			/* We have written something to the buffer, and it's not already
			 * running output.
			 */
			u8 *dma_outbuf = NULL;
			size_t maxitems;
			this->outbuf.setup_dma_output(&dma_outbuf, &maxitems);
			// If we don't have any items to send, it means we didn't just write.
			configASSERT(maxitems);
			if (maxitems > this->oblocksize)
				maxitems = this->oblocksize;
			XUartPs_Send(&this->UartInst, dma_outbuf, maxitems);
			this->write_running = true;
		}
		byteswritten += batch_byteswritten;
		if (!IN_INTERRUPT())
			taskEXIT_CRITICAL();
		// </critical>

		if (IN_INTERRUPT())
			break; // Interrupts can't wait for more.
		if (byteswritten == len)
			break;
		if (!sub.wait(abstimeout.get_timeout()))
			break; // Timed out.
	}
	return byteswritten;
}

void PS_UART::_HandleInterrupt(u32 Event, u32 EventData) {
	//#define XUARTPS_EVENT_RECV_DATA			1U /**< Data receiving done */
	//#define XUARTPS_EVENT_RECV_TOUT			2U /**< A receive timeout occurred */
	//#define XUARTPS_EVENT_SENT_DATA			3U /**< Data transmission done */
	//#define XUARTPS_EVENT_RECV_ERROR		4U /**< A receive error detected */
	//#define XUARTPS_EVENT_MODEM				5U /**< Modem status changed */
	//#define XUARTPS_EVENT_PARE_FRAME_BRKE	6U /**< A receive parity, frame, break error detected */
	//#define XUARTPS_EVENT_RECV_ORERR		7U /**< A receive overrun error detected */

	if (Event == XUARTPS_EVENT_RECV_DATA || Event == XUARTPS_EVENT_RECV_TOUT
			|| Event == XUARTPS_EVENT_RECV_ERROR) {

		if (EventData > 0) {
			this->inbuf.notify_dma_input_occurred(EventData);
			this->readwait.wake(); // We received something.
			u8 *dma_inbuf;
			size_t maxitems;
			this->inbuf.setup_dma_input(&dma_inbuf, &maxitems);
			if (maxitems)
				XUartPs_Recv(&this->UartInst, dma_inbuf, maxitems); // Set next input
			else
				XUartPs_DisableInterruptMask(&this->UartInst, IXR_RECV_ENABLE); // Turn off receive until our buffer drains.
		}
	}
	// if (Event == XUARTPS_EVENT_RECV_TOUT) { } // Recv data event.  Handled above.
	if (Event == XUARTPS_EVENT_SENT_DATA) {
		/* This event indicates data transfer is COMPLETE.
		 * It is safe to enqueue more data now.
		 */

		this->outbuf.notify_dma_output_occurred(EventData);
		u8 *dma_outbuf = NULL;
		size_t maxitems;
		this->outbuf.setup_dma_output(&dma_outbuf, &maxitems);
		if (maxitems > this->oblocksize)
			maxitems = this->oblocksize;
		if (maxitems) {
			XUartPs_Send(&this->UartInst, dma_outbuf, maxitems);
			this->write_running = true;
			this->writewait.wake();
		}
		else {
			this->write_running = false;
		}
	}
	if (Event == XUARTPS_EVENT_RECV_ERROR) {
		this->error_mask |= (1<<XUARTPS_EVENT_RECV_ERROR); // Set error mask flag.  This is probably an overrun error.
		// This appears to also be a recv data event.  Handled above.
	}
	// if (Event == XUARTPS_EVENT_MODEM) { } // Not relevant to UW IPMC.
	if (Event == XUARTPS_EVENT_PARE_FRAME_BRKE) {
		this->error_mask |= (1<<XUARTPS_EVENT_PARE_FRAME_BRKE); // Not really relevant to us, but will include it for now.
	}
	if (Event == XUARTPS_EVENT_RECV_ORERR) {
		this->error_mask |= (1<<XUARTPS_EVENT_RECV_ORERR); // Set error mask flag.  The xuartps driver does not appear to send this event.
	}
}
