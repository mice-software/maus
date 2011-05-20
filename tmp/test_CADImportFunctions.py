from CADImport import CADImport
import os.path

if __name__ == "__main__":
    
        test1 = CADImport("fastradModel.xml", "GDML2G4MICE.xsl", "FieldInfoTest.xml", "OUTPUTFILE.txt")
        test1.XSLTParse()
        OutputFile = test1.Output
        if os.path.getsize(OutputFile)==0: raise StandardError("No output file produced")
        else: print "Output file ok!"

        test1.AppendMerge()
        count1 = 0
        fout = open(test1.MergeOut, 'r')
        for line in fout.readlines():
                if line.find('EDIT')>=0: count1 = count1 + 1
        if count1 != 0: raise StandardError("EDIT was not replaced")
        else: print "EDIT was replaced"

        count2 = 0
        fin = open(test1.MergeIn, 'r')
        for line in fin.readlines():
                if line.find('EDIT')>=0: count2 = count2 + 1
        if count2 != 1: raise StandardError("Wihin Merge.xsl, there is no EDIT or there are too many")
        else: print "EDIT was replaced"