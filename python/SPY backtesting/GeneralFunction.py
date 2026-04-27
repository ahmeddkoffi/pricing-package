
import scipy.optimize as opt
import plotly.graph_objects as go
import yfinance as yf
import numpy as np
import pandas as pd
import pypfopt as pf

def download_data(tickers, start_date, end_date):
    """Downloads adjusted closing prices and returns the returns."""
    try:
        data = yf.download(tickers, start=start_date, end=end_date)['Close']
        returns = data.pct_change().dropna()
        return returns
    except Exception as e:
        print(f"Error during data download: {e}")
        return None



def calculate_metrics(portfolio_returns, spy_returns, confidence_level=0.95):
    """Calculates performance metrics for a given tranche, including VaR."""
    try:
        # Covariance matrix and beta
        cov_matrix = np.cov(portfolio_returns, spy_returns)
        beta = cov_matrix[0, 1] / cov_matrix[1, 1]

        # Expected Annual Return and Annual Volatility
        expected_annual_return = float(portfolio_returns.mean() * 252)
        annual_volatility = float(portfolio_returns.std() * np.sqrt(252))

        # Cumulative Return
        cumulative_return = float(((1 + portfolio_returns).prod() - 1))

        # Sharpe Ratio and Treynor Ratio
        sharpe_ratio = float(cumulative_return / portfolio_returns.std()/np.sqrt(63)) # 2% Risk free rate annually what gives 0.66% quaterly(because the portfolio is rebalanced quaterly)
        treynor_ratio = float(cumulative_return / beta)

        # Value at Risk (VaR) - Parametric method (normal distribution assumption)
        var = np.percentile(portfolio_returns, (1 - confidence_level) * 100)  # VaR at given confidence level

        return {
            "Expected Annual Return": expected_annual_return,
            "Annual Volatility": annual_volatility,
            "Sharpe Ratio": sharpe_ratio,
            "Treynor Ratio": treynor_ratio,
            "Cumulative Return": cumulative_return,
            "VaR (at {}% confidence)".format(int(confidence_level * 100)): var
        }
    except Exception as e:
        print(f"Error during metrics calculation: {e}")
        return None


def split_data(returns, benchmarks_return, train_size):
    """Splits the data into training and test sets."""
    try:
        train_data = returns.iloc[:train_size]
        test_data = returns.iloc[train_size:]
        spy_test_data = benchmarks_return.iloc[train_size:]
        return train_data, test_data, spy_test_data
    except Exception as e:
        print(f"Error during data splitting: {e}")


def weight_dic(tickers, weights):
    """Creates a dictionary of weights for a given portfolio."""
    try:
        weight_dict = {}
        for ticker, weight in zip(tickers, weights):
            weight_dict[ticker] = weight
        return weight_dict

    except Exception as e:
        print(f"Error creating the weight dictionary: {e}")
        return None


def transaction_costs(w, w0, TC):
    """Calculates transaction costs."""
    return np.sum(TC * np.abs(w - w0)**2)


def objective(w, cov_mat, w0, TC, factor_TC):
    """Objective function for optimization."""
    s = 0
    for i in range(len(w)):
        for j in range(len(w)):
            s += (w[i] * (cov_mat @ w)[i] - w[j] * (cov_mat @ w)[j])**2
    return s + factor_TC * transaction_costs(w, w0, TC)


def optimize_risk_par(df, w0=np.array([0.2, 0.2, 0.2, 0.2, 0.2]), TC=np.array([0.0005, 0.0005, 0.0005, 0.0005, 0.0005]), factor_TC=.0001):
    """
    Calculates the weights for the risk parity portfolio.
    df: DataFrame with the ETF returns
    w0: Initial weights (current portfolio)
    TC: Transaction costs (5 bps)
    factor_TC: Scaling factor for transaction costs (tune this as needed)
    """
    try:
        cov_mat = df.cov().values  # Covariance matrix of ETF returns

        # Constraints: weights sum to 1 and are long-only
        constraints = [{'type': 'eq', 'fun': lambda w: np.sum(w) - 1},  # Sum of weights = 1
                       {'type': 'ineq', 'fun': lambda w: w}]           # Weights must be positive (long only)

        # Optimize
        result = opt.minimize(objective, w0, args=(cov_mat, w0, TC, factor_TC), constraints=constraints, tol=1e-20)
        w_opt = result.x  # Optimized weights
        return w_opt

    except Exception as e:
        print(f"Error during optimization: {e}")
        return None


def optimize_min_var(train_data):
    """Performs Min-Var optimization and returns optimal weights."""
    try:
        mean_returns = train_data.mean()
        cov_matrix = train_data.cov()
        ef = pf.EfficientFrontier(mean_returns, cov_matrix)
        weights = ef.min_volatility()
        return weights
    except Exception as e:
        print(f"Error during optimization: {e}")
        return None




color_palette = {
    "Risk Parity": "blue",
    "Equal Weight": "green",
    "SPY": "red",
    "Minimum Variance": "orange"
}

def add_colored_traces(fig, input_fig, row, col):
    global color_palette  # Make color_palette accessible inside the function
    for trace in input_fig.data:
        trace.update(marker=dict(color=color_palette.get(trace.name, 'gray')))  # Fallback to gray
        fig.add_trace(trace, row=row, col=col)


