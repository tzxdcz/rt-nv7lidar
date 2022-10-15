/**
 * @file nv7lidar.c
 * @author shenzan (shenzan@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <rtthread.h>
#include <rtdevice.h>
#define DBG_TAG "nv7lidar"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <nv7lidar.h>
#include <nv7lidar_protocol.h>

rt_device_t nv7_lidar_create(const char* lidar_name)
{
    RT_ASSERT(lidar_name != RT_NULL);

    rt_device_t lidar = rt_device_find(lidar_name);
    if (!lidar)
    {
        LOG_E("Find %s failed!", lidar_name);
        return RT_NULL;
    }
    return lidar;
}

rt_err_t nv7_lidar_init(rt_device_t lidar)
{
    RT_ASSERT(lidar != RT_NULL);

    rt_err_t ret = rt_device_init(lidar);
    if (ret != RT_EOK)
    {
        LOG_E("Initialize device failed!");
        return -RT_ERROR;
    }

    return rt_device_open(lidar, RT_NULL);
}

//rt_err_t nv7_lidar_recev_data(rt_device_t lidar, rt_uint8_t* buffer, rt_size_t len, rt_uint32_t timeout)
//{
//    RT_ASSERT(lidar != RT_NULL);
//
//    int  recvPos = 0;
//    rt_uint32_t startTs = rt_tick_get();
//    rt_uint32_t waitTime;
//
//    LOG_I("%d bytes to receive", len);
//    while ((waitTime = rt_tick_get() - startTs) <= rt_tick_from_millisecond(timeout))
//    {
//        rt_uint8_t ch;
//        rt_device_read(lidar, 0, &ch, 1);
//        buffer[recvPos] = ch;
//        // LOG_I("Received %02X", buffer[recvPos]);
//        recvPos++;
//        if (recvPos == len)
//        {
//            LOG_D("Received content");
//            return RT_EOK;
//        }
//    }
//    return RT_ETIMEOUT;
//}

static rt_err_t nv7_lidar_wait_resp_header(rt_device_t lidar, rt_uint8_t * header, rt_uint32_t timeout)
{
    RT_ASSERT(lidar != RT_NULL);

    int  recvPos = 0;
    rt_uint8_t  recvBuffer[27];

    rt_uint32_t startTs = rt_tick_get();
    rt_uint32_t waitTime;

    while ((waitTime = rt_tick_get() - startTs) <= rt_tick_from_millisecond(timeout))
    {
        rt_size_t remainSize = 27 - recvPos;
        for(rt_size_t i = 0; i < remainSize; i++)
        {
            rt_uint8_t ch;
            if(rt_device_read(lidar, 0, &ch, 1) != 1)
            {
                return RT_ETIMEOUT;
            };
            recvBuffer[recvPos] = ch;
            // LOG_I("Received %02X", recvBuffer[recvPos]);

            switch (recvPos)
            {
                case 0:
                    if (recvBuffer[recvPos] != NV7LIDAR_ANS_SYNC_BYTE1)
                    {
                        continue;
                    }
                    break;
                case 1:
                    if (recvBuffer[recvPos] != NV7LIDAR_ANS_SYNC_BYTE2)
                    {
                        recvPos = 0;
                        continue;
                    }
                    break;
                case 2:
                    remainSize = recvBuffer[recvPos] + 4;
                    break;
                default:
                    break;
            }
            recvPos++;
            if (recvPos == remainSize)
            {
//                LOG_D("%d bytes to receive", remainSize);
                rt_memcpy(header, recvBuffer, remainSize);
                return RT_EOK;
            }
        }
    }

    return RT_ETIMEOUT;
}
/**
 * 单次测量
 * @param lidar
 * @param result_t 测量距离结果
 * @param timeout
 * @return RT_EOK 获得结果
 */
rt_err_t nv7_lidar_get_single_measure(rt_device_t lidar, rt_uint16_t* result_t, rt_uint32_t timeout)
{
    RT_ASSERT(lidar != RT_NULL);
    RT_ASSERT(result_t != RT_NULL);
    rt_err_t res;
    rt_uint8_t resp_buff[27] = {0};
    rt_uint8_t single_cmd[8] = {0xAE,0xA7,0x04,0x00,0x05,0x09,0xBC,0xBE};
    rt_device_write(lidar, 0, (void*)single_cmd, 8);
    res = nv7_lidar_wait_resp_header(lidar, resp_buff, timeout);
    if(res != RT_EOK)
    {
        LOG_D("read timeout");
        return res;
    }
    else {
        if(resp_buff[2] == 0x17 && resp_buff[4] == 0x85)
        {
            *result_t = ((resp_buff[7] << 8) + resp_buff[8])*10;
            LOG_D("single measure result = %d cm",*result_t);
            return res;
        }
        else {
            return RT_ERROR;
        }
    }
}

