import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# 1. Lê o arquivo gerado pelo C++
try:
    df = pd.read_csv('resultados.csv')
except FileNotFoundError:
    print("Erro: O arquivo 'resultados.csv' não foi encontrado.")
    exit()

# CORREÇÃO A: Agrupar valores de 'n' repetidos (ex: os dois n=500) tirando a média do tempo
df = df.groupby('n').mean().reset_index()

# CORREÇÃO B: Substituir tempos exatos de 0.0 por um número minúsculo para a escala log funcionar
df['guloso'] = df['guloso'].replace(0.0, 1e-6)
df['pd'] = df['pd'].replace(0.0, 1e-6)

plt.figure(figsize=(10, 6))

# Plotando as linhas
plt.plot(df['n'], df['guloso'], marker='o', label='Guloso (O(n log n))', color='blue', linewidth=2)
plt.plot(df['n'], df['pd'], marker='s', label='Prog. Dinâmica (O(n * Q))', color='green', linewidth=2)

# Tratando a Força Bruta (ignorando os NaNs gerados após o limite)
df_fb = df.dropna(subset=['fb'])
plt.plot(df_fb['n'], df_fb['fb'], marker='x', label='Força Bruta (O(2^n))', color='red', linestyle='--', markersize=8)

# Configurações de escala
plt.xscale('log')
plt.yscale('log')

plt.title('Comparativo de Tempo: Problema da Mochila 0-1', fontsize=14, fontweight='bold')
plt.xlabel('Número de Itens (n) - Escala Log', fontsize=12)
plt.ylabel('Tempo (segundos) - Escala Log', fontsize=12)

plt.grid(True, which="both", ls="--", alpha=0.5)
plt.legend(fontsize=11)

plt.tight_layout()
plt.savefig('grafico.png', dpi=300)
plt.show()