#include "main.h"

#define DEBUG
#define PWM_RESOLUTION 1023

Source::Source(byte pinPwm, byte pinEn)
{
    _pinPwm = pinPwm;
    _pinEn = pinEn;
    pinMode(_pinPwm, INPUT);
    pinMode(_pinEn, INPUT);
}
void Source::SetPwmResolution(int resolution)
{
    _pwmResolution = resolution;
}
void Source::Begin(bool useUart)
{
    _uart = useUart;
    if (_uart)
    {
        Welcoming();
    }
}
void Source::Listener()
{
    if (_uart)
    {
        Buffer(ReadSerial());
    }
}
byte Source::GetEnable()
{
    byte value = analogRead(_pinEn);
    int quarter = _pwmResolution / 4;
    if (value < quarter) //  Выключить драйвер сервопривода
    {
        return 0;
    }
    else if( value > _pwmResolution - quarter) // Включить драйвер сервопривода
    {
        return 1;
    }
    return 2; // Активировать режим калибровки энкодера
}
void Source::Welcoming()
{
    Serial.println();
    Serial.println("Terminal is run");
    Serial.println("__________________");
    Serial.println("ENTER THE COMMAND:");
    Serial.print(">");
}
int Source::GetTargetDeg()
{
    if (_uart)
    {
        return _targetDeg;
    }
    return analogRead(_pinPwm);
}
byte Source::ReadSerial()
{
    return (Serial.available() > 0) ? Serial.read() : 0;
}
void Source::Buffer(byte ascii)
{
    if (ascii != CONTROL_CHAR_NUL) // Полученное значение не NUL
    {
        if (ascii <= CONTROL_CHAR_US || ascii == CONTROL_CHAR_Delete) // Диаппазон управляющих символов
        {
            if (ascii == CONTROL_CHAR_BS) // Bacspace
            {
                Backspace(ascii);
            }
            else if (ascii == CONTROL_CHAR_CR) // Enter
            {
                if (!_option && CommandSearch(_dirOption)) // Меню опций
                {
                    _option = true;
                    Print("OPTIONS");
                    goto end;
                }
                if (_option && CommandSearch(_dirSource)) // Меню выбора источника
                {
                    _sourse = true;
                    Print("SOURSE");
                    goto end;
                }
                if (_option && CommandSearch(_dirRotate)) // Меню выбора угла поворота
                {
                    _rotate = true;
                    Print("ROTATE");
                    goto end;
                }
                if (_option && CommandSearch(_dirCalibrate)) // Калибровка среднего положения
                {
                    _calibrate = true;
                    Print("CALIBRATE");
                    goto end;
                }
                if (_option && !_sourse && !_rotate && CommandSearch(_dirBack)) // Терминал
                {
                    _option = false;
                    Print("TERMINAL");
                    goto end;
                }
                if (_sourse && CommandSearch(_dirBack)) // Меню опций
                {
                    _sourse = false;
                    Print("OPTIONS");
                    goto end;
                }
                if (_rotate && CommandSearch(_dirBack)) // Меню опций
                {
                    _rotate = false;
                    Print("OPTIONS");
                    goto end;
                }
                if (_calibrate && CommandSearch(_dirBack)) // Меню опций
                {
                    _calibrate = false;
                    Print("OPTIONS");
                    goto end;
                }
                if (CommandSearch(_dirExit)) // Терминал
                {
                    _option = false;
                    _sourse = false;
                    _rotate = false;
                    _calibrate = false;
                    Print("TERMINAL");
                    goto end;
                }
                if (_sourse && CommandSearch(_dirPwm)) // Источник PWM
                {
                    _uart = false;
                    Print("SOURSE");
                    goto end;
                }
                if (_sourse && CommandSearch(_dirUart)) // Источник UART
                {
                    _uart = true;
                    Print("SOURSE");
                    goto end;
                }
                if (_rotate && IsNumeric()) // Диаппазон поворота сервопривода
                {
                    _maxAngle = _buffer.toInt();
                    _targetDeg = _maxAngle / 2; // Перекалибровка положения при изменении диаппазона
                    Print("ROTATE");
                    goto end;
                }
                if (_uart && IsNumeric()) // Указание угла для выполнения поворота
                {
                    _targetDeg = _buffer.toInt();
                    _targetDeg = (_targetDeg > _maxAngle) ? _maxAngle : _targetDeg;
                    Print("MOVE TO");
                    Print("TERMINAL");
                    goto end;
                }
                Print("UNKNOWN COMMAND");
                end:
                _buffer = "";
            }
        }
        else if (ascii >= INT_ZERO && ascii <= INT_NINE) // Диаппазон чисел
        {
            byte numeric = NumericAsciiConvertor(ascii);
            _buffer += numeric;
            Print(numeric);
        }
        else // Диаппазон символов
        {
            char characteristic = CharAsciiConvertor(ascii);
            _buffer += characteristic;
            Print(characteristic);
        }
    }
}
void Source::Backspace(byte ascii)
{
    _buffer.remove(_buffer.length() - 1);
    Serial.println();
    Serial.print(_buffer);
}
byte Source::NumericAsciiConvertor(byte ascii)
{
    switch (ascii)
    {
        case INT_ZERO: return 0;
        case INT_ONE: return 1;
        case INT_TWO: return 2;
        case INT_THREE: return 3;
        case INT_FOUR: return 4;
        case INT_FIVE: return 5;
        case INT_SIX: return 6;
        case INT_SEVEN: return 7;
        case INT_EIGHT: return 8;
        case INT_NINE: return 9;
        default: return 0;
    }
}
char Source::CharAsciiConvertor(byte ascii)
{
    switch (ascii)
    {
        case SYMBOL_32: return 32;
        case SYMBOL_33: return 33;
        case SYMBOL_34: return 34;
        case SYMBOL_35: return 35;
        case SYMBOL_36: return 36;
        case SYMBOL_37: return 37;
        case SYMBOL_38: return 38;
        case SYMBOL_39: return 39;
        case SYMBOL_40: return 40;
        case SYMBOL_41: return 41;
        case SYMBOL_42: return 42;
        case SYMBOL_43: return 43;
        case SYMBOL_44: return 44;
        case SYMBOL_MINUS: return 45;
        case SYMBOL_46: return 46;
        case SYMBOL_47: return 47;
        case SYMBOL_58: return 58;
        case SYMBOL_59: return 59;
        case SYMBOL_60: return 60;
        case SYMBOL_61: return 61;
        case SYMBOL_62: return 62;
        case SYMBOL_63: return 63;
        case SYMBOL_64: return 64;
        case CHAR_CAPITAL_A: return 65;
        case CHAR_CAPITAL_B: return 66;
        case CHAR_CAPITAL_C: return 67;
        case CHAR_CAPITAL_D: return 68;
        case CHAR_CAPITAL_E: return 69;
        case CHAR_CAPITAL_F: return 70;
        case CHAR_CAPITAL_G: return 71;
        case CHAR_CAPITAL_H: return 72;
        case CHAR_CAPITAL_I: return 73;
        case CHAR_CAPITAL_J: return 74;
        case CHAR_CAPITAL_K: return 75;
        case CHAR_CAPITAL_L: return 76;
        case CHAR_CAPITAL_M: return 77;
        case CHAR_CAPITAL_N: return 78;
        case CHAR_CAPITAL_O: return 79;
        case CHAR_CAPITAL_P: return 80;
        case CHAR_CAPITAL_Q: return 81;
        case CHAR_CAPITAL_R: return 82;
        case CHAR_CAPITAL_S: return 83;
        case CHAR_CAPITAL_T: return 84;
        case CHAR_CAPITAL_U: return 85;
        case CHAR_CAPITAL_V: return 86;
        case CHAR_CAPITAL_W: return 87;
        case CHAR_CAPITAL_X: return 88;
        case CHAR_CAPITAL_Y: return 89;
        case CHAR_CAPITAL_Z: return 90;
        case SYMBOL_91: return 91;
        case SYMBOL_92: return 92;
        case SYMBOL_93: return 93;
        case SYMBOL_94: return 94;
        case SYMBOL_95: return 95;
        case SYMBOL_96: return 96;
        case CHAR_CURSIVE_A: return 97;
        case CHAR_CURSIVE_B: return 98;
        case CHAR_CURSIVE_C: return 99;
        case CHAR_CURSIVE_D: return 100;
        case CHAR_CURSIVE_E: return 101;
        case CHAR_CURSIVE_F: return 102;
        case CHAR_CURSIVE_G: return 103;
        case CHAR_CURSIVE_H: return 104;
        case CHAR_CURSIVE_I: return 105;
        case CHAR_CURSIVE_J: return 106;
        case CHAR_CURSIVE_K: return 107;
        case CHAR_CURSIVE_L: return 108;
        case CHAR_CURSIVE_M: return 109;
        case CHAR_CURSIVE_N: return 110;
        case CHAR_CURSIVE_O: return 111;
        case CHAR_CURSIVE_P: return 112;
        case CHAR_CURSIVE_Q: return 113;
        case CHAR_CURSIVE_R: return 114;
        case CHAR_CURSIVE_S: return 115;
        case CHAR_CURSIVE_T: return 116;
        case CHAR_CURSIVE_U: return 117;
        case CHAR_CURSIVE_V: return 118;
        case CHAR_CURSIVE_W: return 119;
        case CHAR_CURSIVE_X: return 120;
        case CHAR_CURSIVE_Y: return 121;
        case CHAR_CURSIVE_Z: return 122;
        case SYMBOL_123: return 123;
        case SYMBOL_124: return 124;
        case SYMBOL_125: return 125;
        case SYMBOL_126: return 126;
        default: return 0;
    }
}
bool Source::CommandSearch(String comDir[])
{
    for (size_t i = 0; i < DIR_SIZE; i++)
    {
        if (_buffer == comDir[i])
        {
            return true;
        }
    }
    return false;
}
bool Source::IsNumeric()
{
    bool isNumeric = true;
    size_t len = _buffer.length();
    for (size_t i = 0; i < len; i++)
    {
        byte ascii = _buffer[i];
        isNumeric = (ascii >= INT_ZERO && ascii <= INT_NINE) ? true : false;
        if (!isNumeric)
        {
            return false;
        }
    }
    return true;
}
void Source::Print(String dir)
{
    Serial.println();
    if (dir == "OPTIONS")
    {
        Serial.println(" _____________________________________________");
        Serial.println("|                                             |");
        Serial.println("|       PLEASE SELECT SUITABLE OPTIONS:       |");
        Serial.println("|_____________________________________________|");
        Serial.println();
        Serial.println(" - source");
        Serial.println(" - rotate");
        Serial.println(" - back");
        Serial.println(" - exit");
        Serial.print(">");
    }
    if (dir == "SOURSE")
    {
        Serial.println(" _____________________________________________");
        Serial.println("|                                             |");
        Serial.println("|             SELECT INPUT SOURSE:            |");
        Serial.println("|_____________________________________________|");
        Serial.print(" Current sourse: ");
        Serial.println((_uart) ? "UART" : "PWM");
        Serial.println();
        Serial.println(" - pwm");
        Serial.println(" - uart");
        Serial.println(" - back");
        Serial.println(" - exit");
        Serial.print(">");
    }
    if (dir == "ROTATE")
    {
        Serial.println(" _____________________________________________");
        Serial.println("|                                             |");
        Serial.println("|          ENTER MAX ROTATION ENGLE:          |");
        Serial.println("|_____________________________________________|");
        Serial.println(" Current value: " + String(_maxAngle, DEC) + " (if 0 then infiniti)");
        Serial.println();
        Serial.println(" - back");
        Serial.println(" - exit");
        Serial.print(">");
    }
    if (dir == "CALIBRATE")
    {
        Serial.println(" _____________________________________________");
        Serial.println("|                                             |");
        Serial.println("|            ENTER TARGET DEGREES:            |");
        Serial.println("|_____________________________________________|");
        Serial.println(" Current degrees: " + String(_maxAngle, DEC));
        Serial.println();
        Serial.println(" - back");
        Serial.println(" - exit");
        Serial.print(">");
    }
    if (dir == "TERMINAL")
    {
        Serial.println("__________________");
        Serial.println("ENTER THE COMMAND:");
        Serial.print(">");
    }
    if (dir == "MOVE TO")
    {
        Serial.print("MOVE TO: " + String(_targetDeg, DEC));
    }
    if (dir == "UNKNOWN COMMAND")
    {
        Serial.println("::::::::::::::::::::::::");
        Serial.println(":::UNKNOWN COMMAND!!!:::");
        Serial.println("::::::::::::::::::::::::");
        Serial.println("__________________");
        Serial.println("ENTER THE COMMAND:");
        Serial.print(">");
    }
}
void Source::Print(int numeric)
{
    Serial.print(numeric);
}
void Source::Print(char characteristic)
{
    Serial.print(characteristic);
}

