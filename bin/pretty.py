import json

def main():

  fh = open('simulation.out')
  for line in fh.readlines():
    spill = json.loads(line)
    print json.dumps(spill, indent = 2)

if __name__ == "__main__":
    main()
