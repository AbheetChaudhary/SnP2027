OPENFHE_INSTALL_PATH = /wintfs/papers/snp/anonymous-submission/openfhe/installed

main: main.cpp
	g++ main.cpp -I$(OPENFHE_INSTALL_PATH)/include/openfhe/pke \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/core/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/binfhe/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/cereal \
	-L$(OPENFHE_INSTALL_PATH)/lib -lOPENFHEpke -lOPENFHEcore \
	-lOPENFHEbinfhe -o main -O2 -ggdb

alloc: alloc_measurements.cpp
	gcc -shared -fPIC hook.c -ldl -o hook.so
	g++ alloc_measurements.cpp -I$(OPENFHE_INSTALL_PATH)/include/openfhe/pke \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/core/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/binfhe/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/cereal \
	-L$(OPENFHE_INSTALL_PATH)/lib -lOPENFHEpke -lOPENFHEcore \
	-lOPENFHEbinfhe -o alloc -O2 -ggdb hook.so

cache: cache_measurements.cpp
	g++ cache_measurements.cpp -I$(OPENFHE_INSTALL_PATH)/include/openfhe/pke \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/core/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/binfhe/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/cereal \
	-L$(OPENFHE_INSTALL_PATH)/lib -lOPENFHEpke -lOPENFHEcore \
	-lOPENFHEbinfhe -o cache -O2 -ggdb

clean:
	rm main
