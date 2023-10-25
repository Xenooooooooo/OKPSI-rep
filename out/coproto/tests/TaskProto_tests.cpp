
#include "TaskProto_tests.h"
#include "coproto/Socket/Socket.h"
#include "coproto/Socket/LocalAsyncSock.h"
#include "macoro/task.h"
#include "macoro/stop.h"
#include "macoro/sync_wait.h"
#include "macoro/when_all.h"
#include "macoro/wrap.h"
#include "macoro/start_on.h"
#include <numeric>
#include "eval.h"

namespace coproto
{

	namespace tests
	{
		namespace {
			std::mutex gPrntMtx;

			auto types = {
				EvalTypes::async//,
				//EvalTypes::blocking
			};
		}

#ifdef COPROTO_CPP20

		void task_proto_test()
		{
			for (auto type : types)//{EvalTypes::blocking})
			{

				int recv = 0;
				std::array<bool, 2> started = { false,false };

				auto task_proto = [&](Socket ss, bool send)->task<void>
				{
					int cc;
					if (send)
					{
						started[0] = true;
						cc = 42;
						co_await ss.send(cc);
					}
					else
					{
						started[1] = true;
						co_await ss.recv(cc);
						recv = cc;
					}
				};

				auto r = eval(task_proto, type);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}


		void task_strSendRecv_Test()
		{
			bool verbose = false;
			int sDone = 0, rDone = 0;
			auto proto = [&](Socket& s, bool party) -> task<void> {

				std::string sStr("hello from 0");
				std::string sRtr; sRtr.resize(sStr.size());
				for (u64 i = 0; i < 5; ++i)
				{
					if (party)
					{
						co_await s.send(sStr);

						if (verbose)
							std::cout << "s send " << std::endl;
						//co_await EndOfRound();
						if (verbose)
							std::cout << "s eor " << std::endl;

						sStr = co_await s.recv<std::string>();
						if (verbose)
							std::cout << "s recv " << std::endl;

						if (sStr != "hello from " + std::to_string(i * 2 + 1))
							throw std::runtime_error(COPROTO_LOCATION);
						sStr.back() += 1;
					}
					else
					{
						co_await s.recv(sRtr);
						if (verbose)
							std::cout << "r recv " << std::endl;

						if (sRtr != "hello from " + std::to_string(i * 2 + 0))
							throw std::runtime_error(COPROTO_LOCATION);

						sRtr.back() += 1;
						co_await s.send(sRtr);
						if (verbose)
							std::cout << "r send " << std::endl;
						//co_await EndOfRound();
						if (verbose)
							std::cout << "r eor " << std::endl;

					}
				}
				if (party)
				{
					if (verbose)
						std::cout << "s done " << std::endl;
					++sDone;
				}
				else
				{
					if (verbose)
						std::cout << "r done " << std::endl;
					++rDone;
				}

			};



			for (auto t : types)
			{
				auto r = eval(proto, t);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}

		}
		void task_resultSendRecv_Test()
		{

			auto proto = [](Socket& sock, bool party) -> task<void> {
				std::string str("hello from 0");
				//co_await Name("main");

				for (u64 i = 0; i < 5; ++i)
				{
					if (party)
					{
						//std::cout << " p1 sent " << i << std::endl;
						auto ec = co_await macoro::wrap(sock.send(str));
						//std::cout << " p1 sent " << i << " ok" << std::endl;

						//std::cout << " p1 recv " << i << std::endl;
						auto r = co_await(sock.recv<std::string>() | macoro::wrap());
						//std::cout << " p1 recv " << i << " ok " << std::endl;

						if (r.has_error())
							throw std::runtime_error(COPROTO_LOCATION);

						str = r.value();

						if (str != "hello from " + std::to_string(i * 2 + 1))
							throw std::runtime_error(COPROTO_LOCATION);
						str.back() += 1;
					}
					else
					{
						//std::cout << " p0 recv " << i << std::endl;
						co_await sock.recv(str);
						//std::cout << " p0 recv " << i << " ok" << std::endl;

						if (str != "hello from " + std::to_string(i * 2 + 0))
							throw std::runtime_error(COPROTO_LOCATION);

						str.back() += 1;

						//std::cout << " p0 sent " << i << std::endl;
						co_await sock.send(str);
						//std::cout << " p0 sent " << i << " ok" << std::endl;

					}
				}
			};


			for (auto t : types)
			{
				auto r = eval(proto, t);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}

		void task_typedRecv_Test()
		{
			bool verbose = false;
			auto proto = [verbose](Socket& sock, bool party) -> task<void> {

				std::vector<u64> buff, rBuff;
				for (u64 i = 0; i < 5; ++i)
				{
					if (party)
					{
						buff.resize(1 + i * 2);
						std::fill(buff.begin(), buff.end(), i * 2);
						if (verbose)std::cout << "send0 b " << i << std::endl;
						co_await sock.send(std::move(buff));
						if (verbose)std::cout << "send0 e " << i << std::endl;
						if (verbose)std::cout << "recv1 b " << i << std::endl;
						rBuff = co_await sock.recv<std::vector<u64>>();
						if (verbose)std::cout << "recv1 e " << i << std::endl;

						buff.resize(2 + i * 2);
						std::fill(buff.begin(), buff.end(), i * 2 + 1);

						if (buff != rBuff)
							throw std::runtime_error(COPROTO_LOCATION);
					}
					else
					{
						if (verbose)std::cout << "recv0 b " << i << std::endl;
						rBuff = co_await sock.recv<std::vector<u64>>();
						if (verbose)std::cout << "recv0 e " << i << std::endl;
						buff.resize(1 + i * 2);
						std::fill(buff.begin(), buff.end(), i * 2);

						if (buff != rBuff)
							throw std::runtime_error(COPROTO_LOCATION);

						buff.resize(buff.size() + 1);
						std::fill(buff.begin(), buff.end(), i * 2 + 1);
						if (verbose)std::cout << "send1 b " << i << std::endl;
						co_await sock.send(std::move(buff));
						if (verbose)std::cout << "send1 e " << i << std::endl;

					}
				}
			};


			for (auto t : types)
			{
				eval(proto, t);
			}

		}
		void task_zeroSendRecv_Test()
		{

			auto proto = [](Socket& s, bool party) -> task<void> {

				std::vector<u64> buff;
				co_await s.send(buff);
			};


			for (auto t : types)
			{
				auto r = eval(proto, t);

				try {
					std::get<0>(r).result();
				}
				catch (std::system_error& e)
				{
					if (e.code() != code::sendLengthZeroMsg)
						throw std::runtime_error("");
				}

				try {
					std::get<1>(r).result();
				}
				catch (std::system_error& e)
				{
					if (e.code() != code::sendLengthZeroMsg)
						throw std::runtime_error("");
				}
			}
		}
		void task_zeroSendRecv_ErrorCode_Test()
		{

			auto proto = [](Socket& s, bool party) -> task<void> {

				std::vector<u64> buff;
				auto ec = co_await(s.send(buff) | macoro::wrap());

				if (as_error_code(ec.error()) != code::sendLengthZeroMsg)
					throw std::runtime_error("");

			};
			for (auto t : types)
			{
				auto r = eval(proto, t);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}

		void task_badRecvSize_Test()
		{
			auto proto = [](Socket& s, bool party) -> task<void> {

				std::vector<u64> buff(3);

				if (party)
				{
					co_await s.send(buff);
				}
				else
				{
					buff.resize(1);
					co_await s.recv(buff);
				}
			};

			for (auto t : types)
			{
				auto r = eval(proto, t);


				try { std::get<0>(r).result(); throw std::runtime_error(""); }
				catch (BadReceiveBufferSize& b) {
					if (b.mBufferSize != 8 || b.mReceivedSize != 24)
						throw;
				};
				try {
					std::get<1>(r).result();
					throw std::runtime_error("");
				}
				catch (std::system_error& e)
				{
					if (e.code() != code::remoteClosed)
						throw;
				}
			}
		}

		void task_badRecvSize_ErrorCode_Test()
		{
			auto proto = [](Socket& s, bool party) -> task<void> {

				std::vector<u64> buff(3);

				if (party)
				{
					auto r = co_await(s.send(buff) | macoro::wrap());
					if (as_error_code(r.error()) != code::remoteClosed)
						throw std::runtime_error("");

					r = co_await(s.send(buff) | macoro::wrap());
					if (as_error_code(r.error()) != code::remoteClosed)
						throw std::runtime_error("");

				}
				else
				{
					buff.resize(1);
					auto r = co_await(s.recv(buff) | macoro::wrap());

					if (as_error_code(r.error()) != code::badBufferSize)
						throw std::runtime_error("");


					r = co_await macoro::wrap(s.recv(buff));
					if (as_error_code(r.error()) != code::cancel)
						throw std::runtime_error("");

					r = co_await macoro::wrap(s.send(buff));
					if (as_error_code(r.error()) != code::cancel)
						throw std::runtime_error("");
				}
			};


			for (auto t : types)
			{
				auto r = eval(proto, t);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}

		void task_moveSend_Test()
		{

			for (auto t : types)
			{
				macoro::thread_pool ex;
				auto proto = [t, &ex](Socket& s, bool party) -> task<void>
				{
					int i = 0;
					std::vector<int> v(10);
					if (t == EvalTypes::blocking)
					{
						auto t0 = (s.send(std::move(i)) | macoro::start_on(ex));
						co_await s.recv(i);
						macoro::sync_wait(t0);

						auto t1 = (s.send(std::move(v)) | macoro::start_on(ex));
						co_await s.recvResize(v);
						macoro::sync_wait(t1);
					}
					else
					{
						co_await s.send(std::move(i));
						co_await s.recv(i);

						co_await s.send(std::move(v));
						co_await s.recvResize(v);
					}
				};

				std::thread thrd;
				auto work = ex.make_work();
				if (t == EvalTypes::blocking)
					thrd = std::thread([&] {ex.run(); });

				auto r = eval(proto, t);

				work = {};
				if (t == EvalTypes::blocking)
					thrd.join();

				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}

		void task_throws_Test()
		{

			auto proto = [](Socket& s, bool party) -> task<void>
			{
				try {

					if (party)
						throw std::runtime_error("");
					else
					{
						//throw std::runtime_error("");
						char c = co_await s.recv<char>();
					}
				}
				catch (...)
				{
					s.close();
				}
				co_return;
			};


			for (auto t : types)
			{
				auto r = eval(proto, t);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}


		task<int> task_echoServer(Socket ss, u64 idx, u64 length, u64 rep, std::string name, bool v)
		{
#ifdef COPROTO_LOGGING
			auto np = name + "_server_" + std::to_string(idx) + "_" + std::to_string(length);
			co_await Name(np);
#endif

			auto exp = std::vector<char>(length);
			std::iota(exp.begin(), exp.end(), 0);

			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " s start " << idx << " " << length << std::endl;
			}

			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " s recv " << idx << " " << length << " begin" << std::endl;
			}
			auto msg = std::vector<char>();

			for (u64 i = 0; i < rep; ++i)
			{
				auto r = ss.recv<std::vector<char>>();
#ifdef COPROTO_LOGGING
				r.setName(np + "_r" + std::to_string(i));
#endif
				msg = co_await std::move(r);
				//msg = co_await recv<std::vector<char>>();
				if (exp != msg)
				{
					std::lock_guard<std::mutex> lock(gPrntMtx);
					std::cout << "bad msg " << COPROTO_LOCATION << std::endl;
					throw std::runtime_error("");
				}
			}

			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " s recv " << idx << " " << length << " done" << std::endl;
			}

			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " s send " << idx << " " << length << std::endl;
			}
			for (u64 i = 0; i < rep; ++i)
			{
				auto s = ss.send(msg);
#ifdef COPROTO_LOGGING
				s.setName(np + "_s" + std::to_string(i));
#endif
				co_await std::move(s);
			}

			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " s send " << idx << " " << length << " done" << std::endl;
			}

			//co_await EndOfRound();

			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " s EOR " << idx << " " << length << " " << std::endl;
			}

			if (idx)
			{
				co_return co_await task_echoServer(ss, idx - 1, length, rep, name, v);
			}
			else
			{
				if (v)
					std::cout << name << " ###################### s done " << idx << " " << length << std::endl;
				co_return 0;
			}
		}
		task<int> task_echoClient(Socket ss, u64 idx, u64 length, u64 rep, std::string name, bool v)
		{
#ifdef COPROTO_LOGGING
			auto np = name + "_client_" + std::to_string(idx) + "_" + std::to_string(length);
			co_await Name(np);
#endif
			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " c start " << idx << " " << length << std::endl;
			}
			auto msg = std::vector<char>(length);
			std::iota(msg.begin(), msg.end(), 0);
			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " c send " << idx << " " << length << std::endl;
			}
			for (u64 i = 0; i < rep; ++i)
			{
				auto s = ss.send(msg);
#ifdef COPROTO_LOGGING
				s.setName(np + "_s" + std::to_string(i));
#endif
				co_await std::move(s);
			}

			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " c send " << idx << " " << length << " done" << std::endl;
			}
			//co_await EndOfRound();

			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " c EOR " << idx << " " << length << " " << std::endl;
				std::cout << name << " c recv " << idx << " " << length << " begin" << std::endl;
			}
			for (u64 i = 0; i < rep; ++i)
			{

				auto r = ss.recv<std::vector<char>>();
#ifdef COPROTO_LOGGING
				r.setName(np + "_r" + std::to_string(i));
#endif
				auto msg2 = co_await std::move(r);
				//auto msg2 = co_await recv<std::vector<char>>();
				if (msg2 != msg)
				{
					std::lock_guard<std::mutex> lock(gPrntMtx);
					std::cout << "bad msg " << COPROTO_LOCATION << std::endl;
					throw std::runtime_error("");
				}
			}
			if (v) {
				std::lock_guard<std::mutex> lock(gPrntMtx);
				std::cout << name << " c recv " << idx << " " << length << " done" << std::endl;
			}
			if (idx)
			{
				co_return co_await task_echoClient(ss, idx - 1, length, rep, name, v);
			}
			else
			{

				if (v) {
					std::lock_guard<std::mutex> lock(gPrntMtx);
					std::cout << name << " ###################### c done " << idx << " " << length << std::endl;
				}
				co_return 0;
			}
		}


		void task_nestedProtocol_Test()
		{
			bool verbose = false;
			auto proto = [verbose](Socket& s, bool party) -> task<void> {
				std::string str("hello from 0");
				u64 n = 5;
				if (party)
				{
					//std::cout << "p1 send " << std::endl;
					auto r = co_await macoro::wrap(s.send(std::move(str)));
					r.value();
					//throw std::runtime_error(COPROTO_LOCATION);

					co_await task_echoServer(s, n, 10, 1, "p1", verbose);
				}
				else
				{
					//std::cout << "p0 recv " << std::endl;
					co_await s.recv(str);
					//std::cout << " p0 recv" << std::endl;

					if (str != "hello from 0")
						throw std::runtime_error(COPROTO_LOCATION);

					co_await task_echoClient(s, n, 10, 1, "p0", verbose);
					//std::cout << " p0 sent" << std::endl;

				}
			};

			for (auto t : types)
			{
				auto r = eval(proto, t);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}
		struct ThrowServerException : public std::runtime_error
		{
			ThrowServerException(std::string s)
				: std::runtime_error(s)
			{}
		};

		task<void> task_throwServer(Socket& s, u64 i)
		{
			auto msg = co_await s.recv<std::string>();
			co_await s.send((msg));

			if (i)
				co_await task_throwServer(s, i - 1);
			else
				throw ThrowServerException(COPROTO_LOCATION);
		}

		task<void> task_throwClient(Socket& s, u64 i)
		{
			auto msg = std::string("hello world");
			co_await s.send(msg);
			if (msg != co_await s.recv<std::string>())
			{
				throw std::runtime_error("hello world");
			}

			if (i)
				co_await task_throwClient(s, i - 1);
		}

		void task_nestedProtocol_Throw_Test()
		{

			auto proto = [](Socket& s, bool party) -> task<void> {

				if (party)
				{
					std::vector<u64> buff(10);
					co_await s.send(buff);
					co_await task_throwServer(s, 4);
				}
				else
				{
					std::vector<u64> buff(10);
					co_await s.recv(buff);
					co_await task_throwClient(s, 4);
				}
			};

			for (auto t : types)
			{
				auto r = eval(proto, t);
				try
				{
					std::get<0>(r).result();
				}
				catch (std::system_error& e)
				{
					if (e.code() != code::remoteClosed)
						throw std::runtime_error(COPROTO_LOCATION);
				}
				try
				{
					std::get<1>(r).result();
				}
				catch (ThrowServerException&)
				{
				}
			}
		}

		void task_nestedProtocol_ErrorCode_Test()
		{

			bool hasEc = false;
			u64 n = 5;
			auto proto = [&hasEc, n](Socket& s, bool party) -> task<void> {

				if (party)
				{
					std::vector<u64> buff(10);
					co_await s.send(buff);
					auto ec = co_await macoro::wrap(task_throwServer(s, n));

					if (as_error_code(ec.error()) == code::uncaughtException)
						hasEc = true;

					//ec = co_await send(buff).wrap();

					//if(ec != code::ioError)

				}
				else
				{
					std::vector<u64> buff(10);
					co_await s.recv(buff);
					co_await task_throwClient(s, n);
				}
			};


			for (auto t : types)
			{
				auto r = eval(proto, t);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}




		void task_asyncProtocol_Test()
		{

//#define MULTI
			//STExecutor ex;
			bool print = false;
			// level of recursion
			u64 n = 10;
			// number of messages sent at each level.
			u64 rep = 5;

			auto proto = [n, print, rep](Socket& s, bool party, macoro::thread_pool& ex) -> task<void> {

				co_await macoro::transfer_to(ex);

				if (party)
				{
					auto name = std::string("                  p1");
					//co_await Name(name);
					std::vector<u64> buff(10);


					//{
					//	std::lock_guard<std::mutex> lock(gPrntMtx);
					//	std::cout << "\n" << name << " begin" << std::endl;
					//}

					co_await s.recv(buff);
					//co_await EndOfRound();
					if (print)
					{
						std::lock_guard<std::mutex> lock(gPrntMtx);
						std::cout << "\n" << name << " mid" << std::endl;
					}
					auto fu0 = (task_echoServer(s.fork(), n, 5, rep, name, print) | macoro::start_on(ex));
//#ifdef MULTI
					auto fu1 = (task_echoServer(s.fork(), n + 2, 6, rep, name, print) | macoro::start_on(ex));
					auto fu2 = (task_echoServer(s.fork(), n, 7, rep, name, print) | macoro::start_on(ex));
					auto fu3 = (task_echoServer(s.fork(), n + 7, 8, rep, name, print) | macoro::start_on(ex));
					auto fu4 = (task_echoServer(s.fork(), n, 9, rep, name, print) | macoro::start_on(ex));
//#endif

					co_await task_echoClient(s, n, 10, rep, name, print);
					//co_await send(buff);

					co_await fu0;
//#ifdef MULTI
					co_await fu1;
					co_await fu2;
					co_await fu3;
					co_await fu4;
//#endif
				}
				else
				{
					auto name = std::string("p0");
					//co_await Name(name);

					//{
					//	std::lock_guard<std::mutex> lock(gPrntMtx);
					//	std::cout << "\n" << name << " begin" << std::endl;
					//}
					std::vector<u64> buff(10);
					co_await s.send(buff);
					//co_await recv(buff);

					if (print)
					{
						std::lock_guard<std::mutex> lock(gPrntMtx);
						std::cout << "\n" << name << " mid" << std::endl;
					}
					auto fu0 = (task_echoClient(s.fork(), n, 5, rep, name, print) | macoro::start_on(ex));
//#ifdef MULTI
					auto fu1 = (task_echoClient(s.fork(), n + 2, 6, rep, name, print) | macoro::start_on(ex));
					auto fu2 = (task_echoClient(s.fork(), n, 7, rep, name, print) | macoro::start_on(ex));
					auto fu3 = (task_echoClient(s.fork(), n + 7, 8, rep, name, print) | macoro::start_on(ex));
					auto fu4 = (task_echoClient(s.fork(), n, 9, rep, name, print) | macoro::start_on(ex));
//#endif
					co_await task_echoServer(s, n, 10, rep, name, print);
					//co_await recv(buff);

					co_await fu0;
//#ifdef MULTI
					co_await fu1;
					co_await fu2;
					co_await fu3;
					co_await fu4;
//#endif
				}
			};
//#ifdef MULTI 
//#undef MULTI
//#endif

			for (auto t : types/*{ EvalTypes::async }*/)
			{
				auto r = evalEx(proto, t);
				std::get<0>(r).result();
				std::get<1>(r).result();
			}
		}

		void task_asyncProtocol_Throw_Test()
		{

			u64 n = 3;
			auto proto = [n](Socket& s, bool party, macoro::thread_pool& ex) -> task<void> {
				co_await macoro::transfer_to(ex);

				if (party)
				{
					std::vector<u64> buff(10);
					co_await s.send(buff);

					co_await (task_throwServer(s, n) | macoro::start_on(ex));
				}
				else
				{
					std::vector<u64> buff(10);
					co_await s.recv(buff);
					co_await (task_throwClient(s, n) | macoro::start_on(ex));
				}

			};


			for (auto t : types/*{EvalTypes::blocking}*/)
			{
				auto r = evalEx(proto, t);
				std::get<0>(r).result();

				try { std::get<1>(r).result(); }
				catch (ThrowServerException&) { }
			}
		}

		void task_endOfRound_Test()
		{
			return;
			u64 done = 0;

			auto proto = [&done](Socket& s, bool party, macoro::thread_pool& sched) -> task<void> {

				if (party)
				{
					co_await s.send(42);
					co_await sched.post();
					int i;
					for (u64 j = 0; j < 10; ++j)
					{
						co_await s.recv(i);
						co_await s.send(42);
						co_await sched.post();
					}
				}
				else
				{
					int i;
					for (u64 j = 0; j < 10; ++j)
					{
						co_await s.recv(i);
						co_await s.send(42);
						co_await sched.post();
					}

					co_await s.recv(i);
				}
				++done;
			};


			{
				//STExecutor ex;
				//auto s = RoundFunctionSock::makePair();
				//auto t0 = proto(s[0], 0, ex) | macoro::start_on(ex);
				//auto t1 = proto(s[1], 1, ex) | macoro::start_on(ex);

				//while (ex.mQueue_.size() ||
				//	s[0].mImpl->mInbound.size() ||
				//	s[1].mImpl->mInbound.size())
				//{
				//	ex.run();
				//	s[1].setInbound(s[0].getOutbound());
				//	s[0].setInbound(s[1].getOutbound());
				//}

				//auto breakFn = [&]() { return ex.mQueue_.size() == 0; };

				//if (done != 2)
				//	throw COPROTO_RTE;
			}
		}

		void task_errorSocket_Test()
		{

//#define MULTI
			bool print = false;
			u64 n = 2;
			u64 rep = 1;
			auto proto = [n, print, rep](Socket& s, bool party, macoro::thread_pool& ex) -> task<void> {

				if (party)
				{
					auto name = std::string("p1");
					//co_await Name(name);
					std::vector<u64> buff(10);



					co_await s.recv(buff);
					//co_await EndOfRound();

					auto fu0 = task_echoServer(s.fork(), n, 5, rep, name, print) | macoro::wrap() | macoro::start_on(ex);

#ifdef MULTI
					auto fu1 = task_echoServer(s.fork(), n + 2, 6, rep, name, print) | macoro::wrap() | macoro::start_on(ex);
					auto fu2 = task_echoServer(s.fork(), n, 7, rep, name, print) | macoro::wrap() | macoro::start_on(ex);
#endif

					auto r = co_await (task_echoClient(s, n, 10, rep, name, print) | macoro::wrap());
					//co_await send(buff);

					auto r0 = co_await fu0;
#ifdef MULTI
					auto r1 = co_await fu1;
					auto r2 = co_await fu2;
#endif
					r.value();
					r0.value();
#ifdef MULTI
					r1.value();
					r2.value();
#endif
				}
				else
				{
					auto name = std::string("p0");
					//co_await Name(name);
					std::vector<u64> buff(10);
					co_await s.send(buff);
					//co_await recv(buff);
					auto fu0 = task_echoClient(s.fork(), n, 5, rep, name, print) 
						| macoro::wrap() 
						| macoro::start_on(ex);
#ifdef MULTI
					auto fu1 = task_echoClient(s.fork(), n + 2, 6, rep, name, print) | macoro::wrap() | macoro::start_on(ex);
					auto fu2 = task_echoClient(s.fork(), n, 7, rep, name, print) | macoro::wrap() | macoro::start_on(ex);
#endif
					auto r = co_await (task_echoServer(s, n, 10, rep, name, print) | macoro::wrap());
					//co_await recv(buff);

					auto r0 = co_await fu0;
#ifdef MULTI
					auto r1 = co_await fu1;
					auto r2 = co_await fu2;
#endif					
					r.value();
					r0.value();
#ifdef MULTI
					r1.value();
					r2.value();
#endif
				}
			};
#ifdef MULTI 
#undef MULTI
#endif

			for (auto type : types)
			{
				//std::array<Socket, 2> s;
				//STExecutor ex;
				u64 numOps = 0;
				if (type == EvalTypes::async)
				{
					//auto ss = LocalAsyncSocket::makePair(ex);
					//s[0] = ss[0];
					//s[1] = ss[1];

					macoro::thread_pool stx0;
					auto w0 = stx0.make_work();
					stx0.create_thread();

					auto s = LocalAsyncSocket::makePair();
					s[0].mSock->errFn() = [&numOps]()->error_code {
						++numOps;
						return code::success;
					};
					s[1].mSock->errFn() = [&numOps]() ->error_code {
						++numOps;
						return code::success;
					};
					auto w = macoro::when_all_ready(proto(s[0], 0, stx0), proto(s[1], 1, stx0));
					auto r = macoro::sync_wait(std::move(w));


					for (u64 i = 0; i < numOps; ++i)
					{
						u64 idx = 0;
						auto s = LocalAsyncSocket::makePair();
						s[0].mSock->errFn() = [&idx, i]() ->error_code {
							if (idx++ == i)
								return code::DEBUG_ERROR;
							return code::success;
						};
						s[1].mSock->errFn() = [&idx, i]()->error_code {
							if (idx++ == i)
								return code::DEBUG_ERROR;
							return code::success;
						};
						auto w = macoro::when_all_ready(proto(s[0], 0, stx0), proto(s[1], 1, stx0));
						auto r = macoro::sync_wait(std::move(w));

						error_code e0(code::success), e1(code::success);
						try { std::get<0>(r).result(); }
						catch (std::system_error& e) { e0 = e.code(); }
						try { std::get<1>(r).result(); }
						catch (std::system_error& e) { e1 = e.code(); }

						if (!e0 && !e1)
							throw std::runtime_error("error was expected: \n" + 
								e0.message() + "\n" +e1.message());
					}

					w0 = {};
					stx0.join();
				}
				else
				{

					//auto ss = LocalBlockingSock::makePair();
					//s[0] = ss[0];
					//s[1] = ss[1];
				}


				//LocalEvaluator eval;
				//auto s = eval.getSocketPair(type);
				//auto p0 = proto(s[0], 0);
				//auto p1 = proto(s[1], 1);
				//auto ec = eval.eval(p0, p1, type);
				//auto numOps = eval.mOpIdx;

				//if (ec.mRes0 || ec.mRes1)
				//	throw std::runtime_error(ec.message());

				//for (u64 i = 0; i < numOps; ++i)
				//{
				//	LocalEvaluator eval;
				//	auto s = eval.getSocketPair(type);
				//	auto p0 = proto(s[0], 0);
				//	auto p1 = proto(s[1], 1);

				//	eval.mErrorIdx = i;
				//	auto ec = eval.eval(p0, p1, type);


				//	if (ec.mRes0 != code::DEBUG_ERROR && ec.mRes1 != code::DEBUG_ERROR)
				//		throw std::runtime_error("error was expected: " + ec.message());
				//}


			}

		}

		void task_cancel_send_test()
		{

			macoro::stop_source src;
			auto token = src.get_token();

			auto proto = [&](Socket& s, bool party) -> task<void>
			{
				int i;
				co_await s.send(i, token);
			};

			for (auto t : types)
			{
				std::thread thrd = std::thread([&] {
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
					src.request_stop();
					});

				auto r = eval(proto, t);

				thrd.join();
				try { std::get<0>(r).result(); }
				catch (std::system_error& e) { COPROTO_ASSERT(e.code() == code::operation_aborted); }
				try { std::get<1>(r).result(); }
				catch (std::system_error& e) { COPROTO_ASSERT(e.code() == code::operation_aborted); }


			}
		}

		void task_cancel_recv_test()
		{

			macoro::stop_source src;
			auto token = src.get_token();

			auto proto = [&](Socket& s, bool party) -> task<void>
			{
				int i;
				co_await s.recv(i, token);
			};

			for (auto t : types)
			{
				std::thread thrd = std::thread([&] {
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
					src.request_stop();
					});

				auto r = eval(proto, t);

				thrd.join();
				try { std::get<0>(r).result(); }
				catch (std::system_error& e) { COPROTO_ASSERT(e.code() == code::operation_aborted); }
				try { std::get<1>(r).result(); }
				catch (std::system_error& e) { COPROTO_ASSERT(e.code() == code::operation_aborted); }


			}
		}
#endif
	}

}