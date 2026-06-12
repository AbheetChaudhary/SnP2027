/*
 * Copyright (C) 2011-2021 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "Enclave.h"
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>

#include <ostream>
#include <stdio.h>

#include <iostream>
#include <iostream>

#include <cstdio>
#include <cassert>
#include <cstdlib>

#include "openfhe.h"

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

#include <cstddef>
#include <cstdint>
#include <new>

#include <new>
#include <atomic>
#include <cstddef>
#include <mutex>

#define HEAP_SIZE (80 * 1024 * 1024)

struct Block {
    size_t size;
    bool free;
    Block* next;
};

// Align to 16 bytes for safety
const size_t ALIGNMENT = 16;
const size_t BLOCK_SIZE = (sizeof(Block) + ALIGNMENT - 1) & ~(ALIGNMENT - 1);

struct Arena {
    alignas(std::max_align_t) char data[HEAP_SIZE] = { 1 };
    Block* free_list;
    std::mutex mtx; // Needed for coalescing/searching logic safety

    Arena() {
        // data[0] = 0;
        // Initialize the entire 128MB as one giant free block
        free_list = reinterpret_cast<Block*>(data);
        free_list->size = HEAP_SIZE - BLOCK_SIZE;
        free_list->free = true;
        free_list->next = nullptr;
    }
};

Arena& get_global_arena() {
    static Arena instance;
    return instance;
}

void* operator new(size_t size) {
    // printf("allocating: %zu\n", size);
    Arena& arena = get_global_arena();
    std::lock_guard<std::mutex> lock(arena.mtx);

    // 1. Align the requested size
    size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);

    Block* curr = arena.free_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            // Can we split this block?
            // We need enough room for the requested size + another header + minimum data
            if (curr->size >= (size + BLOCK_SIZE + ALIGNMENT)) {
                Block* next_block = reinterpret_cast<Block*>(reinterpret_cast<char*>(curr) + BLOCK_SIZE + size);
                next_block->size = curr->size - size - BLOCK_SIZE;
                next_block->free = true;
                next_block->next = curr->next;

                curr->size = size;
                curr->next = next_block;
            }

            curr->free = false;
            return reinterpret_cast<void*>(reinterpret_cast<char*>(curr) + BLOCK_SIZE);
        }
        curr = curr->next;
    }

    throw std::bad_alloc();
}

void operator delete(void* ptr) {
    if (!ptr) return;

    Arena& arena = get_global_arena();
    std::lock_guard<std::mutex> lock(arena.mtx);

    // Get the header by stepping back from the pointer
    Block* curr = reinterpret_cast<Block*>(static_cast<char*>(ptr) - BLOCK_SIZE);
    curr->free = true;

    // --- Coalescing logic ---
    Block* iter = arena.free_list;
    while (iter && iter->next) {
        if (iter->free && iter->next->free) {
            // Merge iter with iter->next
            iter->size += BLOCK_SIZE + iter->next->size;
            iter->next = iter->next->next;
            // Don't move iter forward yet, it might merge with the NEW next
        } else {
            iter = iter->next;
        }
    }
}

// Boilerplate overrides
void* operator new[](size_t size) { return operator new(size); }
void operator delete[](void* ptr) { operator delete(ptr); }
void operator delete(void* ptr, size_t) { operator delete(ptr); }
void operator delete[](void* ptr, size_t) { operator delete(ptr); }

namespace bfv {
    int sample_code(void) {
        using namespace lbcrypto;

        /*
        unsigned long long begin;
        ocall_rdtsc(&begin);
        */
        auto begin_context = rdtsc_fenced();

        // Sample Program: Step 1: Set CryptoContext
        CCParams<CryptoContextBFVRNS> parameters;
        parameters.SetPlaintextModulus(65537);
        parameters.SetMultiplicativeDepth(2);

        CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);

        // Enable features that you wish to use
        cryptoContext->Enable(PKE);
        cryptoContext->Enable(KEYSWITCH);
        cryptoContext->Enable(LEVELEDSHE);

        auto end_context = rdtsc_fenced();

        // Sample Program: Step 2: Key Generation

        auto begin_keygen = rdtsc_fenced();
        // Initialize Public Key Containers
        KeyPair<DCRTPoly> keyPair;

        // Generate a public/private key pair
        keyPair = cryptoContext->KeyGen();

        // unsigned long long end;
        // ocall_rdtsc(&end);

        // Generate the relinearization key
        cryptoContext->EvalMultKeyGen(keyPair.secretKey);

        // Generate the rotation evaluation keys
        cryptoContext->EvalRotateKeyGen(keyPair.secretKey, {1, 2, -1, -2});
        auto end_keygen = rdtsc_fenced();

        // Sample Program: Step 3: Encryption
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

        // Sample Program: Step 4: Evaluation
        auto begin_evaluation = rdtsc_fenced();

        // Homomorphic additions
        auto ciphertextAdd12     = cryptoContext->EvalAdd(ciphertext1, ciphertext2);
        auto ciphertextAddResult = cryptoContext->EvalAdd(ciphertextAdd12, ciphertext3);

        // Homomorphic multiplications
        auto ciphertextMul12      = cryptoContext->EvalMult(ciphertext1, ciphertext2);
        auto ciphertextMultResult = cryptoContext->EvalMult(ciphertextMul12, ciphertext3);

        // Homomorphic rotations
        auto ciphertextRot1 = cryptoContext->EvalRotate(ciphertext1, 1);
        auto ciphertextRot2 = cryptoContext->EvalRotate(ciphertext1, 2);
        auto ciphertextRot3 = cryptoContext->EvalRotate(ciphertext1, -1);
        auto ciphertextRot4 = cryptoContext->EvalRotate(ciphertext1, -2);

        auto end_evaluation = rdtsc_fenced();

        // Sample Program: Step 5: Decryption
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

        /*
        std::ostringstream sgx_cout;

        sgx_cout << "Plaintext #1: " << plaintext1 << std::endl;
        sgx_cout << "Plaintext #2: " << plaintext2 << std::endl;
        sgx_cout << "Plaintext #3: " << plaintext3 << std::endl;

        // Output results
        sgx_cout << "\nResults of homomorphic computations" << std::endl;
        sgx_cout << "#1 + #2 + #3: " << plaintextAddResult << std::endl;
        sgx_cout << "#1 * #2 * #3: " << plaintextMultResult << std::endl;
        sgx_cout << "Left rotation of #1 by 1: " << plaintextRot1 << std::endl;
        sgx_cout << "Left rotation of #1 by 2: " << plaintextRot2 << std::endl;
        sgx_cout << "Right rotation of #1 by 1: " << plaintextRot3 << std::endl;
        sgx_cout << "Right rotation of #1 by 2: " << plaintextRot4 << std::endl;

        std::string temp_str = sgx_cout.str();
        const char *buffer = temp_str.c_str();
        printf(buffer);
        */

        // unsigned long long end = rdtsc_fenced();

        // return (end - begin);
        printf("context,%.4g\n", (double)(end_context - begin_context));
        printf("keygen,%.4g\n", (double)(end_keygen - begin_keygen));
        printf("encryption,%.4g\n", (double)(end_encryption - begin_encryption));
        printf("evaluation,%.4g\n", (double)(end_evaluation - begin_evaluation));
        printf("decryption,%.4g\n", (double)(end_decryption - begin_decryption));

        return 0;

        // return 42;
    }
}

