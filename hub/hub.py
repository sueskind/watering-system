import datetime as dt
import json
import os
import pickle
import random
import time
from collections import defaultdict

import numpy as np
import pandas as pd
import requests as req

from constants import SensorModes, ThresholdModes, PumpModes
from settings import CONFIG_PATH, REFRESH_INTERVAL, SAVE_INTERVAL, SESSIONS_DIR


def save_measurements(session_dir, all_measurements):
    with open(os.path.join(session_dir, f"measurements.pkl"), "wb") as f:
        pickle.dump(all_measurements, f)


def measure(hostname, sensor_numbers):
    values = req.get(f"http://{hostname}/measure").json()
    return [time.time()] + list(np.array(values)[sensor_numbers])


def pump(hostname, pumps, mode):
    if mode == PumpModes.RANDOM:
        pumps = random.choice(pumps)

    for p in pumps:
        params = {
            "pumpNumber": p["number"],
            "duration": int(p["duration"] * 1000)
        }
        req.get(f"http://{hostname}/water", params=params)


def should_pump(last_pump_time, plant, measurements):
    # don't pump if waitMin is not reached
    if time.time() - last_pump_time < plant["pumpWaitMin"]:
        return False

    # pump if waitMax is reached
    if time.time() - last_pump_time > plant["pumpWaitMax"]:
        return True

    sensors_would_pump = []
    for sensor in plant["sensors"]:
        median_window = sensor["medianWindow"]
        mean_window = sensor["meanWindow"]
        sensor_measurements = measurements[sensor["number"]][-max(median_window, mean_window):]
        sensor_measurements = sensor_measurements.rolling(median_window, min_periods=1).median()
        sensor_measurements = sensor_measurements.rolling(mean_window, min_periods=1).mean()

        measurement = sensor_measurements.iloc[-1]
        sensors_would_pump.append(
            (sensor["thresholdMode"] == ThresholdModes.FLOOR and measurement < sensor["threshold"]) or
            (sensor["thresholdMode"] == ThresholdModes.CEILING and measurement > sensor["threshold"])
        )

    if plant["sensorMode"] == SensorModes.ALL:
        return all(sensors_would_pump)
    elif plant["sensorMode"] == SensorModes.ANY:
        return any(sensors_would_pump)


def main():
    with open(CONFIG_PATH, "r") as f:
        config = json.load(f)

    session_dir = os.path.join(SESSIONS_DIR, f"session_{dt.datetime.now():%Y-%m-%d_%H-%M-%S}")
    os.makedirs(session_dir)
    last_save_time = 0

    last_measurement_times = defaultdict(float)
    last_pump_times = defaultdict(lambda: defaultdict(float))

    sensor_numbers = {controller["hostname"]: list({sensor["number"]
                                                    for plant in controller["plants"]
                                                    for sensor in plant["sensors"]})
                      for controller in config["controllers"]}
    all_measurements = {controller["hostname"]: pd.DataFrame(columns=["time"] + sensor_numbers[controller["hostname"]])
                        for controller in config["controllers"]}

    while True:

        for controller in config["controllers"]:

            hostname = controller["hostname"]
            measurements = all_measurements[hostname]

            if time.time() - last_measurement_times[hostname] > controller["pollingInterval"]:
                measurements.loc[len(measurements)] = measure(hostname, sensor_numbers[hostname])

                for plant_number, plant in enumerate(controller["plants"]):

                    if should_pump(last_pump_times[hostname][plant_number], plant, measurements):
                        pump(hostname, plant["pumps"], plant["pumpMode"])
                        last_pump_times[hostname][plant_number] = time.time()

                last_measurement_times[hostname] = time.time()

        if time.time() - last_save_time > SAVE_INTERVAL:
            save_measurements(session_dir, all_measurements)
            last_save_time = time.time()

        time.sleep(REFRESH_INTERVAL)


if __name__ == '__main__':
    main()
