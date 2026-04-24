/*******************************************************************************
* @copyright: Hangzhou Cloudvoyage Cross-border E-commerce Co., Ltd.
* @filename:  main.c
* @brief:     
* @author:    tiedan
* @email:    tiedan1026@gmail.com
* @version:   V1.0/2026.04.19
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define PWM_TARGET_FREQ_HZ 17570UL

/* 任务周期（ms），可通过宏调整每个任务响应时间 */
#define TASK_PERIOD_ENCODER_MS 2U
#define TASK_PERIOD_APP_MS     10U
#define TASK_PERIOD_PWM_MS     5U
#define TASK_PERIOD_PRINT_MS   100U
#define TASK_PERIOD_TM1729_MS  50U
#define TASK_PERIOD_DEFAULT_MS 1000U
#define TASK_PERIOD_UART_CMD_MS 2U

/* 业务参数 */
#define SPEED_STEP_PERCENT     5U
#define LONG_PRESS_MS          3000U
#define KEY_DEBOUNCE_TICKS     3U
#define BLINK_INTERVAL_MS      500U
#define SETTING_TIMEOUT_MS     5000U

#define EC11_GPIO_PORT GPIOC
#define EC11_PIN_A GPIO_Pin_3
#define EC11_PIN_B GPIO_Pin_4
#define EC11_PIN_KEY GPIO_Pin_5

/* TM1729 (legacy wiring compatible with old project) */
#define TM1729_GPIO_PORT      GPIOC
#define TM1729_PIN_SCL        GPIO_Pin_6
#define TM1729_PIN_SDA        GPIO_Pin_7
#define TM1729_SLAVE_ADDRESS  0x7CU
#define TM1729_ICSET          0xEAU
#define TM1729_BLKCTL         0xF0U
#define TM1729_DISCTL         0xA2U
#define TM1729_MODESET        0xC8U
#define TM1729_APCTL          0xFCU
#define TM1729_ADSET          0x00U
#define TM1729_LCD_BUF_LEN    14U

#define TM1729_COM1 0U
#define TM1729_COM2 1U
#define TM1729_COM3 2U
#define TM1729_COM4 3U

/* 根据用户提供的 LCD 真值表建立玻璃屏引脚映射：PIN5..PIN13 为 9 条 SEG 线 */
#define TM1729_SEG_PIN5   0U
#define TM1729_SEG_PIN6   4U
#define TM1729_SEG_PIN7   8U
#define TM1729_SEG_PIN8   12U
#define TM1729_SEG_PIN9   16U
#define TM1729_SEG_PIN10  20U
#define TM1729_SEG_PIN11  24U
#define TM1729_SEG_PIN12  28U
#define TM1729_SEG_PIN13  32U

#define LCD_1stNUM_a   (TM1729_SEG_PIN5 + TM1729_COM1)
#define LCD_1stNUM_b   (TM1729_SEG_PIN6 + TM1729_COM1)
#define LCD_1stNUM_c   (TM1729_SEG_PIN6 + TM1729_COM3)
#define LCD_1stNUM_d   (TM1729_SEG_PIN5 + TM1729_COM4)
#define LCD_1stNUM_e   (TM1729_SEG_PIN5 + TM1729_COM3)
#define LCD_1stNUM_f   (TM1729_SEG_PIN5 + TM1729_COM2)
#define LCD_1stNUM_g   (TM1729_SEG_PIN6 + TM1729_COM2)

#define LCD_2ndNUM_a   (TM1729_SEG_PIN13 + TM1729_COM1)
#define LCD_2ndNUM_b   (TM1729_SEG_PIN12 + TM1729_COM1)
#define LCD_2ndNUM_c   (TM1729_SEG_PIN12 + TM1729_COM3)
#define LCD_2ndNUM_d   (TM1729_SEG_PIN13 + TM1729_COM4)
#define LCD_2ndNUM_e   (TM1729_SEG_PIN13 + TM1729_COM3)
#define LCD_2ndNUM_f   (TM1729_SEG_PIN13 + TM1729_COM2)
#define LCD_2ndNUM_g   (TM1729_SEG_PIN12 + TM1729_COM2)

#define LCD_3ndNUM_a   (TM1729_SEG_PIN11 + TM1729_COM1)
#define LCD_3ndNUM_b   (TM1729_SEG_PIN10 + TM1729_COM1)
#define LCD_3ndNUM_c   (TM1729_SEG_PIN10 + TM1729_COM3)
#define LCD_3ndNUM_d   (TM1729_SEG_PIN11 + TM1729_COM4)
#define LCD_3ndNUM_e   (TM1729_SEG_PIN11 + TM1729_COM3)
#define LCD_3ndNUM_f   (TM1729_SEG_PIN11 + TM1729_COM2)
#define LCD_3ndNUM_g   (TM1729_SEG_PIN10 + TM1729_COM2)

#define LCD_4ndNUM_a   (TM1729_SEG_PIN9 + TM1729_COM1)
#define LCD_4ndNUM_b   (TM1729_SEG_PIN8 + TM1729_COM1)
#define LCD_4ndNUM_c   (TM1729_SEG_PIN8 + TM1729_COM3)
#define LCD_4ndNUM_d   (TM1729_SEG_PIN9 + TM1729_COM4)
#define LCD_4ndNUM_e   (TM1729_SEG_PIN9 + TM1729_COM3)
#define LCD_4ndNUM_f   (TM1729_SEG_PIN9 + TM1729_COM2)
#define LCD_4ndNUM_g   (TM1729_SEG_PIN8 + TM1729_COM2)

