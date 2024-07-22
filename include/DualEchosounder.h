#if !defined(DUALECHOSOUNDER_H)
#define DUALECHOSOUNDER_H

#include "ISonar.h"
#include <cstdint>
#include <thread>
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <map>

#include "serial/serial.h"
#include "EchosounderCommands.h"
#include "Echosounder.h"

namespace
{
    std::map<int, EchosounderCommandList> DualEchosounderCommands =
    {
        { EchosounderCommandIds::IdInfo,              { "#info",       "",      "" }},
        { EchosounderCommandIds::IdRange,             { "#range",      "50000", " - #range[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdRangeH,            { "#rangeh",     "50000", " - #rangeh[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdRangeL,            { "#rangel",     "50000", " - #rangel[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdInterval,          { "#interval",   "1.0",   " - #interval[ ]{0,}\\[[ ]{0,}(([0-9]*[.])?[0-9]+) sec[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdPingonce,          { "#pingonce",   "0",     " - #pingonce[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTxLength,          { "#txlength",   "50",    " - #txlength[ ]{0,}\\[[ ]{0,}([0-9]{1,}) uks[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTxLengthH,         { "#txlengthh",  "50",    " - #txlengthh[ ]{0,}\\[[ ]{0,}([0-9]{1,}) uks[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTxLengthL,         { "#txlengthl",  "100",   " - #txlengthl[ ]{0,}\\[[ ]{0,}([0-9]{1,}) uks[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTxPower,           { "#txpower",    "0.0",   " - #txpower[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdGain,              { "#gain",       "0.0",   " - #gain[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdGainH,             { "#gainh",      "0.0",   " - #gainh[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdGainL,             { "#gainl",      "0.0",   " - #gainl[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGMode,           { "#tvgmode",    "1",     " - #tvgmode[ ]{0,}\\[[ ]{0,}([0-4]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGAbs,            { "#tvgabs",     "0.140", " - #tvgabs[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB\\/m[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGAbsH,           { "#tvgabsh",    "0.140", " - #tvgabsh[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB\\/m[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGAbsL,           { "#tvgabsl",    "0.060", " - #tvgabsl[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB\\/m[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGSprd,           { "#tvgsprd",    "15.0",  " - #tvgsprd[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+)[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGSprdH,          { "#tvgsprdh",   "15.0",  " - #tvgsprdh[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+)[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGSprdL,          { "#tvgsprdl",   "15.0",  " - #tvgsprdl[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+)[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdAttn,              { "#attn",       "0",     " - #attn[ ]{0,}\\[[ ]{0,}([0-9]{1,}) uks[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdAttnH,             { "#attnh",      "0",     " - #attnh[ ]{0,}\\[[ ]{0,}([0-9]{1,}) uks[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdAttnL,             { "#attnl",      "0",     " - #attnl[ ]{0,}\\[[ ]{0,}([0-9]{1,}) uks[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSound,             { "#sound",      "1500",  " - #sound[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mps[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdDeadzone,          { "#deadzone",   "300",   " - #deadzone[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdDeadzoneH,         { "#deadzoneh",  "300",   " - #deadzoneh[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdDeadzoneL,         { "#deadzonel",  "500",   " - #deadzonel[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdThreshold,         { "#threshold",  "10",    " - #threshold[ ]{0,}\\[[ ]{0,}([0-9]{1,}) %[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdThresholdH,        { "#thresholdh", "10",    " - #thresholdh[ ]{0,}\\[[ ]{0,}([0-9]{1,}) %[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdThresholdL,        { "#thresholdl", "10",    " - #thresholdl[ ]{0,}\\[[ ]{0,}([0-9]{1,}) %[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdOffset,            { "#offset",     "0",     " - #offset[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdOffsetH,           { "#offseth",    "0",     " - #offseth[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdOffsetL,           { "#offsetl",    "0",     " - #offsetl[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdMedianFlt,         { "#medianflt",  "2",     " - #medianflt[ ]{0,}\\[[ ]{0,}([0-9]{1,3})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSMAFlt,            { "#movavgflt",  "1",     " - #movavgflt[ ]{0,}\\[[ ]{0,}([0-9]{1,3})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEADBT,           { "#nmeadbt",    "1",     " - #nmeadbt[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEADPT,           { "#nmeadpt",    "0",     " - #nmeadpt[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEAMTW,           { "#nmeamtw",    "1",     " - #nmeamtw[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEAXDR,           { "#nmeaxdr",    "1",     " - #nmeaxdr[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEAEMA,           { "#nmeaema",    "0",     " - #nmeaema[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEAZDA,           { "#nmeazda",    "0",     " - #nmeazda[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdOutrate,           { "#outrate",    "0.0",   " - #nmearate[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) sec[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEADPTOffset,     { "#nmeadptoff", "0.0",   " - #nmeadptoff[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) m[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEADPTZero,       { "#nmeadpzero", "1",     " - #nmeadpzero[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdOutput,            { "#output",     "3",     " - #output[ ]{0,}\\[[ ]{0,}([0-9]{1,})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdAltprec,           { "#altprec",    "3",     " - #altprec[ ]{0,}\\[[ ]{0,}([1-4]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSamplFreq,         { "#samplfreq",  "0",     " - #samplfreq[ ]{0,}\\[[ ]{0,}([0-9]{1,6})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTime,              { "#time",       "0",     " - #time[ ]{0,}\\[[ ]{0,}([0-9]{1,})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSyncExtern,        { "#syncextern", "0",     " - #syncextern[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSyncExternMode,    { "#syncextmod", "1",     " - #syncextmod[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSyncOutPolarity,   { "#syncoutpol", "1",     " - #syncoutpol[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdAnlgMode,          { "#anlgmode",   "0",     " - #anlgmode[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdAnlgRate,          { "#anlgrate",   "0.100", " - #anlgrate[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) V\\/m[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdAnlgMaxOut,        { "#anlgmax",    "4",     " - #anlgmax[ ]{0,}\\[[ ]{0,}([1-4]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdVersion,           { "#version",    "",      " S\\/W Ver: ([0-9]{1,}[.][0-9]{1,}) .*" }},

        { EchosounderCommandIds::IdSetHighFreq,       { "#setfh",      "",      "" }},
        { EchosounderCommandIds::IdSetLowFreq,        { "#setfl",      "",      "" }},
        { EchosounderCommandIds::IdSetDualFreq,       { "#setfd",      "",      "" }},

        { EchosounderCommandIds::IdGetHighFreq,       { "#getfh",      "",      ".*High Frequency:[ ]{0,}([0-9]{4,})Hz.*" }},
        { EchosounderCommandIds::IdGetLowFreq,        { "#getfl",      "",      ".*Low Frequency:[ ]{0,}([0-9]{4,})Hz.*" }},
        { EchosounderCommandIds::IdGetWorkFreq,       { "#getf",       "",      ".*:[ ]{1,}([0-9]{4,})Hz[ ]{0,}\\(Active\\).*" }},

        { EchosounderCommandIds::IdGo,                { "#go",         "",      "" }}
    };
}

class DualEchosounder : public Echosounder
{
public:
    DualEchosounder(std::shared_ptr<serial::Serial> SerialPort, std::map<int, EchosounderCommandList> &CommandList = DualEchosounderCommands);
    virtual ~DualEchosounder();
};


#endif // DUALECHOSOUNDER_H
