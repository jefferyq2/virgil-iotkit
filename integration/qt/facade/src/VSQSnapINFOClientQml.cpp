//  Copyright (C) 2015-2019 Virgil Security, Inc.
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//      (1) Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//      (2) Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in
//      the documentation and/or other materials provided with the
//      distribution.
//
//      (3) Neither the name of the copyright holder nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
//  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
//  Lead Maintainer: Virgil Security Inc. <support@virgilsecurity.com>

#include <virgil/iot/qt/VSQIoTKit.h>

int VSQSnapInfoClientQml::rowCount(const QModelIndex & parent ) const {
    Q_UNUSED(parent);
    return devicesList().size();
}

QVariant VSQSnapInfoClientQml::data(const QModelIndex & index, int role) const {
    if(!index.isValid() || index.row() >= devicesList().size())
        return QVariant();

    const VSQDeviceInfo& deviceInfo = devicesList()[index.row()];
    switch(role){
    case MacAddress : return deviceInfo.m_mac.description();
    case DeviceRoles : return deviceInfo.m_deviceRoles.description();
    case ManufactureId : return deviceInfo.m_manufactureId.description();
    case DeviceType : return deviceInfo.m_deviceType.description();
    case FwVer : return deviceInfo.m_fwVer.description();
    case TlVer : return deviceInfo.m_tlVer.description();
    case Sent : return deviceInfo.m_sent;
    case Received : return deviceInfo.m_received;
    case LastTimestamp : return deviceInfo.m_lastTimestamp.toString("hh:mm:ss");
    case IsActive : return deviceInfo.m_isActive;
    case HasGeneralInfo : return deviceInfo.m_hasGeneralInfo;
    case HasStatistics : return deviceInfo.m_hasStatistics;
    default :   Q_ASSERT(false); return QString("Unsupported");
    }
}

QHash<int, QByteArray> VSQSnapInfoClientQml::roleNames() const {
    static const QHash<int, QByteArray> roles{
    { MacAddress, "MacAddress" },
    { DeviceRoles, "DeviceRoles" },
    { ManufactureId, "ManufactureId" },
    { DeviceType, "DeviceType" },
    { FwVer, "FwVer" },
    { TlVer, "TlVer" },
    { Sent, "Sent" },
    { Received, "Received" },
    { LastTimestamp, "LastTimestamp" },
    { IsActive, "IsActive" },
    { HasGeneralInfo, "HasGeneralInfo" },
    { HasStatistics, "HasStatistic" }
    };

    return roles;
}

void VSQSnapInfoClientQml::startNotifyProcess(const VSQDeviceInfo& device) {
    emitDataChanged(device);
}

void VSQSnapInfoClientQml::generalInfoProcess(const VSQDeviceInfo& device) {
    emitDataChanged(device);
}

void VSQSnapInfoClientQml::statisticsProcess(const VSQDeviceInfo& device) {
    emitDataChanged(device);
}

void VSQSnapInfoClientQml::emitDataChanged(const VSQDeviceInfo& device) const {
    auto &devices = devicesList();

    for(size_t pos = 0; pos < devices.size(); ++pos){
        if( devices[pos] == device ){
            auto curIndex = index(pos, pos);
            emit dataChanged( curIndex, curIndex);
        }
    }

    Q_ASSERT(false && "Unreachable code");
}

