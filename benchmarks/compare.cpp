#include "BenchmarkFunctions.hpp"

#include "Bisseccao.hpp"
#include "Proposta.hpp"

#ifdef BENCHMARK_WITH_FALSE_POSITION
#include "FalsaPosicao.hpp"
#endif

#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct BenchmarkOptions {
    std::string format = "table";
    ProposedMethodOptions proposedOptions;
};

struct Method {
    std::string name;
    std::function<ResultadoMetodo(const BenchmarkCase&, const ProposedMethodOptions&)> execute;
};

// Lê um argumento numérico não negativo da linha de comando.
// Entrada: texto recebido e referência para armazenamento. Saída: verdadeiro quando o valor é finito e válido.
bool parseDouble(const std::string& text, double& value) {
    try {
        std::size_t processed = 0;
        value = std::stod(text, &processed);
        return processed == text.size() && std::isfinite(value) && value >= 0.0;
    } catch (const std::exception&) {
        return false;
    }
}

// Lê um número inteiro positivo de iterações da linha de comando.
// Entrada: texto recebido e referência para armazenamento. Saída: verdadeiro quando há um valor positivo válido.
bool parseIterations(const std::string& text, std::size_t& value) {
    try {
        std::size_t processed = 0;
        const unsigned long long parsed = std::stoull(text, &processed);
        if (processed != text.size() || parsed == 0U) {
            return false;
        }
        value = static_cast<std::size_t>(parsed);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Mostra o formato aceito pelo executável de benchmark.
// Entrada: nome do programa. Saída: texto de ajuda no fluxo de erro.
void printUsage(const char* program) {
    std::cerr << "uso: " << program
              << " [--format table|csv] [--x-abs N] [--x-rel N] [--f-tol N] [--max-iter N]\n";
}

// Monta a lista de métodos atualmente disponíveis para comparação.
// Entrada: não recebe argumentos. Saída: métodos com adaptadores para uma interface comum.
std::vector<Method> methods() {
    std::vector<Method> available = {
        {"experimental", [](const BenchmarkCase& test, const ProposedMethodOptions& options) {
            return ProposedMethod::execute(test.function, test.a, test.b, options);
        }},
        {"bisection", [](const BenchmarkCase& test, const ProposedMethodOptions& options) {
            return Bisseccao::executar(test.function, test.a, test.b,
                options.xAbsoluteTolerance, static_cast<int>(options.maxIterations));
        }}
    };

#ifdef BENCHMARK_WITH_FALSE_POSITION
    available.push_back({"false-position", [](const BenchmarkCase& test, const ProposedMethodOptions& options) {
        return FalsaPosicao::executar(test.function, test.a, test.b,
            options.xAbsoluteTolerance, static_cast<int>(options.maxIterations));
    }});
#endif

    return available;
}

// Imprime o cabeçalho da saída formatada em tabela.
// Entrada: não recebe argumentos. Saída: colunas do relatório no terminal.
void printTableHeader() {
    std::cout << std::left << std::setw(20) << "function"
              << std::setw(17) << "method"
              << std::setw(22) << "status"
              << std::right << std::setw(13) << "error"
              << std::setw(8) << "iters"
              << std::setw(8) << "evals"
              << std::setw(13) << "width"
              << std::setw(13) << "f(root)"
              << std::setw(8) << "attempt"
              << std::setw(8) << "accept"
              << std::setw(8) << "fp-fb"
              << std::setw(8) << "ext-fb" << '\n';
}

// Imprime uma linha do benchmark em formato de tabela.
// Entrada: caso de referência, método usado e resultado calculado. Saída: uma linha de métricas no terminal.
void printTableRow(const BenchmarkCase& test, const Method& method, const ResultadoMetodo& result) {
    const double width = result.intervaloFinal.b - result.intervaloFinal.a;
    std::cout << std::left << std::setw(20) << test.name
              << std::setw(17) << method.name
              << std::setw(22) << textoStatusMetodo(result.status)
              << std::right << std::scientific << std::setprecision(5)
              << std::setw(13) << std::abs(result.raiz - test.exactRoot)
              << std::defaultfloat << std::setw(8) << result.iteracoes
              << std::setw(8) << result.avaliacoesFuncao
              << std::scientific << std::setprecision(5) << std::setw(13) << width
              << std::setw(13) << result.fRaiz
              << std::defaultfloat << std::setw(8) << result.tentativasExtrapolacao
              << std::setw(8) << result.extrapolacoesAceitas
              << std::setw(8) << result.fallbacksFalsaPosicao
              << std::setw(8) << result.fallbacksExtrapolacao << '\n';
}

// Imprime o cabeçalho CSV equivalente às métricas da tabela.
// Entrada: não recebe argumentos. Saída: primeira linha CSV no terminal.
void printCsvHeader() {
    std::cout << "function,method,status,root,f_root,a,b,width,error,iterations,evaluations,"
              << "extrapolation_attempts,extrapolations_accepted,bisection_fallbacks,"
              << "false_position_fallbacks,extrapolation_fallbacks\n";
}

// Imprime uma linha CSV com o resultado de um método para um caso.
// Entrada: caso de referência, método usado e resultado calculado. Saída: linha CSV no terminal.
void printCsvRow(const BenchmarkCase& test, const Method& method, const ResultadoMetodo& result) {
    std::cout << test.name << ',' << method.name << ',' << textoStatusMetodo(result.status) << ','
              << std::setprecision(17) << result.raiz << ',' << result.fRaiz << ','
              << result.intervaloFinal.a << ',' << result.intervaloFinal.b << ','
              << (result.intervaloFinal.b - result.intervaloFinal.a) << ','
              << std::abs(result.raiz - test.exactRoot) << ',' << result.iteracoes << ','
              << result.avaliacoesFuncao << ',' << result.tentativasExtrapolacao << ','
              << result.extrapolacoesAceitas << ',' << result.fallbacksBisseccao << ','
              << result.fallbacksFalsaPosicao << ',' << result.fallbacksExtrapolacao << '\n';
}

} // namespace

// Compara os métodos disponíveis nos casos definidos para o benchmark.
// Entrada: opções de formato e tolerância pela linha de comando. Saída: zero no sucesso e dois para argumento inválido.
int main(int argc, char** argv) {
    BenchmarkOptions benchmarkOptions;

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--format" && index + 1 < argc) {
            benchmarkOptions.format = argv[++index];
        } else if (argument == "--x-abs" && index + 1 < argc) {
            if (!parseDouble(argv[++index], benchmarkOptions.proposedOptions.xAbsoluteTolerance)) {
                printUsage(argv[0]);
                return 2;
            }
        } else if (argument == "--x-rel" && index + 1 < argc) {
            if (!parseDouble(argv[++index], benchmarkOptions.proposedOptions.xRelativeTolerance)) {
                printUsage(argv[0]);
                return 2;
            }
        } else if (argument == "--f-tol" && index + 1 < argc) {
            if (!parseDouble(argv[++index], benchmarkOptions.proposedOptions.functionTolerance)) {
                printUsage(argv[0]);
                return 2;
            }
        } else if (argument == "--max-iter" && index + 1 < argc) {
            if (!parseIterations(argv[++index], benchmarkOptions.proposedOptions.maxIterations)) {
                printUsage(argv[0]);
                return 2;
            }
        } else {
            printUsage(argv[0]);
            return 2;
        }
    }

    if (benchmarkOptions.format != "table" && benchmarkOptions.format != "csv") {
        printUsage(argv[0]);
        return 2;
    }

    const std::vector<Method> availableMethods = methods();
    if (benchmarkOptions.format == "table") {
        printTableHeader();
    } else {
        printCsvHeader();
    }
    for (const BenchmarkCase& test : benchmarkCases()) {
        for (const Method& method : availableMethods) {
            const ResultadoMetodo result = method.execute(test, benchmarkOptions.proposedOptions);
            if (benchmarkOptions.format == "table") {
                printTableRow(test, method, result);
            } else {
                printCsvRow(test, method, result);
            }
        }
    }
    return 0;
}
