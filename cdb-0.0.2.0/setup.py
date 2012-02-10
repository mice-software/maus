#!/usr/bin/python
""" setup script """

import cdb
from distutils.core import setup

setup(
    name="cdb",
    version=cdb.__version__,
    description="Configuration Database client",
    author="MICE",
    author_email='mice-cdb@jiscmail.ac.uk',
    packages=['cdb'],
    url="http://cdb.mice.rl.ac.uk",
    requires=["suds"],
    provides=["cdb"],
)