rt_err_t nv7_lidar_open_continue_measure(rt_device_t lidar, rt_uint32_t timeout)
{
    RT_ASSERT(lidar != RT_NULL);
    rt_err_t res;
    rt_uint8_t resp_buff[27] = {0};
    rt_uint8_t single_cmd[8] = {0xAE,0xA7,0x04,0x00,0x0E,0x12,0xBC,0xBE};
    rt_device_write(lidar, 0, (void*)single_cmd, 8);
    res = nv7_lidar_wait_resp_header(lidar, resp_buff, timeout);
    if(res != RT_EOK)
    {
        LOG_D("read timeout");
        return res;
    }
    else {
        if(resp_buff[2] == 0x04 && resp_buff[4] == 0x8E)
        {
            LOG_D("continue measure is open");
            return res;
        }
        else {
            return RT_ERROR;
        }
    }
}

rt_err_t nv7_lidar_close_continue_measure(rt_device_t lidar, rt_uint32_t timeout)
{
    RT_ASSERT(lidar != RT_NULL);
    rt_err_t res;
    rt_uint8_t resp_buff[27] = {0};
    rt_uint8_t single_cmd[8] = {0xAE,0xA7,0x04,0x00,0x0F,0x13,0xBC,0xBE};
    rt_device_write(lidar, 0, (void*)single_cmd, 8);
    res = nv7_lidar_wait_resp_header(lidar, resp_buff, timeout);
    if(res != RT_EOK)
    {
        LOG_D("read timeout");
        return res;
    }
    else {
        if(resp_buff[2] == 0x04 && resp_buff[4] == 0x8F)
        {
            LOG_D("continue measure is close");
            return res;
        }
        else {
            return RT_ERROR;
        }
    }
}

rt_err_t nv7_lidar_continue_measure_get(rt_device_t lidar, rt_uint16_t* result_t, rt_uint32_t timeout)
{
    RT_ASSERT(lidar != RT_NULL);
    RT_ASSERT(result_t != RT_NULL);
    rt_err_t res;
    rt_uint8_t resp_buff[27] = {0};
    res = nv7_lidar_wait_resp_header(lidar, resp_buff, timeout);
    if(res != RT_EOK)
    {
        LOG_D("read timeout");
        return res;
    }
    else {
        if(resp_buff[2] == 0x17 && resp_buff[4] == 0x85)
        {
            *result_t = ((resp_buff[7] << 8) + resp_buff[8])*10;
            LOG_D("continue measure result = %d cm",*result_t);
            return res;
        }
        else {
            return RT_ERROR;
        }
    }
}

rt_err_t nv7_lidar_open_indicate_laser(rt_device_t lidar, rt_uint32_t timeout)
{
    RT_ASSERT(lidar != RT_NULL);
    rt_err_t res;
    rt_uint8_t resp_buff[27] = {0};
    rt_uint8_t single_cmd[9] = {0xAE,0xA7,0x05,0x00,0x40,0x01,0x46,0xBC,0xBE};
    rt_device_write(lidar, 0, (void*)single_cmd, 9);
    res = nv7_lidar_wait_resp_header(lidar, resp_buff, timeout);
    if(res != RT_EOK)
    {
        LOG_D("read timeout");
        return res;
    }
    else {
        if(resp_buff[2] == 0x04 && resp_buff[4] == 0xC0)
        {
            LOG_D("indicate laser is open");
            return res;
        }
        else {
            return RT_ERROR;
        }
    }
}

rt_err_t nv7_lidar_close_indicate_laser(rt_device_t lidar, rt_uint32_t timeout)
{
    RT_ASSERT(lidar != RT_NULL);
    rt_err_t res;
    rt_uint8_t resp_buff[27] = {0};
    rt_uint8_t single_cmd[9] = {0xAE,0xA7,0x05,0x00,0x40,0x00,0x45,0xBC,0xBE};
    rt_device_write(lidar, 0, (void*)single_cmd, 9);
    res = nv7_lidar_wait_resp_header(lidar, resp_buff, timeout);
    if(res != RT_EOK)
    {
        LOG_D("read timeout");
        return res;
    }
    else {
        if(resp_buff[2] == 0x04 && resp_buff[4] == 0xC0)
        {
            LOG_D("indicate laser is close");
            return res;
        }
        else {
            return RT_ERROR;
        }
    }
}

