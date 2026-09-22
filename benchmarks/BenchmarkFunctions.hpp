#ifndef BENCHMARK_FUNCTIONS_HPP
#define BENCHMARK_FUNCTIONS_HPP

#include "Problemas.hpp"

#include <string>
#include <vector>

// Representa um problema da disciplina e as raízes de referência usadas para medir o erro.
struct BenchmarkCase {
    const ProblemaFuncao* problem;
    std::vector<double> exactRoots;
};

// Fornece os problemas do enunciado com suas raízes de referência no intervalo solicitado.
inline const std::vector<BenchmarkCase>& benchmarkCases() {
    static const std::vector<BenchmarkCase> cases = [] {
        const std::vector<ProblemaFuncao>& problems = Problems::all();
        return std::vector<BenchmarkCase> {
            {&problems[0], {1.4928787086636035}},
            {&problems[1], {2.0}},
            {&problems[2], {-0.9295604598378413}},
            {&problems[3], {4.323239543713715}}
        };
    }();
    return cases;
}

#endif // BENCHMARK_FUNCTIONS_HPP
