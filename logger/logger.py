import time

import requests as req

SENSORS_CONTROLLERS = {"192.168.178.15": [0, 1]}  # example
INTERVAL = 1


def main():
    with open("log.csv", "w") as f:
        heading = "time"
        for controller, sensors in SENSORS_CONTROLLERS.items():
            for sensor in sensors:
                heading += f",{controller}/{sensor}"
        f.write(heading + "\n")

        while True:

            try:
                line = str(int(time.time()))
                for controller, sensors in SENSORS_CONTROLLERS.items():
                    res = req.get(f"http://{controller}/measure").json()
                    for sensor in sensors:
                        line += f",{res[sensor]}"

                f.write(line + "\n")

                print(line)

            except Exception as e:
                print(e)

            time.sleep(INTERVAL)


if __name__ == '__main__':
    main()
