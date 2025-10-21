#include "Crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <stdexcept>
#include <cstring>

namespace {
    static std::string b64encode(const unsigned char* data, size_t len) {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO* mem = BIO_new(BIO_s_mem());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        BIO_push(b64, mem);
        BIO_write(b64, data, (int)len);
        BIO_flush(b64);
        BUF_MEM* bptr;
        BIO_get_mem_ptr(b64, &bptr);
        std::string ret(bptr->data, bptr->length);
        BIO_free_all(b64);
        return ret;
    }

    static std::vector<unsigned char> b64decode(const std::string& in) {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO* mem = BIO_new_mem_buf(in.data(), (int)in.size());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        BIO_push(b64, mem);
        std::vector<unsigned char> out(in.size());
        int decoded = BIO_read(b64, out.data(), (int)out.size());
        if (decoded <= 0) {
            BIO_free_all(b64);
            throw std::runtime_error("Base64 decode failed");
        }
        out.resize(decoded);
        BIO_free_all(b64);
        return out;
    }
}

namespace Crypto {

std::string encrypt(const std::string& plaintext, const std::string& key) {
    const size_t KEY_LEN = 32; // AES-256
    const size_t IV_LEN = 12;  // recommended for GCM
    const size_t TAG_LEN = 16;

    if (key.size() < KEY_LEN) throw std::runtime_error("Key too short for AES-256");

    unsigned char iv[IV_LEN];
    if (RAND_bytes(iv, IV_LEN) != 1) {
        throw std::runtime_error("Failed to generate IV");
    }

    std::vector<unsigned char> out(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    unsigned char tag[TAG_LEN];

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptInit failed");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Set IV len failed");
    }

    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptInit key/iv failed");
    }

    int outlen = 0;
    if (EVP_EncryptUpdate(ctx, out.data(), &outlen, reinterpret_cast<const unsigned char*>(plaintext.data()), (int)plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptUpdate failed");
    }

    int tmplen = 0;
    if (EVP_EncryptFinal_ex(ctx, out.data() + outlen, &tmplen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptFinal failed");
    }
    outlen += tmplen;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LEN, tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Get TAG failed");
    }

    EVP_CIPHER_CTX_free(ctx);

    // compose blob = iv || tag || ciphertext
    std::vector<unsigned char> blob;
    blob.insert(blob.end(), iv, iv + IV_LEN);
    blob.insert(blob.end(), tag, tag + TAG_LEN);
    blob.insert(blob.end(), out.begin(), out.begin() + outlen);

    return b64encode(blob.data(), blob.size());
}

std::string decrypt(const std::string& blobBase64, const std::string& key) {
    const size_t KEY_LEN = 32;
    const size_t IV_LEN = 12;
    const size_t TAG_LEN = 16;

    if (key.size() < KEY_LEN) throw std::runtime_error("Key too short for AES-256");

    std::vector<unsigned char> blob = b64decode(blobBase64);
    if (blob.size() < IV_LEN + TAG_LEN) throw std::runtime_error("Invalid blob");

    const unsigned char* iv = blob.data();
    const unsigned char* tag = blob.data() + IV_LEN;
    const unsigned char* cipher = blob.data() + IV_LEN + TAG_LEN;
    size_t cipher_len = blob.size() - IV_LEN - TAG_LEN;

    std::vector<unsigned char> out(cipher_len + EVP_MAX_BLOCK_LENGTH);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptInit failed");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Set IV len failed");
    }

    if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptInit key/iv failed");
    }

    int outlen = 0;
    if (EVP_DecryptUpdate(ctx, out.data(), &outlen, cipher, (int)cipher_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptUpdate failed");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LEN, const_cast<unsigned char*>(tag)) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Set TAG failed");
    }

    int tmplen = 0;
    if (EVP_DecryptFinal_ex(ctx, out.data() + outlen, &tmplen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptFinal failed: authentication tag mismatch");
    }
    outlen += tmplen;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(reinterpret_cast<char*>(out.data()), outlen);
}

}
