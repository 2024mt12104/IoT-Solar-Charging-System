# =============================================================================
# Project Title:
#   IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING
#   SUPERCAPACITOR ENERGY BUFFERING
#
# Author: Ajeesh Kumar R | BITS ID: 2024MT12104
# =============================================================================
#
# setup_environment.ps1
# =====================
# Master setup script - installs all project dependencies.
# Run this ONCE before building any component.
#
# Usage:
#   Open PowerShell as Administrator and run:
#     .\setup_environment.ps1
#
#   Or run individual sections:
#     .\setup_environment.ps1 -Component python
#     .\setup_environment.ps1 -Component esp8266
#     .\setup_environment.ps1 -Component frontend
#     .\setup_environment.ps1 -Component all
#
# =============================================================================

param(
    [ValidateSet("all", "python", "esp8266", "frontend", "stm32")]
    [string]$Component = "all"
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host " IoT Solar Charging System - Environment Setup" -ForegroundColor Cyan
Write-Host " Author: Ajeesh Kumar R (2024MT12104)" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""

# --------------------------------------------------------------------------
# Helper function
# --------------------------------------------------------------------------
function Write-Step($msg) {
    Write-Host "[SETUP] $msg" -ForegroundColor Green
}
function Write-Warn($msg) {
    Write-Host "[WARN]  $msg" -ForegroundColor Yellow
}
function Write-Err($msg) {
    Write-Host "[ERROR] $msg" -ForegroundColor Red
}

# ==========================================================================
# 1. PYTHON DEPENDENCIES (Cloud Backend + ML Model + Simulation)
# ==========================================================================
function Install-PythonDeps {
    Write-Step "Installing Python dependencies..."

    # Check Python is available
    $python = Get-Command python -ErrorAction SilentlyContinue
    if (-not $python) {
        Write-Err "Python not found. Install Python 3.10+ from https://python.org"
        return
    }
    Write-Step "Python found: $($python.Source)"

    # Cloud Backend
    Write-Step "Installing Cloud Backend requirements..."
    pip install -r "$ProjectRoot\cloud\backend\requirements.txt" --user --quiet
    if ($LASTEXITCODE -eq 0) { Write-Step "Cloud Backend dependencies OK" }

    # ML Model
    Write-Step "Installing ML Model requirements..."
    pip install -r "$ProjectRoot\ml_model\requirements.txt" --user --quiet
    if ($LASTEXITCODE -eq 0) { Write-Step "ML Model dependencies OK" }

    # Simulation (uses same packages as ML)
    Write-Step "Python setup complete."
}

# ==========================================================================
# 2. ESP8266 DEPENDENCIES (Arduino CLI + Board + Libraries)
# ==========================================================================
function Install-ESP8266Deps {
    Write-Step "Setting up ESP8266 Arduino environment..."

    # Check if arduino-cli is installed
    $cli = Get-Command arduino-cli -ErrorAction SilentlyContinue
    if (-not $cli) {
        Write-Step "Installing arduino-cli..."

        # Download and install arduino-cli
        $cliUrl = "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip"
        $cliZip = "$env:TEMP\arduino-cli.zip"
        $cliDir = "$env:LOCALAPPDATA\Arduino CLI"

        Invoke-WebRequest -Uri $cliUrl -OutFile $cliZip
        Expand-Archive -Path $cliZip -DestinationPath $cliDir -Force
        Remove-Item $cliZip

        # Add to PATH for this session
        $env:PATH += ";$cliDir"
        Write-Step "arduino-cli installed to: $cliDir"
        Write-Warn "Add '$cliDir' to your system PATH for future sessions."
    } else {
        Write-Step "arduino-cli found: $($cli.Source)"
    }

    # Add ESP8266 board manager URL
    Write-Step "Adding ESP8266 board manager URL..."
    arduino-cli config init --overwrite 2>$null
    arduino-cli config add board_manager.additional_urls `
        "https://arduino.esp8266.com/stable/package_esp8266com_index.json"

    # Update board index
    Write-Step "Updating board index..."
    arduino-cli core update-index

    # Install ESP8266 core
    Write-Step "Installing ESP8266 core (this may take a few minutes)..."
    arduino-cli core install esp8266:esp8266@3.1.2

    if ($LASTEXITCODE -eq 0) {
        Write-Step "ESP8266 core installed successfully."
    } else {
        Write-Err "ESP8266 core installation failed."
    }

    # Verify
    Write-Step "Verifying ESP8266 installation..."
    arduino-cli core list | Select-String "esp8266"
}

# ==========================================================================
# 3. FRONTEND DEPENDENCIES (Node.js + npm packages)
# ==========================================================================
function Install-FrontendDeps {
    Write-Step "Installing Frontend (React) dependencies..."

    # Check Node.js
    $node = Get-Command node -ErrorAction SilentlyContinue
    if (-not $node) {
        Write-Err "Node.js not found. Install from https://nodejs.org (v18+ recommended)"
        return
    }
    Write-Step "Node.js found: $(node --version)"

    # Install npm packages
    Push-Location "$ProjectRoot\cloud\frontend"
    Write-Step "Running npm install..."
    npm install --quiet
    if ($LASTEXITCODE -eq 0) { Write-Step "Frontend dependencies OK" }
    Pop-Location
}

# ==========================================================================
# 4. STM32 TOOLCHAIN CHECK
# ==========================================================================
function Check-STM32Deps {
    Write-Step "Checking STM32 toolchain..."

    $gcc = Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue
    if ($gcc) {
        Write-Step "arm-none-eabi-gcc found: $($gcc.Source)"
        arm-none-eabi-gcc --version | Select-Object -First 1
    } else {
        Write-Warn "arm-none-eabi-gcc not found in PATH."
        Write-Warn "Options:"
        Write-Warn "  1. Install STM32CubeIDE: https://www.st.com/en/development-tools/stm32cubeide.html"
        Write-Warn "  2. Install GNU Arm Toolchain: https://developer.arm.com/downloads/-/gnu-rm"
        Write-Warn "  STM32 firmware can be built from within STM32CubeIDE without command-line tools."
    }
}

# ==========================================================================
# MAIN EXECUTION
# ==========================================================================
Write-Host ""
switch ($Component) {
    "python"   { Install-PythonDeps }
    "esp8266"  { Install-ESP8266Deps }
    "frontend" { Install-FrontendDeps }
    "stm32"    { Check-STM32Deps }
    "all" {
        Install-PythonDeps
        Write-Host ""
        Install-ESP8266Deps
        Write-Host ""
        Install-FrontendDeps
        Write-Host ""
        Check-STM32Deps
    }
}

Write-Host ""
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host " Setup complete. See individual README files for build steps." -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
