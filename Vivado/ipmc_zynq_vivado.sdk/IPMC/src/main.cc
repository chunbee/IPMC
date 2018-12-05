#include <drivers/network/Network.h>
#include <stdio.h>
#include "limits.h"
#include "string.h"

#include <FreeRTOS.h>
#include <task.h>
#include <IPMC.h>
#include <drivers/ps_uart/PSUART.h>

#include "xparameters.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"

#include <libs/BackTrace.h>
#include <libs/printf.h>
#include <drivers/tracebuffer/TraceBuffer.h>

extern "C" {
void __real_print( const char8 *ptr);
}

// Replace week abort with one that actually does something.
// In case abort gets called there will be a stack trace showing on the console.
// __real_printf is used so that writes to the UART driver are blocking.
void abort() {
	BackTrace *extrace = BackTrace::traceException();

	TaskHandle_t handler = xTaskGetCurrentTaskHandle();
	std::string tskname = "unknown_task";
	if (handler)
		tskname = pcTaskGetName(handler);

	std::string output;
	if (extrace) {
		output += "\n-- ABORT DUE TO EXCEPTION --\n";
		output += extrace->toString();
	} else {
		BackTrace trace;
		trace.trace();
		output += "\n-- ABORT CALLED --\n";
		output += trace.toString();
	}

	output += "-- ASSERTING --\n";

	/* Put it through the trace facility, so regardless of our ability to
	 * put it through the standard log paths, it gets trace logged.
	 */
	std::string log_facility = stdsprintf("ipmc.unhandled_exception.%s", tskname);
	TRACE.log(log_facility.c_str(), log_facility.size(), LogTree::LOG_CRITICAL, output.c_str(), output.size());

	// Put it directly to the UART console, for the same reason.
	std::string wnl_output = output;
	windows_newline(wnl_output);
	__real_print(wnl_output.c_str());

	// Put it through the standard log system.
	LOG[tskname].log(output, LogTree::LOG_CRITICAL);

	configASSERT(0);

	/* This function is attribute noreturn, configASSERT(0) can technically
	 * return within a debugger.  This literally can't, it has no epilogue.
	 *
	 * Ensure it doesn't.
	 */
	while (1);
}

extern "C" {
#include <lwip/opt.h>
}

