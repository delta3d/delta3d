import sys
import glob

FILES = glob.glob(sys.argv[1])
i = 0
while i < len(FILES):
	sys.stdout.write(FILES[i] + " ")
	i += 1


