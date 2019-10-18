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

/*! \file fldt-server.h
 * \brief FLDT for server
 *
 * FLDT protocol is used to download new file version from gateway to client. This module is used to process server part
 * of FLDT protocol.
 *
 * \section fldt_server_usage Usage
 *
 * Server side sends new file versions provided by \ref vs_fldt_server_add_file_type call. Also it sends information about
 * present files by client requests. Files must be previously listed by \ref vs_fldt_server_add_file_type call. If requested
 * file has not been added, \ref vs_fldt_server_add_filetype callback is called to provide such information.
 * In most case it used to output new file version information and gateway address.
 * To successfully file broadcasting \ref vs_update_interface_t must be provided for each file type. You can see
 * function \ref vs_firmware_update_file_type for Firmware example and \ref vs_tl_update_file_type for Trust List one.
 *
 * Here you can see an example of FLDT server initialization :
 * \code
 *  const vs_sdmp_service_t *sdmp_fldt_server;
 *  const vs_mac_addr_t mac_addr;
 *  sdmp_fldt_server = vs_sdmp_fldt_server( &mac_addr, _add_filetype );
 *  STATUS_CHECK( vs_sdmp_register_service(sdmp_fldt_server), "Cannot register FLDT server service" );
 *  STATUS_CHECK( vs_fldt_server_add_file_type( vs_firmware_update_file_type(), vs_firmware_update_ctx(), false ), "Unable to add Firmware file type" );
 *  STATUS_CHECK( vs_fldt_server_add_file_type( vs_tl_update_file_type(), vs_tl_update_ctx(), false ), "Unable to add Trust List file type" ); * \endcode
 *
 * You can see \ref vs_fldt_server_add_filetype function example below :
 * \code
 * static vs_status_e
 * _add_filetype(const vs_update_file_type_t *file_type, vs_update_interface_t **update_ctx) {
 *     switch (file_type->type) {
 *     case VS_UPDATE_FIRMWARE:
 *         *update_ctx = vs_firmware_update_ctx();
 *         break;
 *     case VS_UPDATE_TRUST_LIST:
 *         *update_ctx = vs_tl_update_ctx();
 *         break;
 *     default:
 *         VS_LOG_ERROR("Unsupported file type : %d", file_type->type);
 *         return VS_CODE_ERR_UNSUPPORTED_PARAMETER;
 *     }
 *
 *     return VS_CODE_OK;
 * }
 * \endcode
 *
 */

#ifndef VS_SECURITY_SDK_SDMP_SERVICES_FLDT_SERVER_H
#define VS_SECURITY_SDK_SDMP_SERVICES_FLDT_SERVER_H

#if FLDT_SERVER

#ifdef __cplusplus
extern "C" {
#endif

#include <virgil/iot/update/update.h>
#include <virgil/iot/protocols/sdmp/sdmp-structs.h>
#include <virgil/iot/status_code/status_code.h>

/** Add new file type callback
 *
 * Callback for \ref vs_sdmp_fldt_server function.
 * This callback is used when gateway receives request for file type that has not been added by \ref vs_fldt_server_add_file_type call.
 *
 * \warning Valid pointer to the update context with all callback must be provided.
 *
 * \param[in] file_type \ref vs_update_file_type_t File type descriptor. Cannot be NULL.
 * \param[in, out] update_ctx \ref vs_update_interface_t Pointer to store update nont NULL context pointer for new file type. Cannot be NULL.
 * \return \ref vs_status_e \ref VS_CODE_OK in case of success or error code.
 */
typedef vs_status_e (*vs_fldt_server_add_filetype)(const vs_update_file_type_t *file_type,
                                                   vs_update_interface_t **update_ctx);

/** Initialize FLDT server
 *
 * This call initializes FLDT server. It must be called before any FLDT call.
 *
 * \param[in] gateway_mac \ref vs_mac_addr_t gateway's MAC address. Must not be NULL.
 * \param[in] add_filetype \ref vs_fldt_server_add_filetype callback. Must not be NULL.
 *
 * \return \ref vs_sdmp_service_t SDMP service description. Use this pointer to call \ref vs_sdmp_register_service.
 */
const vs_sdmp_service_t *
vs_sdmp_fldt_server(const vs_mac_addr_t *gateway_mac, vs_fldt_server_add_filetype add_filetype);

/** Add file type
 *
 * FLDT server has the list of file types that it processes. This call adds new file type or update previously added one.
 *
 * \param[in] file_type \ref vs_update_file_type_t file type to be added. Must not be NULL.
 * \param[in] update_ctx \ref vs_update_interface_t update context for current file type. Must not be NULL.
 * \param[in] broadcast_file_info true if gateways has to broadcast information about file provided.
 *
 * \return \ref vs_status_e \ref VS_CODE_OK in case of success or error code.
 */
vs_status_e
vs_fldt_server_add_file_type(const vs_update_file_type_t *file_type,
                             vs_update_interface_t *update_context,
                             bool broadcast_file_info);

#ifdef __cplusplus
}
#endif

#endif // FLDT_SERVER

#endif // VS_SECURITY_SDK_SDMP_SERVICES_FLDT_SERVER_H
