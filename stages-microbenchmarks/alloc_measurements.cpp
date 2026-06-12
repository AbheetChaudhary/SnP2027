#include "openfhe.h"
#include <malloc.h>

extern "C" {
    struct StageMetrics {
        const char *stage_name;
        int         alloc_count;
        int         heap_size;
        int         peak_heap_size;
    };

    extern StageMetrics stages[10];
    extern int current_stage;
}

using namespace lbcrypto;

unsigned long long rdtsc_fenced(void) {
    uint32_t lower, upper;
    asm volatile (
        "mfence \n"
        "rdtsc \n"
        "mfence \n"
        : "=d" (upper), "=a" (lower)
        :: "memory");

    unsigned long long ticks = (((unsigned long long) upper) << 32) +
        (unsigned long long) (lower);

    return ticks;
}

void set_stage(const char* stage_name) {
    current_stage += 1;
    stages[current_stage].stage_name = stage_name;

    auto curr_stage_backup = current_stage;

    // disable logging.
    current_stage = -1;

    struct mallinfo2 mi = mallinfo2();
    stages[curr_stage_backup].heap_size = mi.uordblks;

    // re-enable logging
    current_stage = curr_stage_backup;
}

void reset_stage() {
    set_stage("SENTINEL_STAGE");
    current_stage = -1;
}

#define STAGE_COUNT 5

void stage_metrics() {
    for (size_t i = 0; i < STAGE_COUNT; i += 1) {
        auto stage = &stages[i];

        int heap_size_change = stages[i + 1].heap_size - stage->heap_size;

        int peak_growth = stage->peak_heap_size - stage->heap_size;

        printf("%10s: allocations: %5d, net heap growth: %5.2f MB,"
                " peak heap growth: %5.2f\n",
                stage->stage_name, stage->alloc_count, 
                (float) heap_size_change / (float) (1024 * 1024),
                (float) peak_growth / (float) (1024 * 1024));
    }
}

int main() {
    set_stage("context");
    // Sample Program: Step 1 - Set CryptoContext
    CCParams<CryptoContextBGVRNS> parameters;
    parameters.SetMultiplicativeDepth(2);
    parameters.SetPlaintextModulus(65537);

    CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);
    // Enable features that you wish to use
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);

    // printf("done context\n");
    set_stage("keygen");

    // Sample Program: Step 2 - Key Generation

    // Initialize Public Key Containers
    KeyPair<DCRTPoly> keyPair;

    // Generate a public/private key pair
    keyPair = cryptoContext->KeyGen();

    // Generate the relinearization key
    cryptoContext->EvalMultKeyGen(keyPair.secretKey);

    // Generate the rotation evaluation keys
    cryptoContext->EvalRotateKeyGen(keyPair.secretKey, {1, 2, -1, -2});

    // printf("done keygen\n");
    set_stage("encrypt");

    // Sample Program: Step 3 - Encryption

    // First plaintext vector is encoded
    std::vector<int64_t> vectorOfInts1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    Plaintext plaintext1               = cryptoContext->MakePackedPlaintext(vectorOfInts1);
    // Second plaintext vector is encoded
    std::vector<int64_t> vectorOfInts2 = {3, 2, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    Plaintext plaintext2               = cryptoContext->MakePackedPlaintext(vectorOfInts2);
    // Third plaintext vector is encoded
    std::vector<int64_t> vectorOfInts3 = {1, 2, 5, 2, 5, 6, 7, 8, 9, 10, 11, 12};
    Plaintext plaintext3               = cryptoContext->MakePackedPlaintext(vectorOfInts3);

    // The encoded vectors are encrypted
    auto ciphertext1 = cryptoContext->Encrypt(keyPair.publicKey, plaintext1);
    auto ciphertext2 = cryptoContext->Encrypt(keyPair.publicKey, plaintext2);
    auto ciphertext3 = cryptoContext->Encrypt(keyPair.publicKey, plaintext3);

    // printf("done encryption\n");
    set_stage("evaluate");

    // Sample Program: Step 4 - Evaluation

    // Homomorphic additions
    auto ciphertextAdd12     = cryptoContext->EvalAdd(ciphertext1, ciphertext2);
    auto ciphertextAddResult = cryptoContext->EvalAdd(ciphertextAdd12, ciphertext3);

    // Homomorphic multiplications
    // modulus switching is done automatically because by default the modulus
    // switching method is set to AUTO (rather than MANUAL)
    auto ciphertextMul12      = cryptoContext->EvalMult(ciphertext1, ciphertext2);
    auto ciphertextMultResult = cryptoContext->EvalMult(ciphertextMul12, ciphertext3);
    // Homomorphic rotations
    auto ciphertextRot1 = cryptoContext->EvalRotate(ciphertext1, 1);
    auto ciphertextRot2 = cryptoContext->EvalRotate(ciphertext1, 2);
    auto ciphertextRot3 = cryptoContext->EvalRotate(ciphertext1, -1);
    auto ciphertextRot4 = cryptoContext->EvalRotate(ciphertext1, -2);

    // printf("done eval\n");
    set_stage("decrypt");

    // Sample Program: Step 5 - Decryption

    // Decrypt the result of additions
    Plaintext plaintextAddResult;
    cryptoContext->Decrypt(keyPair.secretKey, ciphertextAddResult, &plaintextAddResult);

    // Decrypt the result of multiplications
    Plaintext plaintextMultResult;
    cryptoContext->Decrypt(keyPair.secretKey, ciphertextMultResult, &plaintextMultResult);

    // Decrypt the result of rotations
    Plaintext plaintextRot1;
    cryptoContext->Decrypt(keyPair.secretKey, ciphertextRot1, &plaintextRot1);
    Plaintext plaintextRot2;
    cryptoContext->Decrypt(keyPair.secretKey, ciphertextRot2, &plaintextRot2);
    Plaintext plaintextRot3;
    cryptoContext->Decrypt(keyPair.secretKey, ciphertextRot3, &plaintextRot3);
    Plaintext plaintextRot4;
    cryptoContext->Decrypt(keyPair.secretKey, ciphertextRot4, &plaintextRot4);

    plaintextRot1->SetLength(vectorOfInts1.size());
    plaintextRot2->SetLength(vectorOfInts1.size());
    plaintextRot3->SetLength(vectorOfInts1.size());
    plaintextRot4->SetLength(vectorOfInts1.size());

    // printf("done decryption\n");
    reset_stage();

    stage_metrics();

    return 0;

}


