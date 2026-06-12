NOTE: update the path to OpenFHE installation in the `Makefile` and `export`
file, it should point to the `installed` directory of the `openfhe` project
present at the root of our submission.

Then run `make` to build `main` executable. Run it to get the runtimes of
different stages of openfhe. Stages like context creation, key generation,
encryption, etc.

Before running the binary do not forget to run `source export`, otherwise the
`main` will not be able to locate OpenFHE shared libs.

We run it with openmp threads disabled, since threading support is not in our SGX
port of OpenFHE.

```
$ OMP_NUM_THREADS=1 ./main
done context
done keygen
done encryption
done eval
done decryption
context,3.219e+07
keygen,2.209e+08
encryption,5.951e+07
evaluation,1.826e+08
decryption,3.731e+07
total: 532624561

```