#define LCD_5thNUM_a   (TM1729_SEG_PIN7 + TM1729_COM1)
#define LCD_5thNUM_b   (TM1729_SEG_PIN12 + TM1729_COM4)
#define LCD_5thNUM_c   (TM1729_SEG_PIN8 + TM1729_COM4)
#define LCD_5thNUM_d   (TM1729_SEG_PIN7 + TM1729_COM4)
#define LCD_5thNUM_e   (TM1729_SEG_PIN7 + TM1729_COM3)
#define LCD_5thNUM_f   (TM1729_SEG_PIN7 + TM1729_COM2)
#define LCD_5thNUM_g   (TM1729_SEG_PIN10 + TM1729_COM4)

#define LCD_RPM_S1     (TM1729_SEG_PIN6 + TM1729_COM4)

/* Private variables ---------------------------------------------------------*/
static uint16_t PWMPeriod;

/* 编码器基础信息 */
static volatile int32_t EncoderPosition = 0;
static volatile int8_t EncDeltaPending = 0;
static volatile uint8_t KeyShortPressEvent = 0;
static volatile uint8_t KeyLongPressEvent = 0;
static volatile uint8_t KeyPressedState = 1U; /* active-low: 0=pressed */

/* 业务数据 */
typedef enum {
  APP_MODE_IDLE = 0,
  APP_MODE_SETTING,
  APP_MODE_RUNNING,
  APP_MODE_PAUSED
} AppMode_t;

typedef enum {
  SET_FIELD_NONE = 0,
  SET_FIELD_SPEED,
  SET_FIELD_MIN,
  SET_FIELD_SEC
} SetField_t;

static volatile AppMode_t AppMode = APP_MODE_IDLE;
static AppMode_t AppModeBeforeSetting = APP_MODE_IDLE;
static volatile SetField_t SettingField = SET_FIELD_NONE;

static uint8_t ConfigSpeedPercent = 0U;  /* 上电默认显示速度参数为0 */
static uint8_t ConfigMin = 1U;           /* 模拟“上次设置值” */
static uint8_t ConfigSec = 0U;

static uint8_t EditSpeedPercent = 0U;
static uint8_t EditMin = 1U;
static uint8_t EditSec = 0U;

static uint16_t CountdownSec = 60U;
static uint8_t MotorSpeedPercent = 0U;

static volatile uint16_t CurrentDutyPermille = 0U;
static volatile uint8_t DutyChanged = 1U;

static uint8_t TM1729_BitBuf[TM1729_LCD_BUF_LEN] = {0};
static const uint8_t TM1729_NumModel[17] = {
  0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U, 0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU,
  0x77U, 0x7CU, 0x39U, 0x5EU, 0x79U, 0x71U, 0x00U
};
const unsigned char LCD_NumPos[] =
{
  LCD_1stNUM_a,
  LCD_1stNUM_b,
  LCD_1stNUM_c,
  LCD_1stNUM_d,
  LCD_1stNUM_e,
  LCD_1stNUM_f,
  LCD_1stNUM_g,

  LCD_2ndNUM_a,
  LCD_2ndNUM_b,
  LCD_2ndNUM_c,
  LCD_2ndNUM_d,
  LCD_2ndNUM_e,
  LCD_2ndNUM_f,
  LCD_2ndNUM_g,

  LCD_3ndNUM_a,
  LCD_3ndNUM_b,
  LCD_3ndNUM_c,
  LCD_3ndNUM_d,
  LCD_3ndNUM_e,
  LCD_3ndNUM_f,
  LCD_3ndNUM_g,

  LCD_4ndNUM_a,
  LCD_4ndNUM_b,
  LCD_4ndNUM_c,
  LCD_4ndNUM_d,
  LCD_4ndNUM_e,
  LCD_4ndNUM_f,
  LCD_4ndNUM_g,

  LCD_5thNUM_a,
  LCD_5thNUM_b,
  LCD_5thNUM_c,
  LCD_5thNUM_d,
  LCD_5thNUM_e,
  LCD_5thNUM_f,
  LCD_5thNUM_g,
};

/* 设置界面闪烁控制 */
static uint16_t BlinkElapsedMs = 0U;
static uint8_t BlinkVisible = 1U;
static uint16_t SettingIdleElapsedMs = 0U;
static uint8_t CountdownExpiredWhileSetting = 0U;

/* UART 命令输入系统 ---------------------------------------------------------*/
#define UART_RX_RING_SIZE  64U
#define CMD_LINE_MAX       64U

typedef void (*CmdCallback_t)(const char* args);

typedef struct {
  const char*    cmd;
  CmdCallback_t  cb;
} CmdEntry_t;

static volatile uint8_t  uartRxRing[UART_RX_RING_SIZE];
static volatile uint8_t  uartRxHead = 0U;
static volatile uint8_t  uartRxTail = 0U;
static uint8_t           cmdLineBuf[CMD_LINE_MAX];
static uint8_t           cmdLineLen = 0U;

/* 注册指令回调函数原型 */
static void Cmd_Help(const char* args);
static void Cmd_SetSpeed(const char* args);
static void Cmd_Start(const char* args);
static void Cmd_Pause(const char* args);
static void Cmd_Status(const char* args);

/* 指令表：添加新指令只需在此增加 { "name", CallbackFn } 项 */
static const CmdEntry_t cmdTable[] = {
  { "help",   Cmd_Help   },
  { "h",      Cmd_Help   },
  { "speed",  Cmd_SetSpeed },
  { "start",  Cmd_Start  },
  { "pause",  Cmd_Pause  },
  { "status", Cmd_Status },
  { NULL,     NULL       }
};

/* Private function prototypes -----------------------------------------------*/
static void UART_Config(void);
static void TIM1_PWM_Config(void);
static void TIM1_SetDutyPermille(uint16_t dutyPermille);
static void TM1729_Init(void);
static void TM1729_DemoUpdate(AppMode_t mode);

static void EC11_Config(void);

/* 任务处理函数 */
static void Task_Encoder(void);
static void Task_App(void);
static void Task_PWM(void);
static void Task_Print(void);
static void Task_TM1729(void);
static void Task_Idle(void);
static void Task_UartCmd(void);

