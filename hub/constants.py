from enum import Enum


class ThresholdModes(Enum):
    FLOOR = "floor"
    CEILING = "ceiling"


class SensorModes(Enum):
    ALL = "all"
    ANY = "any"


class PumpModes(Enum):
    ALL = "all"
    RANDOM = "random"
