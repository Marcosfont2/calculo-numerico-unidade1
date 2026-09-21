#ifndef TIPOS_HPP
#define TIPOS_HPP

#include <string>
#include <vector>
#include <functional>
#include <cstddef>

// Representa os motivos de término compartilhados pelos métodos de refinamento.
enum class MetodoStatus {
    NaoExecutado,
    RaizExata,
    ToleranciaFuncao,
    ToleranciaIntervalo,
    ArgumentoInvalido,
    EntradaNaoFinita,
    IntervaloInvalido,
    SemMudancaDeSinal,
    AvaliacaoNaoFinita,
    MultiplasMudancasDeSinal,
    FalhaNumerica,
    MaximoDeIteracoes,
    Estagnacao
};

// Converte o status interno para um texto curto usado na saída do benchmark.
// Entrada: um valor de MetodoStatus. Saída: descrição estável do motivo de término.
inline const char* textoStatusMetodo(MetodoStatus status) {
    switch (status) {
        case MetodoStatus::NaoExecutado: return "not-run";
        case MetodoStatus::RaizExata: return "exact-root";
        case MetodoStatus::ToleranciaFuncao: return "function-tolerance";
        case MetodoStatus::ToleranciaIntervalo: return "interval-tolerance";
        case MetodoStatus::ArgumentoInvalido: return "invalid-argument";
        case MetodoStatus::EntradaNaoFinita: return "nonfinite-input";
        case MetodoStatus::IntervaloInvalido: return "invalid-interval";
        case MetodoStatus::SemMudancaDeSinal: return "no-sign-change";
        case MetodoStatus::AvaliacaoNaoFinita: return "nonfinite-evaluation";
        case MetodoStatus::MultiplasMudancasDeSinal: return "multiple-sign-changes";
        case MetodoStatus::FalhaNumerica: return "numeric-failure";
        case MetodoStatus::MaximoDeIteracoes: return "max-iterations";
        case MetodoStatus::Estagnacao: return "stagnation";
    }
    return "unknown-status";
}

// Representa um intervalo [a, b]
struct Intervalo {
    double a;
    double b;
};

// Estrutura para armazenar o resultado de qualquer método de refinamento
struct ResultadoMetodo {
    std::string nomeMetodo;
    double raiz = 0.0;
    double fRaiz = 0.0;
    int iteracoes = 0;
    double tempoMicrosegundos = 0.0;
    bool convergiu = false;
    std::string mensagemErro = "";
    MetodoStatus status = MetodoStatus::NaoExecutado;
    Intervalo intervaloFinal = {0.0, 0.0};
    std::size_t avaliacoesFuncao = 0;
    std::size_t tentativasExtrapolacao = 0;
    std::size_t extrapolacoesAceitas = 0;
    std::size_t fallbacksBisseccao = 0;
    std::size_t fallbacksFalsaPosicao = 0;
    std::size_t fallbacksExtrapolacao = 0;
};

// Representa a função e seus parâmetros associados
struct ProblemaFuncao {
    std::string id;                                // Ex: "f1", "f2"
    std::string expressao;                         // Ex: "2x^4 + 4x^3 + 3x^2 - 10x - 15"
    std::function<double(double)> f;              // Função f(x)
    std::function<double(double)> df;             // Derivada f'(x) (nos casos que for necessária)
    std::function<double(double)> phi;            // função de iteração para o ponto fixo
    Intervalo intervaloInicial;                    // Intervalo I = [a, b]
    double passoH;                                 // Passo h para varredura
};

#endif // TIPOS_HPP