/* 业务辅助函数 */
static uint16_t ComposeSeconds(uint8_t minute, uint8_t second);
static void EnterSettingMode(void);
static void ExitSettingMode(uint8_t applyChanges);
static void ResetSettingUiTimers(void);
static void AdvanceSettingField(void);
static void AdjustCurrentSettingByStep(int8_t step);
static void ApplyStartAction(void);
static void ApplyPauseAction(void);
static const char* GetModeName(AppMode_t mode);

static void TM1729_ShortDelay(void);
static void TM1729_Start(void);
static void TM1729_Stop(void);
static void TM1729_SendByte(uint8_t dat);
static void TM1729_SetBit(uint8_t bit);
static void TM1729_ClrBit(uint8_t bit);
static void TM1729_SetNumAt(uint8_t idx, uint8_t num);
static void TM1729_Show4Digits(uint8_t d3, uint8_t d2, uint8_t d1, uint8_t d0);
static void TM1729_SetRpmIndicator(uint8_t enable);
static void TM1729_ShowValueOnRange(uint16_t value, uint8_t firstIdx, uint8_t lastIdx);
static void TM1729_WriteBuffer(void);
static void TM1729_PortInit(void);

/* 任务调度结构 */
typedef void (*TaskHandler_t)(void);
typedef struct {
  uint16_t period_ms;    /* 周期 */
  volatile uint16_t cnt; /* 倒计时 */
  TaskHandler_t handler;
} Task_t;

static Task_t tasks[] = {
  { TASK_PERIOD_ENCODER_MS, 0, Task_Encoder },
  { TASK_PERIOD_UART_CMD_MS, 0, Task_UartCmd },
  { TASK_PERIOD_APP_MS,     0, Task_App     },
  { TASK_PERIOD_PWM_MS,     0, Task_PWM     },
  { TASK_PERIOD_PRINT_MS,   0, Task_Print   },
  { TASK_PERIOD_TM1729_MS,  0, Task_TM1729  },
  { TASK_PERIOD_DEFAULT_MS, 0, Task_Idle    }
};

#define TASKS_COUNT (sizeof(tasks)/sizeof(tasks[0]))

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (KEIL_startup_hk32f0301mxxc.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_hk32f0301mxxc.c file
     */

  /* UART configuration */
  UART_Config();
  /* PWM配置 */
  TIM1_PWM_Config();
  /* EC11 GPIO配置 */
  EC11_Config();
  /* TM1729 屏幕配置（复用老项目接线与时序） */
  TM1729_Init();

  /* Configure SysTick Timer for 1ms interrupts */
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }

  CountdownSec = ComposeSeconds(ConfigMin, ConfigSec);

  printf("\n\r========================================\n\r");
  printf("HK32F0301MxxxxC Homogenizer Demo\n\r");
  printf("========================================\n\r");
  printf("UART1: 115200 baud, 8N1\n\r");
  printf("TIM1 PWM: 17.57kHz, PD2 pin (CH2)\n\r");
  printf("EC11 A/B/KEY: PC3/PC4/PC5 (active low)\n\r");
  printf("TM1729 SCL/SDA: PC6/PC7\n\r");
  printf("Short Press: enter/advance setting\n\r");
  printf("Long Press (3s): start/pause\n\r");
  printf("========================================\n\r\n\r");

  /* Loop until the end of transmission */
  while (UART_GetFlagStatus(EVAL_COM1, UART_FLAG_TC) == RESET)
  {
  }

  printf("Starting scheduler...\n\r");
  printf("homogenizer > ");

  /* 主循环：轮询任务表，倒计时为0则执行任务并重置倒计时 */
  while(1)
  {
    for (uint32_t i = 0; i < TASKS_COUNT; ++i)
    {
      if (tasks[i].cnt == 0U)
      {
        /* 重置倒计时（关中断保证原子性），随后调用任务处理函数 */
        __disable_irq();
        tasks[i].cnt = tasks[i].period_ms;
        __enable_irq();

        tasks[i].handler();
      }
    }
  }
}
static void Task_Encoder(void)
{
  static const int8_t transition[16] =
  {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0
  };
  static uint8_t prevAB = 0x03;
  static int8_t stepAcc = 0;

  static uint8_t keyStable = 1U;
  static uint8_t keyDebounce = 0;
  static uint16_t keyPressMs = 0U;
  static uint8_t longFired = 0U;

  uint8_t a = (GPIO_ReadInputDataBit(EC11_GPIO_PORT, EC11_PIN_A) != Bit_RESET) ? 1U : 0U;
  uint8_t b = (GPIO_ReadInputDataBit(EC11_GPIO_PORT, EC11_PIN_B) != Bit_RESET) ? 1U : 0U;
  uint8_t currAB = (uint8_t)((a << 1) | b);
  uint8_t idx = (uint8_t)((prevAB << 2) | currAB);
  int8_t delta = transition[idx];

  if (delta != 0)
  {
    stepAcc += delta;

    if (stepAcc >= 4)
    {
      stepAcc = 0;
      EncoderPosition++;
      if (EncDeltaPending < 120)
      {
        EncDeltaPending++;
      }
    }
    else if (stepAcc <= -4)
    {
      stepAcc = 0;
      EncoderPosition--;
      if (EncDeltaPending > -120)
      {
        EncDeltaPending--;
      }
    }
  }

  prevAB = currAB;

  /* Active-low key with debounce + short/long press events */
  {
    uint8_t keyRaw = (GPIO_ReadInputDataBit(EC11_GPIO_PORT, EC11_PIN_KEY) != Bit_RESET) ? 1U : 0U;

    if (keyRaw != keyStable)
    {
      if (++keyDebounce >= KEY_DEBOUNCE_TICKS)
      {
        keyDebounce = 0;
        keyStable = keyRaw;
        KeyPressedState = keyStable;

        if (keyStable == 0U)
        {
          keyPressMs = 0U;
          longFired = 0U;
        }
        else
        {
          if ((longFired == 0U) && (keyPressMs > 0U))
          {
            KeyShortPressEvent = 1U;
          }
          keyPressMs = 0U;
          longFired = 0U;
        }
      }
    }
    else
    {
      keyDebounce = 0;

      if (keyStable == 0U)
      {
        if (keyPressMs < 60000U)
        {
          keyPressMs += TASK_PERIOD_ENCODER_MS;
        }

        if ((longFired == 0U) && (keyPressMs >= LONG_PRESS_MS))
        {
          KeyLongPressEvent = 1U;
          longFired = 1U;
        }
      }
    }
  }
}

