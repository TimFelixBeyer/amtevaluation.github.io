import os
import subprocess

from setuptools import setup, find_packages
from setuptools.command.install import install


class CustomInstallCommand(install):
    def run(self):
        # Compile C++ code
        setup_dir = os.path.dirname(os.path.abspath(__file__))
        script_path = os.path.join(setup_dir, 'muster/compile.sh')
        subprocess.check_call(['sh', script_path])
        # Run the standard install process
        install.run(self)


setup(
    name='muster',
    version='0.0.1',
    description='A simple wrapper for MUSTER',
    packages=find_packages(),
    package_data={
        'muster': ['evaluate_XML_voicePlus.sh', 'Programs/*']
    },
    include_package_data=True,
    cmdclass={'install': CustomInstallCommand},
)