#include "BenchmarkFunctions.hpp"

#include "Bisseccao.hpp"
#include "FalsaPosicao.hpp"
#include "Isolamento.hpp"
#include "Newton.hpp"
#include "PontoFixo.hpp"
#include "Proposta.hpp"
#include "Secante.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct BenchmarkOptions {
    std::string format = "table";
    double tolerance = 1e-6;
    std::size_t maxIterations = 1000;
    std::size_t repetitions = 30;
};

struct Method {
    std::string name;
    std::function<ResultadoMetodo(const ProblemaFuncao&, const Intervalo&, const BenchmarkOptions&)> execute;
};

bool parseDouble(const std::string& text, double& value) {
    try {
        std::size_t processed = 0;
        value = std::stod(text, &processed);
        return processed == text.size() && std::isfinite(value) && value >= 0.0;
    } catch (const std::exception&) {
        return false;
    }
}

bool parseIterations(const std::string& text, std::size_t& value) {
    try {
        std::size_t processed = 0;
        const unsigned long long parsed = std::stoull(text, &processed);
        if (processed != text.size() || parsed == 0U || parsed > static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
            return false;
        }
        value = static_cast<std::size_t>(parsed);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void printUsage(const char* program) {
    std::cerr << "uso: " << program
              << " [--format table|csv] [--tol N] [--max-iter N] [--repetitions N]\n";
}

std::vector<Method> methods() {
    return {
        {"experimental", [](const ProblemaFuncao& problem, const Intervalo& bracket, const BenchmarkOptions& options) {
            ProposedMethodOptions proposedOptions;
            proposedOptions.xAbsoluteTolerance = options.tolerance;
            proposedOptions.xRelativeTolerance = 0.0;
            proposedOptions.functionTolerance = options.tolerance;
            proposedOptions.maxIterations = options.maxIterations;
            return ProposedMethod::execute(problem.f, bracket.a, bracket.b, proposedOptions);
        }},
        {"bisection", [](const ProblemaFuncao& problem, const Intervalo& bracket, const BenchmarkOptions& options) {
            return Bisseccao::executar(problem.f, bracket.a, bracket.b, options.tolerance,
                static_cast<int>(options.maxIterations));
        }},
        {"false-position", [](const ProblemaFuncao& problem, const Intervalo& bracket, const BenchmarkOptions& options) {
            return FalsaPosicao::executar(problem.f, bracket.a, bracket.b, options.tolerance,
                static_cast<int>(options.maxIterations));
        }},
        {"secant", [](const ProblemaFuncao& problem, const Intervalo& bracket, const BenchmarkOptions& options) {
            return Secante::executar(problem.f, bracket.a, bracket.b, options.tolerance,
                static_cast<int>(options.maxIterations));
        }},
        {"newton", [](const ProblemaFuncao& problem, const Intervalo& bracket, const BenchmarkOptions& options) {
            const double x0 = bracket.a + (bracket.b - bracket.a) / 2.0;
            return Newton::executar(problem.f, problem.df, x0, options.tolerance,
                static_cast<int>(options.maxIterations));
        }},
        {"fixed-point", [](const ProblemaFuncao& problem, const Intervalo& bracket, const BenchmarkOptions& options) {
            const double x0 = bracket.a + (bracket.b - bracket.a) / 2.0;
            return PontoFixo::executar(problem.f, problem.phi, x0, options.tolerance,
                static_cast<int>(options.maxIterations));
        }}
    };
}

ResultadoMetodo executeMedian(const Method& method, const ProblemaFuncao& problem,
    const Intervalo& bracket, const BenchmarkOptions& options) {
    ResultadoMetodo result;
    std::vector<double> times;
    times.reserve(options.repetitions);
    for (std::size_t repetition = 0; repetition < options.repetitions; ++repetition) {
        result = method.execute(problem, bracket, options);
        times.push_back(result.tempoMicrosegundos);
    }
    std::sort(times.begin(), times.end());
    const std::size_t middle = times.size() / 2;
    result.tempoMicrosegundos = times.size() % 2 == 0
        ? (times[middle - 1] + times[middle]) / 2.0
        : times[middle];
    return result;
}

bool referenceRootFor(const BenchmarkCase& benchmarkCase, const Intervalo& bracket, double& root) {
    const double scale = std::max({1.0, std::abs(bracket.a), std::abs(bracket.b)});
    const double slack = 32.0 * std::numeric_limits<double>::epsilon() * scale;
    std::size_t matches = 0;
    for (const double candidate : benchmarkCase.exactRoots) {
        if (bracket.a - slack <= candidate && candidate <= bracket.b + slack) {
            root = candidate;
            ++matches;
        }
    }
    return matches == 1;
}

std::size_t totalEvaluations(const ResultadoMetodo& result) {
    return result.avaliacoesFuncao + result.avaliacoesDerivada + result.avaliacoesPhi;
}

double finalWidth(const ResultadoMetodo& result) {
    if (!std::isfinite(result.intervaloFinal.a) || !std::isfinite(result.intervaloFinal.b)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return result.intervaloFinal.b - result.intervaloFinal.a;
}

std::string displayNumber(double value, int precision = 5) {
    if (!std::isfinite(value)) {
        return "N/A";
    }
    std::ostringstream output;
    output << std::scientific << std::setprecision(precision) << value;
    return output.str();
}

void printTableHeader() {
    std::cout << std::left
              << std::setw(9) << "problem"
              << std::setw(18) << "bracket"
              << std::setw(16) << "method"
              << std::setw(22) << "status"
              << std::right
              << std::setw(13) << "root"
              << std::setw(13) << "f(root)"
              << std::setw(13) << "error"
              << std::setw(7) << "iters"
              << std::setw(8) << "f-eval"
              << std::setw(8) << "df-eval"
              << std::setw(9) << "phi-eval"
              << std::setw(8) << "total"
              << std::setw(12) << "time-us"
              << std::setw(22) << "final-bracket"
              << std::setw(13) << "width"
              << std::setw(8) << "attempt"
              << std::setw(8) << "accept"
              << std::setw(8) << "bis-fb"
              << std::setw(8) << "fp-fb"
              << std::setw(8) << "ext-fb" << '\n';
}

void printTableRow(const BenchmarkCase& benchmarkCase, const Intervalo& bracket,
    double exactRoot, const Method& method, const ResultadoMetodo& result) {
    std::ostringstream bracketText;
    bracketText << '[' << std::fixed << std::setprecision(4) << bracket.a << ',' << bracket.b << ']';
    std::string finalBracket = "N/A";
    if (std::isfinite(result.intervaloFinal.a) && std::isfinite(result.intervaloFinal.b)) {
        std::ostringstream finalBracketText;
        finalBracketText << '[' << std::scientific << std::setprecision(5)
                         << result.intervaloFinal.a << ',' << result.intervaloFinal.b << ']';
        finalBracket = finalBracketText.str();
    }
    std::cout << std::left
              << std::setw(9) << benchmarkCase.problem->id
              << std::setw(18) << bracketText.str()
              << std::setw(16) << method.name
              << std::setw(22) << textoStatusMetodo(result.status)
              << std::right
              << std::setw(13) << displayNumber(result.raiz)
              << std::setw(13) << displayNumber(result.fRaiz)
              << std::setw(13) << displayNumber(std::abs(result.raiz - exactRoot))
              << std::setw(7) << result.iteracoes
              << std::setw(8) << result.avaliacoesFuncao
              << std::setw(8) << result.avaliacoesDerivada
              << std::setw(9) << result.avaliacoesPhi
              << std::setw(8) << totalEvaluations(result)
              << std::setw(12) << displayNumber(result.tempoMicrosegundos)
              << std::setw(22) << finalBracket
              << std::setw(13) << displayNumber(finalWidth(result))
              << std::setw(8) << result.tentativasExtrapolacao
              << std::setw(8) << result.extrapolacoesAceitas
              << std::setw(8) << result.fallbacksBisseccao
              << std::setw(8) << result.fallbacksFalsaPosicao
              << std::setw(8) << result.fallbacksExtrapolacao << '\n';
}

void printCsvHeader() {
    std::cout << "problem,bracket_a,bracket_b,method,status,root,f_root,final_a,final_b,width,error,"
              << "iterations,f_evaluations,derivative_evaluations,phi_evaluations,total_evaluations,time_us,"
              << "extrapolation_attempts,extrapolations_accepted,bisection_fallbacks,false_position_fallbacks,"
              << "extrapolation_fallbacks\n";
}

void printCsvRow(const BenchmarkCase& benchmarkCase, const Intervalo& bracket,
    double exactRoot, const Method& method, const ResultadoMetodo& result) {
    std::cout << std::setprecision(17)
              << benchmarkCase.problem->id << ',' << bracket.a << ',' << bracket.b << ','
              << method.name << ',' << textoStatusMetodo(result.status) << ','
              << result.raiz << ',' << result.fRaiz << ','
              << result.intervaloFinal.a << ',' << result.intervaloFinal.b << ','
              << finalWidth(result) << ',' << std::abs(result.raiz - exactRoot) << ','
              << result.iteracoes << ',' << result.avaliacoesFuncao << ','
              << result.avaliacoesDerivada << ',' << result.avaliacoesPhi << ','
              << totalEvaluations(result) << ',' << result.tempoMicrosegundos << ','
              << result.tentativasExtrapolacao << ',' << result.extrapolacoesAceitas << ','
              << result.fallbacksBisseccao << ',' << result.fallbacksFalsaPosicao << ','
              << result.fallbacksExtrapolacao << '\n';
}

} // namespace

int main(int argc, char** argv) {
    BenchmarkOptions options;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--format" && index + 1 < argc) {
            options.format = argv[++index];
        } else if (argument == "--tol" && index + 1 < argc) {
            if (!parseDouble(argv[++index], options.tolerance)) {
                printUsage(argv[0]);
                return 2;
            }
        } else if (argument == "--max-iter" && index + 1 < argc) {
            if (!parseIterations(argv[++index], options.maxIterations)) {
                printUsage(argv[0]);
                return 2;
            }
        } else if (argument == "--repetitions" && index + 1 < argc) {
            if (!parseIterations(argv[++index], options.repetitions)) {
                printUsage(argv[0]);
                return 2;
            }
        } else {
            printUsage(argv[0]);
            return 2;
        }
    }
    if (options.format != "table" && options.format != "csv") {
        printUsage(argv[0]);
        return 2;
    }

    const std::vector<Method> availableMethods = methods();
    if (options.format == "table") {
        printTableHeader();
    } else {
        printCsvHeader();
    }
    for (const BenchmarkCase& benchmarkCase : benchmarkCases()) {
        const std::vector<Intervalo> brackets = Isolamento::buscarSubintervalos(*benchmarkCase.problem);
        for (const Intervalo& bracket : brackets) {
            double exactRoot = 0.0;
            if (!referenceRootFor(benchmarkCase, bracket, exactRoot)) {
                std::cerr << "erro: nao foi possivel associar uma raiz de referencia unica ao subintervalo ["
                          << bracket.a << ", " << bracket.b << "] de " << benchmarkCase.problem->id << '\n';
                return 1;
            }
            for (const Method& method : availableMethods) {
                const ResultadoMetodo result = executeMedian(method, *benchmarkCase.problem, bracket, options);
                if (options.format == "table") {
                    printTableRow(benchmarkCase, bracket, exactRoot, method, result);
                } else {
                    printCsvRow(benchmarkCase, bracket, exactRoot, method, result);
                }
            }
        }
    }
    return 0;
}