Encoder::Encoder(byte pinPwm, int pwmResolution)
{
    _pinPwm = pinPwm;
    _pwmResolution = pwmResolution;
    _degrees = (_pwmResolution + 1.00) / 360;
    pinMode(_pinPwm, INPUT);
}
short Encoder::GetCalibAngle()
{
    return _calibAngle;
}
void Encoder::SetCalibAngle(short delta)
{
    _calibAngle = delta;
}
short Encoder::GetCurrentDeg()
{
    int a = 127;
    float base = 0.00;
    for (int i = 0; i < a; i++)
    {
        base += map(analogRead(_pinPwm), 0, _pwmResolution, 0, 359);
    }
    base = base / a;
    short sum = base + _calibAngle;
    if (sum > 360)
    {
        return sum - 360;
    }
    return sum;
}

Servo::Servo(byte pinEn, byte pinFw, byte pinBack, byte pinPwm)
{
    _pinEn = pinEn;
    _pinFw = pinFw;
    _pinBack = pinBack;
    _pinPwm = pinPwm;
    pinMode(_pinEn, OUTPUT);
    pinMode(_pinFw, OUTPUT);
    pinMode(_pinBack, OUTPUT);
    pinMode(_pinPwm, OUTPUT);
}
void Servo::Enable(bool en)
{
    digitalWrite(_pinEn, en);
}
void Servo::Listener(Source &sourse, Encoder &encoder, GyverPID &pid)
{
    if (sourse.GetEnable() == 1) // Работа сервопривода разрешена из внешнего источника или из терминала
    {
        pid.setpoint = sourse.GetTargetDeg();
        pid.input = encoder.GetCurrentDeg();
        Serial.println(pid.input);
        if (pid.setpoint > pid.input)
        {
            Enable(true);
            pid.Ki = 0.5;
            if (pid.setpoint - pid.input <= 2)
            {
                pid.Kp = 1.0;
                pid.setLimits(20, 300);
            }
            else
            {
                pid.Kp = 1.8;
                pid.setLimits(20, PWM_RESOLUTION);
            }
            pid.setDirection(!Direction(true));
        }
        else if (pid.setpoint < pid.input)
        {
            pid.Ki = 0.5;
            Enable(true);
            if (pid.input - pid.setpoint <= 2)
            {
                pid.Kp = 1.0;
                pid.setLimits(20, 300);
            }
            else
            {
                pid.Kp = 1.8;
                pid.setLimits(20, PWM_RESOLUTION);
            }
            pid.setDirection(!Direction(false));
        }
        else
        {
            pid.Kp = 0;
            pid.Ki = 0;
            pid.setLimits(0, 0);
        }
        pid.getResultTimer();
        PWM_set(_pinPwm, pid.output);
    }
    else // Работа сервопривода запрещена
    {
        Enable(false);
        if (sourse.GetEnable() == 2) // Калибровка энкодера
        {
            encoder.SetCalibAngle(sourse.GetTargetDeg() - encoder.GetCurrentDeg());
        }
    }
}
bool Servo::Direction(bool rightDir)
{
    digitalWrite(_pinFw, rightDir);
    digitalWrite(_pinBack, !rightDir);
    return rightDir;
}

Source Uart(A1, A2);
Encoder MyEncoder(A0, PWM_RESOLUTION);
Servo MyServo(6, 5, 4, 9);
GyverPID MyPid(0, 0, 0, 1);

void setup()
{
    Serial.begin(100000);
    PWM_resolution(9, 9, FAST_PWM);
    Uart.Begin(true);
    Uart.SetPwmResolution(256);
    MyServo.Enable(true);
    MyPid.setMode(false);
}

void loop()
{
    Uart.Listener();
    MyServo.Listener(Uart, MyEncoder, MyPid);
}