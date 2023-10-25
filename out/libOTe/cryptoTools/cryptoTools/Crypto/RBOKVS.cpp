// Copyright 2023
// Author: lx1234, lych, 70, Xeno

#include <cryptoTools/Crypto/RBOKVS.h>
#include <thread>

namespace osuCrypto{
    RBOKVSParam RBOKVS::getParams(const u64 &n, const double &epsilon, const u64 &stasSecParam, const block &seed){
        RBOKVSParam param;
        param.mNumRows = n;
        param.mScaler = 1 + epsilon;
        param.mStasSecParam = stasSecParam;

        // generate randomness of hash function
        PRNG prng(seed);
        param.mR1 = prng.get<block>();
        param.mR2 = prng.get<block>();
        param.mSeed = prng.get<block>();

        u64 nn = log2ceil(n);
        // compute width of band
        if (std::abs(epsilon - 0.03) <1e-5){
            // epsilon = 0.03
            // n = 2^10, \lambda = 0.08047w - 3.464
            // n = 2^14, \lambda = 0.08253w - 5.751
            // n = 2^16, \lambda = 0.08241w - 7.023
            // n = 2^18, \lambda = 0.08192w - 8.569
            // n = 2^20, \lambda = 0.08313w - 10.880
            // n = 2^24, \lambda = 0.08253w - 14.671
            if (nn <= 10){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 3.464) / 0.08047), param.numCols());
            } else if (nn <= 14){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 5.751) / 0.08253), param.numCols());
            } else if (nn <= 16){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 7.023) / 0.08241), param.numCols());
            } else if (nn <= 18){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 8.569) / 0.08192), param.numCols());
            } else if (nn <= 20){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 10.880) / 0.08313), param.numCols());
            } else if (nn <= 24){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 14.671) / 0.08253), param.numCols());
            } else {
                std::cout << "no proper parameter for this n, use parameters for n = 2^24" << std::endl;
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 14.671) / 0.08253), param.numCols());
            }
        } else if (std::abs(epsilon - 0.05) < 1e-5){
            // epsilon = 0.05
            // n = 2^10, \lambda = 0.1388w - 4.424
            // n = 2^14, \lambda = 0.1389w - 6.976
            // n = 2^16, \lambda = 0.1399w - 8.942
            // n = 2^18, \lambda = 0.1388w - 10.710
            // n = 2^20, \lambda = 0.1407w - 12.920
            // n = 2^24, \lambda = 0.1376w - 16.741
            if (nn <= 10){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 4.424) / 0.1388), param.numCols());
            } else if (nn <= 14){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 6.976) / 0.1389), param.numCols());
            } else if (nn <= 16){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 8.942) / 0.1399), param.numCols());
            } else if (nn <= 18){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 10.710) / 0.1388), param.numCols());
            } else if (nn <= 20){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 12.920) / 0.1407), param.numCols());
            } else if (nn <= 24){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 16.741) / 0.1376), param.numCols());
            } else {
                std::cout << "no proper parameter for this n, use n = 2^24" << std::endl;
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 16.741) / 0.1376), param.numCols());
            }
        } else if (std::abs(epsilon - 0.07) < 1e-5){
            // epsilon = 0.07
            // n = 2^10, \lambda = 0.1947w - 5.383
            // n = 2^14, \lambda = 0.1926w - 8.150
            // n = 2^16, \lambda = 0.1961w - 10.430
            // n = 2^18, \lambda = 0.1955w - 12.300
            // n = 2^20, \lambda = 0.1939w - 14.100
            if (nn <= 10){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 5.383) / 0.1947), param.numCols());
            } else if (nn <= 14){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 8.150) / 0.1926), param.numCols());
            } else if (nn <= 16){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 10.430) / 0.1961), param.numCols());
            } else if (nn <= 18){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 12.300) / 0.1955), param.numCols());
            } else if (nn <= 20){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 14.100) / 0.1939), param.numCols());
            } else {
                std::cout << "no proper parameter for this n, use n = 2^20" << std::endl;
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 14.100) / 0.1939), param.numCols());
            }
        } else if (std::abs(epsilon - 0.1) < 1e-5){
            // epsilon = 0.1
            // n = 2^10, \lambda = 0.2747w - 6.296
            // n = 2^14, \lambda = 0.2685w - 9.339
            // n = 2^16, \lambda = 0.2740w - 11.610
            // n = 2^18, \lambda = 0.2715w - 13.390
            // n = 2^20, \lambda = 0.2691w - 15.210
            // n = 2^24, \lambda = 0.2751w - 19.830
            if (nn <= 10){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 6.296) / 0.2747), param.numCols());
            } else if (nn <= 14){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 9.339) / 0.2685), param.numCols());
            } else if (nn <= 16){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 11.610) / 0.2740), param.numCols());
            } else if (nn <= 18){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 13.390) / 0.2715), param.numCols());
            } else if (nn <= 20){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 15.210) / 0.2691), param.numCols());
            } else if (nn <= 24){
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 19.830) / 0.2751), param.numCols());
            } else {
                std::cout << "no proper parameter for this n, use n = 2^24" << std::endl;
                param.mBandWidth = std::min<u64>(static_cast<u64>((stasSecParam + 19.830) / 0.2751), param.numCols());
            }
        } else {
            throw std::runtime_error("no proper parameter for this epsilon");
        }
        return param;
    }

    void RBOKVS::init(const u64 &n, const double &epsilon, const u64 &stasSecParam, const block &seed){
        init(getParams(n, epsilon, stasSecParam, seed));
    }

    void RBOKVS::init(const RBOKVSParam &param){
        mN = param.mNumRows;
        mSize = param.numCols();
        mW = param.mBandWidth;
        mSsp = param.mStasSecParam;
        mRPos = param.mR1;
        mRBand = param.mR2;
        mPrng.SetSeed(param.mSeed);
        mTimer.reset();
    }

    void RBOKVS::setSeed(const block &seed){
        PRNG prng(seed);
        mRPos = prng.get<block>();
        mRBand = prng.get<block>();
        mPrng.SetSeed(prng.get<block>());
    }

    u64 RBOKVS::hashPos(const block &input){
        if (mSize == mW){
            return 0;
        }
        u8 hashOut[8];
        u64 out = 0;

        blake3_hasher_init(&mHasher);
        blake3_hasher_update(&mHasher, &mRPos, sizeof(mRPos));
        blake3_hasher_update(&mHasher, &input, sizeof(input));
        blake3_hasher_finalize(&mHasher, hashOut, 8);

        // todo: memory copy
        for (u64 i = 0; i < 8; ++i){
            out ^= (static_cast<u64>(hashOut[i]) << (56 - i * 8));
        }
        return out % (mSize - mW);
    }

    void RBOKVS::hashBand(const block &input, block *output){
        // number of blocks to store the band multiplied by 16(number of bytes in a block)
        u64 wBlockBytes = divCeil(mW, 128) * 16;
        u64 wBytes = divCeil(mW, 8);
        u8 hashOut[wBlockBytes];
        blake3_hasher_init(&mHasher);
        blake3_hasher_update(&mHasher, &mRBand, sizeof(mRBand));
        blake3_hasher_update(&mHasher, &input, sizeof(input));
        blake3_hasher_finalize(&mHasher, hashOut, wBytes);

        hashOut[wBytes - 1] &= ~(0xFF >> (8 - mW % 8));

        // padding zero to the end of the last block
        memset(hashOut + wBytes, 0, wBlockBytes - wBytes);

        for (u64 i = 0; i < wBlockBytes / 16; ++i){
            output[i].data()[7] = hashOut[i * 16];
            output[i].data()[6] = hashOut[i * 16 + 1];
            output[i].data()[5] = hashOut[i * 16 + 2];
            output[i].data()[4] = hashOut[i * 16 + 3];
            output[i].data()[3] = hashOut[i * 16 + 4];
            output[i].data()[2] = hashOut[i * 16 + 5];
            output[i].data()[1] = hashOut[i * 16 + 6];
            output[i].data()[0] = hashOut[i * 16 + 7];

            output[i].data()[15] = hashOut[i * 16 + 8];
            output[i].data()[14] = hashOut[i * 16 + 9];
            output[i].data()[13] = hashOut[i * 16 + 10];
            output[i].data()[12] = hashOut[i * 16 + 11];
            output[i].data()[11] = hashOut[i * 16 + 12];
            output[i].data()[10] = hashOut[i * 16 + 13];
            output[i].data()[9] = hashOut[i * 16 + 14];
            output[i].data()[8] = hashOut[i * 16 + 15];
        }
    }

    EncodeStatus RBOKVS::reformalize(MatrixRow &row){
        u64 wBlocks = divCeil(mW, 128);
        u64 numZeroBlocks = wBlocks;
        // find the first non-zero block
        for (u64 i = 0 ; i < wBlocks; ++i){
            if (row.data[i] != ZeroBlock){
                numZeroBlocks = i;
                break;
            }
        }

        if (numZeroBlocks == wBlocks){
            // all zero row
            if (row.val == ZeroBlock){
                // use a special value to indicate a meaningless row
                row.startPos = -1;
                return EncodeStatus::ALLZERO;
            } else {
                return EncodeStatus::FAIL;
            }
        }

        // shift the first non-zero block to the firset block and pad zeros
        u64 leftShift = static_cast<u64>(row.data[numZeroBlocks].mData[0] == 0) + 2 * numZeroBlocks;
        u64 *ptr = reinterpret_cast<u64*>(row.data.get());
        if (leftShift != 0){
            memmove(ptr, ptr + leftShift, sizeof(u64) * (2 * wBlocks - leftShift));
            memset(ptr + 2 * wBlocks - leftShift, 0, sizeof(u64) * leftShift);
            row.startPos += leftShift * 64;
        }

        // shift the first 1 to the first bit
        int numLeadingZeros = __builtin_clzll(ptr[0]);
        // no extra space
        // todo: maybe handle all block at one time but use more space
        if (numLeadingZeros != 0){
            for (u64 i = 0; i < 2 * wBlocks - leftShift - 1; i++){
                ptr[i] = (ptr[i] << numLeadingZeros) | (ptr[i + 1] >> (64 - numLeadingZeros));
            }
            ptr[2 * wBlocks - leftShift - 1] <<= numLeadingZeros;

            row.startPos += numLeadingZeros;
        }

        return EncodeStatus::SUCCESS;
    }

    EncodeStatus RBOKVS::insert(u64 *bitToRowMap, MatrixRow *rows, u64 rowIdx){
        EncodeStatus status = reformalize(rows[rowIdx]);
        if (status == EncodeStatus::FAIL || status == EncodeStatus::ALLZERO){
            return status;
        }

        u64 wBlocks = divCeil(mW, 128);
        u64 collidingRowIdx = bitToRowMap[rows[rowIdx].startPos];
        while (collidingRowIdx != mN){
            // band XOR
            for (u64 i = 0; i < wBlocks; ++i){
                rows[rowIdx].data[i] ^= rows[collidingRowIdx].data[i];
            }
            // value XOR
            rows[rowIdx].val ^= rows[collidingRowIdx].val;
            // reformalize row
            status = reformalize(rows[rowIdx]);
            if (status == EncodeStatus::FAIL || status == EncodeStatus::ALLZERO){
                return status;
            }
            collidingRowIdx = bitToRowMap[rows[rowIdx].startPos];
        }

        // When previous process is finished, that means this row find an empty index.
        // (this place hasn't been inserted)
        // then directly insert this row
        bitToRowMap[rows[rowIdx].startPos] = rowIdx;
        return EncodeStatus::SUCCESS;
    }

    EncodeStatus RBOKVS::encode(const block *keys, const block *vals, block *output){
        mTimer.setTimePoint("encode start");

        std::unique_ptr<u64[]> bitToRowMap(new u64[mSize]);
        std::unique_ptr<MatrixRow[]> rows(new MatrixRow[mN]);
        u64 wBlocks = divCeil(mW, 128);

        // initialize
        for (u64 i = 0; i < mSize; ++i){
            bitToRowMap[i] = mN;
        }
        memset(output, 0, sizeof(block) * mSize);

        mTimer.setTimePoint("alloc and init");

        // initialize rows
        // todo: maybe parallelize this
        for (u64 i = 0; i < mN; ++i){
            rows[i].startPos = hashPos(keys[i]);
            rows[i].data.reset(new block[wBlocks]);
            hashBand(keys[i], rows[i].data.get());
            rows[i].val = vals[i];
        }

        mTimer.setTimePoint("alloc and hash");

        EncodeStatus status;
        for (u64 i = 0; i < mN; ++i){
            status = insert(bitToRowMap.get(), rows.get(), i);
            if (status == EncodeStatus::FAIL){
                return status;
            }
        }

        mTimer.setTimePoint("elimination");

        // solve the linear system
        u64 *ptr, tmp, j;
        for (i64 i = mSize - 1; i >= 0; --i){
            if (bitToRowMap[i] != mN){
                ptr = reinterpret_cast<u64*>(rows[bitToRowMap[i]].data.get());
                output[i] = rows[bitToRowMap[i]].val;
                // first way to solve the linear system
                // for (u64 j = i + 1; j < i + mW && j < mSize; ++j){
                //     if ((ptr[(j - i) / 64] >> (63 - (j - i) % 64)) & 1){
                //         output[i] ^= output[j];
                //     }
                // }
                ptr[0] &= 0x7FFFFFFFFFFFFFFF;
                j = i;
                // expand the loop, handling 64 bits at one time
                // except the last u64(maybe less than 64 bits)
                for ( ; j < i + mW - 64 && j < mSize - 64; j+=64){
                    tmp = ptr[(j - i) / 64];
                               output[i] ^= (tmp & 1) ? output[j + 63] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 62] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 61] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 60] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 59] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 58] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 57] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 56] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 55] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 54] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 53] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 52] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 51] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 50] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 49] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 48] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 47] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 46] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 45] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 44] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 43] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 42] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 41] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 40] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 39] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 38] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 37] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 36] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 35] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 34] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 33] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 32] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 31] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 30] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 29] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 28] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 27] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 26] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 25] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 24] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 23] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 22] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 21] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 20] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 19] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 18] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 17] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 16] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 15] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 14] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 13] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 12] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 11] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 10] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 9] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 8] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 7] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 6] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 5] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 4] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 3] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 2] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j + 1] : ZeroBlock;
                    tmp >>= 1; output[i] ^= (tmp & 1) ? output[j] : ZeroBlock;
                }
                for ( ; j < i + mW && j < mSize; ++j){
                    if((ptr[(j - i) / 64] >> (63 - (j - i) % 64)) & 1){
                        output[i] ^= output[j];
                    }
                }
            } else{
                output[i] = mPrng.get<block>();
            }
        }

        mTimer.setTimePoint("back substitution");

        return EncodeStatus::SUCCESS;
    }

    block RBOKVS::decode(const block *codeWords, const block &key){
        u64 startPos = hashPos(key);
        block data[divCeil(mW, 128)], res = ZeroBlock;
        hashBand(key, data);

        u64 *ptr = reinterpret_cast<u64*>(data);
        u64 tmp, j;
        for (j = 0 ; j < mW - 64; j+=64){
            tmp = ptr[j / 64];
                       res ^= (tmp & 1) ? codeWords[startPos+ j + 63] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 62] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 61] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 60] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 59] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 58] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 57] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 56] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 55] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 54] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 53] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 52] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 51] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 50] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 49] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 48] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 47] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 46] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 45] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 44] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 43] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 42] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 41] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 40] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 39] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 38] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 37] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 36] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 35] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 34] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 33] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 32] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 31] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 30] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 29] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 28] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 27] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 26] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 25] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 24] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 23] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 22] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 21] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 20] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 19] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 18] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 17] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 16] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 15] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 14] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 13] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 12] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 11] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 10] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 9] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 8] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 7] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 6] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 5] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 4] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 3] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 2] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j + 1] : ZeroBlock;
            tmp >>= 1; res ^= (tmp & 1) ? codeWords[startPos+ j] : ZeroBlock;
        }
        for ( ; j < mW; ++j){
            if((ptr[j / 64] >> (63 - j % 64)) & 1){
                res ^= codeWords[startPos + j];
            }
        }
        // don't expand the loop
        // for (u64 i = 0; i < mW; ++i){
        //     if ((ptr[i / 64] >> (63 - i % 64)) & 1){
        //         res ^= codeWords[startPos + i];
        //     }
        // }
        return res;
    }

    void RBOKVS::decode(const block *codeWords, const block *keys, u64 size, block *output, u64 numThreads){
        numThreads = std::max<u64>(1u, numThreads);
        u64 batchSize = size / numThreads;
        std::thread decodeThrds[numThreads];
        for (u64 i = 0; i < numThreads; ++i){
            decodeThrds[i] = std::thread([&, i](){
                const u64 start = i * batchSize;
                const u64 end = (i == numThreads - 1) ? size : (i + 1) * batchSize;
                for (u64 j = start; j < end; ++j){
                    output[j] = decode(codeWords, keys[j]);
                }
            });
        }
        for (auto &thrd : decodeThrds) thrd.join();
    }
}