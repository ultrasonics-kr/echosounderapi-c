// Copyright (c) EofE Ultrasonics Co., Ltd., 2024
#include <stdio.h>

#include "EchosounderCWrapper.h"

int main()
{
    pSnrCtx snrctx = SingleEchosounderOpen("\\\\.\\COM31", 115200U);
    //pSnrCtx snrctx = DualEchosounderOpen("\\\\.\\COM41", 115200U);

    if (NULL != snrctx)
    {
        bool sf = EchosounderDetect(snrctx);
        printf("Sonar detected => %d\n", sf);
    }

    EchosounderClose(snrctx);    
    return 0;
}
