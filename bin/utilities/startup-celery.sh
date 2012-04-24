#/bin/sh

# Ensure MAUS environment is available.
if [ -z "$MAUS_ROOT_DIR" ]; then
     echo "Setting MAUS_ROOT_DIR..."
     source `pwd`/env.sh
fi
echo "MAUS_ROOT_DIR: $MAUS_ROOT_DIR"

# Invoke Celery worker startup command.
#
# -c N      : number of sub-processes to spawn. If omitted
#             then N is set to number of processors on host.
# -l LEVEL  : logging level. One of DEBUG, INFO, WARNING, 
#             ERROR, CRITICAL, FATAL.
# --purge   : purge old messages from RabbitMQ task queue.
# --pidfile : file into which the worker's main process ID is
#             written.
# For full information run "celeryd --help" or see

# Create a temporary file for the process ID.
pidfile=`mktemp -p $MAUS_ROOT_DIR celery-pidXXXX`
echo "Celery worker main process ID will be written to $pidfile"

# Start up the Celery worker.
celeryd -c 8 -l INFO --purge --pidfile=$pidfile
