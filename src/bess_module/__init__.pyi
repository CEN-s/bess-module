"""

_bess
=====

Backend C++ de alta performance para simulação de sistemas BESS
(Battery Energy Storage System).

O módulo expõe a classe :class:`BESS`, que simula armazenamento e
despacho de energia ao longo de uma curva de 24 horas.

Características:
- Compatível com NumPy (buffer protocol)
- Iterável como sequência Python
- Suporte a pickle
- Execução thread-safe
"""

from __future__ import annotations

from bess_module._bess import BESS

__all__: list = ["BESS", "HOURS"]
HOURS: int = 24
__version__: str = "0.0.1"
