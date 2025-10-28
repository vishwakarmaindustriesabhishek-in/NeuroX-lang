#ifndef NEUROX_SECURITY_H
#define NEUROX_SECURITY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Cryptographic algorithms
typedef enum {
    NRX_CRYPTO_AES_128,
    NRX_CRYPTO_AES_256,
    NRX_CRYPTO_CHACHA20,
} nrx_cipher_t;

typedef enum {
    NRX_HASH_SHA256,
    NRX_HASH_SHA512,
    NRX_HASH_BLAKE2B,
} nrx_hash_t;

// Key types
typedef enum {
    NRX_KEY_SYMMETRIC,
    NRX_KEY_PUBLIC,
    NRX_KEY_PRIVATE,
} nrx_key_type_t;

// Key structure
typedef struct {
    nrx_key_type_t type;
    uint8_t *data;
    size_t length;
} nrx_key_t;

// Encryption/Decryption
int nrx_encrypt(nrx_cipher_t cipher, const nrx_key_t *key, const uint8_t *iv,
                const uint8_t *plaintext, size_t plaintext_len,
                uint8_t *ciphertext, size_t *ciphertext_len);

int nrx_decrypt(nrx_cipher_t cipher, const nrx_key_t *key, const uint8_t *iv,
                const uint8_t *ciphertext, size_t ciphertext_len,
                uint8_t *plaintext, size_t *plaintext_len);

// Hashing
int nrx_hash(nrx_hash_t hash, const uint8_t *data, size_t len, uint8_t *digest);
int nrx_hmac(nrx_hash_t hash, const nrx_key_t *key, const uint8_t *data, 
             size_t len, uint8_t *mac);

// Digital signatures (Ed25519)
typedef struct {
    uint8_t public_key[32];
    uint8_t private_key[64];
} nrx_keypair_t;

int nrx_keypair_generate(nrx_keypair_t *keypair);
int nrx_sign(const nrx_keypair_t *keypair, const uint8_t *message, size_t len,
             uint8_t *signature);
int nrx_verify(const uint8_t *public_key, const uint8_t *message, size_t len,
               const uint8_t *signature);

// TLS/SSL context
typedef struct nrx_tls_context_t nrx_tls_context_t;

nrx_tls_context_t *nrx_tls_create(void);
void nrx_tls_free(nrx_tls_context_t *ctx);

int nrx_tls_set_ca_cert(nrx_tls_context_t *ctx, const char *ca_cert_path);
int nrx_tls_set_client_cert(nrx_tls_context_t *ctx, const char *cert_path, const char *key_path);
int nrx_tls_set_verify_mode(nrx_tls_context_t *ctx, bool verify_peer);

// Secure storage (encrypted key-value store)
typedef struct nrx_secure_storage_t nrx_secure_storage_t;

nrx_secure_storage_t *nrx_secure_storage_init(const char *storage_path, const nrx_key_t *master_key);
void nrx_secure_storage_deinit(nrx_secure_storage_t *storage);

int nrx_secure_storage_set(nrx_secure_storage_t *storage, const char *key, 
                           const uint8_t *value, size_t value_len);
int nrx_secure_storage_get(nrx_secure_storage_t *storage, const char *key,
                           uint8_t *value, size_t *value_len);
int nrx_secure_storage_delete(nrx_secure_storage_t *storage, const char *key);

// Access control
typedef enum {
    NRX_PERM_READ = 0x01,
    NRX_PERM_WRITE = 0x02,
    NRX_PERM_EXECUTE = 0x04,
    NRX_PERM_ADMIN = 0x08,
} nrx_permission_t;

typedef struct {
    char subject[64];       // User/robot ID
    char resource[128];     // Resource path
    uint8_t permissions;    // Bitfield of nrx_permission_t
} nrx_acl_entry_t;

typedef struct nrx_acl_t nrx_acl_t;

nrx_acl_t *nrx_acl_create(void);
void nrx_acl_free(nrx_acl_t *acl);

int nrx_acl_add_entry(nrx_acl_t *acl, const nrx_acl_entry_t *entry);
int nrx_acl_remove_entry(nrx_acl_t *acl, const char *subject, const char *resource);
bool nrx_acl_check_permission(nrx_acl_t *acl, const char *subject, 
                              const char *resource, nrx_permission_t perm);

// Audit logging
typedef enum {
    NRX_AUDIT_AUTH,
    NRX_AUDIT_ACCESS,
    NRX_AUDIT_COMMAND,
    NRX_AUDIT_CONFIG_CHANGE,
    NRX_AUDIT_SECURITY_EVENT,
} nrx_audit_type_t;

typedef struct {
    nrx_audit_type_t type;
    uint64_t timestamp_us;
    char subject[64];
    char action[128];
    char resource[128];
    bool success;
    char details[256];
} nrx_audit_entry_t;

typedef struct nrx_audit_log_t nrx_audit_log_t;

nrx_audit_log_t *nrx_audit_log_init(const char *log_path);
void nrx_audit_log_deinit(nrx_audit_log_t *log);

int nrx_audit_log_write(nrx_audit_log_t *log, const nrx_audit_entry_t *entry);
int nrx_audit_log_query(nrx_audit_log_t *log, nrx_audit_type_t type,
                        uint64_t start_time, uint64_t end_time,
                        nrx_audit_entry_t *entries, size_t max_entries);

// Firmware signing and verification
typedef struct {
    uint8_t version[4];
    uint8_t hash[32];
    uint8_t signature[64];
    uint32_t size;
    uint64_t timestamp;
} nrx_firmware_header_t;

int nrx_firmware_sign(const uint8_t *firmware, size_t firmware_len,
                      const nrx_keypair_t *keypair, nrx_firmware_header_t *header);
int nrx_firmware_verify(const uint8_t *firmware, size_t firmware_len,
                        const nrx_firmware_header_t *header, const uint8_t *public_key);

// OTA update with dual-bank
typedef enum {
    NRX_OTA_IDLE,
    NRX_OTA_DOWNLOADING,
    NRX_OTA_VERIFYING,
    NRX_OTA_INSTALLING,
    NRX_OTA_COMPLETE,
    NRX_OTA_ERROR,
} nrx_ota_state_t;

typedef struct nrx_ota_t nrx_ota_t;

nrx_ota_t *nrx_ota_init(const uint8_t *public_key);
void nrx_ota_deinit(nrx_ota_t *ota);

int nrx_ota_start(nrx_ota_t *ota, const char *firmware_url);
nrx_ota_state_t nrx_ota_get_state(nrx_ota_t *ota);
float nrx_ota_get_progress(nrx_ota_t *ota);
int nrx_ota_apply(nrx_ota_t *ota);  // Reboot into new firmware
int nrx_ota_rollback(nrx_ota_t *ota);  // Revert to previous firmware

// Secure boot
int nrx_secure_boot_verify(void);
int nrx_secure_boot_lock(void);

// Random number generation (CSPRNG)
int nrx_random_bytes(uint8_t *buffer, size_t len);
uint32_t nrx_random_uint32(void);
float nrx_random_float(void);  // [0.0, 1.0)

// Key derivation (PBKDF2, HKDF)
int nrx_derive_key(nrx_hash_t hash, const uint8_t *password, size_t password_len,
                   const uint8_t *salt, size_t salt_len, uint32_t iterations,
                   uint8_t *derived_key, size_t key_len);

// Secure erase
void nrx_secure_zero(void *ptr, size_t len);

#endif // NEUROX_SECURITY_H
