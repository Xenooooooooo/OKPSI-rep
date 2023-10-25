#include "AsioSocket_tests.h"
#include "coproto/Socket/AsioSocket.h"
#include "Tests.h"
#include "macoro/thread_pool.h"
#include "macoro/start_on.h"
#include <thread>
namespace coproto
{
	namespace tests
	{
#ifdef COPROTO_ENABLE_BOOST
		void AsioSocket_Accept_test()
		{
			boost::asio::io_context ioc;
			optional<boost::asio::io_context::work> w(ioc);
			auto f = std::async([&] { ioc.run(); });

			std::string address("localhost:1212");

			auto r = macoro::sync_wait(macoro::when_all_ready(
				macoro::make_task(AsioAcceptor(address, ioc)),
				macoro::make_task(AsioConnect(address, ioc))
			));

			std::get<0>(r).result();
			std::get<1>(r).result();

			std::vector<u8> buff(10);

			w.reset();
			f.get();
		}

		void AsioSocket_Accept_sCancel_test()
		{

			boost::asio::io_context ioc;
			optional<boost::asio::io_context::work> w(ioc);
			auto f = std::async([&] { ioc.run(); });
			std::string address("localhost:1212");
			AsioAcceptor a(address, ioc);

			macoro::stop_source src;
			auto token = src.get_token();
			std::thread thrd([&] {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				src.request_stop();
				});
			try {

				try {
					auto s = macoro::sync_wait(macoro::make_task(a.accept(token)));
					throw MACORO_RTE_LOC;
				}
				catch (std::system_error e)
				{
				}
			}
			catch (...)
			{
				w.reset();
				f.get();
				thrd.join();
			}

			w.reset();
			f.get();
			thrd.join();
		}

		void AsioSocket_Accept_cCancel_test()
		{

			boost::asio::io_context ioc;
			optional<boost::asio::io_context::work> w(ioc);
			auto f = std::async([&] { ioc.run(); });
			std::string address("localhost:1212");

			macoro::stop_source src;
			auto token = src.get_token();
			std::thread thrd([&] {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				src.request_stop();
				});
			try {

				try {
					auto s = macoro::sync_wait(AsioConnect(address, ioc, token));
					throw MACORO_RTE_LOC;
				}
				catch (std::system_error e)
				{
				}
			}
			catch (std::exception& e)
			{
				std::cout << "e " << e.what() << std::endl;
				w.reset();
				f.get();
				thrd.join();
			}

			w.reset();
			f.get();
			thrd.join();
		}

