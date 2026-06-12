OPENFHE_INSTALL_PATH = /wintfs/papers/snp/anonymous-submission/openfhe/installed

all: main.cpp
	g++ main.cpp -I$(OPENFHE_INSTALL_PATH)/include/openfhe/pke \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/core/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/binfhe/ \
	-I$(OPENFHE_INSTALL_PATH)/include/openfhe/cereal \
	-L$(OPENFHE_INSTALL_PATH)/lib -lOPENFHEpke -lOPENFHEcore \
	-lOPENFHEbinfhe -o main -O2