static void Task_App(void)
{
  int8_t delta = 0;
  uint8_t shortEvt = 0U;
  uint8_t longEvt = 0U;

  __disable_irq();
  delta = EncDeltaPending;
  EncDeltaPending = 0;
  shortEvt = KeyShortPressEvent;
  KeyShortPressEvent = 0U;
  longEvt = KeyLongPressEvent;
  KeyLongPressEvent = 0U;
  __enable_irq();

  if (longEvt != 0U)
  {
    if (AppMode == APP_MODE_RUNNING)
    {
      ApplyPauseAction();
    }
    else
    {
      ApplyStartAction();
    }
  }

  if (shortEvt != 0U)
  {
    if (AppMode == APP_MODE_SETTING)
    {
      AdvanceSettingField();
    }
    else
    {
      EnterSettingMode();
    }
  }

  while (delta > 0)
  {
    AdjustCurrentSettingByStep(1);
    delta--;
  }

  while (delta < 0)
  {
    AdjustCurrentSettingByStep(-1);
    delta++;
  }

  if (AppMode == APP_MODE_SETTING)
  {
    BlinkElapsedMs += TASK_PERIOD_APP_MS;
    SettingIdleElapsedMs += TASK_PERIOD_APP_MS;

    if (BlinkElapsedMs >= BLINK_INTERVAL_MS)
    {
      BlinkElapsedMs = 0U;
      BlinkVisible = (uint8_t)(1U - BlinkVisible);
    }

    if (SettingIdleElapsedMs >= SETTING_TIMEOUT_MS)
    {
      ExitSettingMode(1U);
    }
  }
}

/* PWM任务：在DutyChanged被置位时应用新的占空比 */
static void Task_PWM(void)
{
  if (DutyChanged)
  {
    uint16_t dutyPermille = 0U;

    /* 运行态：输出 MotorSpeedPercent；
       设置转速页仅在“从运行中进入设置”时才实时输出 EditSpeedPercent，其它情况输出0 */
    if (AppMode == APP_MODE_RUNNING)
    {
      dutyPermille = (uint16_t)MotorSpeedPercent * 10U;
    }
    else if (AppMode == APP_MODE_SETTING && SettingField == SET_FIELD_SPEED && AppModeBeforeSetting == APP_MODE_RUNNING && CountdownExpiredWhileSetting == 0U)
    {
      dutyPermille = (uint16_t)EditSpeedPercent * 10U;
    }
    else if (AppMode == APP_MODE_SETTING && AppModeBeforeSetting == APP_MODE_RUNNING && CountdownExpiredWhileSetting == 0U)
    {
      dutyPermille = (uint16_t)MotorSpeedPercent * 10U;
    }
    else
    {
      dutyPermille = 0U;
    }

    CurrentDutyPermille = dutyPermille;
    TIM1_SetDutyPermille(CurrentDutyPermille);
    DutyChanged = 0U;
  }
}

/* 打印任务：每100ms汇总并打印PWM与旋钮信息 */
static void Task_Print(void)
{
  uint16_t showSec = CountdownSec;
  uint8_t showMin = (uint8_t)(showSec / 60U);
  uint8_t showS = (uint8_t)(showSec % 60U);
  uint8_t speedShow = (AppMode == APP_MODE_RUNNING) ? MotorSpeedPercent : 0U;
  const char* modeName = GetModeName(AppMode);

  if (AppMode == APP_MODE_SETTING)
  {
    showMin = EditMin;
    showS = EditSec;
    speedShow = EditSpeedPercent;
  }

  /* 单行输出，使用回车覆盖上一行，尾部填充空格避免残留字符 */
  if ((AppMode == APP_MODE_SETTING) && (SettingField == SET_FIELD_SPEED) && (BlinkVisible == 0U))
  {
    printf("\rMODE:%-10s | SPEED: --%% | TIME:%02u:%02u | KEY:%-8s | POS:%6ld    ",
           modeName,
           (unsigned)showMin,
           (unsigned)showS,
           (KeyPressedState == 0U) ? "PRESSED" : "RELEASED",
           (long)EncoderPosition);
  }
  else if ((AppMode == APP_MODE_SETTING) && (SettingField == SET_FIELD_MIN) && (BlinkVisible == 0U))
  {
    printf("\rMODE:%-10s | SPEED:%3u%% | TIME:--:%02u | KEY:%-8s | POS:%6ld    ",
           modeName,
           (unsigned)speedShow,
           (unsigned)showS,
           (KeyPressedState == 0U) ? "PRESSED" : "RELEASED",
           (long)EncoderPosition);
  }
  else if ((AppMode == APP_MODE_SETTING) && (SettingField == SET_FIELD_SEC) && (BlinkVisible == 0U))
  {
    printf("\rMODE:%-10s | SPEED:%3u%% | TIME:%02u:-- | KEY:%-8s | POS:%6ld    ",
           modeName,
           (unsigned)speedShow,
           (unsigned)showMin,
           (KeyPressedState == 0U) ? "PRESSED" : "RELEASED",
           (long)EncoderPosition);
  }
  else
  {
    printf("\rMODE:%-10s | SPEED:%3u%% | TIME:%02u:%02u | KEY:%-8s | POS:%6ld    ",
           modeName,
           (unsigned)speedShow,
           (unsigned)showMin,
           (unsigned)showS,
           (KeyPressedState == 0U) ? "PRESSED" : "RELEASED",
           (long)EncoderPosition);
  }

}

