from abc import ABCMeta, abstractmethod
from typing import Any

from base.grid import Grid


class Exporter(metaclass=ABCMeta):

    @abstractmethod
    def render(self, grid: Grid, ** kwargs: Any) -> None:
        """Base """

    @property
    def name(self) -> str:
        return self.__class__.__name__
