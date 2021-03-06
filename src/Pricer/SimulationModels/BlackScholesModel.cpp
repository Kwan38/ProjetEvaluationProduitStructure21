#include "BlackScholesModel.hpp"


BlackScholesModel::BlackScholesModel(int assetNb, int economyNb, RateModelGen **rateModel, PnlVect *simulationSchedule)
        : ModelGen(assetNb, economyNb, rateModel, "Black Scholes"), scheduleSimulation(simulationSchedule) {
    this->choleskyCorr = pnl_mat_create_from_scalar(assetNb,assetNb,0);
    for (int i = 0; i < assetNb; ++i)
        MLET(choleskyCorr,i,i) = 1.;
    this->Gi_ = pnl_vect_new();
    this->LGi_ = pnl_vect_new();
    this->St = pnl_vect_new();
}

void BlackScholesModel::Simulate(double t, double maturity, PnlMat *path, const PnlMat *past, int stepNb) {

    if(scheduleSimulation == NULL){
// Simple case of t == 0
        if (t==0. || past == NULL) {
            Simulate(maturity, path, stepNb);
            return;
        }

// Case in t != 0
        // Copy of the past matrix in the path matrix before S_t
        for (int i = 0; i < past->m - 1; ++i) {
            for (int d = 0; d < past->n; ++d) {
                PNL_MSET(path,i,d,MGET(past,i,d));
            }
        }
        // NB : The last row is S(t), doesn't belong to a step of constatation !
        pnl_vect_resize(St,path->n);
        pnl_mat_get_row(St, past, past->m - 1);
        // Size of a step
        double step = maturity/(double)stepNb;
        double sqrtStep = sqrt(step);
        // The index after t
        int indexBeforet = past->m - 1;
        // Number of value to simulate
        int nbToSimulate = stepNb - indexBeforet + 1;
        // Initialisation of previous value due to simulate S~
        double value_tiMinus1[path->n];
        // Size of the first step (t_(i+1) - t)
        double firstStep = MAX(step * indexBeforet - t,0);
        double sqrtFirstStep = sqrt(firstStep);
        // G_0 : First gaussian vector
        pnl_vect_rng_normal_d(Gi_,path->n,rng);
        // All the LdGi
        LGi_ = pnl_mat_mult_vect(choleskyCorr,Gi_);

        double sigma_d, Sd_t, LdGi, value;
        for (int d = 0; d < path->n; ++d) {
            sigma_d = GET(volatility,d);
            Sd_t = GET(St, d);
            LdGi = GET(LGi_, d);

            value = exp(rateModels[assetList->assets[d]->change]->GetIntegralRate(t, t + firstStep)
                        - (sigma_d * sigma_d / 2) * firstStep + sigma_d * sqrtFirstStep * LdGi
            );

            // Store in path
            MLET(path, indexBeforet, d) = (Sd_t * value);
            // Store in value_tiMinus1
            value_tiMinus1[d] = value;
        }
        // For all other simulation
        double tiMinus1;
        for (int i = 1; i < nbToSimulate; ++i) {
            // Same processus that beside but with a different step
            tiMinus1 = (indexBeforet + i - 1) * step;
            pnl_vect_rng_normal_d(Gi_, path->n, rng);
            LGi_ = pnl_mat_mult_vect(choleskyCorr, Gi_);
            // For each asset
            for (int d = 0; d < path->n; ++d) {
                sigma_d = GET(volatility,d);
                Sd_t = GET(St, d);
                LdGi = GET(LGi_, d);

                value = value_tiMinus1[d] * exp(
                        rateModels[assetList->assets[d]->change]->GetIntegralRate(tiMinus1, tiMinus1 + step)
                        - (sigma_d * sigma_d/2) * step + sigma_d * sqrtStep * LdGi
                );

                // Store in path and value_tiMinus1
                PNL_MSET(path, (indexBeforet + i), d, (Sd_t * value));
                value_tiMinus1[d] = value;
            }
        }
    }else{

        //Init historical data
        if(past == NULL){
            for (int d = 0; d < path->n; ++d)
                PNL_MSET(path, 0, d, GET(spot,d));
        }else{
            for (int i = 0; i < past->m - 1; ++i) {
                for (int d = 0; d < past->n; ++d) {
                    PNL_MSET(path,i,d,MGET(past,i,d));
                }
            }
        }
        // NB : The last row is S(t), doesn't belong to a step of constatation !
        pnl_vect_resize(St,path->n);
        pnl_mat_get_row(St, past, past->m - 1);

        // Size of a step
       //--> FAUX double step = GET(scheduleSimulation,0);


        double step;
        // The index before
        int indexBeforet;
        int tmp = 0;
        double firstStep;
        for(int i = 0; i < scheduleSimulation->size;i++){
            if(t <= tmp){
                indexBeforet = i;
                firstStep = MAX(tmp - t,0);
                break;
            }
            tmp+=GET(scheduleSimulation,i);
        }

        double sqrtFirstStep = sqrt(firstStep);




        // Number of value to simulate
        int nbToSimulate = stepNb - indexBeforet ;

        // Initialisation of previous value due to simulate S~
        double value_tiMinus1[path->n];

        // Size of the first step (t_(i+1) - t)


        double firstStep = MAX(step * indexBeforet - t,0);
        double sqrtFirstStep = sqrt(firstStep);
        // G_0 : First gaussian vector
        pnl_vect_rng_normal_d(Gi_,path->n,rng);
        // All the LdGi
        LGi_ = pnl_mat_mult_vect(choleskyCorr,Gi_);

        double sigma_d, Sd_t, LdGi, value;
        for (int d = 0; d < path->n; ++d) {
            sigma_d = GET(volatility,d);
            Sd_t = GET(St, d);
            LdGi = GET(LGi_, d);

            value = exp(rateModels[assetList->assets[d]->change]->GetIntegralRate(t, t + firstStep)
                        - (sigma_d * sigma_d / 2) * firstStep + sigma_d * sqrtFirstStep * LdGi
            );

            // Store in path
            MLET(path, indexBeforet, d) = (Sd_t * value);
            // Store in value_tiMinus1
            value_tiMinus1[d] = value;
        }

        // For all other simulation
        double tiMinus1;
        for (int i = 1; i < nbToSimulate; ++i) {
            step = GET(scheduleSimulation,i);
            double sqrtStep = sqrt(step);
            // Same processus that beside but with a different step
            tiMinus1 = (indexBeforet + i - 1) * step;
            pnl_vect_rng_normal_d(Gi_, path->n, rng);
            LGi_ = pnl_mat_mult_vect(choleskyCorr, Gi_);
            // For each asset
            for (int d = 0; d < path->n; ++d) {
                sigma_d = GET(volatility, d);
                Sd_t = GET(St, d);
                LdGi = GET(LGi_, d);

                value = value_tiMinus1[d] * exp(
                        rateModels[assetList->assets[d]->change]->GetIntegralRate(tiMinus1, tiMinus1 + step)
                        - (sigma_d * sigma_d / 2) * step + sigma_d * sqrtStep * LdGi
                );

                // Store in path and value_tiMinus1
                PNL_MSET(path, (indexBeforet + i), d, (Sd_t * value));
                value_tiMinus1[d] = value;
            }
        }
    }
}


