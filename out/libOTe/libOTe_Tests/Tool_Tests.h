#pragma once

#include "cryptoTools/Common/BitVector.h"
#include "cryptoTools/Common/block.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace tests_libOTe
{
    void RBOKVS_Correctness_Test();
    void RBOKVS_Efficiency_Test();
}