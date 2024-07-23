// Copyright (c) EofE Ultrasonics Co., Ltd., 2024
#include "Echosounder.h"

#include <iostream>
#include <initializer_list>
#include <chrono>
#include <deque>
#include <iterator>
#include <vector>
#include <map>

Echosounder::Echosounder(std::shared_ptr<serial::Serial> SerialPort, std::map<int, EchosounderCommandList>& CommandList) :
    serial_port_(SerialPort),
    is_running_(false),
    echosounder_commands_(CommandList)
{
    is_detected_ = Detect();

    if (false != is_detected_)
    {
        GetSonarInfo();
    }
}

int Echosounder::SendCommandResponseCheck()
{
    int result = 0;

    std::deque<uint8_t>magicidbuffer = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };
    const uint8_t invalidargtoken[]  = { 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'a', 'r', 'g', 'u', 'm', 'e', 'n', 't', '\r', '\n'};
    const uint8_t invalidcmdtoken[]  = { 'I', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\r', '\n' };
    const uint8_t okgotoken[]        = { 'O', 'K', ' ', 'g', 'o', '\r', '\n' };
    const uint8_t oktoken[]          = { 'O', 'K', '\r', '\n' };

    command_result_.clear();
    const auto time_begin = std::chrono::steady_clock::now();

    for (;;)
    {
        uint8_t ch;
        std::size_t br = serial_port_->read(&ch, 1);

        if (br > 0)
        {
            magicidbuffer.pop_front();
            magicidbuffer.push_back(ch);

            command_result_.push_back(ch);

            if (std::equal(magicidbuffer.end() - sizeof(oktoken), magicidbuffer.end(), oktoken))
            {
                is_running_ = false;
                result = 1;
                break;
            }
            else if (std::equal(magicidbuffer.end() - sizeof(okgotoken), magicidbuffer.end(), okgotoken))
            {
                is_running_ = true;
                result = 1;
                break;
            }
            else if (std::equal(magicidbuffer.end() - sizeof(invalidcmdtoken), magicidbuffer.end(), invalidcmdtoken))
            {
                is_running_ = false;
                result = 2;
                break;
            }
            else if (std::equal(magicidbuffer.end() - sizeof(invalidargtoken), magicidbuffer.end(), invalidargtoken))
            {
                is_running_ = false;
                result = 3;
                break;
            }
            else
            {
                // do nothing
            }
        }

        const auto period = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_begin);

        if (period.count() > 4000LL)
        {
            result = -2;
            break;
        }
    }

    return result;
}

int Echosounder::WaitCommandPrompt(int64_t timeoutms) const
{
    int result = 0;

    std::deque<uint8_t>magicidbuffer = { '0', '0', '0', '0', '0' };
    const uint8_t prompttoken[] = { '>' };

    const auto time_begin = std::chrono::steady_clock::now();

    for (;;)
    {
        uint8_t ch;
        std::size_t br = serial_port_->read(&ch, 1);

        if (br > 0)
        {
            magicidbuffer.pop_front();
            magicidbuffer.push_back(ch);

            if (std::equal(magicidbuffer.end() - sizeof(prompttoken), magicidbuffer.end(), prompttoken))
            {
                result = 1;
                break;
            }
            else
            {
                // do nothing
            }
        }

        const auto period = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_begin);

        if (period.count() > timeoutms)
        {
            result = -2;
            break;
        }
    }

    return result;
}

int Echosounder::SendCommand(EchosounderCommandIds Command)
{
    int retvalue = 0;

    bool wasrunning = is_running_;
    if (false != is_running_)
    {
        Stop();
    }

    const std::string fullcommand = std::string(echosounder_commands_[Command].command_text) + '\r';
    serial_port_->write(fullcommand);

    retvalue = SendCommandResponseCheck();
    WaitCommandPrompt(1000);

    if (false != wasrunning)
    {
        Start();
    }

    return retvalue;
}

