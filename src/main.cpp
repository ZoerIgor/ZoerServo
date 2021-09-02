#include "servo.h"
#include "EEPROM.h"

//#define DEBUG

#define SERIAL_SPEED 100000

#define EEPROM_NULL 32769
#define EEPROM_ADDRESS 0

#define SOURSE_PIN_ADC A1
#define SOURSE_PIN_ENABLE 12
#define SOURSE_PIN_CALIBRATE 11
#define SOURSE_PIN_RESET 10
#define SOURSE_REGISTER_PIN_ENABLE ((PINB >> 4) & 1) == 1
#define SOURSE_REGISTER_PIN_CALIBRATE ((PINB >> 3) & 1) == 1
#define SOURSE_REGISTER_PIN_RESET ((PINB >> 2) & 1) == 1

#define ENCODER_PIN_ADC A0
#define ENCODER_ADC_RESOLUTION 1024

#define MOTOR_DRIVER_PIN_ENABLE 6
#define MOTOR_DRIVER_PIN_FORWARD 5
#define MOTOR_DRIVER_PIN_BACK 4
#define MOTOR_ENABLE_HIGH PORTD |= (1 << 6)
#define MOTOR_ENABLE_LOW PORTD &= ~(1 << 6)
#define MOTOR_FORWARD_HIGH PORTD |= (1 << 5)
#define MOTOR_FORWARD_LOW PORTD &= ~(1 << 5)
#define MOTOR_BACK_HIGH PORTD |= (1 << 4)
#define MOTOR_BACK_LOW PORTD &= ~(1 << 4)
#define MOTOR_DRIVER_PIN_PWM 9

#define GYVER_PID_BIT_RESOLUTION 9
#define GYVER_PID_CALCULATE_FREQUENCY 1
#define GYVER_PID_MAX_PWM_RESOLUTION 511

Source sourse(SOURSE_PIN_ADC, SOURSE_PIN_ENABLE, SOURSE_PIN_CALIBRATE, SOURSE_PIN_RESET);
Encoder encoder(ENCODER_PIN_ADC, ENCODER_ADC_RESOLUTION);
MotorDriver driver(MOTOR_DRIVER_PIN_ENABLE, MOTOR_DRIVER_PIN_FORWARD, MOTOR_DRIVER_PIN_BACK, MOTOR_DRIVER_PIN_PWM);
GyverPID pid;

uint8_t analog_ref = DEFAULT;
volatile int analogPin = ENCODER_PIN_ADC;
volatile short adcEncoder;
volatile short adcSourse;

short ReadEeprom()
{
    short value = 0;
    EEPROM.get(EEPROM_ADDRESS, value);
    return value - EEPROM_NULL;
}

void WriteEeprom(short value)
{
    #ifdef DEBUG
        Serial.println("WRITE");
    #else
        EEPROM.put(EEPROM_ADDRESS, value + EEPROM_NULL);
    #endif
}

void ADC_init()
{
    ADCSRA = 0;
    ADCSRB = 0;
    ADMUX |= (1 << REFS0);
    analog_ref = ADMUX;
    ADMUX |= (0 & 0x07);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS0);
    ADCSRA &= ~ (1 << ADPS1);
    ADCSRA |= (1 << ADATE);
    ADCSRA |= (1 << ADIE);
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect)
{
    if (analogPin == ENCODER_PIN_ADC) // Обратная последовательность из-за задержки при считывании ADC
    {
        sourse._adcValue = ADCL | (ADCH << 8);
    }
    else
    {
        encoder._adcValue = ADCL | (ADCH << 8);
    }
    analogPin = (analogPin == ENCODER_PIN_ADC) ? SOURSE_PIN_ADC : ENCODER_PIN_ADC;
    ADMUX = analog_ref | ((analogPin - 14) & 0x07);
}

void setup()
{
    Serial.begin(SERIAL_SPEED);
    ADC_init();
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
        if (sourse.GetEnable(SOURSE_REGISTER_PIN_ENABLE)) // Работа сервопривода разрешена из внешнего источника или из терминала
        {
            pid.setpoint = sourse.GetTargetDeg();
            pid.input = encoder.GetCurrentDeg();
            MOTOR_ENABLE_HIGH;
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
                MOTOR_FORWARD_HIGH;
                MOTOR_BACK_LOW;
                pid.setDirection(false);
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
                MOTOR_FORWARD_LOW;
                MOTOR_BACK_HIGH;
                pid.setDirection(true);
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
            MOTOR_ENABLE_LOW;
            if(sourse.GetCalibrate(SOURSE_REGISTER_PIN_CALIBRATE)) // Калибровка энкодера
            {
                WriteEeprom(encoder.SetCalibAngle(180 - encoder.GetBaseDeg()));
            }
            if(sourse.GetReset(SOURSE_REGISTER_PIN_RESET)) // Сброс калибровочного угла энкодера
            {
                WriteEeprom(0);
                encoder.SetCalibAngle(0);
            }
        }
        sourse.CalibrateUnlock(SOURSE_REGISTER_PIN_CALIBRATE); // Разблокировка команды
        sourse.ResetUnlock(SOURSE_REGISTER_PIN_RESET); // Разблокировка команды
    }
}