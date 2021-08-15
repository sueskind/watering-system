from enum import Enum


class ThresholdModes(Enum):
    FLOOR = "floor"
    CEILING = "ceiling"


class SensorModes(Enum):
    MEAN = "mean"
    MEDIAN = "median"
    AND = "and"
    OR = "or"


class PumpModes(Enum):
    ALL = "all"
    RANDOM = "random"
