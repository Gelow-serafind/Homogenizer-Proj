# HK32F0301MxxxxC UART Printf + PWM 示例项目

## 项目概述

这是一个基于 HK32F0301MxxxxC 微控制器的综合示例项目，结合了 UART printf 功能和 TIM1 PWM 输出功能。项目严格遵循 HK32 SDK V3.0.0 的示例代码规范。

## 功能特性

### 1. UART Printf 功能
- UART1 串口通信
- 波特率: 115200 bps
- 数据格式: 8位数据位，1位停止位，无校验
- printf 函数重定向到串口
- 通过 PA3 (TX) 和 PD6 (RX) 引脚

### 2. TIM1 PWM 功能
- PWM 频率: 17.57kHz
- PWM 模式: PWM2 模式
- 输出引脚: PD1 (TIM1_CH1N)
- 初始占空比: 50%
- 动态占空比控制: 10% ~ 90% 渐变

### 3. 系统功能
- 系统时钟: 48MHz HSI
- SysTick 定时器: 1ms 中断
- 延时函数支持

## 硬件连接

### 评估板引脚配置
| 功能 | 引脚 | 说明 |
|------|------|------|
| UART1 TX | PA3 | 串口发送 |
| UART1 RX | PD6 | 串口接收 |
| TIM1 PWM | PD1 | PWM 输出 |
| 调试接口 | PA13/PA14 | SWD 调试 |

### 外部连接要求
1. **串口调试**：连接 PA3 (TX) 到 USB-TTL 转换器的 RX，PD6 (RX) 到 USB-TTL 转换器的 TX
2. **PWM 输出**：连接 PD1 到外部设备（如电机驱动器、LED 等）
3. **电源**：3.3V 供电

## 软件架构

### 文件结构
```
UART_example/
├── inc/
│   ├── main.h              # 主程序头文件
│   ├── hk32f0301mxxc_it.h  # 中断头文件
│   └── hk32f0301mxxc_conf.h # 配置文件
├── src/
│   ├── main.c              # 主程序（UART + PWM）
│   └── hk32f0301mxxc_it.c  # 中断服务程序
└── MDK-ARM/
    ├── Project.uvprojx     # Keil 项目文件
    └── startup_hk32f0301mxxc.s # 启动文件
```

### 主要函数

#### main.c
- `main()`: 主函数，初始化系统并运行主循环
- `UART_Config()`: 配置 UART1 串口
- `TIM1_PWM_Config()`: 配置 TIM1 PWM 输出
- `Delay()`: 毫秒级延时函数
- `SysTick_Handler()`: SysTick 中断处理程序
- `fputc()`: printf 重定向函数

#### 主循环功能
主程序演示 PWM 占空比渐变控制：
1. 占空比从 10% 渐变到 90%（步进 5%）
2. 占空比从 90% 渐变回 10%（步进 5%）
3. 每个占空比保持 500ms
4. 通过串口输出当前 PWM 占空比

## 编译配置

### 编译器设置
- **IDE**: Keil MDK-ARM
- **编译器**: ARM Compiler 5/6 兼容
- **设备**: HK32F0301MF4P7C
- **优化级别**: Level 1 (-O1)
- **启用 MicroLIB**: 是

### 预定义宏
```
USE_HK32F0301MF4P7C_EVAL  # 启用评估板功能
```

### ARM Compiler 6 兼容性
已修复 ARM Compiler 6 的兼容性问题：
- 修改 `hk32f0301mxxc_def.h` 中的 `NOP()` 宏
- 针对 ARM Compiler 6 使用 `__asm volatile ("nop")`
- 保持对 ARM Compiler 5、GCC 和 IAR 的兼容性

## 编译和调试

### 编译步骤
1. 打开 Keil MDK-ARM
2. 打开 `MDK-ARM/Project.uvprojx`
3. 确保选择正确的设备 (HK32F0301MF4P7C)
4. 点击 "Rebuild All" 编译项目
5. 编译成功后下载到目标板

### 串口调试
1. 连接 USB-TTL 转换器到电脑
2. 设置串口参数：115200 bps, 8N1
3. 打开串口调试工具（如 Putty、SecureCRT 等）
4. 复位或上电开发板
5. 观察串口输出信息

## 技术参数

### PWM 参数计算
- 系统时钟: 48MHz
- TIM1 预分频器: 0（不分频）
- TIM1 自动重载值: 2731
- PWM 频率: 48MHz / (2731 + 1) = 17.57kHz
- 占空比分辨率: 1/2732 ≈ 0.0366%

### 占空比控制
- 占空比范围: 100 ~ 900（对应 10% ~ 90%）
- 控制函数: `TIM_SetCompare1(TIM1, duty)`
- 占空比值: duty = (占空比百分比 × 2731) / 100

## 故障排除

### 常见问题
1. **编译错误**: 确保已修复 ARM Compiler 6 兼容性问题
2. **无串口输出**: 检查串口连接和波特率设置
3. **无 PWM 输出**: 检查 PD1 引脚连接和示波器设置
4. **程序不运行**: 检查 SWD 调试连接和供电

### ARM Compiler 6 错误修复
如果遇到 `__nop()` 未声明的错误，确保 `hk32f0301mxxc_def.h` 文件已正确修改：
```c
/* NOP 宏定义 - 支持各种编译器 */
#if defined ( __CC_ARM )   /* ARM Compiler 5 */
    #define NOP()   __nop()
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6000000)  /* ARM Compiler 6 */
    /* 使用内联汇编，避免头文件依赖 */
    #define NOP()   __asm volatile ("nop")
#elif defined ( __GNUC__ )  /* GCC Compiler */
    #define NOP()   asm("nop")
#elif defined ( __ICCARM__ )  /* IAR Compiler */
    #define NOP()   __no_operation()
#else
    #error "Unsupported compiler"
#endif
```

## 扩展应用

### 1. 电机控制
- 修改 PWM 频率以适应电机需求
- 添加方向控制和使能控制
- 实现速度闭环控制

### 2. LED 调光
- 修改 PWM 频率为几百 Hz
- 实现呼吸灯效果
- 添加颜色混合控制（RGB LED）

### 3. 通信扩展
- 添加 UART 命令解析
- 实现 MODBUS 协议
- 添加无线通信模块接口

## 版本历史

### V1.1.0 (2025-07-16)
- 基于 UART_Printf 示例创建
- 添加 TIM1 PWM 功能（17.57kHz，PWM2 模式）
- 实现 PWM 占空比渐变控制
- 修复 ARM Compiler 6 兼容性问题
- 添加详细的项目文档

### V1.0.0 (2025-07-16)
- 初始版本：UART_Printf 示例

## 许可证

本项目基于 HK32 SDK V3.0.0 示例代码修改，遵循原 SDK 的许可证条款。

## 技术支持

如需技术支持，请参考：
- HK32 官方网站
- HK32 SDK 文档
- 评估板用户手册