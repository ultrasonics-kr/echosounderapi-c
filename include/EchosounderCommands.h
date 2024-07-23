// Copyright (c) EofE Ultrasonics Co., Ltd., 2024
#if !defined(SINGLESONARCOMMANDS_H)
#define SINGLESONARCOMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

enum EchosounderCommandIds
{
    IdInfo = 0, 
    IdGo,
    IdRange,
    IdRangeH,
    IdRangeL,
    IdInterval,
    IdPingonce,
    IdTxLength,
    IdTxLengthH,
    IdTxLengthL,
    IdTxPower,
    IdGain,
    IdGainH,
    IdGainL,
    IdTVGMode,
    IdTVGAbs,
    IdTVGAbsH,
    IdTVGAbsL,
    IdTVGSprd,
    IdTVGSprdH,
    IdTVGSprdL,
    IdAttn,
    IdAttnH,
    IdAttnL,
    IdSound,
    IdDeadzone,
    IdDeadzoneH,
    IdDeadzoneL,
    IdThreshold,
    IdThresholdH,
    IdThresholdL,
    IdOffset,
    IdOffsetH,
    IdOffsetL,
    IdMedianFlt,
    IdSMAFlt,
    IdNMEADBT,
    IdNMEADPT,
    IdNMEAMTW,
    IdNMEAXDR,
    IdNMEAEMA,
    IdNMEAZDA,
    IdOutrate,
    IdNMEADPTOffset,
    IdNMEADPTZero,
    IdOutput,
    IdAltprec,
    IdSamplFreq,
    IdTime,
    IdSyncExtern,
    IdSyncExternMode,
    IdSyncOutPolarity,
    IdAnlgMode,
    IdAnlgRate,
    IdAnlgMaxOut,
    IdVersion,

    IdSetHighFreq,
    IdSetLowFreq,
    IdSetDualFreq,

    IdGetHighFreq,
    IdGetLowFreq,
    IdGetWorkFreq
};

typedef enum EchosounderCommandIds EchosounderCommandIds_t;

struct EchosounderCommandList
{
    const char* command_text;
    const char* default_value;
    const char* regex_match_text;
};

#ifdef __cplusplus
}
#endif

#endif //!SINGLESONARCOMMANDS_H
