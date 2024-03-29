import math
import statistics
import random
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm
import numpy as np
import plotly.graph_objects as go
from plotly.subplots import make_subplots

class Autocall_CCBN(object):
    """
    Autocallable contingent coupon barrier notes pricing module
    with Monte Carlo simulation to price.
    """

    def __init__(self, coupon, principal, call_level, barrier, strike, fixing_schedule):
        """
        Initialize the Autocall_CCBN object with parameters.
        """
         
        self._coupon = coupon
        self._principal = principal
        self._call_level = call_level  
        self._barrier = barrier
        self._strike = strike
        self._fixing_schedule = fixing_schedule
        self._npaths_mc = 10000
        self._nsteps_mc = 300
        self._rnd_seed = 10000
        self._delta_bump=10
        self._delta_bump_is_percent=True
        self._gamma_bump=10
        self._gamma_bump_is_percent=True
        self._vega_bump=0.1
        self._vega_bump_is_percent=False
        self._theta_bump=1/365
        self._vanna_dvega = False
        self._rho_bump = 0.1
        self._spot_minimum = 10e-6

    def gen_fixings(self, spot, vol, r, div):
        """
        Generate fixing schedule based on the given parameters.
        """
        if spot <= 0:
            spot = self._spot_minimum
        s_fix = []
        t_fix = self._fixing_schedule
        for index in range(len(self._fixing_schedule)):
            if index == 0:
                t = t_fix[index]
                sigma = math.sqrt(t)
                fix = spot * math.exp((r - div - 0.5 * vol**2) * t + vol * random.gauss(0, sigma))
            else:
                t = t_fix[index] - t_fix[index - 1]
                sigma = math.sqrt(t)
                fix = s_fix[index - 1] * math.exp((r - div - 0.5 * vol**2) * t + vol * random.gauss(0, sigma))
            s_fix.append(fix)
        return s_fix

    def pricer(self, spot, vol, rate, div, debug=False):
        """
        Price the Autocallable product using Monte Carlo simulation.
        """
        n_paths = self._npaths_mc
        rndseed = self._rnd_seed
        if rndseed is not None:
            random.seed(rndseed)
        T = self._fixing_schedule[-1]
        K = self._strike
        C = self._call_level
        coupon = self._coupon
        B = self._barrier
        N = self._principal
        t_fix = self._fixing_schedule
        n_fix = len(t_fix)  # total number of fixings is n_fix
        n_KI = 0  # number of paths ended with knock-in
        stat_call = [0] * n_fix  # create an array n_fix * 1 to store statistics of call event
        payoff_by_path = []

        for i in range(n_paths):
            s_fix = self.gen_fixings(spot, vol, rate, div)
            payoff = 0
            missed_coupon = 1
            exp = math.exp
            ds_principal = exp(-rate * T) * N # Nominal value discounted

            for j in range(n_fix):
                df = exp(-rate * t_fix[j])
                if s_fix[j] < C:  # If the spot on observation date does not reach callable level
                    if s_fix[j] >= B:
                        payoff = df * N * coupon * i + payoff  # pay coupon/ N*df is the discounted valu of the nominal at a given fixing date 
                        missed_coupon = 1  
                    elif s_fix[j] < B:
                        if j < n_fix - 1:  # observation date is not the last one
                            missed_coupon = missed_coupon+1  # pay nothing but i increase as we will the missed coupon
                        else:
                            payoff = payoff + (s_fix[j] - K) / K * N
                            n_KI = n_KI + 1
                elif s_fix[j] >= C:
                    stat_call[j] = stat_call[j] + 1  # knock out at observation date j
                    payoff = N * (1+coupon * i) + payoff 
                    ds_principal = math.exp(-rate * t_fix[j]) * N
                    break

            payoff = payoff + ds_principal - N  
            payoff_by_path.append(payoff)

        price = statistics.mean(payoff_by_path)

        if debug:
            print('Autocall price = %.4f' % price)
            path_no_call = n_paths - sum(stat_call) # Number of path without early redemption
            percent_KI = n_KI / n_paths * 100
            percent_no_call = path_no_call / n_paths * 100
            print('Call event distribution:')
            for i in range(n_fix):
                stat_call[i] = stat_call[i] / n_paths * 100  # transform to percentage
                print('%.2f%%' % stat_call[i], end=', ')
            print('Probability of no call event: %.2f%%.' % percent_no_call)
            print('Probability of knock-in at maturity: %.2f' % percent_KI)

        return price


    def delta(self, spot, vol, rate, div, model):
        """
        Calculate the delta of the Autocallable product.
        """
        if spot <= 0:
            spot = self._spot_minimum
        bumpvalue = self._delta_bump
        is_bump_percent = self._delta_bump_is_percent
        price = model(spot, vol, rate, div)

        if is_bump_percent:
            spot_up = spot * (1 + bumpvalue / 100)
        else:
            spot_up = spot + bumpvalue

        price_up = model(spot_up, vol, rate, div)
        delta_fd = (price_up - price) / (spot_up - spot)

        return delta_fd

    def gamma(self, spot, vol, rate, div, model):
        """
        Calculate the gamma of the Autocallable product.
        """
        if spot <= 0:
            spot = 0.0001
        bumpvalue = self._gamma_bump
        is_bump_percent = self._gamma_bump_is_percent
        price = model(spot, vol, rate, div)

        if is_bump_percent:
            spot_up = spot * (1 + bumpvalue / 100)
            spot_down = spot * (1 - bumpvalue / 100)
        else:
            spot_up = spot + bumpvalue
            spot_down = spot - bumpvalue

        price_up = model(spot_up, vol, rate, div)
        price_down = model(spot_down, vol, rate, div)
        gamma_fd = (price_up + price_down - 2 * price) / ((spot_up - spot) ** 2)

        return gamma_fd

    def vega(self, spot, vol, rate, div, model):
        """
        Calculate the vega of the Autocallable product.
        """
        if spot <= 0:
            spot = 0.0001
        price = model(spot, vol, rate, div)
        bumpvalue = self._vega_bump

        if self._vega_bump_is_percent:
            vol_up = vol * (1 + bumpvalue / 100)
        else:
            vol_up = vol + bumpvalue

        price_up = model(spot, vol_up, rate, div)
        vega_fd = (price_up - price) / (vol_up - vol)

        return vega_fd

    def theta(self, spot, vol, rate, div, model):
        """
        Calculate the theta of the Autocallable product.
        """
        if spot <= 0:
            spot = 0.0001
        price = model(spot, vol, rate, div)
        bumpvalue = self._theta_bump
        old_fixing_time = self._fixing_schedule
        new_fixing_time = []

        for fix_time in old_fixing_time:
            new_time = fix_time - bumpvalue
            if new_time > 0:
                new_fixing_time.append(new_time)

        self._fixing_schedule = new_fixing_time
        price_shift = model(spot, vol, rate, div)
        theta_fd = (price_shift - price) / bumpvalue * 1 / 365
        self._fixing_schedule = old_fixing_time

        return theta_fd

    def vanna(self, spot, vol, rate, div, model):
        """
        Calculate the vanna of the Autocallable product.
        """
        if spot <= 0:
            spot = 0.0001
        if not self._vanna_dvega:
            delta = self.delta(spot, vol, rate, div, model)
            bumpvalue = self._vega_bump

            if self._vega_bump_is_percent:
                vol_up = vol * (1 + bumpvalue / 100)
            else:
                vol_up = vol + bumpvalue

            delta_up = self.delta(spot, vol_up, rate, div, model)
            vanna_value = (delta_up - delta) / (vol_up - vol)
        else:
            vega = self.vega(spot, vol, rate, div, model)
            bumpvalue = self._delta_bump

            if self._delta_bump_is_percent:
                spot_up = spot * (1 + bumpvalue / 100)
            else:
                spot_up = spot + bumpvalue

            vega_up = self.vega(spot, vol, rate, div, model)
            vanna_value = (vega_up - vega) / (spot_up - spot)

        return vanna_value

    def volga(self, spot, vol, rate, div, model):
        """
        Calculate the volga of the Autocallable product.
        """
        if spot <= 0:
            spot = self._spot_minimum
        vega = self.vega(spot, vol, rate, div, model)
        bumpvalue = self._vega_bump

        if self._vega_bump_is_percent:
            vol_up = vol * (1 + bumpvalue / 100)
        else:
            vol_up = vol + bumpvalue

        vega_up = self.vega(spot, vol_up, rate, div, model)
        volga_value = (vega_up - vega) / (vol_up - vol)

        return volga_value
    
    def rho(self,spot,vol,rate,div,model):
        if spot<=0:
            spot = self._spot_minimum
        bumpvalue = self._rho_bump
        price = model(spot,vol,rate,div)
        rate_up = rate + bumpvalue
        price_up = model(spot,vol,rate_up,div)
        rho = (price_up - price) / (rate_up - rate) / 100
        return rho

    
    

    def spot_ladder(self,spot_list, vol, rate, div):
            n=len(spot_list)
            pl = []
            delta=[]
            gamma=[]
            vega=[]
            theta=[]
            rho=[]
            volga=[]
            vanna=[]
            i = 0
            for s in spot_list: 
                progress= int(i/n * 100)
                print('Spot = %f, in progress %d complete' % (s, progress))
                y_pl= self.pricer(s,vol,rate,div)
                pl.append(y_pl)
                y_delta = self.delta(s,vol,rate,div,self.pricer)
                delta.append(y_delta)
                y_gamma = self.gamma(s,vol,rate,div,self.pricer)
                gamma.append(y_gamma)
                y_vega = self.vega(s,vol,rate,div,self.pricer)
                vega.append(y_vega)
                y_rho = self.rho(s,vol,rate,div,self.pricer)
                rho.append(y_rho)        
                y_theta = self.theta(s,vol,rate,div,self.pricer)
                theta.append(y_theta)
                y_vanna = self.vanna(s,vol,rate,div,self.pricer)
                vanna.append(y_vanna)
                y_volga = self.volga(s,vol,rate,div,self.pricer)
                volga.append(y_volga)
                i= i + 1
                
                
            fig = make_subplots(rows=2, cols=4, subplot_titles=['P&L', 'Delta', 'Gamma', 'Theta', 'Rho', 'Vega', 'Volga', 'Vanna'],
                    shared_xaxes=True, vertical_spacing=0.1, horizontal_spacing=0.1)

            # Adding traces to the subplots
            fig.add_trace(go.Scatter(x=spot_list, y=pl, mode='lines', name='P&L'), row=1, col=1)
            fig.add_trace(go.Scatter(x=spot_list, y=delta, mode='lines', name='Delta'), row=1, col=2)
            fig.add_trace(go.Scatter(x=spot_list, y=gamma, mode='lines', name='Gamma'), row=1, col=3)
            fig.add_trace(go.Scatter(x=spot_list, y=theta, mode='lines', name='Theta'), row=1, col=4)
            fig.add_trace(go.Scatter(x=spot_list, y=rho, mode='lines', name='Rho'), row=2, col=1)
            fig.add_trace(go.Scatter(x=spot_list, y=vega, mode='lines', name='Vega'), row=2, col=2)
            fig.add_trace(go.Scatter(x=spot_list, y=volga, mode='lines', name='Volga'), row=2, col=3)
            fig.add_trace(go.Scatter(x=spot_list, y=vanna, mode='lines', name='Vanna'), row=2, col=4)

            # Update layout
            fig.update_layout(height=500, width=1000, showlegend=True)

            # Show the plot
            fig.show()



def main_autocall():
    # Parameters
    spot = 274.6
    vol = 0.2
    rate = 0.01
    div = 0.005
    principal = -1000
    strike = spot
    call_level = spot
    barrier = 0.7 * spot

    fixings_schedule = [91 / 365 * (i + 1) for i in range(12)]
    coupon = 10.35 / 100 / 4

    # Create Autocall_CCBN instance
    autocall1 = Autocall_CCBN(coupon, principal, call_level, barrier, strike, fixings_schedule)

    # Set Monte Carlo simulation parameters
    autocall1._npaths_mc = 1000 
    autocall1._delta_bump = 10
    autocall1._gamma_bump = 10
    autocall1._vega_bump = 0.1
    autocall1._theta_bump = 0.1

    # Uncomment the following line to print debug information from the pricer
    autocall1.pricer(spot, vol, rate, div, True)

    # Generate spot ladder and plot Greeks
    spot_list = np.arange(150, 400, 5)
    #autocall1.spot_ladder(spot_list, vol, rate, div)
    

if __name__ == '__main__':
    main_autocall()
