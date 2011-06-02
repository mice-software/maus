import fnmatch
import os


def search_replace(pattern):
    matches = []
    for root, dirnames, filenames in os.walk('.'):
        for filename in fnmatch.filter(filenames, pattern):
            matches.append(os.path.join(root, filename))

    for filename in matches:
        my_file = open(filename, 'r')
        my_lines = my_file.readlines()
        my_file.close()

        my_file = open(filename, 'w')

        for line in my_lines:
            done_already = False

            if '#include' in line:
                if len(line.split('"')) == 3:
                    old_location = line.split('"')[1]
                    old_filename = old_location.split("/")[-1]

                    for root, dirnames, filenames in os.walk('.'):
                        for filename in fnmatch.filter(filenames, old_filename):
                            new_filename= os.path.join(root, filename)[2:]
                            print new_filename
                            if not done_already:
                                my_file.write(line.replace(old_location,new_filename))
                                done_already = True

                    
            if not done_already:
                my_file.write(line)

        my_file.close()
        


if __name__ == "__main__":
    search_replace("*.cc")
    search_replace("*.hh")
