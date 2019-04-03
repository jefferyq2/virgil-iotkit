/**
 * Copyright (C) 2016 Virgil Security Inc.
 *
 * Lead Maintainer: Virgil Security Inc. <support@virgilsecurity.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     (1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *     (2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *
 *     (3) Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <virgil/iot/initializer/VirgilCryptoSigner.h>
#include <virgil/iot/initializer/Crc16.h>
#include "virgil/crypto/VirgilSigner.h"
#include <virgil/sdk/crypto/Crypto.h>

using virgil::iot::initializer::VirgilCryptoSigner;
using virgil::iot::initializer::VirgilByteArray;
using virgil::crypto::VirgilSigner;
using virgil::sdk::crypto::Crypto;

VirgilCryptoSigner::VirgilCryptoSigner(std::shared_ptr<virgil::sdk::crypto::Crypto> crypto,
                                       sdk::crypto::keys::PrivateKey privateKey)
    : crypto_(std::move(crypto)), privateKey_(std::move(privateKey)) {
}

VirgilByteArray VirgilCryptoSigner::sign(const VirgilByteArray &data) {
    auto signer =  VirgilSigner(VirgilHashAlgorithm::SHA256);
    auto privateKeyData = crypto_->exportPrivateKey(privateKey_);
    return signer.sign(data, privateKeyData);
}

bool VirgilCryptoSigner::verify(const VirgilByteArray &data, const VirgilByteArray &signature, const VirgilByteArray &publicKey) {
    auto signer = VirgilSigner(virgil::crypto::foundation::VirgilHash::Algorithm::SHA256);
    return signer.verify(data, signature, publicKey);
}

uint16_t VirgilCryptoSigner::signerId() {
    auto publicKey = crypto_->extractPublicKeyFromPrivateKey(privateKey_);
    auto publicKeyBytes = crypto_->exportPublicKey(publicKey);
    uint8_t * tinyKey = publicKeyBytes.data() + publicKeyBytes.size() - 64;

    return Crc16::calc(tinyKey, 64);
}

VirgilByteArray VirgilCryptoSigner::publicKeyFull() {
    auto publicKey = crypto_->extractPublicKeyFromPrivateKey(privateKey_);
    return crypto_->exportPublicKey(publicKey);
}