		void AsioSocket_sendRecv_test()
		{
			auto s = AsioSocket::makePair();


			std::vector<u8> sb(10), rb(10);
			sb[4] = 5;
			auto a0 = s[0].mSock->send(sb);
			auto a1 = s[1].mSock->recv(rb);

			auto task_ = [&](bool sender) -> task<void> {

				MC_BEGIN(task<>, &, sender);
				if (sender)
					MC_AWAIT(a0);
				else
					MC_AWAIT(a1);

				MC_END();
			};

			auto r = macoro::sync_wait(macoro::when_all_ready(task_(0), task_(1)));
			std::get<0>(r).result();
			std::get<1>(r).result();

			if (sb != rb)
				throw MACORO_RTE_LOC;
		}
		void AsioSocket_asyncSend_test()
		{


			std::vector<u8> sb(10), rb(10);

			macoro::thread_pool::work work;
			macoro::thread_pool tp(2, work);



			std::array<AsioSocket,2> ss; 
			auto task_ = [&](bool sender) -> task<void> {

				MC_BEGIN(task<>, &, sender);
				if (sender)
				{
					MC_AWAIT_SET(ss[0], AsioAcceptor("localhost:1212", global_io_context()));
					//std::cout << "connected"<<std::endl; 
					MC_AWAIT(ss[0].send(std::move(sb)));


					MC_AWAIT(ss[0].flush());
					// destroy the socket before the send operation completes. 
					ss[0] = {};
				}
				else
				{
					MC_AWAIT_SET(ss[1], AsioConnect("localhost:1212", global_io_context()));

					//MC_AWAIT(tp.schedule_after(std::chrono::milliseconds(100)));
					MC_AWAIT(ss[1].recv(rb));
				}
				MC_END();
			};
			
			auto e0 = task_(0)
				| macoro::start_on(tp);
			auto e1 = task_(1)
				| macoro::start_on(tp);


			// bool done = false;
			// auto ff = std::async([&]{
			// 	std::this_thread::sleep_for(std::chrono::seconds(1));
			// 	if(!done)
			// 	{
			// 		u64 i = 0;
			// 		for(auto l : ggLog)
			// 			std::cout << i++ << ": " << l << std::endl;
			// 	}
			// });
			auto r = macoro::sync_wait(macoro::when_all_ready(
				std::move(e0), std::move(e1)
			));
			// done = true;
			// ff.get();

			std::get<0>(r).result();
			std::get<1>(r).result();
			work.reset();
		}
		void AsioSocket_parSendRecv_test()
		{

			u64 trials = 50;
			u64 numOps = 50;
			boost::asio::io_context ioc;
			optional<boost::asio::io_context::work> w(ioc);

			std::vector<std::thread> thrds(4);
			for (auto& t : thrds)
				t = std::thread([&] {ioc.run(); });

			for (u64 tt = 0; tt < trials; ++tt)
			{

				auto s = AsioSocket::makePair(ioc);

				auto f1 = [&](u64 idx) {
					MC_BEGIN(task<void>, idx, &numOps, &s,
						v = u64{},
						i = u64{},
						buffer = span<u8>{},
						r = std::pair<error_code, u64>{});

					buffer = span<u8>((u8*)&v, sizeof(v));

					//MC_AWAIT(macoro::transfer_to(ex[idx]));

					for (i = 1; i <= numOps; ++i)
					{

						if (idx == 0) {
							MC_AWAIT_SET(r, s[0].mSock->recv(buffer));
							COPROTO_ASSERT(v == -i);
						}
						if (idx == 1)
						{
							MC_AWAIT_SET(r, s[1].mSock->recv(buffer));
							COPROTO_ASSERT(v == i);
						}
						if (idx == 2)
						{
							v = i;
							MC_AWAIT_SET(r, s[0].mSock->send(buffer));
						}
						if (idx == 3)
						{
							v = -i;
							MC_AWAIT_SET(r, s[1].mSock->send(buffer));
						}

						COPROTO_ASSERT(!r.first);
						COPROTO_ASSERT(r.second == sizeof(v));

						//MC_AWAIT(macoro::transfer_to(ex[idx]));
					}
					MC_END();
				};

				macoro::sync_wait(macoro::when_all_ready(f1(0), f1(1), f1(2), f1(3)));
			}

			w.reset();
			for (auto& t : thrds)
				t.join();
		}

		void AsioSocket_cancellation_test()
		{


			{
				auto s = AsioSocket::makePair();

				boost::asio::socket_base::receive_buffer_size option(1024);
				s[0].mSock->mState->mSock_.set_option(option);
				s[1].mSock->mState->mSock_.set_option(option);

				boost::asio::socket_base::send_buffer_size option2(1024);
				s[0].mSock->mState->mSock_.set_option(option2);
				s[1].mSock->mState->mSock_.set_option(option2);

				//std::vector<u8> sb(10'000'000'000);
				auto n = 1'000'000;
				u8* sd = new u8[n];
				span<u8> sb(sd, n);
				sb[4] = 5;
				macoro::stop_source src;
				auto token = src.get_token();
				auto a0 = s[0].mSock->send(sb, token);
				std::pair<error_code, u64> res;
				//auto a1 = s[1].mSock->recv(rb);
				auto b = std::chrono::system_clock::now();
				auto task_ = [&](bool sender) -> task<void> {

					MC_BEGIN(task<>, &, sender);
					if (sender)
					{
						MC_AWAIT_SET(res, a0);

						if (res.second == sb.size())
						{
							throw std::runtime_error(COPROTO_LOCATION);
						}
						if (res.first != code::operation_aborted)
						{
							throw std::runtime_error(COPROTO_LOCATION);
						}
					}
					else
					{
						src.request_stop();
					}

					MC_END();
				};

				auto r = macoro::sync_wait(macoro::when_all_ready(task_(1), task_(0)));
				delete[] sd;

				std::get<0>(r).result();
				std::get<1>(r).result();
			}

			{
				auto s = AsioSocket::makePair();
				std::vector<u8> sb(10);
				sb[4] = 5;
				macoro::stop_source src;
				auto token = src.get_token();
				auto a0 = s[0].mSock->recv(sb, token);
				std::pair<error_code, u64> res;
				//auto a1 = s[1].mSock->recv(rb);
				auto b = std::chrono::system_clock::now();

				auto task_ = [&](bool sender) -> task<void> {

					MC_BEGIN(task<>, &, sender);
					if (sender)
					{

						MC_AWAIT_SET(res, a0);

						if (res.second)
						{
							throw std::runtime_error(COPROTO_LOCATION);
						}
						if (res.first != code::operation_aborted)
						{
							throw std::runtime_error(COPROTO_LOCATION);
						}
					}
					else
					{
						src.request_stop();
					}

					MC_END();
				};

				auto r = macoro::sync_wait(macoro::when_all_ready(task_(1), task_(0)));

				std::get<0>(r).result();
				std::get<1>(r).result();
			}



		}


