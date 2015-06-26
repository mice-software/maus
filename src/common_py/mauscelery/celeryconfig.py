"""
Celery configuration file.
"""
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

# General Celery configuration
#
# See http://ask.github.com/celery/configuration.html
#

# Configure broker communications.
BROKER_HOST = "localhost"
BROKER_PORT = 5672
BROKER_USER = "maus"
BROKER_PASSWORD = "suam"
BROKER_VHOST = "maushost"

# Define back-end for task meta-data and return values.
# AMQP backend is non-persistent by default.
# Can only fetch the result of a task once as it is sent as a message.
CELERY_RESULT_BACKEND = "amqp"
# Results expire in 30 seconds.
CELERY_TASK_RESULT_EXPIRES = 60
#CELERY_AMQP_TASK_RESULT_EXPIRES = 30 deprecated

# List modules each worker should import.
CELERY_IMPORTS = ("mauscelery.state", "mauscelery.tasks", \
  "mauscelery.mausprocess", "mauscelery.mausworker",)

# Disable log colors.
CELERYD_LOG_COLOR = 0
