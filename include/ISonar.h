// Copyright (c) EofE Ultrasonics Co., Ltd., 2024
#pragma once

/**
 *  @class ISonar
 *  Interface class for the abstract sonar
 */

class ISonar
{
public:

    virtual ~ISonar();

    virtual void GetSettings() = 0;
    virtual void SetSettings() = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual bool Detect() = 0;

    virtual bool IsRunning() = 0;
    virtual bool IsDetected() = 0;
};
