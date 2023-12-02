//2023 lych
#include "Mpsi.h"
#include "volePSI/RsPsi.h"
#include <array>
#include <map>
#include <future>
#include <thread>
#include "volePSI/SimpleIndex.h"
#include "libdivide.h"
#include "coproto/Socket/AsioSocket.h"
#include <cryptoTools/Crypto/RBOKVS.h>

#define BASE_PORT 2000
// #define CUCKOO_HASH_NUM 3
// #define GCT_Bin_Size 1<<14
using RBOKVS = oc::RBOKVS;

namespace volePSI
{
    // Sender_Id = [0,User_Num-2] ; Receiver_Id = User_Num-1 ;
    void Mpsi_User::run(u64 User_Num, u64 My_Id, u64 Set_Size, u64 Lambda, u64 Thread_Num, block Seed, std::vector<block> Inputs){
        std::vector<Socket> Chl;
        PRNG Prng(Seed);
        
        setTimePoint("Start");
        
        if (My_Id == User_Num - 1){
            Chl.resize(User_Num - 1);
            for (u64 i = 0ull; i < User_Num - 1; i++)
                Chl[i]=coproto::asioConnect("localhost:" + std::to_string(BASE_PORT + i), true);

            setTimePoint("Chl Finish");
            // std::cout <<"Reciver: Chl Finish"<<std::endl;

            std::vector<block> Rand_Num(Set_Size);
            Prng.get<block>(Rand_Num);
            
            // Baxos Paxos;
            // Paxos.init(Set_Size, GCT_Bin_Size, CUCKOO_HASH_NUM, Lambda, PaxosParam::GF128, Seed);
            
            RBOKVS Paxos;
            Paxos.init(Set_Size, 0.1, Lambda, Seed);

            std::vector<block> GCT(Paxos.mSize);
            
            // std::cout <<"Receiver: before encode"<<std::endl;
            
            // if prng is nullptr and numThreads > 1, it will be wrong
            // Paxos.solve<block>(Inputs, Rand_Num, GCT, &Prng, 1);

            Paxos.encode(Inputs.data(), Rand_Num.data(), GCT.data());


            setTimePoint("GCT Finish");
            // std::cout <<"Receiver: GCT Finish" << std::endl;

            std::vector<std::vector<block>> Share(User_Num - 2);

            for (u64 i = 0ull; i < User_Num - 1 - 1; i++){
                // Share[i].resize(Paxos.size());
                Share[i].resize(Paxos.mSize);
                Prng.get<block>(Share[i]);
                // for (u64 j = 0ull; j < Paxos.size(); j++)
                for (u64 j = 0ull; j < Paxos.mSize; j++)
                    GCT[j] = GCT[j] ^ Share[i][j];
            }

            std::vector<std::thread> shareThrds(User_Num - 1);
            for (u64 i = 0; i < User_Num - 2; ++i){
                shareThrds[i] = std::thread([&, i]() {
                    coproto::sync_wait(Chl[i].send(Share[i]));
                });
            }
            shareThrds[User_Num - 2] = std::thread([&, User_Num]() {
                coproto::sync_wait(Chl[User_Num - 2].send(GCT));
            });
            for (auto& thrd : shareThrds) thrd.join();

            setTimePoint("Share Finish");
            // std::cout <<"Reciver: Share Finish"<<std::endl;


            // coproto::sync_wait(Chl[User_Num -1 -1].flush());
            
            setTimePoint("2PSI Begin");

            RsPsiReceiver Psi_Receiver;

            // if (mTimer)
            //     Psi_Receiver.setTimer(getTimer());

            Psi_Receiver.init(Set_Size,Set_Size,Lambda,Seed,false,Thread_Num);
            auto p = Psi_Receiver.run(Rand_Num,Chl[User_Num -1 -1]);
            auto re = macoro::sync_wait(macoro::when_all_ready(std::move(p)));

            setTimePoint("2PSI Finish");
            // std::cout <<"Reciver: 2-PSI Finish"<<std::endl;           

            Size_Intersection = Psi_Receiver.mIntersection.size();
            Multi_Intersection.clear();
            for (u64 i = 0ull; i < Size_Intersection; i++)
                Multi_Intersection.push_back(Inputs[Psi_Receiver.mIntersection[i]]);
            
            setTimePoint("Get Intersection Finish");

            for (u64 i = 0ull; i < User_Num - 1; i++){
                coproto::sync_wait(Chl[i].flush());
                Comm += Chl[i].bytesSent();
                Chl[i].close();
            }
            //std::cout <<"Get Intersection Finish"<<std::endl;  
            //std::cout << User <<std::endl;
            setTimePoint("Close Chl Finish");
            // std::cout <<"Reciver: Close Chl Finish"<<std::endl;
        }
        else if (My_Id == User_Num - 2)
        {
            Chl.resize(User_Num - 1);
            Chl[0] = coproto::asioConnect("localhost:" + std::to_string(BASE_PORT + My_Id), false);
            for (u64 i = 1; i < User_Num - 1; i++)
                Chl[i] = coproto::asioConnect("localhost:" + std::to_string(BASE_PORT + My_Id * 100 + i - 1), false);

            setTimePoint("Chl Finish");
            // std::cout <<"Sender: Chl Finish"<<std::endl;

            // Baxos Paxos;
            // Paxos.init(Set_Size, GCT_Bin_Size, CUCKOO_HASH_NUM, Lambda, PaxosParam::GF128, Seed);
            
            RBOKVS Paxos;
            Paxos.init(Set_Size, 0.1, Lambda, Seed);
            
            std::vector<std::vector<block>> GCT(User_Num - 1);

            // GCT[0].resize(Paxos.size());
            GCT[0].resize(Paxos.mSize);

            coproto::sync_wait(Chl[0].recv(GCT[0]));
            std::vector<block> Result(Set_Size);
            
            // Paxos.decode<block>(Inputs, Result, GCT[0], Thread_Num);
            Paxos.decode(GCT[0].data(), Inputs.data(), Inputs.size(), Result.data(), 1);

            // recv GCT from P1 - Pn-2
            std::vector<std::thread> recvThrds(User_Num - 2);
            for (u64 i = 1; i < User_Num - 1; ++i){
                recvThrds[i - 1] = std::thread([&, i]() {
                    // GCT[i].resize(Paxos.size());
                    GCT[i].resize(Paxos.mSize);
                    coproto::sync_wait(Chl[i].recv(GCT[i]));
                });
            }
            for (auto& thrd : recvThrds) thrd.join();

            setTimePoint("receive GCT Finish");
            // std::cout <<"Sender: receive GCT Finish"<<std::endl;
            
            for (u64 i = 1; i < User_Num - 1; i++){
                // for (u64 j = 0; j < Paxos.size(); j++)
                for (u64 j = 0; j < Paxos.mSize; j++)
                    GCT[0][j] ^= GCT[i][j];
            }
            // std::cout << "Sender: GCT oplus finish" << std::endl;
            // Paxos.decode<block>(Inputs, Result, GCT[0], Thread_Num);
            Paxos.decode(GCT[0].data(), Inputs.data(), Inputs.size(), Result.data(), 1);

            setTimePoint("Decode Finish");
            // std::cout <<"Sender: Decode Finish"<<std::endl;

            RsPsiSender Psi_Sender;
            // if(mTimer)
            //     Psi_Sender.setTimer(getTimer());

            Psi_Sender.init(Set_Size,Set_Size,Lambda,Seed,false,Thread_Num);
            auto p = Psi_Sender.run(Result,Chl[0]);
            auto re = macoro::sync_wait(macoro::when_all_ready(std::move(p)));

            setTimePoint("2PSI Finish");
            // std::cout <<"Sender: 2-PSI Finish"<<std::endl;

            for (u64 i = 0ull; i < User_Num - 1; i++){
                coproto::sync_wait(Chl[i].flush());
                Comm += Chl[i].bytesSent();
                Chl[i].close();
            }

            setTimePoint("Close Chl Finish");
            // std::cout <<"Sender: Close Chl Finish"<<std::endl;
        }
        else
        {
            Chl.resize(2);
            Chl[0] = coproto::asioConnect("localhost:" + std::to_string(BASE_PORT + My_Id), false);
            Chl[1] = coproto::asioConnect("localhost:" + std::to_string(BASE_PORT + (User_Num - 2) * 100 + My_Id), true);

            setTimePoint("Chl Finish");
            // std::cout <<"Client: Chl Finish" << std::endl;

            // Baxos Paxos;
            // Paxos.init(Set_Size, GCT_Bin_Size, CUCKOO_HASH_NUM, Lambda, PaxosParam::GF128, Seed);

            RBOKVS Paxos;
            Paxos.init(Set_Size, 0.1, Lambda, Seed);
            
            // std::vector<block> Share(Paxos.size()), Decode_Share(Set_Size);
            std::vector<block> Share(Paxos.mSize), Decode_Share(Set_Size);
            
            coproto::sync_wait(Chl[0].recv(Share));

            setTimePoint("receive Share Finish");
            // std::cout <<"Client: receive Share Finish" << std::endl;

            // std::vector<block> GCT(Paxos.size());
            // Paxos.decode<block>(Inputs, Decode_Share, Share, Thread_Num);
            std::vector<block> GCT(Paxos.mSize);
            Paxos.decode(Share.data(), Inputs.data(), Inputs.size(), Decode_Share.data(), 1);

            // if prng is nullptr and numThreads > 1, it will be wrong
            // Paxos.solve<block>(Inputs, Decode_Share, GCT, &Prng, 1);
            Paxos.encode(Inputs.data(), Decode_Share.data(), GCT.data());

            setTimePoint("GCT Reconstruction Finish");
            // std::cout <<"Client: GCT Reconstruction Finish" << std::endl;

            coproto::sync_wait(Chl[1].send(GCT));
            
            setTimePoint("Send GCT Finish");
            // std::cout <<"Client: Send GCT Finish" << std::endl;
            
            coproto::sync_wait(Chl[0].flush());
            coproto::sync_wait(Chl[1].flush());
            Comm += Chl[1].bytesSent();
            Chl[0].close();
            Chl[1].close();

            setTimePoint("Close Chl Finish");
            // std::cout <<"Client: Close Chl Finish" << std::endl;
        }
        return ;
    }
}