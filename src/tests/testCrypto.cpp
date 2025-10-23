#include "../core/Crypto.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

void testBasicEncryptDecrypt() {
    std::cout << "Testing basic encryption/decryption..." << std::endl;
    const std::string key(32, 'K');  // 32-byte test key
    const std::string plaintext = "Hello, World!";
    
    std::string encrypted = Crypto::encrypt(plaintext, key);
    assert(!encrypted.empty() && "Encryption produced empty result");
    assert(encrypted != plaintext && "Encrypted text matches plaintext");
    
    std::string decrypted = Crypto::decrypt(encrypted, key);
    assert(decrypted == plaintext && "Decryption failed to recover original text");
}

void testEmptyString() {
    std::cout << "Testing empty string..." << std::endl;
    const std::string key(32, 'K');
    const std::string plaintext = "";
    
    std::string encrypted = Crypto::encrypt(plaintext, key);
    std::string decrypted = Crypto::decrypt(encrypted, key);
    assert(decrypted == plaintext && "Empty string encryption/decryption failed");
}

void testLongString() {
    std::cout << "Testing long string..." << std::endl;
    const std::string key(32, 'K');
    std::string plaintext(1024, 'X');  // 1KB string
    
    std::string encrypted = Crypto::encrypt(plaintext, key);
    std::string decrypted = Crypto::decrypt(encrypted, key);
    assert(decrypted == plaintext && "Long string encryption/decryption failed");
}

void testWrongKey() {
    std::cout << "Testing wrong decryption key..." << std::endl;
    const std::string key1(32, 'K');
    const std::string key2(32, 'X');
    const std::string plaintext = "Secret message";
    
    std::string encrypted = Crypto::encrypt(plaintext, key1);
    try {
        std::string decrypted = Crypto::decrypt(encrypted, key2);
        assert(false && "Decryption with wrong key should fail");
    } catch (const std::exception& e) {
        // Expected exception
    }
}

void testInvalidKey() {
    std::cout << "Testing invalid key size..." << std::endl;
    const std::string shortKey = "tooshort";
    const std::string plaintext = "Test";
    
    try {
        std::string encrypted = Crypto::encrypt(plaintext, shortKey);
        assert(false && "Encryption with short key should fail");
    } catch (const std::exception& e) {
        // Expected exception
    }
}

void testMultiBlockContent() {
    std::cout << "Testing multi-block content..." << std::endl;
    const std::string key(32, 'K');
    const std::string plaintext = 
        "Line 1\n"
        "Line 2\n"
        "\n"
        "Block 2 Line 1\n"
        "Block 2 Line 2\n";
    
    std::string encrypted = Crypto::encrypt(plaintext, key);
    std::string decrypted = Crypto::decrypt(encrypted, key);
    assert(decrypted == plaintext && "Multi-block content encryption/decryption failed");
}

int main() {
    std::cout << "\n=== Starting Crypto Tests ===\n";
    
    try {
        testBasicEncryptDecrypt();
        testEmptyString();
        testLongString();
        testWrongKey();
        testInvalidKey();
        testMultiBlockContent();
        
        std::cout << "Crypto tests completed successfully!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in Crypto tests: " << e.what() << std::endl;
        return 1;
    }
}