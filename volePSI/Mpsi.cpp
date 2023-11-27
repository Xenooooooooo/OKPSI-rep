//2023 lych
#include "Mpsi.h"
#include "volePSI/RsPsi.h"
#include <array>
#include <map>
#include <future>
#include "volePSI/SimpleIndex.h"
#include "libdivide.h"
#include "coproto/Socket/AsioSocket.h"

#define BASE_PORT 2000
#define CUCKOO_HASH_NUM 3
#define GCT_Bin_Size 1<<14

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
            //std::cout <<"Chl Finish"<<std::endl;

            std::vector<block> Rand_Num(Set_Size);
            Prng.get<block>(Rand_Num);
            Baxos Paxos;
            Paxos.init(Set_Size, GCT_Bin_Size, CUCKOO_HASH_NUM, Lambda, PaxosParam::GF128, Seed);
            u64 Paxos_Size = Paxos.size();
            std::vector<block> GCT(Paxos_Size);
            Paxos.solve<block>(Inputs, Rand_Num, GCT, &Prng, Thread_Num);

            setTimePoint("GCT Finish");
            //std::cout <<"GCT Finish"<<std::endl;

            std::vector<block> Share(Paxos_Size);

            for (u64 i = 0ull; i < User_Num - 1 - 1; i++){
                Prng.get<block>(Share);
                coproto::sync_wait(Chl[i].send(Share));
                for (u64 j = 0ull; j < Paxos_Size; j++)
                    GCT[j] = GCT[j] ^ Share[j];
            }
            coproto::sync_wait(Chl[User_Num - 1 - 1].send(GCT));

            setTimePoint("Share Finish");
            //std::cout <<"Share Finish"<<std::endl;


            coproto::sync_wait(Chl[User_Num -1 -1].flush());
            
            setTimePoint("2PSI Begin");

            RsPsiReceiver Psi_Receiver;

            // if (mTimer)
            //     Psi_Receiver.setTimer(getTimer());

            Psi_Receiver.init(Set_Size,Set_Size,Lambda,Seed,false,Thread_Num);
            auto p = Psi_Receiver.run(Rand_Num,Chl[User_Num -1 -1]);
            auto re = macoro::sync_wait(macoro::when_all_ready(std::move(p)));
            coproto::sync_wait(Chl[User_Num -1 -1].flush());

            setTimePoint("2PSI Finish");
            //std::cout <<"2-PSI Finish"<<std::endl;           

            Size_Intersection = Psi_Receiver.mIntersection.size();
            Multi_Intersection.clear();
            for (u64 i = 0ull; i < Size_Intersection; i++)
                Multi_Intersection.push_back(Inputs[Psi_Receiver.mIntersection[i]]);

            for (u64 i = 0ull; i < User_Num - 1; i++)
                Comm += Chl[i].bytesSent();
            
            setTimePoint("Get Intersection Finish");
            //std::cout <<"Get Intersection Finish"<<std::endl;  
            //std::cout << User <<std::endl;
        }
        else{
            Chl.resize(3);
            Chl[0]=coproto::asioConnect("localhost:" + std::to_string(BASE_PORT + My_Id), false);
            if (My_Id != 0ull) Chl[1]=coproto::asioConnect("localhost:" + std::to_string(BASE_PORT + 100 + My_Id), true);
            if (My_Id != User_Num -1 -1) Chl[2]=coproto::asioConnect("localhost:" + std::to_string(BASE_PORT + 100 + My_Id + 1), false);

            setTimePoint("Chl Finish");

            std::vector<block> Share,Docode_Share(Set_Size);
            coproto::sync_wait(Chl[0].recvResize(Share));

            setTimePoint("Share Finish");

            Baxos Paxos;
            Paxos.init(Set_Size, GCT_Bin_Size, CUCKOO_HASH_NUM, Lambda, PaxosParam::GF128, Seed);
            u64 Paxos_Size = Paxos.size();
            std::vector<block> GCT(Paxos_Size);
            Paxos.decode<block>(Inputs, Docode_Share, Share, Thread_Num);
            Paxos.solve<block>(Inputs, Docode_Share, GCT, &Prng, Thread_Num);
            if (My_Id != 0ull){
                std::vector<block> GCT_Receive;
                coproto::sync_wait(Chl[1].recvResize(GCT_Receive));
                for (u64 i = 0ull; i < Paxos_Size; i++)
                    GCT[i]= GCT[i] ^ GCT_Receive[i];
            }

            setTimePoint("GCT Reconstruction Finish");

            if (My_Id != User_Num -1 -1){
                coproto::sync_wait(Chl[2].send(GCT));
                setTimePoint("Send GCT Finish");
            }
            else{
                
                std::vector<block> Result(Set_Size);
                Paxos.decode<block>(Inputs, Result, GCT, Thread_Num);

                setTimePoint("Final Decoding Finish");

                coproto::sync_wait(Chl[0].flush());

                setTimePoint("2PSI Begin");                

                RsPsiSender Psi_Sender;
                // if(mTimer)
                //     Psi_Sender.setTimer(getTimer());

                Psi_Sender.init(Set_Size,Set_Size,Lambda,Seed,false,Thread_Num);
                auto p = Psi_Sender.run(Result,Chl[0]);
                auto re = macoro::sync_wait(macoro::when_all_ready(std::move(p)));

                setTimePoint("2PSI Finish");

                coproto::sync_wait(Chl[0].flush());
                
            }
            Comm += Chl[(My_Id == User_Num -1 -1)?(0):(2)].bytesSent();
        }
        return ;
    }
}