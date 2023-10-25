#include "OT_Tests.h"

#include "libOTe/Base/BaseOT.h"
#include "libOTe/Tools/Tools.h"
#include "libOTe/Tools/LinearCode.h"

#include <cryptoTools/Crypto/RBOKVS.h>

#include "Common.h"
#include <thread>
#include <vector>
#include <cryptoTools/Common/BitVector.h>
#include <cryptoTools/Common/Matrix.h>
#include <cryptoTools/Common/Timer.h>

#include <cryptoTools/Crypto/BLAKE3/blake3.h>

#ifdef GetMessage
#undef GetMessage
#endif

#ifdef  _MSC_VER
#pragma warning(disable: 4800)
#endif //  _MSC_VER


using namespace osuCrypto;
using namespace boost;

namespace tests_libOTe
{

    void RBOKVS_Correctness_Test(){
#ifdef CRYPTOTOOLS_RBOKVS_H
        u64 n = 1 << 16;
        RBOKVS okvsTest;
        // epsilon \in {0.03, 0.05, 0.07, 0.1} 
        okvsTest.init(n, 0.1, 40, sysRandomSeed());

        // print n, m, w
        std::cout << "\nn: " << okvsTest.mN << std::endl;
        std::cout << "m: " << okvsTest.mSize << std::endl;
        std::cout << "w: " << okvsTest.mW << std::endl;

        // test data
        std::unique_ptr<block[]> keys(new block[n]);
        std::unique_ptr<block[]> values(new block[n]);
        std::unique_ptr<block[]> codeWords(new block[okvsTest.mSize]);
        std::unique_ptr<block[]> output(new block[n]);

        PRNG prng(sysRandomSeed());
        prng.get(keys.get(), n);
        prng.get(values.get(), n);
        
        okvsTest.setSeed(prng.get<block>());
        okvsTest.mTimer.reset();
        // encode
        if (okvsTest.encode(keys.get(), values.get(), codeWords.get()) == EncodeStatus::FAIL){
            throw std::runtime_error("encoding is failed");
        }

        // decode
        for (u64 i = 0; i < n; ++i){
            if (okvsTest.decode(codeWords.get(), keys[i]) != values[i]){
                throw std::runtime_error("decoding is not correct");
            }
        }

        // batch decode
        okvsTest.decode(codeWords.get(), keys.get(), n, output.get(), 8);
        for (u64 i = 0; i < n; ++i){
            if (output[i] != values[i]){
                throw std::runtime_error("batch decoding is wrong");
                // std::cout << i << std::endl;
            }
        }

#else
        throw UnitTestSkipped("CRYPTOTOOLS_RBOKVS_H is not defined.");
#endif
    }

    void RBOKVS_Efficiency_Test(){
#ifdef CRYPTOTOOLS_RBOKVS_H
        u64 n = 1 << 20;
        RBOKVS okvsTest;
        // epsilon \in {0.03, 0.05, 0.07, 0.1} 
        okvsTest.init(n, 0.1, 40, sysRandomSeed());

        // print n, m, w
        std::cout << "\nn: " << okvsTest.mN << std::endl;
        std::cout << "m: " << okvsTest.mSize << std::endl;
        std::cout << "w: " << okvsTest.mW << std::endl;

        // test data
        std::unique_ptr<block[]> keys(new block[n]);
        std::unique_ptr<block[]> values(new block[n]);
        std::unique_ptr<block[]> codeWords(new block[okvsTest.mSize]);
        std::unique_ptr<block[]> output(new block[n]);

        PRNG prng(sysRandomSeed());
        prng.get(keys.get(), n);
        prng.get(values.get(), n);

        u64 numFails = 0, numTrials = 20;
        
        Timer timer;
        timer.setTimePoint("start");
        for (u64 i = 0; i < numTrials; ++i){
            okvsTest.setSeed(prng.get<block>());
            okvsTest.mTimer.reset();
            // encode
            if (okvsTest.encode(keys.get(), values.get(), codeWords.get()) == EncodeStatus::FAIL){
                ++numFails;
                continue;
            }

            std::cout << "Trial " << i << std::endl;
        }
        timer.setTimePoint("end");
        std::cout << "numFails / numTrials: " << numFails << " / " << numTrials << std::endl;
        std::cout << timer << '\n' << std::endl;

        std::cout << "--------------Detail Time-------------\n" << std::endl;
        std::cout << okvsTest.mTimer << std::endl;

#else
        throw UnitTestSkipped("CRYPTOTOOLS_RBOKVS_H is not defined.");
#endif
    }
}
