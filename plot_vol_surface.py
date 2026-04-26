import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm


df = pd.read_csv("data/vol_surface_sample.csv")

pivot = df.pivot(index="maturity", columns="strike", values="implied_vol")

maturities = pivot.index.to_numpy()
strikes = pivot.columns.to_numpy()
vol_grid = pivot.to_numpy()


def bilinear_interpolate(maturities, strikes, vol_grid, maturity, strike):
    """
    Interpolation bilinéaire sur une grille maturity x strike.
    """

    # Trouver les indices qui encadrent maturity
    i = np.searchsorted(maturities, maturity) - 1
    j = np.searchsorted(strikes, strike) - 1

    # Sécurité : rester dans la grille
    i = np.clip(i, 0, len(maturities) - 2)
    j = np.clip(j, 0, len(strikes) - 2)

    T1, T2 = maturities[i], maturities[i + 1]
    K1, K2 = strikes[j], strikes[j + 1]

    v11 = vol_grid[i, j]
    v12 = vol_grid[i, j + 1]
    v21 = vol_grid[i + 1, j]
    v22 = vol_grid[i + 1, j + 1]

    # Poids d'interpolation
    wT = (maturity - T1) / (T2 - T1)
    wK = (strike - K1) / (K2 - K1)

    # Interpolation en strike
    v_T1 = (1 - wK) * v11 + wK * v12
    v_T2 = (1 - wK) * v21 + wK * v22

    # Interpolation en maturité
    return (1 - wT) * v_T1 + wT * v_T2


def get_vol(maturity, strike):
    """
    Équivalent Python de ImpliedVolSurface::getVol.
    """

    # Cas 1 : le point existe exactement dans la grille
    for i, T in enumerate(maturities):
        if abs(maturity - T) < 1e-12:
            for j, K in enumerate(strikes):
                if abs(strike - K) < 1e-12:
                    return vol_grid[i, j]

    # Cas 2 : sinon interpolation
    return bilinear_interpolate(maturities, strikes, vol_grid, maturity, strike)


# Grille brute
K_grid, T_grid = np.meshgrid(strikes, maturities)

# Grille fine
fine_maturities = np.linspace(maturities.min(), maturities.max(), 80)
fine_strikes = np.linspace(strikes.min(), strikes.max(), 120)

fine_K_grid, fine_T_grid = np.meshgrid(fine_strikes, fine_maturities)

# Application de get_vol sur chaque point de la grille fine
fine_vol = np.array([
    [get_vol(T, K) for K in fine_strikes]
    for T in fine_maturities
])


fig1 = plt.figure(figsize=(10, 6))
ax1 = fig1.add_subplot(111, projection="3d")
ax1.scatter(K_grid, T_grid, vol_grid, s=50)
ax1.set_xlabel("Strike")
ax1.set_ylabel("Maturity")
ax1.set_zlabel("Implied Vol")
ax1.set_title("ImpliedVolGrid - grille discrete")
plt.tight_layout()
plt.savefig("implied_vol_grid.png", dpi=200)


fig2 = plt.figure(figsize=(10, 6))
ax2 = fig2.add_subplot(111, projection="3d")
ax2.plot_surface(
    fine_K_grid,
    fine_T_grid,
    fine_vol,
    cmap=cm.viridis,
    linewidth=0,
    antialiased=True
)
ax2.scatter(K_grid, T_grid, vol_grid, s=20)
ax2.set_xlabel("Strike")
ax2.set_ylabel("Maturity")
ax2.set_zlabel("Implied Vol")
ax2.set_title("ImpliedVolSurface - interpolation bilineaire")
plt.tight_layout()
plt.savefig("implied_vol_surface.png", dpi=200)

plt.show()