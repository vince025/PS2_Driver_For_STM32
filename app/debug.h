/**********************************************************************
* @file debug.h
* @author qingsheng.zhang@saiterobot.com
* @version V1.0
* @brief 
* @copyright Copyright (c) 2024,
**********************************************************************/
#ifndef BSP_DEBUG_H_
#define BSP_DEBUG_H_

#include <stdio.h>
#include <stdarg.h>

#define DEBUG_PRINTF_EN             1

int _write(int32_t file, uint8_t *ptr, int32_t len);
int trace_printf(const char* format, ...);

#if DEBUG_PRINTF_EN
#define DEBUG_PRINTF(fmt, ...)      trace_printf(fmt, ##__VA_ARGS__)
#define DEBUG_LINE(fmt, ...)        trace_printf("%s[%d] # "fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define DEBUG_FUNC(fmt, ...)        trace_printf("%s # "fmt, __func__, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#define DEBUG_LINE(...)
#define DEBUG_FUNC(...)
#endif

#endif // BSP_DEBUG_H_
