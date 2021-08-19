#pragma once
#ifndef _SERVO_
    #define _SERVO_
    #include <Arduino.h>
    #include "GyverPID.h"
    #include <GyverPWM.h>

    enum EnumSourse{DISABLE = 0, ENABLE = 1, CALIBRATE = 2, RESET = 3};
    class Source
    {
        public:
            Source(byte pinPwm, byte pinControl);
            void SetPinPwmMaxResolutionValue(int resolution);
            void SetPinEnResolution(int resolution);
            void Begin(bool useUart);
            void Listener();
            byte GetCommand();
            int GetTargetDeg();
        private:
            void Welcoming();
            byte ReadSerial(); // Чтение буфера UART
            void Buffer(byte ascii);
            void Backspace(byte ascii);
            byte NumericAsciiConvertor(byte ascii);
            char CharAsciiConvertor(byte ascii);
            bool CommandSearch(String comDir[]);
            bool IsNumeric();
            void Print(String dir);
            void Print(int numeric);
            void Print(char characteristic);
            bool _option, _sourse, _rotate, _calibrate, _uart;
            byte _pinPwm, _pinControl;
            int _pwmMaxResolution = 1023, _enResolution = 256, _maxAngle = 360, _targetDeg = _maxAngle / 2;
            String _buffer = "";
            #define DIR_SIZE 4
            String _dirOption[DIR_SIZE] = {"O", "o", "OPTIONS", "options"};
            String _dirSource[DIR_SIZE] = {"S", "s", "SOURCE", "source"};
            String _dirRotate[DIR_SIZE] = {"R", "r", "ROTATE", "rotate"};
            String _dirCalibrate[DIR_SIZE] = {"C", "c", "CALIBRATE", "calibrate"};
            String _dirBack[DIR_SIZE] = {"B", "b", "BACK", "back"};
            String _dirExit[DIR_SIZE] = {"E", "e", "EXIT", "exit"};
            String _dirPwm[DIR_SIZE] = {"P", "p", "PWM", "pwm"};
            String _dirUart[DIR_SIZE] = {"U", "u", "UART", "uart"};
            #define CONTROL_CHAR_NUL 0
            #define CONTROL_CHAR_SOH 1
            #define CONTROL_CHAR_STX 2
            #define CONTROL_CHAR_ETX 3
            #define CONTROL_CHAR_EOT 4
            #define CONTROL_CHAR_ENQ 5
            #define CONTROL_CHAR_ACK 6
            #define CONTROL_CHAR_BEL 7
            #define CONTROL_CHAR_BS 8
            #define CONTROL_CHAR_TAB 9
            #define CONTROL_CHAR_LF 10
            #define CONTROL_CHAR_VT 11
            #define CONTROL_CHAR_FF 12
            #define CONTROL_CHAR_CR 13
            #define CONTROL_CHAR_SO 14
            #define CONTROL_CHAR_SI 15
            #define CONTROL_CHAR_DLE 16
            #define CONTROL_CHAR_DC1 17
            #define CONTROL_CHAR_DC2 18
            #define CONTROL_CHAR_DC3 19
            #define CONTROL_CHAR_DC4 20
            #define CONTROL_CHAR_NAK 21
            #define CONTROL_CHAR_SYN 22
            #define CONTROL_CHAR_ETB 23
            #define CONTROL_CHAR_CAN 24
            #define CONTROL_CHAR_EM 25
            #define CONTROL_CHAR_SUB 26
            #define CONTROL_CHAR_ESC 27
            #define CONTROL_CHAR_FS 28
            #define CONTROL_CHAR_GS 29
            #define CONTROL_CHAR_RS 30
            #define CONTROL_CHAR_US 31
            #define CONTROL_CHAR_Delete 127
            #define SYMBOL_32 32
            #define SYMBOL_33 33
            #define SYMBOL_34 34
            #define SYMBOL_35 35
            #define SYMBOL_36 36
            #define SYMBOL_37 37
            #define SYMBOL_38 38
            #define SYMBOL_39 39
            #define SYMBOL_40 40
            #define SYMBOL_41 41
            #define SYMBOL_42 42
            #define SYMBOL_43 43
            #define SYMBOL_44 44
            #define SYMBOL_MINUS 45
            #define SYMBOL_46 46
            #define SYMBOL_47 47
            #define INT_ZERO 48
            #define INT_ONE 49
            #define INT_TWO 50
            #define INT_THREE 51
            #define INT_FOUR 52
            #define INT_FIVE 53
            #define INT_SIX 54
            #define INT_SEVEN 55
            #define INT_EIGHT 56
            #define INT_NINE 57
            #define SYMBOL_58 58
            #define SYMBOL_59 59
            #define SYMBOL_60 60
            #define SYMBOL_61 61
            #define SYMBOL_62 62
            #define SYMBOL_63 63
            #define SYMBOL_64 64
            #define CHAR_CAPITAL_A 65
            #define CHAR_CAPITAL_B 66
            #define CHAR_CAPITAL_C 67
            #define CHAR_CAPITAL_D 68
            #define CHAR_CAPITAL_E 69
            #define CHAR_CAPITAL_F 70
            #define CHAR_CAPITAL_G 71
            #define CHAR_CAPITAL_H 72
            #define CHAR_CAPITAL_I 73
            #define CHAR_CAPITAL_J 74
            #define CHAR_CAPITAL_K 75
            #define CHAR_CAPITAL_L 76
            #define CHAR_CAPITAL_M 77
            #define CHAR_CAPITAL_N 78
            #define CHAR_CAPITAL_O 79
            #define CHAR_CAPITAL_P 80
            #define CHAR_CAPITAL_Q 81
            #define CHAR_CAPITAL_R 82
            #define CHAR_CAPITAL_S 83
            #define CHAR_CAPITAL_T 84
            #define CHAR_CAPITAL_U 85
            #define CHAR_CAPITAL_V 86
            #define CHAR_CAPITAL_W 87
            #define CHAR_CAPITAL_X 88
            #define CHAR_CAPITAL_Y 89
            #define CHAR_CAPITAL_Z 90
            #define SYMBOL_91 91
            #define SYMBOL_92 92
            #define SYMBOL_93 93
            #define SYMBOL_94 94
            #define SYMBOL_95 95
            #define SYMBOL_96 96
            #define CHAR_CURSIVE_A 97
            #define CHAR_CURSIVE_B 98
            #define CHAR_CURSIVE_C 99
            #define CHAR_CURSIVE_D 100
            #define CHAR_CURSIVE_E 101
            #define CHAR_CURSIVE_F 102
            #define CHAR_CURSIVE_G 103
            #define CHAR_CURSIVE_H 104
            #define CHAR_CURSIVE_I 105
            #define CHAR_CURSIVE_J 106
            #define CHAR_CURSIVE_K 107
            #define CHAR_CURSIVE_L 108
            #define CHAR_CURSIVE_M 109
            #define CHAR_CURSIVE_N 110
            #define CHAR_CURSIVE_O 111
            #define CHAR_CURSIVE_P 112
            #define CHAR_CURSIVE_Q 113
            #define CHAR_CURSIVE_R 114
            #define CHAR_CURSIVE_S 115
            #define CHAR_CURSIVE_T 116
            #define CHAR_CURSIVE_U 117
            #define CHAR_CURSIVE_V 118
            #define CHAR_CURSIVE_W 119
            #define CHAR_CURSIVE_X 120
            #define CHAR_CURSIVE_Y 121
            #define CHAR_CURSIVE_Z 122
            #define SYMBOL_123 123
            #define SYMBOL_124 124
            #define SYMBOL_125 125
            #define SYMBOL_126 126
    };
    class Encoder
        {
            public:
                Encoder(byte pinPwm, int pwmResolution);
                short GetCalibAngle();
                short SetCalibAngle(short delta);
                short GetCurrentDeg();
            private:
                byte _pinPwm;
                short _calibAngle = 0;
                int _pwmMaxResolution;
                float _degrees;
        };
    class MotorDriver
    {
        public:
            MotorDriver(byte pinEn, byte pinFw, byte pinBack, byte pinPwm);
            void Enable(bool en);
            bool Direction(bool rightDir);
            byte GetPwmPin(){return _pinPwm;}
        private:
            byte _pinEn, _pinFw, _pinBack, _pinPwm;
            int _lastDeg;
            float _kp, _ki, _kd;
    };
#endif