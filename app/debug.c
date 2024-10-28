/**********************************************************************
* @file debug.c
* @author qingsheng.zhang@saiterobot.com
* @version V1.0
* @date 2024年7月26日
* @brief 
* @copyright Copyright (c) 2024,
**********************************************************************/
#include "debug.h"
#include "usart.h"

#define UART_PRINT_BUFFER_SIZE      1024

#if DEBUG_PRINTF_EN

static char uart_print_buf[UART_PRINT_BUFFER_SIZE] = {0};

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 10);
    return ch;
}

// if use nano lib, rewirte _write interface, and printf need add '\n' at the end of format string
int _write(int32_t file, uint8_t *ptr, int32_t len)
{
    /* Implement your write code here, this is used by puts and printf for example */
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

int trace_printf(const char* format, ...)
{
    int ret;
    va_list ap;

    va_start (ap, format);

    // TODO: rewrite it to no longer use newlib, it is way too heavy

    //static char buf[UART_PRINT_BUFFER_SIZE];

    // Print to the local buffer
    ret = vsnprintf (uart_print_buf, sizeof(uart_print_buf), format, ap);
    va_end (ap);
    if (ret > 0)
    {
        // Transfer the buffer to the device
        ret = _write(0, (uint8_t*)uart_print_buf, (size_t)ret);
    }

    return ret;
}

#endif //#if DEBUG_PRINTF_EN
