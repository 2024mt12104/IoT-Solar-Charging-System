"""
Synthetic Dataset Generation for IoT Solar Charging System
Generates realistic charging data with all relevant parameters for ML prediction.
"""

import numpy as np
import pandas as pd

np.random.seed(42)

N_SAMPLES = 1000

# --- Feature Generation with realistic ranges and correlations ---

# Time of day (0-23 hours)
time_of_day = np.random.uniform(5, 19, N_SAMPLES)  # Charging mostly during daylight

# Cloud cover (0-100%)
cloud_cover = np.random.beta(2, 5, N_SAMPLES) * 100  # Skewed towards clearer skies

# Solar Irradiance (W/m²) - correlated with time of day and cloud cover
solar_peak = 1000 * np.sin(np.pi * (time_of_day - 5) / 14)  # Peak at noon
solar_irradiance = np.clip(solar_peak * (1 - cloud_cover / 120) + np.random.normal(0, 50, N_SAMPLES), 50, 1100)

# Battery Capacity (mAh) - fixed options
battery_capacity = np.random.choice([2000, 3000, 4000, 5000, 6000], N_SAMPLES)

# Battery Age (cycles)
battery_age = np.random.exponential(200, N_SAMPLES).astype(int)
battery_age = np.clip(battery_age, 0, 1500)

# Battery Internal Resistance (Ohms) - increases with age
battery_internal_resistance = 0.05 + (battery_age / 1500) * 0.15 + np.random.normal(0, 0.01, N_SAMPLES)
battery_internal_resistance = np.clip(battery_internal_resistance, 0.03, 0.25)

# State of Charge at start (SOC %) - lower SOC means longer charge time
soc = np.random.uniform(5, 85, N_SAMPLES)

# Temperature (°C) - affects efficiency
temperature = 20 + 10 * np.sin(np.pi * (time_of_day - 6) / 12) + np.random.normal(0, 5, N_SAMPLES)
temperature = np.clip(temperature, -5, 55)

# Charging Efficiency (%) - affected by temperature and battery age
temp_factor = 1 - 0.005 * np.abs(temperature - 25)
age_factor = 1 - (battery_age / 1500) * 0.15
charging_efficiency = np.clip(85 * temp_factor * age_factor + np.random.normal(0, 3, N_SAMPLES), 50, 98)

# Supercapacitor Voltage (V) - energy buffer (2.5V - 5.5V typical)
supercap_voltage = 2.5 + (solar_irradiance / 1100) * 3.0 + np.random.normal(0, 0.2, N_SAMPLES)
supercap_voltage = np.clip(supercap_voltage, 2.0, 5.5)

# Voltage (V) - battery voltage during charging (correlates with SOC)
voltage = 3.0 + (soc / 100) * 1.2 + np.random.normal(0, 0.05, N_SAMPLES)
voltage = np.clip(voltage, 2.8, 4.25)

# Current (A) - charging current (affected by solar irradiance and PWM)
base_current = (solar_irradiance / 1100) * 2.5
current = np.clip(base_current + np.random.normal(0, 0.1, N_SAMPLES), 0.1, 3.0)

# PWM Duty Cycle (%) - controls charging rate
pwm_duty_cycle = np.clip((current / 3.0) * 100 + np.random.normal(0, 5, N_SAMPLES), 10, 100)

# Load Connected (W) - any parasitic load during charging
load_connected = np.random.choice([0, 0, 0, 0.5, 1.0, 1.5, 2.0, 3.0], N_SAMPLES)

# --- Target: Actual Charging Time (minutes) ---
# Physics-based formula with noise
energy_needed = battery_capacity * (100 - soc) / 100 * voltage  # mWh approximate
effective_power = (current * voltage * charging_efficiency / 100) - load_connected  # W
effective_power = np.clip(effective_power, 0.1, None)  # Avoid division by zero

charging_time = (energy_needed / effective_power) * 60 / 1000  # Convert to minutes
# Add effects of temperature, resistance, supercap buffering
charging_time *= (1 + battery_internal_resistance * 2)
charging_time *= (1 - (supercap_voltage - 2.0) / 20)  # Supercap helps reduce time slightly
charging_time += np.random.normal(0, 5, N_SAMPLES)  # Noise
charging_time = np.clip(charging_time, 5, 600)  # 5 min to 10 hours max

# --- Create DataFrame ---
df = pd.DataFrame({
    'Voltage_V': np.round(voltage, 3),
    'Current_A': np.round(current, 3),
    'Temperature_C': np.round(temperature, 2),
    'PWM_Duty_Cycle_Percent': np.round(pwm_duty_cycle, 2),
    'SOC_Percent': np.round(soc, 2),
    'Supercapacitor_Voltage_V': np.round(supercap_voltage, 3),
    'Solar_Irradiance_Wm2': np.round(solar_irradiance, 2),
    'Battery_Capacity_mAh': battery_capacity,
    'Battery_Internal_Resistance_Ohm': np.round(battery_internal_resistance, 4),
    'Charging_Efficiency_Percent': np.round(charging_efficiency, 2),
    'Time_of_Day_Hour': np.round(time_of_day, 2),
    'Cloud_Cover_Percent': np.round(cloud_cover, 2),
    'Battery_Age_Cycles': battery_age,
    'Load_Connected_W': load_connected,
    'Actual_Charging_Time_Min': np.round(charging_time, 2)
})

# Save dataset
df.to_csv('ml_model/charging_dataset.csv', index=False)
print(f"Dataset generated: {len(df)} samples, {len(df.columns)} features")
print(f"\nDataset Statistics:\n{df.describe()}")