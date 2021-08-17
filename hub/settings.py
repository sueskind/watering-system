from os.path import abspath, dirname, join

PROJECT_ROOT = dirname(dirname(abspath(__file__)))
SESSIONS_DIR = join(PROJECT_ROOT, "sessions")

CONFIG_PATH = join(PROJECT_ROOT, "config.json")

REFRESH_INTERVAL = 0.05  # seconds
SAVE_INTERVAL = 60  # seconds
