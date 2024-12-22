from setuptools import setup, find_packages
from setuptools.command.build_py import build_py
import subprocess
import os

class CustomBuildPy(build_py):
    def run(self):
        # This happens before the normal 'build_py' steps
        # so that the created files will be picked up by package_data
        script_path = os.path.join(
            os.path.dirname(os.path.abspath(__file__)),
            'muster', 'compile.sh'
        )
        subprocess.check_call(['sh', script_path])

        # Now run the normal build_py to compile .py files and collect data
        super().run()

setup(
    name='muster',
    version='0.0.1',
    description='A simple wrapper for MUSTER',
    packages=find_packages(),
    package_data={
        'muster': [
            'evaluate_XML_voicePlus.sh',
            'Programs/*'
        ],
    },
    include_package_data=True,
    cmdclass={
        'build_py': CustomBuildPy,
    },
)
