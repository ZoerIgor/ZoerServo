#include "servo.h"
#include "EEPROM.h"

#define DEBUG1

#define SERIAL_SPEED 100000
#define EEPROM_NULL 32769
#define EEPROM_ADDRESS 0

#define SOURSE_PIN_PWM A1
#define SOURSE_PIN_ENABLE 12
#define SOURSE_PIN_CALIBRATE 11
#define SOURSE_PIN_RESET 10

#define ENCODER_PIN_INPUT A0
#define ENCODER_PWM_RESOLUTION 1024

#define MOTOR_DRIVER_PIN_ENABLE 6
#define MOTOR_DRIVER_PIN_FORWARD 5
#define MOTOR_DRIVER_PIN_BACK 4
#define MOTOR_DRIVER_PIN_PWM 9

#define GYVER_PID_BIT_RESOLUTION 9
#define GYVER_PID_CALCULATE_FREQUENCY 1
#define GYVER_PID_MAX_PWM_RESOLUTION 511

Source sourse(SOURSE_PIN_PWM, SOURSE_PIN_ENABLE, SOURSE_PIN_CALIBRATE, SOURSE_PIN_RESET);
Encoder encoder(ENCODER_PIN_INPUT, ENCODER_PWM_RESOLUTION);
MotorDriver driver(MOTOR_DRIVER_PIN_ENABLE, MOTOR_DRIVER_PIN_FORWARD, MOTOR_DRIVER_PIN_BACK, MOTOR_DRIVER_PIN_PWM);
GyverPID pid;

short ReadEeprom()
{
    short value = 0;
    EEPROM.get(EEPROM_ADDRESS, value);
    return value - EEPROM_NULL;
}

void WriteEeprom(short value)
{
    EEPROM.put(EEPROM_ADDRESS, value + EEPROM_NULL);
}

void setup()
{
    Serial.begin(SERIAL_SPEED);
    EEPROM.begin();
    encoder.SetCalibAngle(ReadEeprom());
    //sourse.Begin(true);
    PWM_resolution(MOTOR_DRIVER_PIN_PWM, GYVER_PID_BIT_RESOLUTION, FAST_PWM);
    pid.setDt(GYVER_PID_CALCULATE_FREQUENCY);
}

void loop()
{
    while (1)
    {   
        //sourse.Listener();
        if (sourse.GetEnable()) // Работа сервопривода разрешена из внешнего источника или из терминала
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
        else // Работа сервопривода запрещена
        {
            driver.Enable(false);
            if(sourse.GetCalibrate()) // Калибровка энкодера
            {
                WriteEeprom(encoder.SetCalibAngle(180 - encoder.GetBaseDeg()));
            }
            if(sourse.GetReset()) // Сброс калибровочного угла энкодера
            {
                WriteEeprom(0);
                encoder.SetCalibAngle(0);
            }
        }
        sourse.GetCalibrate(); // Разблокировка команды
        sourse.GetReset(); // Разблокировка команды
    }
}