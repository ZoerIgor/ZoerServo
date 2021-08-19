#include "servo.h"
#include "EEPROM.h"

#define SERIAL_SPEED 100000
#define EEPROM_ADDRESS 0

#define SOURSE_PIN_PWM A1
#define SOURSE_PIN_ENABLE A2

#define ENCODER_PIN_INPUT A0
#define ENCODER_PWM_RESOLUTION 1024

#define MOTOR_DRIVER_PIN_ENABLE 6
#define MOTOR_DRIVER_PIN_FORWARD 5
#define MOTOR_DRIVER_PIN_BACK 4
#define MOTOR_DRIVER_PIN_PWM 9

#define GYVER_PID_BIT_RESOLUTION 9
#define GYVER_PID_CALCULATE_FREQUENCY 1
#define GYVER_PID_MAX_PWM_RESOLUTION 511

Source sourse(SOURSE_PIN_PWM, SOURSE_PIN_ENABLE);
Encoder encoder(ENCODER_PIN_INPUT, ENCODER_PWM_RESOLUTION);
MotorDriver driver(MOTOR_DRIVER_PIN_ENABLE, MOTOR_DRIVER_PIN_FORWARD, MOTOR_DRIVER_PIN_BACK, MOTOR_DRIVER_PIN_PWM);
GyverPID pid;

unsigned short ReadEeprom()
{
    unsigned short value = 0;
    EEPROM.get(EEPROM_ADDRESS, value);
    return (value > 359 && value < 0) ? 0 : value;
}

void WriteEeprom(short value)
{
    EEPROM.put(EEPROM_ADDRESS, value);
}

void setup()
{
    Serial.begin(SERIAL_SPEED);
    EEPROM.begin();
    encoder.SetCalibAngle(ReadEeprom());
    PWM_resolution(MOTOR_DRIVER_PIN_PWM, GYVER_PID_BIT_RESOLUTION, FAST_PWM);
    pid.setDt(GYVER_PID_CALCULATE_FREQUENCY);
}

void loop()
{
    if (sourse.GetCommand() == EnumSourse::ENABLE) // Работа сервопривода разрешена из внешнего источника или из терминала
    {
        pid.setpoint = sourse.GetTargetDeg();
        pid.input = encoder.GetCurrentDeg();
        driver.Enable(true);
        if (pid.setpoint > pid.input)
        {
            pid.Ki = 0.5;
            if (pid.setpoint - pid.input <= 2)
            {
                pid.Kp = 1.0;
                pid.setLimits(20, 300);
            }
            else
            {
                pid.Kp = 1.8;
                pid.setLimits(20, GYVER_PID_MAX_PWM_RESOLUTION);
            }
            pid.setDirection(!driver.Direction(true));
        }
        else if (pid.setpoint < pid.input)
        {
            pid.Ki = 0.5;
            if (pid.input - pid.setpoint <= 2)
            {
                pid.Kp = 1.0;
                pid.setLimits(20, 300);
            }
            else
            {
                pid.Kp = 1.8;
                pid.setLimits(20, GYVER_PID_MAX_PWM_RESOLUTION);
            }
            pid.setDirection(!driver.Direction(false));
        }
        else
        {
            pid.Kp = 0;
            pid.Ki = 0;
            pid.setLimits(0, 0);
        }
        pid.getResultTimer();
        PWM_set(driver.GetPwmPin(), pid.output);
    }
    else if(sourse.GetCommand() == EnumSourse::DISABLE) // Работа сервопривода запрещена
    {
        driver.Enable(false);
    }
    else if(sourse.GetCommand() == EnumSourse::CALIBRATE) // Калибровка энкодера
    {
        driver.Enable(false);
        WriteEeprom(encoder.SetCalibAngle(sourse.GetTargetDeg() - encoder.GetCurrentDeg()));
    }
    else if(sourse.GetCommand() == EnumSourse::RESET) // Сброс калибровочного угла энкодера
    {
        driver.Enable(false);
        WriteEeprom(0);
    }
}