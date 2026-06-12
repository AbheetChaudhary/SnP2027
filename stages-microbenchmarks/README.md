NOTE: update the path to OpenFHE installation in the `Makefile` and `export`
file, it should point to the `installed` directory of the `openfhe` project
present at the root of our submission.

Then do `make cache` or `make alloc` to build `cache` and `alloc` binaries. They 
are used to provide the cache and memory allocation benchmarks respectively.

Before running the binary do not forget to run `source export`, otherwise the
`cache` and `alloc` executables will not be able to locate OpenFHE shared libs.

We run it with openmp threads disabled, since threading support is not in our SGX
port of OpenFHE.

```
$ OMP_NUM_THREADS=1 ./cache
$ OMP_NUM_THREADS=1 ./alloc
```

