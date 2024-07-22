#include "SingleEchosounder.h"

SingleEchosounder::SingleEchosounder(std::shared_ptr<serial::Serial> SerialPort, std::map<int, EchosounderCommandList>& CommandList) :
    Echosounder(SerialPort, CommandList)
{

}

SingleEchosounder::~SingleEchosounder()
{

}
