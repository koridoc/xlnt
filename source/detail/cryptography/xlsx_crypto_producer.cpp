// Copyright (c) 2014-2017 Thomas Fussell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, WRISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE
//
// @license: http://www.opensource.org/licenses/mit-license.php
// @author: see AUTHORS file

#include <detail/constants.hpp>
#include <detail/unicode.hpp>
#include <detail/cryptography/aes.hpp>
#include <detail/cryptography/base64.hpp>
#include <detail/cryptography/compound_document.hpp>
#include <detail/cryptography/encryption_info.hpp>
#include <detail/cryptography/value_traits.hpp>
#include <detail/cryptography/xlsx_crypto_producer.hpp>
#include <detail/external/include_libstudxml.hpp>
#include <detail/serialization/vector_streambuf.hpp>
#include <detail/serialization/xlsx_producer.hpp>
#include <xlnt/utils/exceptions.hpp>

namespace {

using xlnt::detail::encryption_info;

encryption_info generate_encryption_info(const std::u16string &/*password*/)
{
    encryption_info result;

    result.is_agile = true;

    result.agile = encryption_info::agile_encryption_info{};

    result.agile.key_data.block_size = 16;
    result.agile.key_data.cipher_algorithm = "AES";
    result.agile.key_data.cipher_chaining = "ChainingModeCBC";
    result.agile.key_data.hash_algorithm = "SHA512";
    result.agile.key_data.hash_size = 64;
    result.agile.key_data.key_bits = 256;
    result.agile.key_data.salt_size = 16;
    result.agile.key_data.salt_value = 
    { 
        { 40, 183, 193, 64, 115, 97, 10, 177, 122, 50, 243, 123, 229, 145, 162, 247 }
    };

    result.agile.data_integrity.hmac_key = 
    {
        { 90, 206, 203, 147, 102, 81, 82, 14, 118, 94, 168, 38, 200, 79, 13, 147, 60,
        123, 167, 220, 17, 165, 124, 188, 206, 74, 98, 33, 156, 63, 220, 152, 180, 201,
        167, 183, 141, 252, 182, 55, 90, 189, 187, 167, 230, 186, 61, 239, 80, 49, 54,
        208, 52, 133, 232, 187, 117, 136, 213, 48, 133, 15, 7, 126 }
    };
    result.agile.data_integrity.hmac_value = 
    {
        { 49, 128, 174, 178, 161, 48, 1, 82, 241, 103, 72, 223, 103, 111, 204, 73,
        210, 70, 254, 43, 12, 134, 180, 201, 124, 153, 214, 115, 82, 184, 78, 2,
        166, 106, 69, 18, 173, 177, 40, 238, 243, 240, 3, 86, 145, 218, 223, 177,
        36, 34, 44, 159, 104, 163, 217, 42, 203, 135, 173, 14, 218, 172, 72, 224 }
    };

    result.agile.key_encryptor.spin_count = 100000;
    result.agile.key_encryptor.block_size = 16;
    result.agile.key_encryptor.cipher_algorithm = "AES";
    result.agile.key_encryptor.cipher_chaining = "ChainingModeCBC";
    result.agile.key_encryptor.hash = xlnt::detail::hash_algorithm::sha512;
    result.agile.key_encryptor.hash_size = 64;
    result.agile.key_encryptor.key_bits = 256;
    result.agile.key_encryptor.salt_size = 16;
    result.agile.key_encryptor.salt_value = 
    {
        { 98, 169, 85, 224, 173, 253, 2, 52, 199, 108, 195, 73, 116, 112, 72, 165 }
    };
    result.agile.key_encryptor.verifier_hash_input =
    {
        { 179, 105, 118, 193, 217, 180, 248, 7, 174, 45, 186, 17, 202, 101, 178, 12 }
    };
    result.agile.key_encryptor.verifier_hash_value =
    {
        { 82, 190, 235, 102, 30, 33, 103, 191, 3, 160, 153, 30, 127, 117, 8, 195, 65,
        245, 77, 219, 85, 28, 206, 236, 55, 86, 243, 49, 104, 128, 243, 138, 227, 113,
        82, 88, 88, 73, 243, 108, 193, 11, 84, 162, 235, 189, 9, 137, 151, 97, 43,
        137, 197, 72, 164, 192, 65, 252, 253, 227, 236, 242, 252, 179 }
    };
    result.agile.key_encryptor.encrypted_key_value =
    {
        { 220, 6, 106, 218, 31, 210, 9, 75, 28, 154, 173, 232, 190, 109, 112, 203, 25,
        5, 45, 152,75, 131, 122, 17, 166, 95, 117, 124, 121, 123, 32, 133 }
    };

    return result;
}

std::vector<std::uint8_t> write_agile_encryption_info(
    const encryption_info &info)
{
    static const auto &xmlns = xlnt::constants::ns("encryption");
    static const auto &xmlns_p = xlnt::constants::ns("encryption-password");

    std::vector<std::uint8_t> encryption_info;
    xlnt::detail::vector_ostreambuf encryption_info_buffer(encryption_info);
    std::ostream encryption_info_stream(&encryption_info_buffer);
    xml::serializer serializer(encryption_info_stream, "EncryptionInfo");

    serializer.start_element(xmlns, "encryption");

    const auto key_data = info.agile.key_data;
    serializer.start_element(xmlns, "keyData");
    serializer.attribute("saltSize", key_data.salt_size);
    serializer.attribute("blockSize", key_data.block_size);
    serializer.attribute("keyBits", key_data.key_bits);
    serializer.attribute("hashSize", key_data.hash_size);
    serializer.attribute("cipherAlgorithm", key_data.cipher_algorithm);
    serializer.attribute("cipherChaining", key_data.cipher_chaining);
    serializer.attribute("hashAlgorithm", key_data.hash_algorithm);
    serializer.attribute("saltValue",
        xlnt::detail::encode_base64(key_data.salt_value));
    serializer.end_element(xmlns, "keyData");

    const auto data_integrity = info.agile.data_integrity;
    serializer.start_element(xmlns, "dataIntegrity");
    serializer.attribute("encryptedHmacKey",
        xlnt::detail::encode_base64(data_integrity.hmac_key));
    serializer.attribute("encryptedHmacValue",
        xlnt::detail::encode_base64(data_integrity.hmac_value));
    serializer.end_element(xmlns, "dataIntegrity");

    const auto key_encryptor = info.agile.key_encryptor;
    serializer.start_element(xmlns, "keyEncryptors");
    serializer.start_element(xmlns, "keyEncryptor");
    serializer.attribute("uri", "");
    serializer.start_element(xmlns_p, "encryptedKey");
    serializer.attribute("spinCount", key_encryptor.spin_count);
    serializer.attribute("saltSize", key_encryptor.salt_size);
    serializer.attribute("blockSize", key_encryptor.block_size);
    serializer.attribute("keyBits", key_encryptor.key_bits);
    serializer.attribute("hashSize", key_encryptor.hash_size);
    serializer.attribute("cipherAlgorithm", key_encryptor.cipher_algorithm);
    serializer.attribute("cipherChaining", key_encryptor.cipher_chaining);
    serializer.attribute("hashAlgorithm", key_encryptor.hash);
    serializer.attribute("saltValue",
        xlnt::detail::encode_base64(key_encryptor.salt_value));
    serializer.attribute("encryptedVerifierHashInput",
        xlnt::detail::encode_base64(key_encryptor.verifier_hash_input));
    serializer.attribute("encryptedVerifierHashValue",
        xlnt::detail::encode_base64(key_encryptor.verifier_hash_value));
    serializer.attribute("encryptedKeyValue",
        xlnt::detail::encode_base64(key_encryptor.encrypted_key_value));
    serializer.end_element(xmlns_p, "encryptedKey");
    serializer.end_element(xmlns, "keyEncryptor");
    serializer.end_element(xmlns, "keyEncryptors");

    serializer.end_element(xmlns, "encryption");

    return encryption_info;
}

std::vector<std::uint8_t> write_standard_encryption_info(const encryption_info &info)
{
    xlnt::detail::byte_vector result;

    const auto version_major = std::uint16_t(4);
    const auto version_minor = std::uint16_t(2);
    const auto encryption_flags = std::uint32_t(0b00010000 & 0b00100000);

    result.write(version_major);
    result.write(version_minor);
    result.write(encryption_flags);

    const auto header_length = std::uint32_t(32); // calculate this!

    result.write(header_length);
    result.write(std::uint32_t(0)); // skip_flags
    result.write(std::uint32_t(0)); // size_extra
    result.write(std::uint32_t(0x0000660E));
    result.write(std::uint32_t(0x00008004));
    result.write(std::uint32_t(info.standard.key_bits));
    result.write(std::uint32_t(0x00000018));
    result.write(std::uint32_t(0));
    result.write(std::uint32_t(0));

    const auto provider = u"Microsoft Enhanced RSA and AES Cryptographic Provider";
    result.append(xlnt::detail::byte_vector::from(std::u16string(provider)).data());

    result.write(std::uint32_t(info.standard.salt.size()));
    result.append(info.standard.salt);

    result.append(info.standard.encrypted_verifier);

    result.write(std::uint32_t(20));
    result.append(info.standard.encrypted_verifier_hash);

    return result.data();
}

std::vector<std::uint8_t> encrypt_xlsx_agile(
    const encryption_info &info,
    const std::vector<std::uint8_t> &plaintext)
{
    auto key = info.calculate_key();

    auto padded = plaintext;
    padded.resize((plaintext.size() / 16 + (plaintext.size() % 16 == 0 ? 0 : 1)) * 16);
    auto ciphertext = xlnt::detail::aes_ecb_encrypt(padded, key);
    const auto length = static_cast<std::uint64_t>(plaintext.size());
    ciphertext.insert(ciphertext.begin(),
        reinterpret_cast<const std::uint8_t *>(&length),
        reinterpret_cast<const std::uint8_t *>(&length + sizeof(std::uint64_t)));

    return ciphertext;
}

std::vector<std::uint8_t> encrypt_xlsx_standard(
    const encryption_info &info,
    const std::vector<std::uint8_t> &plaintext)
{
    auto key = info.calculate_key();

    auto padded = plaintext;
    padded.resize((plaintext.size() / 16 + (plaintext.size() % 16 == 0 ? 0 : 1)) * 16);
    auto ciphertext = xlnt::detail::aes_ecb_encrypt(padded, key);
    const auto length = static_cast<std::uint64_t>(plaintext.size());
    ciphertext.insert(ciphertext.begin(),
        reinterpret_cast<const std::uint8_t *>(&length),
        reinterpret_cast<const std::uint8_t *>(&length + sizeof(std::uint64_t)));

    return ciphertext;
}

std::vector<std::uint8_t> encrypt_xlsx(
    const std::vector<std::uint8_t> &plaintext,
    const std::u16string &password)
{
    auto encryption_info = generate_encryption_info(password);
    encryption_info.password = u"secret";

    xlnt::detail::compound_document document;

    document.add_stream(u"EncryptionInfo", encryption_info.is_agile
        ? write_agile_encryption_info(encryption_info)
        : write_standard_encryption_info(encryption_info));
    document.add_stream(u"EncryptedPackage", encryption_info.is_agile
        ? encrypt_xlsx_agile(encryption_info, plaintext)
        : encrypt_xlsx_standard(encryption_info, plaintext));

    return document.save();
}

} // namespace

namespace xlnt {
namespace detail {

std::vector<std::uint8_t> XLNT_API encrypt_xlsx(
    const std::vector<std::uint8_t> &plaintext,
    const std::string &password)
{
    return ::encrypt_xlsx(plaintext, utf8_to_utf16(password));
}

void xlsx_producer::write(std::ostream &destination, const std::string &password)
{
    std::vector<std::uint8_t> plaintext;
    vector_ostreambuf plaintext_buffer(plaintext);
    std::ostream decrypted_stream(&plaintext_buffer);
    write(decrypted_stream);

    const auto ciphertext = ::encrypt_xlsx(plaintext, utf8_to_utf16(password));
    vector_istreambuf encrypted_buffer(ciphertext);

    destination << &encrypted_buffer;
}

} // namespace detail
} // namespace xlnt