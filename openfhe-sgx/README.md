Building

- Go to `build` directory and run the following command
```
cmake -DCMAKE_INSTALL_PREFIX=../installed
```

- Then `make -j6` to build.

- Then `make install`, it might fail. Follow the steps in `fix_cmake_install` from
  the `openfhe-sgx` directory.