auto ecall_openfhe_bfv_sample_code(void) -> int {
    // throw 1;
    printf("========================BFV Sample Code========================\n");

    const int ticks = bfv::sample_code();
    (void) ticks;

    // printf("ticks: %lu\n", ticks);

    // printf("===============================================================\n");

    return 42;
}

namespace bgv {
    int sample_code(void) {
        using namespace lbcrypto;
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

        /*
        std::ostringstream sgx_cout;

        sgx_cout << "Plaintext #1: " << plaintext1 << std::endl;
        sgx_cout << "Plaintext #2: " << plaintext2 << std::endl;
        sgx_cout << "Plaintext #3: " << plaintext3 << std::endl;

        // Output results
        sgx_cout << "\nResults of homomorphic computations" << std::endl;
        sgx_cout << "#1 + #2 + #3: " << plaintextAddResult << std::endl;
        sgx_cout << "#1 * #2 * #3: " << plaintextMultResult << std::endl;
        sgx_cout << "Left rotation of #1 by 1: " << plaintextRot1 << std::endl;
        sgx_cout << "Left rotation of #1 by 2: " << plaintextRot2 << std::endl;
        sgx_cout << "Right rotation of #1 by 1: " << plaintextRot3 << std::endl;
        sgx_cout << "Right rotation of #1 by 2: " << plaintextRot4 << std::endl;

        std::string temp_str = sgx_cout.str();
        const char *buffer = temp_str.c_str();
        printf(buffer);
        */

        printf("context,%.4g cycles\n", (double)(end_context - begin_context));
        printf("keygen,%.4g cycles\n", (double)(end_keygen - begin_keygen));
        printf("encryption,%.4g cycles\n", (double)(end_encryption - begin_encryption));
        printf("evaluation,%.4g cycles\n", (double)(end_evaluation - begin_evaluation));
        printf("decryption,%.4g cycles\n", (double)(end_decryption - begin_decryption));

        return 0;
    }
}

auto ecall_openfhe_bgv_sample_code(void) -> int {
    printf("========================BGV Sample Code========================\n");

    const int result = bgv::sample_code();

    // printf("===============================================================\n");

    return 42;
}

