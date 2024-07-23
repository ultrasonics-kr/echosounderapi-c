// Copyright (c) EofE Ultrasonics Co., Ltd., 2024
#include <cstdio>
#include <cstdarg>
#include <string>

#include "Echosounder.h"
#include "DualEchosounder.h"
#include "SingleEchosounder.h"
#include "EchosounderCWrapper.h"
#include "serial/serial.h"

#if defined(_MSC_VER) && _MSC_VER < 1900

#define snprintf c99_snprintf
#define vsnprintf c99_vsnprintf

__inline int c99_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

__inline int c99_snprintf(char *outBuf, size_t size, const char *format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(outBuf, size, format, ap);
    va_end(ap);

    return count;
}

#endif

pSnrCtx SingleEchosounderOpen(const char *portpath, uint32_t baudrate)
{
    pSnrCtx ctx = nullptr;

    try
    {
        std::shared_ptr<serial::Serial> serialPort(new serial::Serial(portpath, baudrate, serial::Timeout::simpleTimeout(SERIALPORT_TIMEOUT_MS)));
        ctx = reinterpret_cast<pSnrCtx>(new SingleEchosounder(serialPort));
    }
    catch(...)
    {
        // In case of any exception this function returns nullptr
    }

    return ctx;
}

pSnrCtx DualEchosounderOpen(const char* portpath, uint32_t baudrate)
{
    pSnrCtx ctx = nullptr;

    try
    {
        std::shared_ptr<serial::Serial> serialPort(new serial::Serial(portpath, baudrate, serial::Timeout::simpleTimeout(SERIALPORT_TIMEOUT_MS)));
        ctx = reinterpret_cast<pSnrCtx>(new DualEchosounder(serialPort));
    }
    catch (...)
    {
        // In case of any exception this function returns nullptr
    }

    return ctx;
}

void EchosounderClose(pSnrCtx snrctx)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    delete ss;
    snrctx = nullptr;
}

size_t EchosounderReadData(pSnrCtx snrctx, uint8_t *buffer, size_t size)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    return ss->GetSerialPort()->read(buffer, size);
}

long EchosounderValueToLong(pcEchosounderValue value)
{
    return std::stol(value->value_text);
}

float EchosounderValueToFloat(pcEchosounderValue value)
{
    return std::stof(value->value_text);
}

const char *EchosounderValueToText(pcEchosounderValue value)
{
    return value->value_text;
}

bool IsValidEchosounderValue(pcEchosounderValue value)
{
    return !((0 == value->value_len) || (0 == strlen(value->value_text)));
}

void LongToEchosounderValue(long num, pEchosounderValue value)
{
    value->value_len = snprintf(value->value_text, sizeof(value->value_text), "%ld", num);
}

void FloatToEchosounderValue(float num, pEchosounderValue value)
{
    value->value_len = snprintf(value->value_text, sizeof(value->value_text), "%f", num);
}

int EchosounderGetValue(pSnrCtx snrctx, EchosounderCommandIds_t command, pEchosounderValue value)
{
    int result = -1;

    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    std::string ssvalue = ss->GetValue(command);

    (void)snprintf(value->value_text, sizeof(value->value_text), "%s", ssvalue.c_str());
    value->value_len = (int)ssvalue.length();

    return (ssvalue.length() > 0) ? 0 : -1;
}

int EchosounderSetValue(pSnrCtx snrctx, EchosounderCommandIds_t command, pcEchosounderValue value)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    bool result = ss->SetValue(command, value->value_text);

    return (false != result) ? 0 : -1;
}

bool EchosounderDetect(pSnrCtx snrctx)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    return ss->Detect();
}

void EchosounderStart(pSnrCtx snrctx)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    ss->Start();
}

void EchosounderStop(pSnrCtx snrctx)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    ss->Stop();
}

bool EchosounderIsRunning(pSnrCtx snrctx)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    return ss->IsRunning();
}

bool EchosounderIsDetected(pSnrCtx snrctx)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    return ss->IsDetected();
}

void EchosounderSetCurrentTime(pSnrCtx snrctx)
{
    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    ss->SetCurrentTime();
}
