#pragma once
#include <string>
#include <vector>

namespace Crypto {
    // Encrypt plaintext using AES-256-GCM.
    // Returns base64(nonce || tag || ciphertext) as string.
    std::string encrypt(const std::string& plaintext, const std::string& key);

    // Decrypt previously encrypted value (base64(nonce||tag||ciphertext)) using key.
    std::string decrypt(const std::string& blobBase64, const std::string& key);
}