namespace ckks_real {
    int sample_code(void) {
        using namespace lbcrypto;
        // Step 1: Setup CryptoContext

        // A. Specify main parameters
        /* A1) Multiplicative depth:
       * The CKKS scheme we setup here will work for any computation
       * that has a multiplicative depth equal to 'multDepth'.
       * This is the maximum possible depth of a given multiplication,
       * but not the total number of multiplications supported by the
       * scheme.
       *
       * For example, computation f(x, y) = x^2 + x*y + y^2 + x + y has
       * a multiplicative depth of 1, but requires a total of 3 multiplications.
       * On the other hand, computation g(x_i) = x1*x2*x3*x4 can be implemented
       * either as a computation of multiplicative depth 3 as
       * g(x_i) = ((x1*x2)*x3)*x4, or as a computation of multiplicative depth 2
       * as g(x_i) = (x1*x2)*(x3*x4).
       *
       * For performance reasons, it's generally preferable to perform operations
       * in the shorted multiplicative depth possible.
       */
        uint32_t multDepth = 1;

        /* A2) Bit-length of scaling factor.
       * CKKS works for real numbers, but these numbers are encoded as integers.
       * For instance, real number m=0.01 is encoded as m'=round(m*D), where D is
       * a scheme parameter called scaling factor. Suppose D=1000, then m' is 10 (an
       * integer). Say the result of a computation based on m' is 130, then at
       * decryption, the scaling factor is removed so the user is presented with
       * the real number result of 0.13.
       *
       * Parameter 'scaleModSize' determines the bit-length of the scaling
       * factor D, but not the scaling factor itself. The latter is implementation
       * specific, and it may also vary between ciphertexts in certain versions of
       * CKKS (e.g., in FLEXIBLEAUTO).
       *
       * Choosing 'scaleModSize' depends on the desired accuracy of the
       * computation, as well as the remaining parameters like multDepth or security
       * standard. This is because the remaining parameters determine how much noise
       * will be incurred during the computation (remember CKKS is an approximate
       * scheme that incurs small amounts of noise with every operation). The
       * scaling factor should be large enough to both accommodate this noise and
       * support results that match the desired accuracy.
       */
        uint32_t scaleModSize = 50;

        /* A3) Number of plaintext slots used in the ciphertext.
       * CKKS packs multiple plaintext values in each ciphertext.
       * The maximum number of slots depends on a security parameter called ring
       * dimension. In this instance, we don't specify the ring dimension directly,
       * but let the library choose it for us, based on the security level we
       * choose, the multiplicative depth we want to support, and the scaling factor
       * size.
       *
       * Please use method GetRingDimension() to find out the exact ring dimension
       * being used for these parameters. Give ring dimension N, the maximum batch
       * size is N/2, because of the way CKKS works.
       */
        uint32_t batchSize = 8;

        /* A4) Desired security level based on FHE standards.
       * This parameter can take four values. Three of the possible values
       * correspond to 128-bit, 192-bit, and 256-bit security, and the fourth value
       * corresponds to "NotSet", which means that the user is responsible for
       * choosing security parameters. Naturally, "NotSet" should be used only in
       * non-production environments, or by experts who understand the security
       * implications of their choices.
       *
       * If a given security level is selected, the library will consult the current
       * security parameter tables defined by the FHE standards consortium
       * (https://homomorphicencryption.org/introduction/) to automatically
       * select the security parameters. Please see "TABLES of RECOMMENDED
       * PARAMETERS" in  the following reference for more details:
       * http://homomorphicencryption.org/wp-content/uploads/2018/11/HomomorphicEncryptionStandardv1.1.pdf
       */
        CCParams<CryptoContextCKKSRNS> parameters;
        parameters.SetMultiplicativeDepth(multDepth);
        parameters.SetScalingModSize(scaleModSize);
        parameters.SetBatchSize(batchSize);

        CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);

        // Enable the features that you wish to use
        cc->Enable(PKE);
        cc->Enable(KEYSWITCH);
        cc->Enable(LEVELEDSHE);
        std::ostringstream sgx_cout;
        sgx_cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << std::endl << std::endl;

        // B. Step 2: Key Generation
        /* B1) Generate encryption keys.
       * These are used for encryption/decryption, as well as in generating
       * different kinds of keys.
       */
        auto keys = cc->KeyGen();

        /* B2) Generate the digit size
       * In CKKS, whenever someone multiplies two ciphertexts encrypted with key s,
       * we get a result with some components that are valid under key s, and
       * with an additional component that's valid under key s^2.
       *
       * In most cases, we want to perform relinearization of the multiplicaiton
       * result, i.e., we want to transform the s^2 component of the ciphertext so
       * it becomes valid under original key s. To do so, we need to create what we
       * call a relinearization key with the following line.
       */
        cc->EvalMultKeyGen(keys.secretKey);

