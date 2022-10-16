# rt-nv7lidar
## 1、介绍

天津瞭望公司的LRF-NV7激光测距模块RT-Thread软件包

### 1.1 目录结构

| 名称 | 说明 |
| ---- | ---- |
| doc  | 文档目录 |
| inc  | 头文件目录 |
| src  | 源代码目录 |

### 1.2 许可证

rt-nv7lidar 遵循 LGPLv2.1 许可，详见 `LICENSE` 文件。

### 1.3 依赖

- RT-Thread 4.0+

## 2、如何使用 rt-nv7lidar

> 说明：描述该 package 位于 menuconfig 的位置，并对与其相关的配置进行介绍

使用 hello package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers --->
        [*] LRF-NV7 laser ranging module
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3、注意事项

无。

## 5、联系方式 & 感谢

* 维护：shenzan
* 主页：https://github.com/tzxdcz/rt-nv7lidar
* 联系: https://github.com/tzxdcz/rt-nv7lidar/issues
