#include "NovaFitness.h"

namespace GuL
{
    NovaFitness::NovaFitness(Stream &stream, uint16_t id) : _stream(stream)
    {
        _id = id;
    }

    bool NovaFitness::sleep()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::WORK_MODE, SettingsAction::Set, WorkMode::SLEEP, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x05, 0xAB};
        return this->send(command);
    }

    bool NovaFitness::wakeup()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::WORK_MODE, SettingsAction::Set, WorkMode::WAKEUP, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x05, 0xAB};
        return this->send(command);
    }

    bool NovaFitness::setToActiveReporting()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::REPORTING_MODE, SettingsAction::Set, ReportingMode::ACTIVE, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x05, 0xAB};
        return this->send(command);
    }

    bool NovaFitness::setToPassiveReporting()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::REPORTING_MODE, SettingsAction::Set, ReportingMode::PASSIVE, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x05, 0xAB};
        return this->send(command);
    }

    bool NovaFitness::poll()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::QUERY_DATA, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x00, 0xAB};
        return this->send(command);
    }

    bool NovaFitness::pollFirmwareVersion()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::FIRMWARE_SETTING, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x00, 0xAB};
        return this->send(command);
    }
    bool NovaFitness::pollReportingMode()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::REPORTING_MODE, SettingsAction::Query, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x00, 0xAB};
        return this->send(command);
    }
    bool NovaFitness::pollWorkingMode()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::WORK_MODE, SettingsAction::Query, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x00, 0xAB};
        return this->send(command);
    }
    bool NovaFitness::pollWorkingPeriod()
    {
        std::vector<uint8_t> command = {0xAA, 0xB4, SettingsType::WORKING_PERIOD, SettingsAction::Query, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0xFF, 0xFF, 0x00, 0xAB};
        return this->send(command);
    }

    bool NovaFitness::read()
    {
        uint8_t byte;
        bool gotValidData = false;

        while (this->_stream.available())
        {
            uint8_t byte = this->_stream.read();
            switch (_parseStep)
            {
            case WAIT_FOR_NEW_FRAME:
                if (byte == 0xAA)
                {
                    _parseStep = RECEIVE_COMMAND_ID;
                    _payload.clear();
                    _payload.emplace_back(byte);
                }
                break;
            case RECEIVE_COMMAND_ID:
                if (byte != SETTINGS_COMMAND_ID && byte != DATA_COMMAND_ID && byte != DATA100_COMMAND_ID)
                {
                    _parseStep = WAIT_FOR_NEW_FRAME;
                }
                _payload.emplace_back(byte);
                break;
            case RECEIVE_PAYLOAD:
                _payload.emplace_back(byte);
                // TODO: Maybe a more dynmaically approach for the frame size? At the moment all frames to receive should have a size of 10 constant
                if (_payload.size() == 9)
                {
                    _parseStep = RECEIVE_TAIL;
                }
                break;
            case RECEIVE_TAIL:
                _payload.emplace_back(byte);
                _parseStep = WAIT_FOR_NEW_FRAME;
                // TODO: Check for the ID also
                if (byte != 0xAB || this->calcChecksum(_payload) != _payload.at(17))
                {
                    break;
                }
                this->unpackFrame();
                gotValidData = true;
            }
        }
        return gotValidData;
    }

    NovaFitness::FirmwareVersion NovaFitness::getVersion()
    {
        return _firmwareVersion;
    }
    uint16_t NovaFitness::getID() { return _id; }
    float NovaFitness::getPM2_5() { return _pm2_5; }
    float NovaFitness::getPM10() { return _pm10; };
    float NovaFitness::getPM100() { return _pm100; }

    NovaFitness::ReportingMode NovaFitness::getReportingMode() { return _reportingMode; }
    NovaFitness::WorkMode NovaFitness::getWorkMode() { return _workMode; }

    void NovaFitness::unpackFrame()
    {
        switch (_payload.at(1))
        {
        case SETTINGS_COMMAND_ID:
            this->handleSettingsPayload();
            break;
        case DATA_COMMAND_ID:
            this->handleDataPayload();
            break;
        case DATA100_COMMAND_ID:
            break;
        }
    }

    bool NovaFitness::send(std::vector<uint8_t> &cmd)
    {
        if (cmd.size() != 19)
        {
            // The frame to send is always 19 bytes long
            return false;
        }
        this->setIdIntoFrame(cmd);
        cmd[17] = this->calcChecksum(cmd);
        size_t sendBytes = _stream.write(cmd.data(), cmd.size());
        return sendBytes == cmd.size();
    }

    void NovaFitness::handleDataPayload()
    {
        _pm2_5 = (float)((_payload.at(3) << 8 | _payload.at(2)) / 10);
        _pm10 = (float)((_payload.at(5) << 8 | _payload.at(4)) / 10);

        // Payload 6 and 7 unhandled 'cause this is only the device id.
        // Maybe handle later if more than one NovaFitness sensor should work on
        // same time -> Each Sensor get own instance of NovaFitness class. Each
        // class should only handle frames of own sensor. Other concept for
        // Stream is neccessary. Maybe observer pattern?
    }

    void NovaFitness::handleData100Payload()
    {
        _pm100 = (_payload[5] << 8) | _payload[4];
    }

    void NovaFitness::handleSettingsPayload()
    {
        switch ((SettingsType)_payload.at(2))
        {
        case REPORTING_MODE:
            _reportingMode = _payload.at(4) ? ACTIVE : PASSIVE;
            break;
        case ID_SETTING:
            _id = (_payload.at(6) << 8) | _payload.at(7);
            break;
        case WORK_MODE:
            _workMode = _payload.at(4) == 0 ? SLEEP : WAKEUP;
            break;
        case WORKING_PERIOD:
            _workingPeriodSeconds = _payload.at(4) == 0 ? 1 : _payload.at(4) * 60;
            break;
        case FIRMWARE_SETTING:
            _firmwareVersion.year = _payload.at(3);
            _firmwareVersion.month = _payload.at(4);
            _firmwareVersion.day = _payload.at(5);
            break;
        }
    }

    void NovaFitness::setIdIntoFrame(std::vector<uint8_t> &cmd)
    {
        cmd[15] = (this->_id >> 8) & 0xFF;
        cmd[16] = this->_id & 0xFF;
    }

    uint8_t NovaFitness::calcChecksum(std::vector<uint8_t> &cmd)
    {
        // "Checksum: Low 8bit of the sum result of Data Bytes（not including packet head, tail and Command ID"
        uint8_t sum = 0;

        for (int8_t i = 2; i < cmd.size() - 1; i++)
        {
            sum += cmd[i];
        }
        return sum;
    }
}