        /* B3) Generate the rotation keys
       * CKKS supports rotating the contents of a packed ciphertext, but to do so,
       * we need to create what we call a rotation key. This is done with the
       * following call, which takes as input a vector with indices that correspond
       * to the rotation offset we want to support. Negative indices correspond to
       * right shift and positive to left shift. Look at the output of this demo for
       * an illustration of this.
       *
       * Keep in mind that rotations work over the batch size or entire ring dimension (if the batch size is not specified).
       * This means that, if ring dimension is 8 and batch
       * size is not specified, then an input (1,2,3,4,0,0,0,0) rotated by 2 will become
       * (3,4,0,0,0,0,1,2) and not (3,4,1,2,0,0,0,0).
       * If ring dimension is 8 and batch
       * size is set to 4, then the rotation of (1,2,3,4) by 2 will become (3,4,1,2).
       * Also, as someone can observe
       * in the output of this demo, since CKKS is approximate, zeros are not exact
       * - they're just very small numbers.
       */
        cc->EvalRotateKeyGen(keys.secretKey, {1, -2});

        // Step 3: Encoding and encryption of inputs

        // Inputs
        std::vector<double> x1 = {0.25, 0.5, 0.75, 1.0, 2.0, 3.0, 4.0, 5.0};
        std::vector<double> x2 = {5.0, 4.0, 3.0, 2.0, 1.0, 0.75, 0.5, 0.25};

        // Encoding as plaintexts
        Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(x1);
        Plaintext ptxt2 = cc->MakeCKKSPackedPlaintext(x2);

        sgx_cout << "Input x1: " << ptxt1 << std::endl;
        sgx_cout << "Input x2: " << ptxt2 << std::endl;

        // Encrypt the encoded vectors
        auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
        auto c2 = cc->Encrypt(keys.publicKey, ptxt2);

        // Step 4: Evaluation

        // Homomorphic addition
        auto cAdd = cc->EvalAdd(c1, c2);

        // Homomorphic subtraction
        auto cSub = cc->EvalSub(c1, c2);

        // Homomorphic scalar multiplication
        auto cScalar = cc->EvalMult(c1, 4.0);

        // Homomorphic multiplication
        auto cMul = cc->EvalMult(c1, c2);

        // Homomorphic rotations
        auto cRot1 = cc->EvalRotate(c1, 1);
        auto cRot2 = cc->EvalRotate(c1, -2);

        // Step 5: Decryption and output
        Plaintext result;
        // We set the cout precision to 8 decimal digits for a nicer output.
        // If you want to see the error/noise introduced by CKKS, bump it up
        // to 15 and it should become visible.
        sgx_cout.precision(8);

        sgx_cout << std::endl << "Results of homomorphic computations: " << std::endl;

        cc->Decrypt(keys.secretKey, c1, &result);
        result->SetLength(batchSize);
        sgx_cout << "x1 = " << result;
        sgx_cout << "Estimated precision in bits: " << result->GetLogPrecision() << std::endl;

        // Decrypt the result of addition
        cc->Decrypt(keys.secretKey, cAdd, &result);
        result->SetLength(batchSize);
        sgx_cout << "x1 + x2 = " << result;
        sgx_cout << "Estimated precision in bits: " << result->GetLogPrecision() << std::endl;

        // Decrypt the result of subtraction
        cc->Decrypt(keys.secretKey, cSub, &result);
        result->SetLength(batchSize);
        sgx_cout << "x1 - x2 = " << result << std::endl;

        // Decrypt the result of scalar multiplication
        cc->Decrypt(keys.secretKey, cScalar, &result);
        result->SetLength(batchSize);
        sgx_cout << "4 * x1 = " << result << std::endl;

        // Decrypt the result of multiplication
        cc->Decrypt(keys.secretKey, cMul, &result);
        result->SetLength(batchSize);
        sgx_cout << "x1 * x2 = " << result << std::endl;

        // Decrypt the result of rotations

        cc->Decrypt(keys.secretKey, cRot1, &result);
        result->SetLength(batchSize);
        sgx_cout << std::endl << "In rotations, very small outputs (~10^-10 here) correspond to 0's:" << std::endl;
        sgx_cout << "x1 rotate by 1 = " << result << std::endl;

        cc->Decrypt(keys.secretKey, cRot2, &result);
        result->SetLength(batchSize);
        sgx_cout << "x1 rotate by -2 = " << result << std::endl;

        std::string temp_str = sgx_cout.str();
        const char *buffer = temp_str.c_str();
        printf(buffer);

        return 42;
    }
}

auto ecall_openfhe_ckks_real_sample_code(void) -> int {
    printf("=====================CKKS(Real) Sample Code====================\n");

    const int result = ckks_real::sample_code();

    printf("===============================================================\n");

    return result;
}
/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

