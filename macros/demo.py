execfile('core/load.py', globals()) # don't remove this!

# load config file
config = None # config file

data = [0, 1, 2] #FetchConfiguration(run = 200)


processors = [Print()]                # Print what's there

# Run the script...
Go(data, processors)


