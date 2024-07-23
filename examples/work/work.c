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

        EchosounderValue testvalue;

        EchosounderSetCurrentTime(snrctx);
        FloatToEchosounderValue(14.0F, &testvalue);

        printf("Set IdTVGSprdH. Result must be -1 for single frequency echosounder\n");
        int result = EchosounderSetValue(snrctx, IdTVGSprdH, &testvalue);
        printf("result = %d\n", result);

        printf("Set IdTVGSprd. Result must be 0 for single frequency echosounder\n");
        FloatToEchosounderValue(10.0F, &testvalue);
        result = EchosounderSetValue(snrctx, IdTVGSprd, &testvalue);
        printf("result = %d\n", result);

        LongToEchosounderValue(10000, &testvalue);
        EchosounderSetValue(snrctx, IdRange, &testvalue);

        LongToEchosounderValue(3, &testvalue);
        EchosounderSetValue(snrctx, IdOutput, &testvalue);

        LongToEchosounderValue(3, &testvalue);
        EchosounderSetValue(snrctx, IdAltprec, &testvalue);

        printf("Start Echosounder\n");
        EchosounderStart(snrctx);
        printf("Echosounder Is Running? -> %d\n", EchosounderIsRunning(snrctx));

        printf("Get IdTVGSprd. Result must be 10.0 for single frequency echosounder\n");
        EchosounderGetValue(snrctx, IdTVGSprd, &testvalue);
        printf("tgvsprd => %.1f\n", EchosounderValueToFloat(&testvalue));

        printf("Receiving 1kB NMEA Data.\n");

        size_t totalbytes = 0;

        while(totalbytes < 1024)
        {          
            uint8_t buffer[16];
            size_t size = EchosounderReadData(snrctx, buffer, 16);

            totalbytes += size;

            for (int i = 0; i < size; i++)
            {
                putchar(buffer[i]);
            }            
        }

        printf("\nStop Echosounder\n");
        EchosounderStop(snrctx);
        printf("Echosounder Is Running? -> %d\n", EchosounderIsRunning(snrctx));
        EchosounderClose(snrctx);    
    }

    return 0;
}
