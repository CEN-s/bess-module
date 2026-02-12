# BESS Simulation Engine

[![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)](https://en.cppreference.com/)
[![Python](https://img.shields.io/badge/python-3.13%2B-blue.svg)](https://www.python.org/)
[![License](https://img.shields.io/badge/license-GPL--3.0-green.svg)](LICENSE)

A high-performance C++ backend for simulating Battery Energy Storage Systems (BESS), featuring seamless Python bindings via Pybind11. This project demonstrates a hybrid architecture where computationally intensive simulation logic is handled by optimized C++ code, while remaining easily accessible for data science workflows in Python.

## 🚀 Key Features

- **Optimized C++ Core**: Fast simulation of energy storage, discharge, and demand curve flattening.
- **Pythonic Interface**: Full Pybind11 integration allowing the BESS simulator to be used as a native Python object.
- **NumPy Compatibility**: Native support for the Python Buffer Protocol, enabling zero-copy data sharing with NumPy arrays.
- **Advanced Simulation Logic**:
  - Daily and monthly energy storage projections.
  - Configurable discharge intervals with validation.
  - Demand curve transformation (flattening logic).
- **Modern Tooling**: Built with CMake for C++ and `uv` for Python package management.

## 🏗️ Architecture

The project follows a modular structure:

- **`include/` & `src/`**: Core C++ implementation.
- **`src/binding.cpp`**: Pybind11 glue code that exposes the C++ classes to Python.
- **`pyproject.toml`**: Modern Python packaging configuration using the `uv` build backend.

## 🛠️ Installation

### Prerequisites
- CMake 3.15+
- C++17 compatible compiler (GCC/Clang/MSVC)
- Python 3.14+
- [uv](https://github.com/astral-sh/uv) (recommended) or `pip`

### Using `uv` (Recommended)
```bash
# Clone the repository
git clone https://github.com/CEN-s/bess-daimon.git
cd bess-daimon

# Install dependencies and the project in editable mode
uv sync
```

### Manual Build (CMake)
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## 💻 Usage Example

```python
import numpy as np
import bess_module

# 1. Define a 24-hour consumer curve (positive=consumption, negative=generation)
# Example: High solar generation during the day
curve = np.array([
    10.0, 10.0, 10.0, 10.0, 10.0, 10.0,  # Night
    -5.0, -15.0, -20.0, -20.0, -15.0, -5.0, # Day (Solar generation)
    10.0, 15.0, 20.0, 25.0, 30.0, 25.0,  # Evening peak
    20.0, 15.0, 10.0, 10.0, 10.0, 10.0
], dtype=np.float64)

# 2. Initialize the BESS simulator
simulator = bess_module.BESS(curve)

# 3. Configure discharge interval (e.g., peak hours 18:00 to 22:00)
simulator.discharge_interval = (18, 22)

# 4. Run the discharge simulation
simulator.generate_resulting_curve()

# 5. Get results
resulting_curve = np.array(simulator.resulting_curve)
monthly_savings = simulator.monthly_stored_energy

print(f"Monthly Energy Stored: {monthly_savings:.2f} kWh")
print(f"Resulting Curve at 19:00: {resulting_curve[18]:.2f} kW")
```

## 📄 License
This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
