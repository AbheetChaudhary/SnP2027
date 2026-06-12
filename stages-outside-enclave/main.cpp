#include "openfhe.h"

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

int main() {
    auto begin = rdtsc_fenced();
    // Sample Program: Step 1 - Set CryptoContext
    auto begin_context = rdtsc_fenced();
    CCParams<CryptoContextBGVRNS> parameters;
    parameters.SetMultiplicativeDepth(2);
    parameters.SetPlaintextModulus(65537);

    CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);
    // Enable features that you wish to use
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);
    auto end_context = rdtsc_fenced();

    printf("done context\n");

    // Sample Program: Step 2 - Key Generation

    // Initialize Public Key Containers
    auto begin_keygen = rdtsc_fenced();
    KeyPair<DCRTPoly> keyPair;

    // Generate a public/private key pair
    keyPair = cryptoContext->KeyGen();

    // Generate the relinearization key
    cryptoContext->EvalMultKeyGen(keyPair.secretKey);

    // Generate the rotation evaluation keys
    cryptoContext->EvalRotateKeyGen(keyPair.secretKey, {1, 2, -1, -2});
    auto end_keygen = rdtsc_fenced();

    printf("done keygen\n");

    // Sample Program: Step 3 - Encryption

    auto begin_encryption = rdtsc_fenced();
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
    auto end_encryption = rdtsc_fenced();

    printf("done encryption\n");

    // Sample Program: Step 4 - Evaluation

    auto begin_evaluation = rdtsc_fenced();
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

    auto end_evaluation = rdtsc_fenced();
    printf("done eval\n");

    // Sample Program: Step 5 - Decryption
    auto begin_decryption = rdtsc_fenced();

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

    auto end_decryption = rdtsc_fenced();
    printf("done decryption\n");

    printf("context,%.4g cycles\n", (double)(end_context - begin_context));
    printf("keygen,%.4g cycles\n", (double)(end_keygen - begin_keygen));
    printf("encryption,%.4g cycles\n", (double)(end_encryption - begin_encryption));
    printf("evaluation,%.4g cycles\n", (double)(end_evaluation - begin_evaluation));
    printf("decryption,%.4g cycles\n", (double)(end_decryption - begin_decryption));

    auto end = rdtsc_fenced();
    printf("total: %zu cycles\n", end - begin);

    return 0;
}
