#include "servo.h"

Source::Source(byte pinPwm, byte pinEnable, byte pinCalibrate, byte pinReset)
{
    _pinPwm = pinPwm;
    _pinEnable = pinEnable;
    _pinCalibrate = pinCalibrate;
    _pinReset = pinReset;
    pinMode(_pinPwm, INPUT);
    pinMode(_pinEnable, INPUT);
    pinMode(_pinCalibrate, INPUT);
    pinMode(_pinReset, INPUT);
}
void Source::SetPinPwmMaxResolutionValue(int resolution)
{
    _pwmMaxResolution = resolution;
}
void Source::SetPinEnResolution(int resolution)
{
    _enResolution = resolution;
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
bool Source::GetEnable()
{
    return (_uart) ? _enable : digitalRead(_pinEnable);
}
bool Source::GetCalibrate()
{
    bool sig = (_uart) ? _calibrate : digitalRead(_pinCalibrate);
    if (sig)
    {
        if (sig == _prevCalib)
        {
            return false;
        }
        else
        {
            _prevCalib = true;
            return true;
        }
    }
    else
    {
        if (_prevCalib)
        {
            _prevCalib = false;
        }
        return false;
    }
}
bool Source::GetReset()
{
    bool sig = (_uart) ? _reset : digitalRead(_pinReset);
    if (sig)
    {
        if (sig == _prevReset)
        {
            return false;
        }
        else
        {
            _prevReset = true;
            return true;
        }
    }
    else
    {
        if (_prevReset)
        {
            _prevReset = false;
        }
        return false;
    }
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
    int a = 127;
    float base = 0.00;
    for (int i = 0; i < a; i++)
    {
        base += map(analogRead(_pinPwm), 0, 1023, 1, 360);
    }
    return base / a;
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
                if (_option && CommandSearch(_dirCalibrate)) // Калибровка среднего положения
                {
                    _enable = false;
                    _calibrate = true;
                    Print("CALIBRATE");
                    Print("OPTIONS");
                    goto end;
                }
                if (_option && CommandSearch(_dirReset)) // Сброс калибровки среднего положения
                {
                    _enable = false;
                    _reset = true;
                    Print("RESET");
                    Print("OPTIONS");
                    goto end;
                }
                if (_option && _sourse && CommandSearch(_dirBack)) // Меню опций
                {
                    _sourse = false;
                    Print("OPTIONS");
                    goto end;
                }
                if (_option && CommandSearch(_dirBack)) // Терминал
                {
                    _option = false;
                    if (_calibrate)
                    {
                        _enable = true;
                        _calibrate = false;
                    }
                    if (_reset)
                    {
                        _enable = true;
                        _reset = false;
                    }
                    Print("TERMINAL");
                    goto end;
                }
                if (CommandSearch(_dirExit)) // Терминал
                {
                    _option = false;
                    _sourse = false;
                    if (_calibrate)
                    {
                        _enable = true;
                        _calibrate = false;
                    }
                    if (_reset)
                    {
                        _enable = true;
                        _reset = false;
                    }
                    Print("TERMINAL");
                    goto end;
                }
                if (_sourse && CommandSearch(_dirPwm)) // Источник PWM
                {
                    _uart = false;
                    Print("END");
                    goto end;
                }
                if (_sourse && CommandSearch(_dirUart)) // Источник UART
                {
                    _uart = true;
                    Print("SOURSE");
                    goto end;
                }
                if (_sourse && CommandSearch(_dirDisable)) // Отключить источник
                {
                    _enable = !_enable;
                    Print("SOURSE");
                    goto end;
                }
                if (_uart && IsNumeric()) // Указание угла для выполнения поворота
                {
                    _targetDeg = _buffer.toInt();
                    _targetDeg = (_targetDeg > 360) ? 360 : _targetDeg;
                    _targetDeg = (_targetDeg <= 0) ? 1 : _targetDeg;
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
        Serial.println(" - calibrate");
        Serial.println(" - reset");
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
        Serial.print(" Power:\t\t");
        Serial.println((_enable) ? "ON" : "OFF");
        Serial.println();
        Serial.println(" - pwm");
        Serial.println(" - uart");
        Serial.println(" - disable");
        Serial.println(" - back");
        Serial.println(" - exit");
        Serial.print(">");
    }
    if (dir == "CALIBRATE")
    {
        Serial.println(" _____________________________________________");
        Serial.println("|                                             |");
        Serial.println("|               CALIBTARE ANGLE               |");
        Serial.println("|_____________________________________________|");
    }
    if (dir == "RESET")
    {
        Serial.println(" _____________________________________________");
        Serial.println("|                                             |");
        Serial.println("|           RESET CALIBTARE ANGLE             |");
        Serial.println("|_____________________________________________|");
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
    if (dir == "END")
    {
        Serial.println(" _____________________________________________");
        Serial.println("|                                             |");
        Serial.println("|             TERMINAL IS CLOSED              |");
        Serial.println("|_____________________________________________|");
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
    _pwmMaxResolution = pwmResolution;
    _degrees = _pwmMaxResolution / 360;
    pinMode(_pinPwm, INPUT);
}
short Encoder::GetCalibAngle()
{
    return _calibAngle;
}
short Encoder::SetCalibAngle(short delta)
{
    _calibAngle = delta;
    return _calibAngle;
}
short Encoder::GetCurrentDeg()
{
    float base = 0.00;
    for (int i = 0; i < _measureCount; i++)
    {
        base += map(analogRead(_pinPwm), 0, _pwmMaxResolution, 1, 360);
    }
    base = base / _measureCount;
    short sum = base + _calibAngle;
    if (sum > 360)
    {
        return sum - 360;
    }
    else if (sum <= 0)
    {
        return 360 + sum;
    }
    return sum;
}
short Encoder::GetBaseDeg()
{
    float base = 0.00;
    for (int i = 0; i < _measureCount; i++)
    {
        base += map(analogRead(_pinPwm), 0, _pwmMaxResolution, 1, 360);
    }
    base = base / _measureCount;
    return base;
}

MotorDriver::MotorDriver(byte pinEn, byte pinFw, byte pinBack, byte pinPwm)
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
void MotorDriver::Enable(bool en)
{
    digitalWrite(_pinEn, en);
}
bool MotorDriver::Direction(bool rightDir)
{
    digitalWrite(_pinFw, rightDir);
    digitalWrite(_pinBack, !rightDir);
    return rightDir;
}