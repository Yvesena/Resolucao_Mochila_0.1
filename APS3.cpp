#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Item {
    int id;
    int peso;
    int valor;
    double razao;
};

// ALGORITMO GULOSO 
bool compararRazao(Item a, Item b) {
    return a.razao > b.razao;
}

int guloso(int Q, vector<Item> itens) {
    sort(itens.begin(), itens.end(), compararRazao); //0(nlogn) 
    int valorTotal = 0;
    int pesoAtual = 0;
    for (const auto& item : itens) {
        if (pesoAtual + item.peso <= Q) {
            pesoAtual += item.peso;
            valorTotal += item.valor;
        }
    }
    return valorTotal;
}

// --- FORÇA BRUTA RECURSIVA ---
int forcaBruta(int Q, const vector<Item>& itens, int n) {
    if (n == 0 || Q == 0) return 0;
    if (itens[n - 1].peso > Q) return forcaBruta(Q, itens, n - 1);
    else return max(itens[n - 1].valor + forcaBruta(Q - itens[n - 1].peso, itens, n - 1),
                    forcaBruta(Q, itens, n - 1)); // 0 ( 2^n)
}

// --- PROGRAMAÇÃO DINÂMICA (Bottom-up com otimização de espaço) ---
int programacaoDinamica(int Q, const vector<Item>& itens) {
    vector<int> dp(Q + 1, 0);// dp[j] vai representar o vm possível para uma m f de capacidade j
    for (const auto& item : itens) {
        for (int j = Q; j >= item.peso; j--)//tenta encaixar o item nas várias capacidades j 
        {
            dp[j] = max(dp[j], dp[j - item.peso] + item.valor);
        }
    }
    return dp[Q]; // re sol ot
} // 0(n x Q)

// --- GERADOR DE INSTÂNCIAS ---
vector<Item> gerarItens(int n, int minP, int maxP, int minV, int maxV) {
    vector<Item> itens;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> disP(minP, maxP);
    uniform_int_distribution<> disV(minV, maxV);

    for (int i = 0; i < n; i++) {
        int p = disP(gen);
        int v = disV(gen);
        itens.push_back({i, p, v, (double)v / p});
    }
    return itens;
}

int main() {
    // Parâmetros da tabela 
    struct Teste { int n, Q, minP, maxP; };
    vector<Teste> testes = {
        {20, 10, 1, 10}, {50, 30, 1, 30}, {100, 30, 1, 30},
        {200, 50, 1, 50}, {500, 10, 1, 10}, {500, 100, 1, 100},
        {1000, 50, 1, 50}, {10000, 100, 1, 100}, {500000, 100, 1, 100}
    };
     //
        
    cout << "n\tQ\tGuloso(s)\tPD(s)\t\tFB(s)" << endl;

    ofstream arquivo("resultados.csv");
    arquivo << "n,Q,guloso,pd,fb\n"; // Cabeçalho do CSV

    std::cout << std::string(65, '-') << "\n";
    std::cout << std::left << std::setw(10) << "n" 
              << std::left << std::setw(10) << "Q"
              << std::right << std::setw(15) << "Guloso (s)"
              << std::right << std::setw(15) << "PD (s)"
              << std::right << std::setw(15) << " FB (s)" << "\n";

    for (auto t : testes) {
        auto itens = gerarItens(t.n, t.minP, t.maxP, 1, 1000);
        
        // Medição Guloso
        auto start = std::chrono::high_resolution_clock::now();
        guloso(t.Q, itens);
        auto stop = std::chrono::high_resolution_clock::now();
        double tGuloso = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() / 1e9;

        // Medição PD
        start = std::chrono::high_resolution_clock::now();
        programacaoDinamica(t.Q, itens);
        stop = std::chrono::high_resolution_clock::now();
        double tPD = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() / 1e9;

        // Medição FB (com trava de segurança)
        double tFB = -1; 
        if (t.n <= 25) { 
            start = std::chrono::high_resolution_clock::now();
            forcaBruta(t.Q, itens, t.n);
            stop = std::chrono::high_resolution_clock::now();
            tFB = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() / 1e9;
        }

        // Salva no arquivo CSV
        arquivo << t.n << "," << t.Q << "," << tGuloso << "," << tPD << ",";
        if (tFB != -1) arquivo << tFB << "\n";
        else arquivo << "NaN\n";

        // ---------------------------------------------------------
        // IMPRESSÃO DA LINHA FORMATADA NO TERMINAL
        // ---------------------------------------------------------
        std::cout << std::left << std::setw(10) << t.n 
                  << std::left << std::setw(10) << t.Q
                  << std::fixed << std::setprecision(6) // Fixa em 6 casas decimais
                  << std::right << std::setw(15) << tGuloso
                  << std::right << std::setw(15) << tPD;
        
        if (tFB != -1) {
            std::cout << std::right << std::setw(15) << tFB << "\n";
        } else {
            std::cout << std::right << std::setw(15) << "NaN" << "\n";
        }
    }

    std::cout << std::string(65, '-') << "\n"; // Linha de fechamento da tabela

    arquivo.close();
    return 0;
}