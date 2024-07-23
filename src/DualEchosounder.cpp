// Copyright (c) EofE Ultrasonics Co., Ltd., 2024
#include "DualEchosounder.h"

DualEchosounder::DualEchosounder(std::shared_ptr<serial::Serial> SerialPort, std::map<int, EchosounderCommandList>& CommandList) :
    Echosounder(SerialPort, CommandList)
{

}

DualEchosounder::~DualEchosounder()
{

}
