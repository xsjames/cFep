/**
 ******************************************************************************
 * @file      log.c
 * @brief     ��־��¼ģ��
 * @details   This file including all API functions's 
 *            implement of log.c.
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "log.h"

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
static FILE *the_log_fp = NULL;
int the_log_level = 0;
//static char the_log_buf[10240]; //10k ��ӡbuf

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
static int
on_exit(void)
{
    log_exit();
    return 0;
}

/**
 ******************************************************************************
 * @brief   ������־����
 * @param[in]  level : ��־����
 *
 * @return  None
 ******************************************************************************
 */
void
log_set_level(int level)
{
    the_log_level = level;
}

/**
 ******************************************************************************
 * @brief   ����ǰ���ڴ�����־�ļ�
 * @retval  None
 ******************************************************************************
 */
static void
log_check_file(time_t t)
{
    static int pre_day = -1;
    char fname[4+2+2+1+3+1];
    struct tm daytime;

    if ((int)((t - _timezone) / (1440 * 60)) != pre_day)
    {
        daytime = *localtime(&t);
        snprintf(fname, sizeof(fname), "%04d%02d%02d.log",
                daytime.tm_year + 1900,
                daytime.tm_mon + 1,
                daytime.tm_mday);
        (void)log_exit();
        the_log_fp = fopen(fname, "a+");
        pre_day = (int)((t - _timezone) / (1440 * 60));
    }
}

/**
 ******************************************************************************
 * @brief      ��־ģ���ʼ��
 * @param[in]  max_frame_size : ������ֽ���
 *
 * @retval     0 : ��ʼ���ɹ�
 * @retval    -1 : ��ʼ��ʧ��
 ******************************************************************************
 */
int
log_init(void)
{
    log_check_file(time(NULL));
    _onexit(on_exit);
    return 0;
}

/**
 ******************************************************************************
 * @brief      ��־ģ���˳�
 * @param[in]  None
 *
 * @retval  NONE
 ******************************************************************************
 */
void
log_exit(void)
{
    if (the_log_fp != NULL)
    {
        (void)fclose(the_log_fp);
        the_log_fp = NULL;
    }
}

/**
 ******************************************************************************
 * @brief   ��ӡʱ��
 * @return  None
 ******************************************************************************
 */
static void
log_time(void)
{
    char buf[1+4+15+1+2];
    struct tm daytime;
    time_t t = time(NULL);
    daytime = *localtime(&t);

    log_check_file(t);
    snprintf(buf, sizeof(buf), "[%04d-%02d-%02d %02d:%02d:%02d] ",
            daytime.tm_year + 1900,
            daytime.tm_mon + 1,
            daytime.tm_mday,
            daytime.tm_hour,
            daytime.tm_min,
            daytime.tm_sec);
    if (the_log_fp)
    {
        (void)fprintf(the_log_fp, buf);
    }
    (void)fprintf(stdout, buf);
}

/**
 ******************************************************************************
 * @brief   ��¼buf
 * @param[in]  level    : ��ӡ����
 * @param[in]  *pformat : ����
 * @param[in]  *pbuffer : �����׵�ַ
 * @param[in]  len      : ����
 *
 * @return  None
 *
 * @todo: �ٶ��Ż�
 ******************************************************************************
 */
void
log_buf(int level,
        const char *pformat,
        const unsigned char *pbuffer,
        int len)
{
    int i;

    if (level <= the_log_level)
    {
        if (the_log_fp)
        {
            log_time();
            (void)fprintf(the_log_fp, pformat);
            (void)fprintf(stdout, pformat);
            for (i = 0; i < len; i++)
            {
                (void)fprintf(the_log_fp, "%02X ", *(pbuffer + i));
                (void)fprintf(stdout, "%02X ", *(pbuffer + i));
            }
            (void)fprintf(the_log_fp, "\n");
            (void)fprintf(stdout, "\n");
            (void)fflush(the_log_fp);
        }
    }
}

/**
 ******************************************************************************
 * @brief   ��־print
 * @param[in]  fmt  : ��־��Ϣ��printf��ͬ
 *
 * @return  None
 ******************************************************************************
 */
void
log_print(int level,
        const char *fmt, ...)
{
    va_list args;

    if (level <= the_log_level)
    {
        va_start( args, fmt );
        log_time();
        if (the_log_fp)
        {
            (void)vfprintf(the_log_fp, fmt, args);
            (void)fflush(the_log_fp);
        }
        (void)vfprintf(stdout, fmt, args);
    }
}

/*----------------------------------log.c------------------------------------*/