#!/usr/bin/env bash
# =============================================================================
# Project Title:
#   IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING
#   SUPERCAPACITOR ENERGY BUFFERING
#
# Author: Ajeesh Kumar R | BITS ID: 2024MT12104
# =============================================================================
#
# setup_environment.sh
# ====================
# Master setup script for macOS/Linux - installs all project dependencies.
# Run this ONCE before building any component.
#
# Usage:
#   chmod +x setup_environment.sh
#   ./setup_environment.sh all
#
#   Or install individual components:
#     ./setup_environment.sh python
#     ./setup_environment.sh esp8266
#     ./setup_environment.sh frontend
#     ./setup_environment.sh stm32
#
# =============================================================================

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
COMPONENT="${1:-all}"

echo "============================================================"
echo " IoT Solar Charging System - Environment Setup"
echo " Author: Ajeesh Kumar R (2024MT12104)"
echo "============================================================"
echo ""

# --------------------------------------------------------------------------
# Helper functions
# --------------------------------------------------------------------------
write_step() { echo -e "\033[0;32m[SETUP] $1\033[0m"; }
write_warn() { echo -e "\033[0;33m[WARN]  $1\033[0m"; }
write_err()  { echo -e "\033[0;31m[ERROR] $1\033[0m"; }

# --------------------------------------------------------------------------
# Detect OS
# --------------------------------------------------------------------------
OS="$(uname -s)"
case "$OS" in
    Linux*)  PLATFORM="linux";;
    Darwin*) PLATFORM="mac";;
    *)       write_err "Unsupported OS: $OS"; exit 1;;
esac
write_step "Detected platform: $PLATFORM"

# ==========================================================================
# 1. PYTHON DEPENDENCIES (Cloud Backend + ML Model + Simulation)
# ==========================================================================
install_python_deps() {
    write_step "Installing Python dependencies..."

    if ! command -v python3 &> /dev/null; then
        write_err "Python 3 not found. Install Python 3.10+ from https://python.org"
        if [ "$PLATFORM" = "mac" ]; then
            write_warn "  brew install python3"
        else
            write_warn "  sudo apt install python3 python3-pip  (Debian/Ubuntu)"
            write_warn "  sudo dnf install python3 python3-pip  (Fedora)"
        fi
        return 1
    fi
    write_step "Python found: $(python3 --version)"

    # Cloud Backend
    write_step "Installing Cloud Backend requirements..."
    pip3 install -r "$PROJECT_ROOT/cloud/backend/requirements.txt" --user --quiet

    # ML Model
    write_step "Installing ML Model requirements..."
    pip3 install -r "$PROJECT_ROOT/ml_model/requirements.txt" --user --quiet

    write_step "Python setup complete."
}

# ==========================================================================
# 2. ESP8266 DEPENDENCIES (Arduino CLI + Board + Libraries)
# ==========================================================================
install_esp8266_deps() {
    write_step "Setting up ESP8266 Arduino environment..."

    if ! command -v arduino-cli &> /dev/null; then
        write_step "Installing arduino-cli..."

        if [ "$PLATFORM" = "mac" ]; then
            if command -v brew &> /dev/null; then
                brew install arduino-cli
            else
                curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
                sudo mv bin/arduino-cli /usr/local/bin/
                rm -rf bin
            fi
        else
            curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
            sudo mv bin/arduino-cli /usr/local/bin/
            rm -rf bin
        fi
        write_step "arduino-cli installed."
    else
        write_step "arduino-cli found: $(which arduino-cli)"
    fi

    # Add ESP8266 board manager URL
    write_step "Adding ESP8266 board manager URL..."
    arduino-cli config init --overwrite 2>/dev/null || true
    arduino-cli config add board_manager.additional_urls \
        "https://arduino.esp8266.com/stable/package_esp8266com_index.json"

    # Update board index
    write_step "Updating board index..."
    arduino-cli core update-index

    # Install ESP8266 core
    write_step "Installing ESP8266 core (this may take a few minutes)..."
    arduino-cli core install esp8266:esp8266@3.1.2

    # Verify
    write_step "Verifying ESP8266 installation..."
    arduino-cli core list | grep esp8266
}

# ==========================================================================
# 3. FRONTEND DEPENDENCIES (Node.js + npm packages)
# ==========================================================================
install_frontend_deps() {
    write_step "Installing Frontend (React) dependencies..."

    if ! command -v node &> /dev/null; then
        write_err "Node.js not found. Install from https://nodejs.org (v18+ recommended)"
        if [ "$PLATFORM" = "mac" ]; then
            write_warn "  brew install node"
        else
            write_warn "  curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -"
            write_warn "  sudo apt install nodejs"
        fi
        return 1
    fi
    write_step "Node.js found: $(node --version)"

    # Install npm packages
    pushd "$PROJECT_ROOT/cloud/frontend" > /dev/null
    write_step "Running npm install..."
    npm install --quiet
    popd > /dev/null

    write_step "Frontend dependencies OK."
}

# ==========================================================================
# 4. STM32 TOOLCHAIN CHECK
# ==========================================================================
check_stm32_deps() {
    write_step "Checking STM32 toolchain..."

    if command -v arm-none-eabi-gcc &> /dev/null; then
        write_step "arm-none-eabi-gcc found: $(which arm-none-eabi-gcc)"
        arm-none-eabi-gcc --version | head -1
    else
        write_warn "arm-none-eabi-gcc not found in PATH."
        write_warn "Options:"
        if [ "$PLATFORM" = "mac" ]; then
            write_warn "  brew install --cask gcc-arm-embedded"
        else
            write_warn "  sudo apt install gcc-arm-none-eabi  (Debian/Ubuntu)"
        fi
        write_warn "  Or install STM32CubeIDE: https://www.st.com/en/development-tools/stm32cubeide.html"
        write_warn "  STM32 firmware can be built from within STM32CubeIDE without command-line tools."
    fi
}

# ==========================================================================
# MAIN EXECUTION
# ==========================================================================
echo ""
case "$COMPONENT" in
    python)   install_python_deps ;;
    esp8266)  install_esp8266_deps ;;
    frontend) install_frontend_deps ;;
    stm32)    check_stm32_deps ;;
    all)
        install_python_deps
        echo ""
        install_esp8266_deps
        echo ""
        install_frontend_deps
        echo ""
        check_stm32_deps
        ;;
    *)
        write_err "Unknown component: $COMPONENT"
        echo "Usage: $0 {all|python|esp8266|frontend|stm32}"
        exit 1
        ;;
esac

echo ""
echo "============================================================"
echo " Setup complete. See individual README files for build steps."
echo "============================================================"
