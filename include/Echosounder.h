// Copyright (c) EofE Ultrasonics Co., Ltd., 2024
#if !defined(ECHOSOUNDER_H)
#define ECHOSOUNDER_H

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

namespace
{
    std::map<int, EchosounderCommandList> SingleSonarCommands =
    {
        { EchosounderCommandIds::IdInfo,            { "#info",       "",      ""}},
        { EchosounderCommandIds::IdRange,           { "#range",      "50000", " - #range[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*"}},
        { EchosounderCommandIds::IdInterval,        { "#interval",   "0.1",   " - #interval[ ]{0,}\\[[ ]{0,}(([0-9]*[.])?[0-9]+) sec[ ]{0,}\\].*"}},
        { EchosounderCommandIds::IdTxLength,        { "#txlength",   "50",    " - #txlength[ ]{0,}\\[[ ]{0,}([0-9]{1,}) uks[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdGain,            { "#gain",       "0.0",   " - #gain[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGMode,         { "#tvgmode",    "1",     " - #tvgmode[ ]{0,}\\[[ ]{0,}([0-4]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGAbs,          { "#tvgabs",     "0.140", " - #tvgabs[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) dB\\/m[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTVGSprd,         { "#tvgsprd",    "15.0",  " - #tvgsprd[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+)[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSound,           { "#sound",      "1500",  " - #sound[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mps[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdDeadzone,        { "#deadzone",   "300",   " - #deadzone[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdThreshold,       { "#threshold",  "10",    " - #threshold[ ]{0,}\\[[ ]{0,}([0-9]{1,}) %[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdOffset,          { "#offset",     "0",     " - #offset[ ]{0,}\\[[ ]{0,}([0-9]{1,}) mm[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdMedianFlt,       { "#medianflt",  "2",     " - #medianflt[ ]{0,}\\[[ ]{0,}([0-9]{1,3})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSMAFlt,          { "#movavgflt",  "1",     " - #movavgflt[ ]{0,}\\[[ ]{0,}([0-9]{1,3})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdOutrate,         { "#nmearate",   "0.0",   " - #outrate[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) sec[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEADBT,         { "#nmeadbt",    "1",     " - #nmeadbt[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEADPT,         { "#nmeadpt",    "1",     " - #nmeadpt[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEADPTOffset,   { "#nmeadptoff", "0.0",   " - #nmeadptoff[ ]{0,}\\[[ ]{0,}([+-]?([0-9]*[.])?[0-9]+) m[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEADPTZero,     { "#nmeadpzero", "1",     " - #nmeadpzero[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEAMTW,         { "#nmeamtw",    "1",     " - #nmeamtw[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdAltprec,         { "#altprec",    "3",     " - #altprec[ ]{0,}\\[[ ]{0,}([1-4]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEAXDR,         { "#nmeaxdr",    "1",     " - #nmeaxdr[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEAEMA,         { "#nmeaema",    "1",     " - #nmeaema[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdNMEAZDA,         { "#nmeazda",    "0",     " - #nmeazda[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdOutput,          { "#output",     "3",     " - #output[ ]{0,}\\[[ ]{0,}([0-9]{1,})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdTime,            { "#time",       "0",     " - #time[ ]{0,}\\[[ ]{0,}([0-9]{1,})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSyncExtern,      { "#syncextern", "0",     " - #syncextern[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSyncExternMode,  { "#syncextmod", "1",     " - #syncextmod[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdSyncOutPolarity, { "#syncoutpol", "1",     " - #syncoutpol[ ]{0,}\\[[ ]{0,}([01]{1})[ ]{0,}\\].*" }},
        { EchosounderCommandIds::IdVersion,         { "#version",    "",      " S\\/W Ver: ([0-9]{1,}[.][0-9]{1,}) .*" }},
        { EchosounderCommandIds::IdGo,              { "#go",         "",      "" }}
    };
}

/**
    @class SingleSonar

    SingleSonar is used for access to Single Frequency Echosounder.
 */

class Echosounder : public ISonar
{    
    /**
    *   Serial port class used by echosounder
    */
    std::shared_ptr<serial::Serial>serial_port_;

    /**
    *   Data return by the unit after host issued command to it
    */
    std::string command_result_;

    /**
    *   #info command result line by line
    */
    std::vector<std::string> info_lines_;

    /**
    *   This map contains all available command for the echosounder
    */
    std::map<int, EchosounderCommandList>& echosounder_commands_;

    /**
    *   This map contains all current settings of the echosounder
    */
    std::map<EchosounderCommandIds_t, std::string> echosounder_settings_;

    /**
    *   Current running status of the echosounder
    */
    bool is_running_;

    /**
    *   Current detected status of the echosounder
    */
    bool is_detected_;

    /**
     *   @brief Send command to the echosounder
     *   @param command - command to send
     *   @return 1 - command successfuly execute, 2 - invalid argument, 3 - invalid command, -2 - timeout occured
     */
    int SendCommand(EchosounderCommandIds command);

    /**
     *   @brief Receive responce for command sent to the echosounder
     *   @return 1 - command successfuly execute, 2 - invalid argument, 3 - invalid command, -2 - timeout occured
     */
    int SendCommandResponseCheck();

    /**
     *   @brief Waiting until echosounder send back "command prompt" character
     *   @param timeoutms - timeout in milliseconds
     *   @return 1 - command prompt received, -2 - timeout occured
     */
    int WaitCommandPrompt(int64_t timeoutms) const;

    void GetAllValues();
    void SetAllValues();

    /**
     *   @brief Send #info command and parse it to echosounder_settings_
     *   @return 1 - command successfuly execute, 2 - invalid argument, 3 - invalid command, -2 - timeout occured
     */
    int GetSonarInfo();

public:

    /**
        Constructor
    */
    Echosounder(std::shared_ptr<serial::Serial> SerialPort, std::map<int, EchosounderCommandList>& CommandList = SingleSonarCommands);

    /**
    *   @brief Set echosounder's value   
    */
    bool SetValue(EchosounderCommandIds Command, const std::string &SonarValue);

    /**
    *   @brief Get echosounder's value. Value is stored internally in the class.
    *   @return std::string reference to Value
    */  
    const std::string &GetValue(EchosounderCommandIds command);

    /**
    *   @brief Set current time to echosounder
    */  
    void SetCurrentTime();

    /**
    *   @brief Get serial port used for access to echosounder.
    *   @return std::shared_ptr<serial::Serial> reference
    */
    std::shared_ptr<serial::Serial> &GetSerialPort();

    virtual void GetSettings() override;
    virtual void SetSettings() override;
    virtual void Start() override;
    virtual void Stop() override;

    virtual bool Detect() override;

    virtual bool IsRunning() override;
    virtual bool IsDetected() override;
};

#endif // SINGLESONAR_H
