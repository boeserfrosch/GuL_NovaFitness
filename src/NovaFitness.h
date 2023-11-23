/**
 * GuL_NovaFitness
 * Copyright (c) 2023 Guido Lehne
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Developed for Arduino-ESP32
 * Created by Guido Lehne
 *
 */
#pragma once

#include <Arduino.h>
#include <vector>

namespace GuL
{

    class NovaFitness
    {
    public:
        struct FirmwareVersion
        {
            uint8_t year = 0;
            uint8_t month = 0;
            uint8_t day = 0;
        };

        enum ReportingMode
        {
            ACTIVE,
            PASSIVE
        };
        enum WorkMode
        {
            SLEEP,
            WAKEUP
        };

        NovaFitness(Stream &stream, uint16_t id = 0xFFFF);
        std::string getSensorName() { return _name; }

        bool sleep();
        bool wakeup();
        bool setToActiveReporting();
        bool setToPassiveReporting();

        bool poll();
        bool pollFirmwareVersion();
        bool pollReportingMode();
        bool pollWorkingMode();
        bool pollWorkingPeriod();
        bool read();

        FirmwareVersion getVersion();
        uint16_t getID();
        float getPM2_5();
        float getPM10();
        float getPM100();
        ReportingMode getReportingMode();
        WorkMode getWorkMode();

    protected:
        enum ParsingSteps
        {
            WAIT_FOR_NEW_FRAME,
            RECEIVE_COMMAND_ID,
            RECEIVE_PAYLOAD,
            RECEIVE_TAIL
        };

        enum CommandID
        {
            SETTINGS_COMMAND_ID = 0xC5,
            DATA_COMMAND_ID = 0xC0,
            DATA100_COMMAND_ID = 0xCF
        };

        enum SettingsType
        {
            REPORTING_MODE = 0x02,
            ID_SETTING = 0x05,
            WORK_MODE = 0x06,
            WORKING_PERIOD = 0x08,
            FIRMWARE_SETTING = 0x07,
            QUERY_DATA = 0x04
        };

        enum SettingsAction
        {
            Query = 0x00,
            Set = 0x01
        };

        FirmwareVersion _firmwareVersion;

        ParsingSteps _parseStep = WAIT_FOR_NEW_FRAME;
        ReportingMode _reportingMode;
        WorkMode _workMode;
        uint16_t _workingPeriodSeconds;
        float _pm2_5 = -1, _pm10 = -1, _pm100 = -1;

        bool send(std::vector<uint8_t> &cmd);
        void unpackFrame();
        void handleDataPayload();
        void handleData100Payload();
        void handleSettingsPayload();
        std::string _name = "NOVAFITNESS";

    private:
        Stream &_stream;
        std::vector<uint8_t> _payload;

        uint8_t
        calcChecksum(std::vector<uint8_t> &cmd);
        void setIdIntoFrame(std::vector<uint8_t> &cmd);

    private:
        uint16_t _id;
        uint8_t _payloadIndex;
        int8_t (NovaFitness::*handle)(uint8_t) = NULL;

    public:
    };

}; // namespace GuL