/* 空任务：占位，可用于低优先级维护工作 */
static void Task_Idle(void)
{
  /* 设置模式下冻结倒计时，避免时间设置过程中继续流逝 */
  if (AppMode != APP_MODE_RUNNING)
  {
    return;
  }

  if (CountdownSec > 0U)
  {
    CountdownSec--;
    if (CountdownSec == 0U)
    {
      ApplyPauseAction();
    }
  }
  else
  {
    ApplyPauseAction();
  }
}

/* UART 命令输入任务：2ms 轮询，接收完整行后解析执行 */
static void UART_ProcessCommandLine(const char* line)
{
  const char* p = line;
  while (*p == ' ') p++;
  if (*p == '\0') return;

  const char* args = p;
  while (*args != '\0' && *args != ' ') args++;
  uint8_t cmdLen = (uint8_t)(args - p);
  while (*args == ' ') args++;

  for (const CmdEntry_t* entry = cmdTable; entry->cmd != NULL; entry++)
  {
    if (strncmp(p, entry->cmd, cmdLen) == 0 && entry->cmd[cmdLen] == '\0')
    {
      entry->cb(args);
      return;
    }
  }

  printf("\r\nUnknown: %s\r\n", line);
}

static void UART_PollRx(void)
{
  /* 将 UART 硬件接收到的字节全部读入环形缓冲 */
  while (UART_GetFlagStatus(EVAL_COM1, UART_FLAG_RXNE) == SET)
  {
    uint8_t ch = (uint8_t)UART_ReceiveData(EVAL_COM1);
    uint8_t next = (uint8_t)((uartRxHead + 1U) % UART_RX_RING_SIZE);
    if (next != uartRxTail)
    {
      uartRxRing[uartRxHead] = ch;
      uartRxHead = next;
    }
  }

  /* 从环形缓冲提取字符进行行编辑和解析 */
  while (uartRxTail != uartRxHead)
  {
    uint8_t ch = uartRxRing[uartRxTail];
    uartRxTail = (uint8_t)((uartRxTail + 1U) % UART_RX_RING_SIZE);

    if (ch == '\n' || ch == '\r')
    {
      /* 跳过紧随其后的连续换行符（兼容 \r\n / \n\r / 单个 \r / 单个 \n） */
      while (uartRxTail != uartRxHead)
      {
        uint8_t nxt = uartRxRing[uartRxTail];
        if (nxt == '\n' || nxt == '\r')
          uartRxTail = (uint8_t)((uartRxTail + 1U) % UART_RX_RING_SIZE);
        else
          break;
      }

      /* 处理收到的行（可能为空行） */
      cmdLineBuf[cmdLineLen] = '\0';
      UART_ProcessCommandLine((const char*)cmdLineBuf);
      cmdLineLen = 0U;

      /* 无论是否空行，都打印下一个提示符 */
      printf("\r\nhomogenizer > ");
    }
    else if (ch == '\b' || ch == 0x7F)
    {
      /* 退格：擦除终端上前一个字符 */
      if (cmdLineLen > 0U)
      {
        cmdLineLen--;
        printf("\b \b");
      }
    }
    else if (cmdLineLen < CMD_LINE_MAX - 1U)
    {
      /* 普通字符：存入缓冲区并回显（远程 echo，不依赖终端本地 echo 设置） */
      cmdLineBuf[cmdLineLen++] = ch;
      printf("%c", (char)ch);
    }
  }
}

static void Task_UartCmd(void)
{
  UART_PollRx();
}

/* ======================== 命令回调函数实现 ======================== */

static void Cmd_Help(const char* args)
{
  (void)args;
  printf("\r\nAvailable commands:\r\n");
  for (const CmdEntry_t* entry = cmdTable; entry->cmd != NULL; entry++)
  {
    printf("  %s\r\n", entry->cmd);
  }
}

static void Cmd_SetSpeed(const char* args)
{
  int val = 0;
  while (*args == ' ') args++;
  while (*args >= '0' && *args <= '9')
  {
    val = val * 10 + (*args - '0');
    args++;
  }
  if (val < 0) val = 0;
  if (val > 100) val = 100;

  if (AppMode == APP_MODE_SETTING)
  {
    EditSpeedPercent = (uint8_t)val;
    ResetSettingUiTimers();
    DutyChanged = 1U;
  }
  ConfigSpeedPercent = (uint8_t)val;
  printf("\r\nSpeed set to %u%%\r\n", (unsigned)ConfigSpeedPercent);
}

static void Cmd_Start(const char* args)
{
  (void)args;
  ApplyStartAction();
  printf("\r\nStart\r\n");
}

static void Cmd_Pause(const char* args)
{
  (void)args;
  ApplyPauseAction();
  printf("\r\nPause\r\n");
}

static void Cmd_Status(const char* args)
{
  (void)args;
  uint8_t m = ConfigMin;
  uint8_t s = ConfigSec;
  printf("\r\nMode:%-10s | Speed:%3u%% | Time:%02u:%02u | Countdown:%4u s\r\n",
         GetModeName(AppMode),
         (unsigned)((AppMode == APP_MODE_RUNNING) ? MotorSpeedPercent : ConfigSpeedPercent),
         (unsigned)m, (unsigned)s,
         (unsigned)CountdownSec);
}

