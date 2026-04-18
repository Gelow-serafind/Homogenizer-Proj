# UART + PWM + EC11 Demo — 时间片状态机架构

简体中文说明（README 同步到项目根目录）。

## 概述

本示例演示在 HK32F0301MF4P7C 平台上，将两项功能（TIM1 PWM 输出与 EC11 旋钮）整合到一个简单的时间片状态机（time-slice scheduler）中运行的设计：

- 高频任务用于对 EC11 进行实时去抖和 4x 正交解码；旋钮操作直接以步进方式调整 PWM 占空比（分辨率 5%）。
- PWM 更新由独立任务在检测到占空比变化时应用，避免频繁写寄存器。
- 所有打印集中到 100ms 的打印任务，避免串口刷屏和分散打印对实时性的影响。

本仓库的主要代码位置：`src/main.c`。

## 特性

- 时间片状态机（任务数组 + SysTick 毫秒倒计时）
- 任务划分：Encoder（2ms）、PWM（5ms）、Print（100ms）、Idle（默认）
- EC11 4x 解码，按完整步（4 次 transition）以 5%（`STEP_PERMILLE = 50`）步进占空比
- PWM 输出使用 TIM1 CH2（PD2），目标频率约 17.57 kHz（`PWM_TARGET_FREQ_HZ`）
- UART 串口统一输出，115200 8N1

## 架构说明（关键点）

1. 时间片调度器

- 使用 `SysTick_Config(SystemCoreClock / 1000)` 产生 1 ms 心跳，在 `SysTick_Handler` 内为每个任务的倒计时（`cnt`）递减。
- 主循环（`while(1)`）轮询任务表 `tasks[]`，倒计时为 0 的任务会被重置为其周期并被执行。任务数组中顺序即表示优先级（数组前面的任务优先）。

2. 任务划分

- `Task_Encoder`（高频任务，默认周期 `TASK_PERIOD_ENCODER_MS = 2` ms）
  - 执行 EC11 A/B 读取并使用 4x 转换表解码
  - 每当累计到完整一步（`stepAcc >= 4` 或 `<= -4`）时：
    - 更新 `EncoderPosition`
    - 以 `STEP_PERMILLE`（默认 50permille = 5%）增加或减少 `CurrentDutyPermille`（范围 0..1000）
    - 设置 `DutyChanged = 1`，等待 PWM 任务应用
  - 同时做按键（KEY）软件消抖，更新 `KeyPressedState`（active-low）

- `Task_PWM`（周期 `TASK_PERIOD_PWM_MS = 5` ms）
  - 仅当 `DutyChanged` 被置位时调用 `TIM1_SetDutyPermille(CurrentDutyPermille)`，写入 CCR2 并触发更新事件。写寄存器操作被限制到必要时刻，降低干扰。

- `Task_Print`（周期 `TASK_PERIOD_PRINT_MS = 100` ms）
  - 汇总并打印当前状态：占空比（%）、`ARR`、`CCR2`、编码器位置、按键状态。

- `Task_Idle`（周期 `TASK_PERIOD_DEFAULT_MS = 1000` ms）
  - 空任务，占位，用于未来维护工作或低优先级任务。

3. 数据与同步

- 共享变量 `CurrentDutyPermille`、`DutyChanged`、`EncoderPosition` 和 `KeyPressedState` 在任务之间使用。对关键倒计时时间点主循环会短暂禁中断以原子性更新计时器（`__disable_irq()` / `__enable_irq()`）。

## 硬件接线

- EC11 旋钮：
  - `EC11 A` -> `PC3`
  - `EC11 B` -> `PC4`
  - `EC11 KEY` -> `PC5`（低电平按下）
- PWM 输出（TIM1_CH2）：`PD2`（经 IOMUX 配置）
- 串口：使用评估板的 `EVAL_COM1`（115200 8N1）用于调试输出

> 请参考工程中的 `doc/接线说明.md` 获取板级连线示意。

## 配置项（在 `src/main.c`）

- `TASK_PERIOD_ENCODER_MS`：编码器任务周期（ms），默认 `2`
- `TASK_PERIOD_PWM_MS`：PWM 应用任务周期（ms），默认 `5`
- `TASK_PERIOD_PRINT_MS`：打印任务周期（ms），默认 `100`
- `STEP_PERMILLE`：每次旋钮完整步调整占空比的步进（permille），默认 `50`（即 5%）
- `PWM_TARGET_FREQ_HZ`：目标 PWM 频率（Hz），默认 `17570UL`
- 初始占空比：`CurrentDutyPermille` 初值在代码中默认 `500`（50%）

如需更改响应速度或步进精度，直接修改相应宏并重新编译。

## 构建与下载（Keil MDK）

1. 使用 Keil MDK 打开项目：`MDK-ARM/Project.uvprojx`。
2. 选择目标设备（HK32F0301MF4P7C）并编译（Rebuild）。
3. 使用评估板支持的下载器（ULINK、STLink 等）将 `Project.axf` 下载到目标板。

在没有 Keil 环境时，也可以使用命令行工具（如果已配置）或工程提供的输出文件配合你现有的烧录工具。

## 运行与验证

1. 串口连接到 `EVAL_COM1`，设置 `115200 8N1`。
2. 上电或复位板子，等待启动信息。
3. 每 100 ms 会看到类似的串口输出行（示例）：

```
PWM Duty: 50%, ARR=XXXXX, CCR2=YYYYY, Encoder Pos=0, Key=Released
```

4. 旋转编码器：顺时针每完整步骤增加 5% 占空比；逆时针每完整步骤减少 5%；超过 100% 或小于 0% 时保持边界值。

## 开发/调试建议

- 若需要更快的编码器响应，可把 `TASK_PERIOD_ENCODER_MS` 设为 1 ms；注意 CPU 占用增加。
- 若要减少串口输出对实时性的影响，可以增大 `TASK_PERIOD_PRINT_MS` 或将打印移入更低优先级的调试命令。

## 贡献

欢迎提交 issue 或 PR。建议遵循下列流程：

1. Fork 本仓库
2. 在 feature 分支上开发并遵守现有代码风格
3. 提交 PR 并附上简短描述与复现步骤

## License

本仓库 README 使用开源惯例格式。请在需要时为项目补充 `LICENSE` 文件（例如 MIT）。

---

若需我把 README 中的英文版、LICENSE 或 CONTRIBUTING 模板一并加入并提交，请回复我将继续操作。
