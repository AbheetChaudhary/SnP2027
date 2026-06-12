S&P Oakland 2027

openfhe: the openfhe version running outside natively

openfhe-sgx: the version of openfhe that we ported to run inside SGX enclaves

stages-microbenchmarks: doing cache and allocation benchmarks for different stages
of a simple openfhe application running outside enclave.

stages-outside-enclave: doing timing benchmarks for different stages of a simple 
openfhe application running OUTSIDE enclaves natively.

stages-inside-enclave: doing timing benchmarks for differnet stages of a simple 
openfhe application running INSIDE SGX enclave.

Make sure that you have intel sgx installed at the correct location. Usually
the correct location is `/opt/intel/`. If you are on arch linux then it can 
be installed using:

```
yay -S intel-sgx-sdk-bin intel-sgx-psw-bin
```
