#include <cstdio>

#include "Echosounder.h"
#include "DualEchosounder.h"
#include "SingleEchosounder.h"
#include "EchosounderCWrapper.h"
#include "serial/serial.h"

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
    return strtol(value->value_text, NULL, 10);
}

float EchosounderValueToFloat(pcEchosounderValue value)
{
    return strtof(value->value_text, NULL);
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
    value->value_len = std::snprintf(value->value_text, sizeof(value->value_text), "%ld", num);
}

void FloatToEchosounderValue(float num, pEchosounderValue value)
{
    value->value_len = std::snprintf(value->value_text, sizeof(value->value_text), "%f", num);
}

int EchosounderGetValue(pSnrCtx snrctx, EchosounderCommandIds_t command, pEchosounderValue value)
{
    int result = -1;

    auto ss = reinterpret_cast<Echosounder*>(snrctx);
    std::string ssvalue = ss->GetValue(command);

    (void)std::snprintf(value->value_text, sizeof(value->value_text), "%s", ssvalue.c_str());
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
