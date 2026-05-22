# AI/ML-Based Charging Time Prediction Module

## Overview
This module implements Machine Learning models to predict battery charging duration based on all monitored IoT parameters from the Solar-Assisted Charging System with Supercapacitor Energy Buffering.

## Parameters Used (14 Features)
| # | Parameter | Unit | Description |
|---|-----------|------|-------------|
| 1 | Voltage | V | Battery voltage during charging |
| 2 | Current | A | Charging current |
| 3 | Temperature | °C | Ambient/battery temperature |
| 4 | PWM Duty Cycle | % | Charge control signal |
| 5 | State of Charge (SOC) | % | Current battery level |
| 6 | Supercapacitor Voltage | V | Energy buffer voltage |
| 7 | Solar Irradiance | W/m² | Solar input power |
| 8 | Battery Capacity | mAh | Battery size |
| 9 | Internal Resistance | Ω | Battery resistance |
| 10 | Charging Efficiency | % | Overall efficiency |
| 11 | Time of Day | Hour | Solar availability factor |
| 12 | Cloud Cover | % | Weather impact |
| 13 | Battery Age | Cycles | Health indicator |
| 14 | Load Connected | W | Parasitic load |

**Target:** Actual Charging Time (minutes)

## Models Implemented
1. **Linear Regression** - Baseline model
2. **Random Forest Regressor** - Ensemble method
3. **Gradient Boosting Regressor** - Sequential ensemble
4. **XGBoost** - Optimized gradient boosting

## How to Run

```bash
# Install dependencies
pip install -r requirements.txt

# Generate dataset
python dataset_generation.py

# Run Jupyter Notebook
jupyter notebook charging_time_prediction.ipynb
```

## Output Visualizations
- Correlation heatmap of all features
- Charging time distribution
- Actual vs Predicted scatter plots (all models)
- Residual analysis plots
- Feature importance charts
- Model comparison (R², RMSE)

## Results
The notebook compares Actual Charging Time vs Predicted Charging Time across all models with metrics: MAE, MSE, RMSE, R², and 5-fold Cross-Validation scores.

## Future Scope
- Real-time IoT data integration (ESP32/MQTT)
- LSTM for time-series prediction
- Anomaly detection for fault identification
- Battery health estimation
- Predictive maintenance
- Smart energy optimization