		void AsioSocket_parCancellation_test(const CLP& cmd)
		{

			u64 trials = cmd.getOr("trials",100);
			u64 numOps = 20;
			boost::asio::io_context ioc;
			optional<boost::asio::io_context::work> w(ioc);

			std::vector<std::thread> thrds(40);
			for (auto& t : thrds)
				t = std::thread([&] {ioc.run(); });

			std::vector<std::vector<i64>> buffers(4);
			for (auto& b : buffers)
			{
				b.resize(numOps);
				for (auto i = 0ull; i < numOps; ++i)
					b[i] = i;
			}


			auto s = AsioSocket::makePair();

			for (u64 tt = 0; tt < trials; ++tt)
			{

				using Log = std::vector<std::pair<const char*, std::thread::id>>;

				std::vector<std::array<macoro::stop_source, 4>> srcs(numOps + 1);
				std::vector< std::array<macoro::stop_token, 4>> tkns(numOps + 1);

				for (u64 i = 1; i <= numOps; ++i)
				{
					for (auto j = 0ull; j < 4; ++j)
						tkns[i][j] = srcs[i][j].get_token();
				}

				auto f1 = [&](u64 idx) {
					MC_BEGIN(task<void>, idx, &numOps, &s, &srcs, &tkns, &buffers,
						v = u64{},
						i = i64{},
						buffer = span<u8>{},
						r = std::pair<error_code, u64>{});

					buffer = span<u8>((u8*)buffers[idx].data(), buffers[idx].size() * sizeof(i64));

					if (idx < 2)
					{
						memset(buffer.data(), 0, buffer.size());
					}

					while(buffer.size())
					{
						++i;

						if (idx == 0) {

							// if we are index i % 4 then we will request a stop
							// on the other end of the socket. Otherwise we will receive
							// and pass in our stop token.
							if (i % 4 == 0 && i < numOps)
							{
#ifdef COPROTO_ASIO_LOG
								s[1].mSock->mState->log("request_stop-send " + std::to_string(-i));
#endif
								srcs[i][3].request_stop();
							}
							else
							{
								MC_AWAIT_SET(r, s[0].mSock->recv(
									buffer.subspan(0, std::min<u64>(sizeof(i64), buffer.size())), 
									i < numOps ? tkns[i][idx] : macoro::stop_token{},
									-i));
								buffer = buffer.subspan(r.second);

								//if (i % 4 == 3)
								//{
								//	COPROTO_ASSERT(r.first == code::operation_aborted);
								//}
								//else
								//{
								//	COPROTO_ASSERT(v == -i);
								//}
							}
						}
						if (idx == 1)
						{
							if (i % 4 == 1 && i < numOps)
							{
#ifdef COPROTO_ASIO_LOG
								s[0].mSock->mState->log("request_stop-send " + std::to_string(i));
#endif
								srcs[i][2].request_stop();
							}
							else
							{
								MC_AWAIT_SET(r, s[1].mSock->recv(
									buffer.subspan(0, std::min<u64>(sizeof(i64), buffer.size())),
									i < numOps ? tkns[i][idx] : macoro::stop_token{},
									i));
								buffer = buffer.subspan(r.second);

								//if (i % 4 == 2)
								//{
								//	COPROTO_ASSERT(r.first == code::operation_aborted);
								//}
								//else
								//{
								//	COPROTO_ASSERT(!r.first && v == i);
								//}
							}
						}
						if (idx == 2)
						{
							if (i % 4 == 2 && i < numOps)
							{
#ifdef COPROTO_ASIO_LOG
								s[1].mSock->mState->log("request_stop-recv " + std::to_string(i));
#endif
								srcs[i][1].request_stop();
							}
							else
							{
								//v = i;
								MC_AWAIT_SET(r, s[0].mSock->send(
									buffer.subspan(0, std::min<u64>(sizeof(i64), buffer.size())),
									i < numOps ? tkns[i][idx] : macoro::stop_token{},
									i));
								buffer = buffer.subspan(r.second);

								//if (i % 4 == 1)
								//{
								//	COPROTO_ASSERT(r.first == code::operation_aborted);
								//}
							}
						}
						if (idx == 3)
						{

							if (i % 4 == 3 && i < numOps)
							{
#ifdef COPROTO_ASIO_LOG
								s[0].mSock->mState->log("request_stop-recv " + std::to_string(-i));
#endif
								srcs[i][0].request_stop();
							}
							else
							{
								//v = -i;
								MC_AWAIT_SET(r, s[1].mSock->send(
									buffer.subspan(0, std::min<u64>(sizeof(i64), buffer.size())),
									i < numOps ? tkns[i][idx] : macoro::stop_token{}, 
									-i));
								buffer = buffer.subspan(r.second);

								//if (i % 4 == 0)
								//{
								//	COPROTO_ASSERT(r.first == code::operation_aborted);
								//}
							}
						}

					}
					MC_END();
				};
				auto r = macoro::sync_wait(macoro::when_all_ready(f1(0), f1(1), f1(2), f1(3)));
				try
				{
					std::get<0>(r).result();
					std::get<1>(r).result();
					std::get<2>(r).result();
					std::get<3>(r).result();
				}
				catch (...)
				{

					w.reset();
					for (auto& t : thrds)
						t.join();
					throw;
				}

				if (!std::equal(buffers[0].begin(), buffers[0].end(), buffers[2].begin()))
					throw MACORO_RTE_LOC;
				if (!std::equal(buffers[1].begin(), buffers[1].end(), buffers[3].begin()))
					throw MACORO_RTE_LOC;
			}

			w.reset();
			for (auto& t : thrds)
				t.join();
		}

