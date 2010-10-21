import logging
import logging.config
import time

logging.config.fileConfig("logging.conf")

#create logger
logger = logging.getLogger()

test = logging.getLogger("root2.test")

#"application" code
logger.debug("debug message")
logger.info("info message")
logger.warn("warn message")
logger.error("error message")
logger.critical("critical message")
test.critical("howdy")