/* TM1729 显示任务：50ms 刷新一次显示缓冲 */
static void Task_TM1729(void)
{
  uint8_t speedToShow;
  memset(TM1729_BitBuf, 0, sizeof(TM1729_BitBuf));

  if (AppMode == APP_MODE_SETTING)
  {
    if (SettingField == SET_FIELD_SPEED)
    {
      /* 设置速度时：数字闪烁，rpm 始终点亮 */
      if (BlinkVisible != 0U)
      {
        TM1729_ShowValueOnRange(EditSpeedPercent, 0U, 4U);
      } else {
        for (uint8_t i = 0; i < 5; ++i) TM1729_SetNumAt(i, 16U);
      }
      TM1729_SetRpmIndicator(1U);
    }
    else
    {
      /* 设置分钟/秒钟时：1、2 显示分钟，3 显示-，4、5 显示秒钟，选中谁谁闪烁 */
      uint8_t min = EditMin;
      uint8_t sec = EditSec;
      uint8_t min_hi = min / 10U;
      uint8_t min_lo = min % 10U;
      uint8_t sec_hi = sec / 10U;
      uint8_t sec_lo = sec % 10U;
      /* 先全部常亮 */
      TM1729_SetNumAt(0, min_hi);
      TM1729_SetNumAt(1, min_lo);
      TM1729_SetNumAt(2, 16U); /* “-” 用空码 */
      TM1729_SetNumAt(3, sec_hi);
      TM1729_SetNumAt(4, sec_lo);
      /* 闪烁选中字段 */
      if (BlinkVisible == 0U)
      {
        if (SettingField == SET_FIELD_MIN)
        {
          TM1729_SetNumAt(0, 16U);
          TM1729_SetNumAt(1, 16U);
        }
        else if (SettingField == SET_FIELD_SEC)
        {
          TM1729_SetNumAt(3, 16U);
          TM1729_SetNumAt(4, 16U);
        }
      }
      TM1729_SetRpmIndicator(0U);
    }
  }
  else
  {
    /* 运行态显示当前转速；停止态显示 0 转速 */
    speedToShow = (AppMode == APP_MODE_RUNNING) ? MotorSpeedPercent : 0U;
    TM1729_ShowValueOnRange(speedToShow, 0U, 4U);
    TM1729_SetRpmIndicator(1U);
  }
  TM1729_WriteBuffer();
}

static uint16_t ComposeSeconds(uint8_t minute, uint8_t second)
{
  return (uint16_t)((uint16_t)minute * 60U + (uint16_t)second);
}

static uint16_t CountdownSecBackup = 0U; // 新增：设置态冻结倒计时
static void EnterSettingMode(void)
{
  if (AppMode != APP_MODE_SETTING)
  {
    AppModeBeforeSetting = AppMode;
    AppMode = APP_MODE_SETTING;
    SettingField = SET_FIELD_SPEED;
    EditSpeedPercent = ConfigSpeedPercent;
    EditMin = ConfigMin;
    EditSec = ConfigSec;
    CountdownSecBackup = CountdownSec; // 进入设置时冻结倒计时
    ResetSettingUiTimers();
  }
}

static void ExitSettingMode(uint8_t applyChanges)
{
  if (AppMode != APP_MODE_SETTING)
  {
    return;
  }

  if (applyChanges != 0U)
  {
    ConfigSpeedPercent = EditSpeedPercent;
    ConfigMin = EditMin;
    ConfigSec = EditSec;
  }

  if (AppModeBeforeSetting == APP_MODE_RUNNING)
  {
    /* 返回运行态：如果有变更则更新目标速度与倒计时 */
    if (applyChanges != 0U)
    {
      MotorSpeedPercent = ConfigSpeedPercent;
      CountdownSec = ComposeSeconds(ConfigMin, ConfigSec);
      DutyChanged = 1U;
    }
    else
    {
      CountdownSec = CountdownSecBackup; // 未提交则恢复冻结前的倒计时
    }
    AppMode = APP_MODE_RUNNING;
  }
  else if (AppModeBeforeSetting == APP_MODE_PAUSED)
  {
    /* 如果进入设置之前是暂停态，则退出后仍为暂停态 */
    AppMode = APP_MODE_PAUSED;
    MotorSpeedPercent = 0U;
    DutyChanged = 1U;
    if (applyChanges != 0U)
    {
      CountdownSec = ComposeSeconds(ConfigMin, ConfigSec);
    }
    else
    {
      CountdownSec = CountdownSecBackup;
    }
  }
  else
  {
    AppMode = APP_MODE_IDLE;
    MotorSpeedPercent = 0U;
    if (applyChanges != 0U)
    {
      CountdownSec = ComposeSeconds(ConfigMin, ConfigSec);
      DutyChanged = 1U;
    }
    else
    {
      CountdownSec = CountdownSecBackup;
    }
  }

  SettingField = SET_FIELD_NONE;
  BlinkVisible = 1U;
  BlinkElapsedMs = 0U;
  SettingIdleElapsedMs = 0U;
  CountdownExpiredWhileSetting = 0U;
}

static void ResetSettingUiTimers(void)
{
  BlinkVisible = 1U;
  BlinkElapsedMs = 0U;
  SettingIdleElapsedMs = 0U;
  CountdownExpiredWhileSetting = 0U;
}

static void AdvanceSettingField(void)
{
  ResetSettingUiTimers();

  if (SettingField == SET_FIELD_SPEED)
  {
    SettingField = SET_FIELD_MIN;
    return;
  }

  if (SettingField == SET_FIELD_MIN)
  {
    SettingField = SET_FIELD_SEC;
    return;
  }

  SettingField = SET_FIELD_SPEED;
}

static void AdjustCurrentSettingByStep(int8_t step)
{
  if (AppMode != APP_MODE_SETTING)
  {
    return;
  }

  if (SettingField == SET_FIELD_SPEED)
  {
    int16_t value = (int16_t)EditSpeedPercent + ((int16_t)step * (int16_t)SPEED_STEP_PERCENT);
    if (value < 0)
    {
      value = 0;
    }
    else if (value > 100)
    {
      value = 100;
    }
    EditSpeedPercent = (uint8_t)value;
    DutyChanged = 1U; // 立即触发PWM刷新
    ResetSettingUiTimers();
    return;
  }

  if (SettingField == SET_FIELD_MIN)
  {
    int16_t value = (int16_t)EditMin + step;
    if (value < 0)
    {
      value = 0;
    }
    else if (value > 99)
    {
      value = 99;
    }
    EditMin = (uint8_t)value;
  }
  else if (SettingField == SET_FIELD_SEC)
  {
    int16_t value = (int16_t)EditSec + step;
    if (value < 0)
    {
      value = 0;
    }
    else if (value > 59)
    {
      value = 59;
    }
    EditSec = (uint8_t)value;
  }

  ResetSettingUiTimers();
}

