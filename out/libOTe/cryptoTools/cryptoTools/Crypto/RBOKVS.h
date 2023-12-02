// Copyright 2023
// Author: lx1234, lych, 70, Xeno

#pragma once

#include <cryptoTools/Crypto/BLAKE3/blake3.h>
#include <cryptoTools/Common/Defines.h>
#include <cryptoTools/Crypto/AES.h>
#include <cryptoTools/Crypto/PRNG.h>
#include <cryptoTools/Common/Timer.h>

#ifndef CRYPTOTOOLS_RBOKVS_H
#define CRYPTOTOOLS_RBOKVS_H

namespace osuCrypto{
    struct MatrixRow{
        u64 startPos;
        std::unique_ptr<block[]> data;
        block val;
        u64 next;
    };

    enum EncodeStatus{
        SUCCESS,
        FAIL,
        ALLZERO
    };

    struct RBOKVSParam{
        u64 mNumRows;
        // numCols = mScaler * numRows
        double mScaler;
        // statistical security parameter
        u64 mStasSecParam;
        // width of band
        u64 mBandWidth;
        // randomness of hash function
        block mR1, mR2;
        // random seed for encode
        block mSeed;

        u64 numCols() const { return static_cast<u64>(mScaler * mNumRows); }
    };

    class RBOKVS{
    public:
        // number of elements(rows)
        u64 mN;
        // number of columns
        u64 mSize;
        // width of band
        u64 mW;
        // statistical security parameter
        u64 mSsp;
        // randomness of hash function
        block mRPos, mRBand;
        // PRNG for encode
        PRNG mPrng;
        // hash function
        // blake3_hasher mHasher;
        Timer mTimer;

        RBOKVS() = default;
        RBOKVS (const RBOKVS& copy) {}
        ~RBOKVS() = default;

        void init(const u64 &n, const double &epsilon, const u64 &stasSecParam, const block &seed);
        void init(const RBOKVSParam &param);

        RBOKVSParam getParams(const u64 &n, const double &epsilon, const u64 &stasSecParam, const block &seed);

        // generate the randomness of hash function
        void setSeed(const block &seed);

        // get the start position of the band
        u64 hashPos(const block &input);
        // get a random band(w bits)
        void hashBand(const block &input, block *output);

        EncodeStatus reformalize(MatrixRow &row);

        EncodeStatus insert(u64 *bitToRowMap, MatrixRow *rows, u64 rowIdx);

        EncodeStatus encode(const block *keys, const block *vals, block *output);

        block decode(const block *codeWords, const block &key);

        void decode(const block *codeWords, const block *keys, u64 size, block *output, u64 numThreads);
    };

}

#endif