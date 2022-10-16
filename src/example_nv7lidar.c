/**
 * @file example_nv7lidar.c
 * @author shenzan (shenzan@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <rtthread.h>
#define DBG_TAG "example_nv7lidar"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <nv7lidar.h>

#ifdef PKG_USING_NV7LIDAR_SAMPLE
static void test_nv7lidar(rt_uint8_t argc, char **argv)
{
    rt_err_t res;
    rt_uint8_t count = 5;
    rt_device_t lidar = nv7_lidar_create("nv7lidar");
    if(lidar == RT_NULL)
    {
        LOG_D("failed to find device nv7lidar");
        return;
    }
    res = nv7_lidar_init(lidar);
    if(res != RT_EOK)
    {
        LOG_D("failed to init lidar device");
        return;
    }
    while(nv7_lidar_open_indicate_laser(lidar, 1000) != RT_EOK);//打开指示激光
    while(count--)
    {
        rt_uint16_t res_t = 0;
        if(nv7_lidar_get_single_measure(lidar, &res_t, 1000) == RT_ERROR)
        {
            LOG_D("no distance");
            rt_thread_mdelay(1000);
        }
    }
    while(nv7_lidar_close_indicate_laser(lidar, 1000) != RT_EOK);//关闭指示激光
}
MSH_CMD_EXPORT(test_nv7lidar,test nv7lidar);
#endif