void BlackScholesModel::Simulate(double maturity, PnlMat *path, int stepNb) {
    // Step initialisation
    double step = maturity/(double)stepNb;
    double sqrtStep = sqrt(step);
    // Spot initialisation
    for (int d = 0; d < path->n; ++d)
        PNL_MSET(path, 0, d, GET(spot,d));
    double sigma_d, Sd_tiMinus1, LdGi, tiMinus1, Sd_ti;
    // For each time
    for (int i = 1; i < path->m; ++i) {
        tiMinus1 = (i-1) * step;
        pnl_vect_rng_normal_d(Gi_,path->n,rng); // Gi gaussian vector
        LGi_ =  pnl_mat_mult_vect(choleskyCorr,Gi_); // All the LdGi
        // For each asset
        for (int d = 0; d < path->n; ++d) {
            sigma_d = GET(volatility,d);
            Sd_tiMinus1 = PNL_MGET(path, (i-1), d);
            LdGi = GET(LGi_,d);

            Sd_ti = Sd_tiMinus1 * exp(
                    rateModels[assetList->assets[d]->change]->GetIntegralRate(tiMinus1, tiMinus1 + step)
                    - (sigma_d * sigma_d / 2) * step + sigma_d * sqrtStep * LdGi
                    );

            PNL_MSET(path,i,d,Sd_ti);
        }
    }
}

