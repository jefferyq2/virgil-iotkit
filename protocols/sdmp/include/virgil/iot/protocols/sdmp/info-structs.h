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


#ifndef VS_SECURITY_SDK_SDMP_SERVICES_INFO_STRUCTS_H
#define VS_SECURITY_SDK_SDMP_SERVICES_INFO_STRUCTS_H

#include <virgil/iot/protocols/sdmp/info-server.h>
#include <virgil/iot/protocols/sdmp.h>
#include <virgil/iot/status_code/status_code.h>
#include <virgil/iot/trust_list/trust_list.h>
#include <virgil/iot/trust_list/tl_structs.h>
#include <virgil/iot/protocols/sdmp/sdmp_structs.h>

typedef struct __attribute__((__packed__)) {
    uint32_t device_roles; // vs_sdmp_device_role_e
    vs_mac_addr_t mac;
} vs_sdmp_info_device_t;

typedef struct __attribute__((__packed__)) {
    vs_fw_manufacture_id_t manufacture_id;
    vs_fw_device_type_t device_type;
    vs_mac_addr_t default_netif_mac;
    vs_firmware_version_t fw_version;
    uint16_t tl_version;
} vs_info_ginf_response_t;

typedef struct __attribute__((__packed__)) {
    uint32_t sent;
    uint32_t received;
} vs_info_stat_response_t;

typedef enum {
    VS_SDMP_INFO_GENERAL = HTONL_IN_COMPILE_TIME(0x0001),
    VS_SDMP_INFO_STATISTICS = HTONL_IN_COMPILE_TIME(0x0002),
} vs_sdmp_info_element_mask_e;

#endif // VS_SECURITY_SDK_SDMP_SERVICES_INFO_STRUCTS_H
