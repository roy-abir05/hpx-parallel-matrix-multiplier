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

    const int TILE_SIZE = 64;
    int num_row_blocks = (m1 + TILE_SIZE - 1) / TILE_SIZE;

    // Parallelize over row blocks
    hpx::experimental::for_loop(hpx::execution::par, 0, num_row_blocks, [&](int i_block) {
        int i_start = i_block * TILE_SIZE;
        int i_end = std::min<int>(i_start + TILE_SIZE, m1);

        for(int k_start = 0; k_start < n1; k_start += TILE_SIZE) {
            int k_end = std::min<int>(k_start + TILE_SIZE, n1);

            for(int j_start = 0; j_start < n2; j_start += TILE_SIZE) {
                int j_end = std::min<int>(j_start + TILE_SIZE, n2);

                // Compute the tile
                for(int i = i_start; i < i_end; ++i) {
                    for(int k = k_start; k < k_end; ++k) {
                        int A_ik = A[i * n1 + k];
                        for(int j = j_start; j < j_end; ++j) {
                            R[i * n2 + j] += A_ik * B[k * n2 + j];
                        }
                    }
                }
            }
        }
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