BlackScholesModel::~BlackScholesModel() {
    pnl_vect_free(&Gi_);
    pnl_vect_free(&LGi_);
    pnl_vect_free(&St);
    pnl_mat_free(&choleskyCorr);
    // Base destructor is called here
}

void BlackScholesModel::SimulateMarket(double maturity, PnlMat *path, int stepNb) {
    // Step Initialisation
    double step = maturity/(double)stepNb;
    double sqrtStep = sqrt(step);
    double addTrendForZC, ti_minus1;
    // Spot Initialisation
    for (int d = 0; d < path->n; ++d)
        PNL_MSET(path, 0, d, GET(spot,d));
    double sigma_d, trend_d, Sd_tiMinus1, LdGi, Sd_ti;
    // For each time
    for (int i = 1; i < path->m; ++i) {
        ti_minus1 = (i-1) * step;
        pnl_vect_rng_normal_d(Gi_,path->n,rng); // Gi gaussian vector
        LGi_ =  pnl_mat_mult_vect(choleskyCorr,Gi_); // All the LdGi
        // For each asset
        for (int d = 0; d < path->n; ++d) {
            sigma_d = GET(volatility,d);
            trend_d = GET(trend,d);
            Sd_tiMinus1 = PNL_MGET(path, (i-1), d);
            LdGi = GET(LGi_,d);

            // Case of X.ZC : XR_f
            addTrendForZC = (assetList->assets[d]->isChange.first)
                            ? rateModels[assetList->assets[d]->isChange.second]->GetIntegralRate(ti_minus1,ti_minus1+step)
                            : 0.;

            Sd_ti = Sd_tiMinus1 * exp( (trend_d - sigma_d * sigma_d / 2) * step + addTrendForZC + sigma_d * sqrtStep * LdGi);

            PNL_MSET(path,i,d,Sd_ti);
        }
    }
}


void BlackScholesModel::SetAssets(AssetList *assets) {
    ModelGen::SetAssets(assets);
    pnl_mat_free(&choleskyCorr);
    choleskyCorr = pnl_mat_copy(assets->correlMatrix);
    pnl_mat_chol(choleskyCorr);
}

void BlackScholesModel::GetParametersFromStats(StatsFactory *stats, PnlVect **trend, PnlMat **volMatrix) {
    *volMatrix = pnl_mat_copy(stats->covar_);
    pnl_mat_chol(*volMatrix); // sigma = cholesky(sigma.sigmaT) = cholesky(covar)
    *trend = pnl_vect_copy(stats->mean_); // drift = mean
    PnlVect *vol;
    vol = pnl_vect_create_from_zero((*volMatrix)->n);
    for (int i = 0; i < (*trend)->size; ++i) { // drift = trend - 1/2 sigma_i^2
        pnl_mat_get_col(vol,*volMatrix,i);
        LET(*trend,i) = GET(*trend,i) + 1/2 * pnl_vect_scalar_prod(vol,vol);
    }
    pnl_vect_free(&vol);
}