static void ApplyStartAction(void)
{
  if (AppMode == APP_MODE_SETTING)
  {
    ConfigSpeedPercent = EditSpeedPercent;
    ConfigMin = EditMin;
    ConfigSec = EditSec;
    SettingField = SET_FIELD_NONE;
    BlinkVisible = 1U;
    BlinkElapsedMs = 0U;
    SettingIdleElapsedMs = 0U;
    CountdownExpiredWhileSetting = 0U;
  }

  if (CountdownSec == 0U)
  {
    CountdownSec = ComposeSeconds(ConfigMin, ConfigSec);
  }
  else if ((AppMode == APP_MODE_SETTING) && (AppModeBeforeSetting == APP_MODE_RUNNING))
  {
    CountdownSec = ComposeSeconds(ConfigMin, ConfigSec);
  }

  AppMode = APP_MODE_RUNNING;
  MotorSpeedPercent = ConfigSpeedPercent;
  DutyChanged = 1U;
}

static void ApplyPauseAction(void)
{
  AppMode = APP_MODE_PAUSED;
  MotorSpeedPercent = 0U;
  DutyChanged = 1U;
}

static const char* GetModeName(AppMode_t mode)
{
  if (mode == APP_MODE_SETTING)
  {
    if (SettingField == SET_FIELD_SPEED)
    {
      return "SETTING-SPEED";
    }
    if (SettingField == SET_FIELD_MIN)
    {
      return "SETTING-MIN";
    }
    if (SettingField == SET_FIELD_SEC)
    {
      return "SETTING-SEC";
    }
    return "SETTING";
  }

  if (mode == APP_MODE_RUNNING)
  {
    return "RUNNING";
  }
  if (mode == APP_MODE_PAUSED)
  {
    return "PAUSED";
  }
  return "IDLE";
}

/**
  * @brief  Configure EC11 pins on PC3/PC4/PC5 as pull-up inputs
  * @param  None
  * @retval None
  */
static void EC11_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = EC11_PIN_A | EC11_PIN_B | EC11_PIN_KEY;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(EC11_GPIO_PORT, &GPIO_InitStructure);
}

static void TM1729_ShortDelay(void)
{
  for (volatile uint32_t i = 0; i < 32U; ++i)
  {
  }
}

static void TM1729_Start(void)
{
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SDA, Bit_SET);
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_SET);
  TM1729_ShortDelay();
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SDA, Bit_RESET);
  TM1729_ShortDelay();
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_RESET);
}

static void TM1729_Stop(void)
{
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_SET);
  TM1729_ShortDelay();
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SDA, Bit_RESET);
  TM1729_ShortDelay();
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SDA, Bit_SET);
  TM1729_ShortDelay();
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_RESET);
}

static void TM1729_SendByte(uint8_t dat)
{
  for (uint8_t i = 0U; i < 8U; ++i)
  {
    GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_RESET);
    if ((dat & 0x80U) != 0U)
    {
      GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SDA, Bit_SET);
    }
    else
    {
      GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SDA, Bit_RESET);
    }
    TM1729_ShortDelay();
    GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_SET);
    TM1729_ShortDelay();
    dat <<= 1;
  }

  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_RESET);
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SDA, Bit_RESET);
  TM1729_ShortDelay();
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_SET);
  TM1729_ShortDelay();
  GPIO_WriteBit(TM1729_GPIO_PORT, TM1729_PIN_SCL, Bit_RESET);
}

static void TM1729_SetBit(uint8_t bit)
{
  uint8_t tmp = (uint8_t)(7U - (bit % 8U));
  TM1729_BitBuf[bit / 8U] |= (uint8_t)(1U << tmp);
}

static void TM1729_ClrBit(uint8_t bit)
{
  uint8_t tmp = (uint8_t)(7U - (bit % 8U));
  TM1729_BitBuf[bit / 8U] &= (uint8_t)(~(1U << tmp));
}

static void TM1729_SetNumAt(uint8_t idx, uint8_t num)
{
  uint8_t seg = TM1729_NumModel[(num <= 16U) ? num : 16U];

  if (idx >= 5U)
  {
    return;
  }

  for (uint8_t i = 0U; i < 7U; ++i)
  {
    if ((seg & 0x01U) != 0U)
    {
      TM1729_SetBit(LCD_NumPos[(idx * 7U) + i]);
    }
    else
    {
      TM1729_ClrBit(LCD_NumPos[(idx * 7U) + i]);
    }
    seg >>= 1;
  }
}

static void TM1729_Show4Digits(uint8_t d3, uint8_t d2, uint8_t d1, uint8_t d0)
{
  memset(TM1729_BitBuf, 0, sizeof(TM1729_BitBuf));

  TM1729_SetNumAt(0U, d3);
  TM1729_SetNumAt(1U, d2);
  TM1729_SetNumAt(2U, d1);
  TM1729_SetNumAt(3U, d0);
}

static void TM1729_SetRpmIndicator(uint8_t enable)
{
  if (enable != 0U)
  {
    TM1729_SetBit(LCD_RPM_S1);
  }
  else
  {
    TM1729_ClrBit(LCD_RPM_S1);
  }
}

