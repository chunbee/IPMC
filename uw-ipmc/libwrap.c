/*
 * libwrap.c
 *
 *  Created on: Sep 14, 2017
 *      Author: jtikalsky
 */

#define SRC_LIBWRAP_C_
#include <libwrap.h> // auto-included from IPMC.h, but include first here to ensure there are no failed hidden dependencies in libwrap.h
#include <stdio.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <IPMC.h>

static SemaphoreHandle_t libwrap_mutex = NULL;
UWIPMC_INITIALIZE_STATIC_SEMAPHORE(libwrap_mutex, Mutex);

int _uwipmc_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    xSemaphoreTake(libwrap_mutex, portMAX_DELAY);
    int ret = vprintf(format, args);
    xSemaphoreGive(libwrap_mutex);
    va_end(args);
    return ret;
}

int _uwipmc_sprintf(char *str, const char *format, ...) {
	va_list args;
	va_start(args, format);
	xSemaphoreTake(libwrap_mutex, portMAX_DELAY);
	int ret = vsprintf(str, format, args);
	xSemaphoreGive(libwrap_mutex);
	va_end(args);
	return ret;
}

int _uwipmc_snprintf(char *str, size_t size, const char *format, ...) {
	va_list args;
	va_start(args, format);
	xSemaphoreTake(libwrap_mutex, portMAX_DELAY);
	int ret = vsnprintf(str, size, format, args);
	xSemaphoreGive(libwrap_mutex);
	va_end(args);
	return ret;
}

int _uwipmc_vprintf(const char *format, va_list ap) {
	xSemaphoreTake(libwrap_mutex, portMAX_DELAY);
	int ret = vprintf(format, ap);
	xSemaphoreGive(libwrap_mutex);
	return ret;
}

int _uwipmc_vsprintf(char *str, const char *format, va_list ap) {
	xSemaphoreTake(libwrap_mutex, portMAX_DELAY);
	int ret = vsprintf(str, format, ap);
	xSemaphoreGive(libwrap_mutex);
	return ret;
}

int _uwipmc_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
	xSemaphoreTake(libwrap_mutex, portMAX_DELAY);
	int ret = vsnprintf(str, size, format, ap);
	xSemaphoreGive(libwrap_mutex);
	return ret;
}


