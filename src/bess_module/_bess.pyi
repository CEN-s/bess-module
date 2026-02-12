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

import collections.abc
import typing
`

class BESS:
    """

    BESS(consumer_curve)

    Simulador de sistema de armazenamento em bateria (BESS).

    A classe recebe uma curva de 24 horas contendo consumo e/ou geração
    e simula o armazenamento de excedente e posterior descarga em um
    intervalo configurável.

    Parâmetros
    ----------
    consumer_curve : Sequence[float]
        Curva horária com 24 valores.

        Convenção:
        - valores positivos → consumo
        - valores negativos → geração (ex: solar)

    Notas
    -----
    - O objeto é iterável e se comporta como uma sequência de 24 valores
      representando a curva resultante após a simulação.
    - Compatível com `numpy.array(bess)`.
    """
    def __buffer__(self, flags):
        """
        Return a buffer object that exposes the underlying memory of the object.
        """
    def __getitem__(self, arg0: typing.SupportsInt) -> float:
        """
        Acesso indexado à curva resultante.

        Suporta índices negativos.
        """
    def __getstate__(self) -> tuple: ...
    def __init__(
        self,
        consumer_curve: typing.Annotated[
            collections.abc.Sequence[typing.SupportsFloat], "FixedSize(24)"
        ],
    ) -> None:
        """
        Inicializa o sistema BESS com uma curva horária de 24 pontos.

        Parâmetros
        ----------
        consumer_curve : array-like de 24 floats
            Curva de consumo/geração em kW.

        Lança
        -----
        ValueError
            Se a curva não tiver exatamente 24 valores.
        """
    def __iter__(self) -> collections.abc.Iterator[float]:
        """
        Itera sobre a curva resultante.
        """
    def __len__(self) -> int:
        """
        Retorna 24.
        """
    def __release_buffer__(self, buffer):
        """
        Release the buffer object that exposes the underlying memory of the object.
        """
    def __setstate__(self, arg0: tuple) -> None: ...
    def generate_resulting_curve(self) -> None:
        """
        Executa a simulação do BESS.

        Este método:
        - armazena excedentes
        - aplica descarga no intervalo definido
        - gera a curva resultante

        Thread-safe e libera o GIL durante a execução.
        """
    def get_power_at_hour(self, hour: typing.SupportsInt) -> float:
        """
        Retorna a potência na hora especificada.

        Parâmetros
        ----------
        hour : int
            Hora no intervalo 1-24.

        Retorna
        -------
        float
            Potência na hora especificada após simulação.
        """
    @property
    def consumer_curve(self) -> typing.Annotated[list[float], "FixedSize(24)"]:
        """
        Curva original de consumo/geração.

        Tipo
        ----
        list[float] (tamanho 24)

        Observações
        -----------
        - Valores negativos representam geração.
        - Alterar a curva invalida a simulação atual.
        """
    @consumer_curve.setter
    def consumer_curve(
        self,
        arg1: typing.Annotated[
            collections.abc.Sequence[typing.SupportsFloat], "FixedSize(24)"
        ],
    ) -> None: ...
    @property
    def daily_stored_energy(self) -> float:
        """
        Energia total armazenada no dia.

        Retorna
        -------
        float
            Energia em kWh armazenada ao longo do dia.
        """
    @property
    def discharge_interval(self) -> tuple[int, int]:
        """
        Intervalo de descarga da bateria.

        Formato
        -------
        tuple[int, int]  (start, end)

        Escala
        ------
        1 a 24 (inclusive)

        Exemplo
        -------
        >>> bess.discharge_interval = (18, 22)

        Notas
        -----
        - O intervalo é inclusivo.
        - Não pode conter horas com geração.
        - A validação ocorre no backend C++.
        """
    @discharge_interval.setter
    def discharge_interval(
        self, arg1: tuple[typing.SupportsInt, typing.SupportsInt]
    ) -> None: ...
    @property
    def monthly_stored_energy(self) -> float:
        """
        Projeção mensal de energia armazenada.

        Retorna
        -------
        float
            Energia mensal estimada em kWh.
        """
    @property
    def resulting_curve(self) -> typing.Annotated[list[float], "FixedSize(24)"]:
        """
        Curva resultante após a simulação do BESS.

        Retorna
        -------
        list[float]
            Curva de 24 valores representando a carga final após
            armazenamento e descarga.
        """

HOURS: int = 24
