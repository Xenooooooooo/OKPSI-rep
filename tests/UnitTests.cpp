

#include "cryptoTools/Common/Log.h"
#include <functional>
#include "UnitTests.h"

#include "Paxos_Tests.h"
#include "RsOprf_Tests.h"
#include "RsOpprf_Tests.h"
#include "RsPsi_Tests.h"
#include "FileBase_Tests.h"

namespace volePSI_Tests
{
    using namespace volePSI;
    oc::TestCollection Tests([](oc::TestCollection& t) {
        
        t.add("Paxos_buildRow_Test         ", Paxos_buildRow_Test);
        t.add("Paxos_solve_Test            ", Paxos_solve_Test);
        t.add("Paxos_solve_u8_Test         ", Paxos_solve_u8_Test);
        t.add("Paxos_solve_mtx_Test        ", Paxos_solve_mtx_Test);
                                           
        t.add("Paxos_invE_Test             ", Paxos_invE_Test);
        t.add("Paxos_invE_g3_Test          ", Paxos_invE_g3_Test);
        t.add("Paxos_solve_gap_Test        ", Paxos_solve_gap_Test);
        t.add("Paxos_solve_rand_Test       ", Paxos_solve_rand_Test);
        t.add("Paxos_solve_rand_gap_Test   ", Paxos_solve_rand_gap_Test);
                                           
        t.add("Baxos_solve_Test            ", Baxos_solve_Test);
        t.add("Baxos_solve_mtx_Test        ", Baxos_solve_mtx_Test);
        t.add("Baxos_solve_par_Test        ", Baxos_solve_par_Test);
        t.add("Baxos_solve_rand_Test       ", Baxos_solve_rand_Test);
        
        t.add("RsOprf_eval_test            ", RsOprf_eval_test);
        t.add("RsOprf_mal_test             ", RsOprf_mal_test);
        t.add("RsOprf_reduced_test         ", RsOprf_reduced_test);
                   
#ifdef VOLE_PSI_ENABLE_OPPRF
        t.add("RsOpprf_eval_blk_test       ", RsOpprf_eval_blk_test);
        t.add("RsOpprf_eval_u8_test        ", RsOpprf_eval_u8_test);

        t.add("RsOpprf_eval_blk_mtx_test   ", RsOpprf_eval_blk_mtx_test);
        t.add("RsOpprf_eval_u8_mtx_test    ", RsOpprf_eval_u8_mtx_test);
#endif

        t.add("Psi_Rs_empty_test           ", Psi_Rs_empty_test);
        t.add("Psi_Rs_partial_test         ", Psi_Rs_partial_test);
        t.add("Psi_Rs_full_test            ", Psi_Rs_full_test);
        t.add("Psi_Rs_reduced_test         ", Psi_Rs_reduced_test);
        t.add("Psi_Rs_multiThrd_test       ", Psi_Rs_multiThrd_test);
        t.add("Psi_Rs_mal_test             ", Psi_Rs_mal_test);
                                           
        t.add("filebase_readSet_Test       ", filebase_readSet_Test);
        t.add("filebase_psi_bin_Test       ", filebase_psi_bin_Test);
        t.add("filebase_psi_csv_Test       ", filebase_psi_csv_Test);
        t.add("filebase_psi_csvh_Test      ", filebase_psi_csvh_Test);

    });
}
