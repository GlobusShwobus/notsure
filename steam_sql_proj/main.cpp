#define _CRTDBG_MAP_ALLOC  

#include <string>
#include <iostream>

#include "RandomNum.h"
#include "Sequence.h"

#include <curl/curl.h>
#include "SQL_inserter.h"
#include "Stuff.h"
#include "Payload.h"

#include "Stopwatch.h"

#include <vector>
#include "Insert_statement.h"

using namespace badSQL;

std::ostream& operator<<(std::ostream& stream, const badCore::BoolMessage& e)noexcept
{
    stream << e.message;
    return stream;
}

//TODO:: serialize payload into a CSV, review generic options

int main() {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
   
    {
        CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
        if (result != CURLE_OK) {
            std::cout << curl_easy_strerror(result) << '\n';
            return -1;
        }
        constexpr int N = 1000;
   
        StatementTemplate stmt("steamdb", "raw_payloads");
        stmt.fields = { "steamid", "payload_type", "payload" };
   
        std::string host = "root";
        std::string ip = "tcp://127.0.0.1:3306";
   
        SQLInserter inserter;
        std::cout << inserter.connect(host, ip) << '\n';
   
   
        // Prepare identical workloads
        badCore::Sequence<Payload> loads;
        loads.set_capacity(N);
   
        for (int i = 0; i < N; i++) {
            Payload load;
            load.recipient_id = i;
            load.label = "summary";
            load.data = R"({"hello_world"})";
            loads.push_back(load);
        }
   
        // ------------------
        // SINGLE INSERT BENCHMARK
        // ------------------
        FinalizedStatement single_insert_statement(stmt);
        std::size_t single_total = 0;
   
   
        badCore::Stopwatch sw;
         inserter.set_auto_commit(false);
         for (int i = 0; i < N; i++)
             inserter.inject(loads[i], single_insert_statement);
   
         inserter.commit();
         inserter.set_auto_commit(true);
         single_total += sw.dt_nanosec();
        
  ////
   
        // ------------------
        // BULK INSERT BENCHMARK
        // ------------------
        FinalizedStatement bulk_insert_statement(stmt, loads.size());
        std::size_t bulk_total = 0;
   
   
        badCore::Stopwatch bw;
   
        inserter.inject_bulk(loads.begin(), loads.end(), bulk_insert_statement);
   
        bulk_total = bw.dt_nanosec();
        
   
   
        // ------------------
        // RESULTS
        // ------------------
   
        std::cout << "\nResults (" << N << " rows)\n";
        std::cout << "Single ns: " << single_total << '\n';
        std::cout << "Bulk  ns: " << bulk_total << '\n';
        std::cout << "Speedup: " << (double)single_total / bulk_total << "x\n";
   
        curl_global_cleanup();
    }
    _CrtDumpMemoryLeaks();
    return 0;
}


/*

            constexpr int N = 500;
            constexpr int REPEAT = 10;

            sql_insert_statement stmt("steamdb", "raw_payloads");
            stmt.fields = { "steamid", "payload_type", "payload" };

            std::string host = "root";
            std::string ip = "tcp://127.0.0.1:3306";

            SQLInserter inserter;
            std::cout << inserter.connect(host, ip) << '\n';


            // Prepare identical workloads
            Sequence<Payload> loads;
            loads.set_capacity(N);

            for (int i = 0; i < N; i++) {
                Payload load;
                load.recipient_id = i;
                load.label = "summary";
                load.data = R"({"hello_world"})";
                loads.push_back(load);
            }



            // ------------------
            // SINGLE INSERT BENCHMARK
            // ------------------

            std::size_t single_total = 0;

            for (int r = 0; r < REPEAT; r++)
            {
                Stopwatch sw;
                inserter.set_auto_commit(false);
                for (int i = 0; i < N; i++)
                    inserter.inject(loads[i], stmt);

                inserter.commit();
                inserter.set_auto_commit(true);
                single_total += sw.dt_nanosec();
            }


            // ------------------
            // BULK INSERT BENCHMARK
            // ------------------

            std::size_t bulk_total = 0;

            for (int r = 0; r < REPEAT; r++)
            {
                Stopwatch sw;

                inserter.inject_bulk(std::span<const Payload>(loads.data(), loads.size()), stmt);//idk why...

                bulk_total += sw.dt_nanosec();
            }


            // ------------------
            // RESULTS
            // ------------------

            auto single_avg = single_total / REPEAT;
            auto bulk_avg = bulk_total / REPEAT;

            std::cout << "\nResults (" << N << " rows)\n";
            std::cout << "Single avg ns: " << single_avg << '\n';
            std::cout << "Bulk   avg ns: " << bulk_avg << '\n';
            std::cout << "Speedup: " << (double)single_avg / bulk_avg << "x\n";
*/