		void AsioSocket_close_test()
		{

			u64 trials = 100;
			u64 numOps = 20;
			boost::asio::io_context ioc;
			optional<boost::asio::io_context::work> w(ioc);

			std::vector<std::thread> thrds(4);
			for (auto& t : thrds)
				t = std::thread([&] {ioc.run(); });
			for (u64 tt = 0; tt < trials; ++tt)
			{

				auto s = AsioSocket::makePair(ioc);
				auto promise = std::promise<void>{};
				auto fut = promise.get_future().share();
				using Log = std::vector<std::pair<const char*, std::thread::id>>;

				auto f1 = [&](u64 idx) {
					MC_BEGIN(task<void>, idx, &fut, &s, tt, &ioc,
						v = u64{},
						i = u64{},
						buffer = span<u8>{},
						r = std::pair<error_code, u64>{});

					buffer = span<u8>((u8*)&v, sizeof(v));

					MC_AWAIT(transfer_to(ioc));
					//MC_AWAIT(ex[0].post());
					fut.get();
					{
						if (tt % 4 == idx)
							s[0].close();

						if (idx == 0) {
							MC_AWAIT_SET(r, s[0].mSock->recv(buffer));
						}
						if (idx == 1)
						{
							MC_AWAIT_SET(r, s[1].mSock->recv(buffer));
						}
						if (idx == 2)
						{
							v = i;
							MC_AWAIT_SET(r, s[0].mSock->send(buffer));
						}
						if (idx == 3)
						{

							v = -i;
							MC_AWAIT_SET(r, s[1].mSock->send(buffer));
						}

						MC_AWAIT(transfer_to(ioc));
						//MC_AWAIT(ex[0].post());
					}

					MC_END();
				};

				auto t0 = f1(0) | macoro::make_eager();
				auto t1 = f1(1) | macoro::make_eager();
				auto t2 = f1(2) | macoro::make_eager();
				auto t3 = f1(3) | macoro::make_eager();

				promise.set_value();
				macoro::sync_wait(macoro::when_all_ready(std::move(t0), std::move(t1), std::move(t2), std::move(t3)));

			}

			w.reset();
			for (auto& t : thrds)
				t.join();
		}
#else
		namespace
		{
			void skip() { throw UnitTestSkipped("Boost not enabled"); }
		}

		void AsioSocket_Accept_test() { skip(); }
		void AsioSocket_Accept_sCancel_test() { skip(); }
		void AsioSocket_Accept_cCancel_test() { skip(); }
		void AsioSocket_sendRecv_test() { skip(); }
		void AsioSocket_asyncSend_test() { skip(); }
		void AsioSocket_parSendRecv_test() { skip(); }
		void AsioSocket_cancellation_test() { skip(); }
		void AsioSocket_parCancellation_test(const CLP&) { skip(); }
		void AsioSocket_close_test() { skip(); }
#endif
	}
}