extern "C" {
/* The private watchdog is used as the timer that generates run time stats.  */
XScuWdt xWatchDogInstance;

/*-----------------------------------------------------------*/

void vAssertCalled(const char * pcFile, unsigned long ulLine) {
	volatile unsigned long ul = 0;

	(void) pcFile;
	(void) ulLine;

	taskENTER_CRITICAL()
	;
	{
		/* Set ul to a non-zero value using the debugger to step out of this
		 function. */
		while (ul == 0) {
			/* Let's trigger the debugger directly, if attached.
			 *
			 * No sense asserting and not realizing it.
			 *
			 * Add 4 to the PC to step over this instruction and continue.
			 */
			__asm__ volatile ("bkpt");

			portNOP();
		}
	}
	taskEXIT_CRITICAL()
	;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
	/* If the buffers to be provided to the Idle task are declared inside this
	 function then they must be declared static - otherwise they will be allocated on
	 the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	 state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	 Note that, as the array is necessarily of type StackType_t,
	 configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 application must provide an implementation of vApplicationGetTimerTaskMemory()
 to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
	/* If the buffers to be provided to the Timer task are declared inside this
	 function then they must be declared static - otherwise they will be allocated on
	 the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	 task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	 Note that, as the array is necessarily of type StackType_t,
	 configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationMallocFailedHook( void )
{
	void __real_xil_printf( const char8 *ctrl1, ...);
	__real_xil_printf("HALT: vApplicationMallocFailedHook() called\n");
	abort();
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	/* Attempt to prevent the handle and name of the task that overflowed its stack
	from being optimized away because they are not used. */
	volatile TaskHandle_t xOverflowingTaskHandle = xTask;
	volatile char *pcOverflowingTaskName = pcTaskName;

	( void ) xOverflowingTaskHandle;
	( void ) pcOverflowingTaskName;

	void __real_xil_printf( const char8 *ctrl1, ...);
	__real_xil_printf( "HALT: Task %s overflowed its stack.", pcOverflowingTaskName );
	abort();
}

/*-----------------------------------------------------------*/

void vInitialiseTimerForRunTimeStats(void) {
	XScuWdt_Config *pxWatchDogInstance;
	uint32_t ulValue;
	const uint32_t ulMaxDivisor = 0xff, ulDivisorShift = 0x08;

	pxWatchDogInstance = XScuWdt_LookupConfig( XPAR_SCUWDT_0_DEVICE_ID);
	XScuWdt_CfgInitialize(&xWatchDogInstance, pxWatchDogInstance, pxWatchDogInstance->BaseAddr);

	ulValue = XScuWdt_GetControlReg(&xWatchDogInstance);
	ulValue |= ulMaxDivisor << ulDivisorShift;
	XScuWdt_SetControlReg(&xWatchDogInstance, ulValue);

	XScuWdt_LoadWdt(&xWatchDogInstance, UINT_MAX);
	XScuWdt_SetTimerMode(&xWatchDogInstance);
	XScuWdt_Start(&xWatchDogInstance);
}

/*-----------------------------------------------------------*/

static void prvSetupHardware(void) {
	BaseType_t xStatus;
	XScuGic_Config *pxGICConfig;

	/* Ensure no interrupts execute while the scheduler is in an inconsistent
	 state.  Interrupts are automatically enabled when the scheduler is
	 started. */
	portDISABLE_INTERRUPTS();

	/* Obtain the configuration of the GIC. */
	pxGICConfig = XScuGic_LookupConfig( XPAR_SCUGIC_SINGLE_DEVICE_ID);

	/* Sanity check the FreeRTOSConfig.h settings are correct for the
	 hardware. */
	configASSERT(pxGICConfig);
	configASSERT(pxGICConfig->CpuBaseAddress == ( configINTERRUPT_CONTROLLER_BASE_ADDRESS + configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET ));
	configASSERT(pxGICConfig->DistBaseAddress == configINTERRUPT_CONTROLLER_BASE_ADDRESS);

	/* Install a default handler for each GIC interrupt. */
	xStatus = XScuGic_CfgInitialize(&xInterruptController, pxGICConfig, pxGICConfig->CpuBaseAddress);
	configASSERT(xStatus == XST_SUCCESS);
	(void) xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	vPortInstallFreeRTOSVectorTable();
}
} // extern "C"

int main() {
	/*
	 * If you want to run this application outside of SDK,
	 * uncomment one of the following two lines and also #include "ps7_init.h"
	 * or #include "ps7_init.h" at the top, depending on the target.
	 * Make sure that the ps7/psu_init.c and ps7/psu_init.h files are included
	 * along with this example source files for compilation.
	 */
	/* ps7_init();*/
	/* psu_init();*/

	/* See http://www.freertos.org/RTOS-Xilinx-Zynq.html. */
	prvSetupHardware();

	//std::terminate();

	init_complete = xEventGroupCreate();

	UWTaskCreate("init", TASK_PRIORITY_WATCHDOG, []() -> void {
		driver_init(true);
		xEventGroupSetBits(init_complete, 0x01);
		ipmc_service_init();
		xEventGroupSetBits(init_complete, 0x02);
		LOG.log(std::string("\n") + generate_banner(), LogTree::LOG_NOTICE); // This is the ONLY place that should EVER log directly to LOG rather than a subtree.
	});

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	 line will never be reached.  If the following line does execute, then
	 there was either insufficient FreeRTOS heap memory available for the idle
	 and/or timer tasks to be created, or vTaskStartScheduler() was called from
	 User mode.  See the memory management section on the FreeRTOS web site for
	 more details on the FreeRTOS heap http://www.freertos.org/a00111.html.  The
	 mode from which main() is called is set in the C start up code and must be
	 a privileged mode (not user mode). */
	for (;;)
		;

	return 0;
}
