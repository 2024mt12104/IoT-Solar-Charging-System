# =============================================================================
# Project Title:
#   IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING
#   SUPERCAPACITOR ENERGY BUFFERING
#
# Author: Ajeesh Kumar R | BITS ID: 2024MT12104
# Programme: M.Tech in Software Systems - Specialization in IoT
# Institution: BITS Pilani - Work Integrated Learning Program (WILP) Division
#
# Academic Purpose Notice:
#   This file is developed solely for academic learning, research, experimentation,
#   and project evaluation purposes under the M.Tech in Software Systems -
#   Specialization in IoT programme at BITS Pilani WILP.
#
# Ownership: All project work is the intellectual work of the above-mentioned
#   author unless otherwise referenced.
# Usage Restriction: Unauthorized copying, redistribution, or commercial usage
#   without prior permission is discouraged.
# =============================================================================
#
# Dockerfile — IoT Solar Charging System Backend
# =============================================================================

FROM python:3.12-slim

LABEL org.opencontainers.image.source="https://github.com/2024mt12104/IoT-Solar-Charging-System"
LABEL org.opencontainers.image.description="Flask REST API for IoT Solar Charging System"
LABEL org.opencontainers.image.licenses="MIT"

WORKDIR /app

# Install dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application
COPY . .

# Expose port
EXPOSE 5000

# Run with gunicorn
CMD ["gunicorn", "--bind", "0.0.0.0:5000", "--workers", "2", "app:app"]
