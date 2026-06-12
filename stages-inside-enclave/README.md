NOTE: update the path to OpenFHE-SGX installation in the `Makefile` on line 121,
it should point to the `installed` directory of the `openfhe-sgx` project
present at the root of our submission.

Then run `make SGX_DEBUG=0` to build `app` executable. Run it to get the runtimes of
different stages of openfhe inside enclave. Stages like context creation, key generation,
encryption, etc.

```
$ make SGX_DEBUG=0
```

Then follow the instruction from the make output to create a signed enclave that
could be loaded. Use this command to sign the enclave:

```
/opt/intel/sgxsdk/bin/x64/sgx_sign sign -key <your key> -enclave enclave.so -out <enclave.signed.so> -config Enclave/Enclave.config.xml
```