bool Echosounder::SetValue(EchosounderCommandIds Command, const std::string &SonarValue)
{
    bool retvalue = false;

    const auto &command0 = echosounder_commands_[Command];

    if (nullptr != command0.command_text)
    {
        const auto command = std::string(command0.command_text);

        if (command.length() > 0)
        {

            bool wasrunning = is_running_;
            if (false != is_running_)
            {
                Stop();
            }

            const std::string fullcommand = command + ' ' + SonarValue + '\r';
            serial_port_->write(fullcommand);

            retvalue = (1 == SendCommandResponseCheck()) ? true : false;

            if (false != retvalue)
            {
                echosounder_settings_[Command] = SonarValue;
            }

            WaitCommandPrompt(1000);

            if (false != wasrunning)
            {
                Start();
            }
        }
    }

    return retvalue;
}

const std::string &Echosounder::GetValue(EchosounderCommandIds Command)
{
    return echosounder_settings_[Command];
}

void Echosounder::GetAllValues()
{
    for (auto it = echosounder_commands_.cbegin(); it != echosounder_commands_.cend(); it++)
    {
        const auto &regexstr = std::regex(it->second.regex_match_text);

        for(auto &line : info_lines_)
        {
            std::smatch match;

            if (false != std::regex_match(line, match, regexstr))
            {
                echosounder_settings_[static_cast<EchosounderCommandIds_t>(it->first)] = match[1].str();
                break;
            }
        }
    }
}

void Echosounder::SetAllValues()
{
    for(auto it = echosounder_commands_.cbegin(); it != echosounder_commands_.cend(); it++)
    {
        SetValue(static_cast<EchosounderCommandIds_t>(it->first), echosounder_settings_[static_cast<EchosounderCommandIds_t>(it->first)]);
    }
}

std::shared_ptr<serial::Serial> &Echosounder::GetSerialPort()
{
    return serial_port_;
}

int Echosounder::GetSonarInfo()
{
    int result = -1;
    info_lines_ = {};

    result = SendCommand(EchosounderCommandIds::IdInfo);

    if (1 == result)
    {
        auto istr = std::istringstream(command_result_);
        std::string line;

        while (std::getline(istr, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(),
                                      [](char &echar)
            {
                if (('\r' == echar) || ('\n' == echar))
                {
                    return true;
                }

                return false;

            }), line.end());

            info_lines_.push_back(line);
        }

        GetAllValues();
    }

    return result;
}

void Echosounder::GetSettings()
{
    if (false != is_detected_)
    {
        if (false != is_running_)
        {
            Stop();
        }

        GetSonarInfo();
    }
}

void Echosounder::SetSettings()
{
    if (false != is_detected_)
    {
        if (false != is_running_)
        {
            Stop();
        }

        SetAllValues();
    }
}

void Echosounder::Start()
{
    if (false != is_detected_)
    {
        if (false == is_running_)
        {
            SendCommand(EchosounderCommandIds::IdGo);
        }
    }
}

void Echosounder::Stop()
{
    if (false != is_detected_)
    {
        if (false != is_running_)
        {
            Detect();
        }
    }
}

bool Echosounder::Detect()
{
    bool result = false;

    for (int i = 0; i < 10; i++)
    {
        serial_port_->write("\r");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        serial_port_->write("\r");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        serial_port_->write("\r");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        serial_port_->write("\r");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        serial_port_->write("\r");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (1 == WaitCommandPrompt(500))
        {
            serial_port_->flush();
            serial_port_->write("#speed\r");

            if (1 != SendCommandResponseCheck())
            {
                result = false;
                break;
            }

            WaitCommandPrompt(1000);
            result = true;
            break;
        }
    }

    return result;
}

void Echosounder::SetCurrentTime()
{
    const auto now = std::chrono::system_clock::now();
    const auto timenow = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    SetValue(EchosounderCommandIds::IdTime, std::to_string(timenow));
}

bool Echosounder::IsRunning()
{
    return is_running_;
}

bool Echosounder::IsDetected()
{
    return is_detected_;
}
