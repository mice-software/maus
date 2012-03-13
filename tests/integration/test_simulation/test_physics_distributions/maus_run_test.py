import subprocess
import os


def main():
    subprocess.Popen(['python', 'PhysicsTest.py',
                      '--in=reference_data.dat',
                      '--out=test_data.dat'])
    os.system('rm MaterialBlock.dat')
    os.system('rm simulation.out')
    os.system('rm cards.py')

if __name__ == "__main__":
    main()


