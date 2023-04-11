##  基于ARM芯片和WiFi通信的病房监控系统

一个非常稚嫩的本科课外电子竞赛项目。

所需硬件基本在文档的竞赛报告中有所体现。

程序在现在看来有非常多的槽点和可以改进之处（比如上下位机WiFi串口通信处理方式有很大不足，数据格式解析和传输方式设计有明显不足）。

### 1. 作品展示

[项目视频](https://www.bilibili.com/video/BV1c3411N73C/?spm_id_from=333.999.0.0&vd_source=0910882e7f985463ad1c0d1501f8bcfb)

### 2. 作品简介

共有两个项目版本，version 1.0 是参与重庆市大学生单片机设计应用大赛（合泰杯）的版本，此时项目初具雏形；version2.0 是参与全国大学生智能互联创新设计大赛等竞赛时的版本，项目略显成熟（相对1.0）。

#### 1. 以合泰芯片HT32F1656为中心，该项目的version 1.0版本（HT32F1656 version1.0）

> 本作品以HT32F1656为主控模块,设计一款可以实时、准确监控病人身体状况、病房环境信息的病房监控系统。该系统由人体监控部分、病房环境监控部分、蜂鸣器报警模块、语音提醒模块、无线通信模块等核心部分构成。利用WIFI通信模块，医护人员在自己的办公场所即可在上位机屏幕实时了解病房与病人的各项状况，其中可检测的数据含：光照度、温湿度、心率血氧饱和度；语音报警模块可以随时提醒病人即时穿戴设备；在病人身体数据不正常的时候，蜂鸣器报警模块可以发声报警。实作作品基本实现了作品构想。

#### 2. 以意法半导体STM32为中心，该项目的version 2.0版本（STM32 version2.0）

> 本作品以STM32F407+STM32F103为主控模块,设计一款可以实时、准确监控病人身体状况、病房环境信息的病房监控系统。利用Wifi通信，医护人员在自己的办公场所即可远程在控制终端屏幕上实时了解病房与各个病人的各项状况，其中可检测的数据含：光照度、温湿度、体温、心率血氧饱和度。
>
> 系统总体实现的功能有：
>
> 1.病患人员监测设备A/B共有功能：
>
> - 利用ESP8266-Wifi模块进行监测设备与控制终端的参数信息与控制信息的数据互传
> - 病患如果遭遇紧急情况，可按动病床旁的自锁按钮，对医护人员进行紧急警报呼救
>
> 2.病患人员监测设备A独占功能：
>
> - 利用MAX44009传感器采集病房的光强度参数信息
> - 通过实时光强度信息，自动操控病房内灯光亮灭，共可实现四级亮度调节，给予病患绝佳的环境光源舒适度
> - 利用SHT30传感器采集病房的温度、湿度信息
>
> 3.病患人员监测设备B独占功能：
>
> - 利用STM32控制双路继电器，接收来自医护人员控制终端的控制信号，从而达到对病房的环境控制：
>   a)天气炎热时，可开启风扇降温
>   b)病房环境干燥时，可开启加湿器调节室内湿度
> - 利用STM32控制ISD1820语音录放电路，接收来自医护人员控制终端的控制信号，从而语音提醒未穿戴设备的病患进行设备穿戴
>
> 4.医护人员控制终端设备功能：
>
> - 通过彩色液晶屏显示病房和各个病患的各项参数信息及设备开关情况
> - 内置蜂鸣器，接收到病患的呼救请求时，会进行持续警报，液晶显示屏病人显示位以醒目红色呈现
> - 矩阵键盘功能：
>   a)第一排按钮，首次按下开启病房加湿器，再次按下关闭加湿器，依次往复
>   b)第二排按钮，首次按下开启病房风扇，再次按下关闭风扇，依次往复
>   c)第三排按钮，每次按下，病患人员监测设备会进行一次提醒病患穿戴监测设备的语音播报提醒
>   d)第四排按钮， 首次按下，关闭病房灯光自动调节同时关闭所有病房的灯光，再次按下开启病房灯光自动调节，依次往复
> - 配备RTC时钟，显示屏实时显示当前日期、时间、星期

### 3. 目录

------document ：存放竞赛报告、项目（演示）说明书以及合泰芯片数据手册

   |

-----project ：软件程序工程

​            |

​             -------HT32F1656 version1.0：以合泰芯片HT32F1656为中心的早期版本

​             -------STM32 version2.0：以意法半导体STM32为中心的版本