static void TM1729_ShowValueOnRange(uint16_t value, uint8_t firstIdx, uint8_t lastIdx)
{
  uint8_t i;
  uint8_t hasOutput = 0U;

  if ((firstIdx >= 5U) || (lastIdx >= 5U) || (firstIdx > lastIdx))
  {
    return;
  }

  for (i = firstIdx; i <= lastIdx; ++i)
  {
    TM1729_SetNumAt(i, 16U);
  }

  for (i = lastIdx; ; --i)
  {
    if ((value == 0U) && (hasOutput != 0U))
    {
      break;
    }

    TM1729_SetNumAt(i, (uint8_t)(value % 10U));
    value /= 10U;
    hasOutput = 1U;

    if (i == firstIdx)
    {
      break;
    }
  }
}

static void TM1729_WriteBuffer(void)
{
  TM1729_Start();
  TM1729_SendByte(TM1729_SLAVE_ADDRESS);
  TM1729_SendByte(TM1729_ADSET);
  for (uint8_t i = 0U; i < TM1729_LCD_BUF_LEN; ++i)
  {
    TM1729_SendByte(TM1729_BitBuf[i]);
  }
  TM1729_Stop();
}

static void TM1729_PortInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = TM1729_PIN_SCL | TM1729_PIN_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(TM1729_GPIO_PORT, &GPIO_InitStructure);
}

static void TM1729_Init(void)
{
  TM1729_PortInit();

  TM1729_Stop();
  TM1729_Start();
  TM1729_SendByte(TM1729_SLAVE_ADDRESS);
  TM1729_SendByte(TM1729_ICSET);
  TM1729_SendByte(TM1729_DISCTL);
  TM1729_SendByte(TM1729_BLKCTL);
  TM1729_SendByte(TM1729_APCTL);
  TM1729_SendByte(TM1729_MODESET);
  TM1729_Stop();

  /* 上电先显示固定测试内容 */
  memset(TM1729_BitBuf, 0, sizeof(TM1729_BitBuf));
  TM1729_SetNumAt(0U, 1U);
  TM1729_SetNumAt(1U, 2U);
  TM1729_SetNumAt(2U, 3U);
  TM1729_SetNumAt(3U, 4U);
  TM1729_SetNumAt(4U, 16U);
  TM1729_SetRpmIndicator(1U);
  TM1729_WriteBuffer();
}

static void TM1729_DemoUpdate(AppMode_t mode)
{
  /* 状态机演示码：IDLE/SETTING/RUNNING/PAUSED -> 1111/2222/3333/4444 */
  if (mode == APP_MODE_SETTING)
  {
    TM1729_Show4Digits(2U, 2U, 2U, 2U);
  }
  else if (mode == APP_MODE_RUNNING)
  {
    TM1729_Show4Digits(3U, 3U, 3U, 3U);
  }
  else if (mode == APP_MODE_PAUSED)
  {
    TM1729_Show4Digits(4U, 4U, 4U, 4U);
  }
  else
  {
    TM1729_Show4Digits(1U, 1U, 1U, 1U);
  }

  TM1729_WriteBuffer();
}

/**
  * @brief  Configure the UART Device
  * @param  None
  * @retval None
  */
static void UART_Config(void)
{
  UART_InitTypeDef UART_InitStructure;

  /* USARTx configured as follow:
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_WordLength = UART_WordLength_8b;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_No;
  UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

  HK_EVAL_COMInit(COM1, &UART_InitStructure);
}

/**
  * @brief  Configure TIM1 for PWM output
  * @param  None
  * @retval None
  */
static void TIM1_PWM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;

  /* Enable peripheral clocks */
  RCC_APBPeriph2ClockCmd(RCC_APBPeriph2_TIM1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

#if defined(HK32F0301MF4P7C)
  /* Ensure package pin mux routes TIM1_CH2 signal to PD2 */
  GPIO_IOMUX_ChangePin(IOMUX_PIN11, IOMUX_PB5_SEL_PD2);
#endif

  /* Configure PD2 as alternate function push-pull (TIM1_CH2) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Connect TIM1_CH2 to PD2 (AF3, per TIM1 mapping) */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_3);

  /* Time base configuration for 17.57kHz PWM */
  PWMPeriod = (uint16_t)((SystemCoreClock / PWM_TARGET_FREQ_HZ) - 1U);
  TIM_TimeBaseStructure.TIM_Period = PWMPeriod;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Match official TIM_7PWMOutputs OC mode/polarity style */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;  /* Initial 0% duty cycle */
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  /* Enable preload for smooth CCR/ARR updates */
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM1, ENABLE);

  /* Enable TIM1 counter */
  TIM_Cmd(TIM1, ENABLE);
  
  /* Enable TIM1 Main Output */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/**
  * @brief  Set TIM1 CH2 duty by permille (0..1000)
  * @param  dutyPermille: duty ratio in permille (1000 = 100%)
  * @retval None
  */
static void TIM1_SetDutyPermille(uint16_t dutyPermille)
{
  uint32_t compare;

  if (dutyPermille >= 1000U)
  {
    /* CCR > ARR keeps PWM1 output continuously active */
    compare = (uint32_t)PWMPeriod + 1U;
  }
  else
  {
    compare = ((uint32_t)(PWMPeriod + 1U) * dutyPermille) / 1000U;
  }

  TIM_SetCompare2(TIM1, (uint16_t)compare);

  /* Force UEV so new CCR takes effect immediately even with preload enabled */
  TIM_GenerateEvent(TIM1, TIM_EventSource_Update);
}

/**
  * @brief  SysTick interrupt handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  /* 为所有任务倒计时（每1ms调用一次） */
  for (uint32_t i = 0; i < TASKS_COUNT; ++i)
  {
    if (tasks[i].cnt > 0U)
    {
      tasks[i].cnt--;
    }
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */

  /* e.g. write a character to the USART */
  UART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until transmit data register is empty */
  while (UART_GetFlagStatus(EVAL_COM1, UART_FLAG_TC) == RESET)
  {
  }

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


