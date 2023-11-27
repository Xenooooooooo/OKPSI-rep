#pragma once

#include "cryptoTools/Common/BitVector.h"
#include "cryptoTools/Common/block.h"
#include <cryptoTools/Common/CLP.h>

namespace tests_libOTe
{
    void RBOKVS_Correctness_Test(const oc::CLP &cmd);
    void RBOKVS_Efficiency_Test(const oc::CLP &cmd);
}