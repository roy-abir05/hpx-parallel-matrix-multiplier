#include <hpx/algorithm.hpp>
#include <hpx/future.hpp>
#include <hpx/init.hpp>
#include <hpx/modules/timing.hpp>
#include <hpx/program_options.hpp>

#include <iostream>
#include <vector>

void print_matrix(std::vector<int> &M, int m, int n)
{
    if(m>10 || n>10) return;

    std::cout << "Result Matrix:" << std::endl;

    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            std::cout << M[i * n + j] << " ";
        }
        std::cout << std::endl;
    }
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    std::size_t const m1 = vm["n"].as<std::size_t>();
    std::size_t const n1 = vm["m"].as<std::size_t>();
    std::size_t const m2 = n1; 
    std::size_t const n2 = vm["k"].as<std::size_t>();

    std::vector<int> A(m1 * n1);
    std::vector<int> B(m2 * n2);
    std::vector<int> R(m1 * n2, 0);

    // Initialization of Test Matrices
    hpx::experimental::for_loop(hpx::execution::par, 0, m1 * n1, [&](auto i) {
        A[i] = i % 100; 
    });
    hpx::experimental::for_loop(hpx::execution::par, 0, m2 * n2, [&](auto i) {
        B[i] = (i + 1) % 100;
    });

    R.resize(m1 * n2, 0);

    hpx::chrono::high_resolution_timer t;

    // Swapped j and k loops for better harwdware prefetching
    hpx::experimental::for_loop(hpx::execution::par, 0, m1, [&](auto i)
                                {
            for(int k=0; k<n1; ++k)
                for(int j=0; j<n2; ++j)
                    R[i * n2 + j] += A[i * n1 + k] * B[k * n2 + j];
    });

    double elapsed = t.elapsed();

    std::cout << "Result Matrix:" << std::endl;
    print_matrix(R, m1, n2);
    std::cout << "Time elapsed: " << elapsed << " seconds\n";

    return hpx::local::finalize();
}

int main(int argc, char* argv[]) {
    using namespace hpx::program_options;
    options_description cmdline("usage: hpx_matrix_mul [options]");
    
    cmdline.add_options()
        ("n", value<std::size_t>()->default_value(1000), "Rows of first matrix")
        ("m", value<std::size_t>()->default_value(1000), "Cols of first / Rows of second")
        ("k", value<std::size_t>()->default_value(1000), "Cols of second matrix");

    hpx::local::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return hpx::local::init(hpx_main, argc, argv, init_args);
}