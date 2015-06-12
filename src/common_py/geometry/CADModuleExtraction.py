"""
R. Bayes

A python class intended to interpret a set of gdml files in terms of a set of 
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

import os
import libxml2
import shutil

class CADModuleExtraction:
    """
    @Class CADModuleExtraction, GDML preparation class.

    This class is intended to extract modules from the source GDML
    files. The first step is to fill materials into the files
    generated from the CAD models. The new files are then separated
    either by physical volume (for the MiceModule extraction) or by
    ranges in z (for generation of Object Modules for systematic
    manipulations). The positions of the magnetic fields are updated
    in the Maus_Information file based on the positions defined in the
    input GDML files.

    """
    #pylint: disable = R0912, R0913, R0914, R0915, C0103
    def __init__(self, gdmlin1, mausInfo, downloadDir, newfile):
        #pylint: disable = R0912, R0913, C0103
        """
        @Method Class constructor

        This method initializes the class. Only two arguements are required.

        @param gdmlin1    The top level GDML file containing the file
                          references to be treated in the module extraction
        @param mausInfo   Information required for the generation of the
                          ParentGeometryFile Generation
        """
        self.gdmllist = []
        self.physlist = []
        if type(gdmlin1) != str:
            raise TypeError(gdmlin1 + " must be a string(file name/path)")
        if gdmlin1[-4:] != 'gdml':
            raise IOError(gdmlin1 + " must be a gdml file")
        else:
            self.gdmlin1 = gdmlin1
            print gdmlin1
            self.gdmllist.append(gdmlin1)
            gdmlbase = libxml2.parseFile(self.gdmlin1)
            vol = next(x for x in gdmlbase.xpathEval("gdml/structure/volume")\
                       if x.prop("name").find("Structure") >= 0)
            physvol = vol.xpathEval("physvol")
            for entry in physvol:
                # print entry.xpathEval("file")[0].prop("name")
                if float(entry.xpathEval("position")[0].prop("z")) == 0.0:
                    self.gdmllist.append( \
                        entry.xpathEval("file")[0].prop("name"))
                else:
                    self.physlist.append( \
                        entry.xpathEval("file")[0].prop("name"))

        if type(mausInfo) != str:
            raise TypeError(mausInfo + " must be a string(file name/path)")
        if mausInfo[-4:] != 'gdml' and mausInfo[-3:] != 'xml':
            raise IOError(mausInfo + " must be a gdml or xml file")
        else:
            self.mausInfo = mausInfo


        if type(downloadDir) != str:
            raise TypeError(downloadDir + " must be a string(path)")
        if not os.path.exists(downloadDir):
            raise IOError(downloadDir + " does not exist.")

        source = gdmlin1.split("/")
        sourceDir = '/'
        self.sourceDir   = sourceDir.join(source[:-1])
        self.downloadDir = downloadDir
        self.outname     = newfile

    def EditMaterials(self):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        """
        @Method EditMaterials

        Update the materials referenced in the GDML files to match the
        GDML file name.
        """
        edlist = []
        for filename in self.gdmllist:
            print filename
            # find the material name in the filename
            material = ''
            matname  = ''
            if filename.find('Fe') >= 0 or filename.find('Iron') >= 0:
                material = 'IRON'
                matname = 'Fe'
            elif filename.find('Cu') >= 0 or filename.find('COPPER') >= 0:
                material = 'COPPER'
                matname  = 'Cu'
                self.add_fields_to_mice_info(os.path.join(self.sourceDir, \
                                                          filename))
            elif filename.find('Pb') >= 0 or filename.find('LEAD') >= 0:
                material = 'LEAD'
                matname  = 'Pb'
            elif filename.find('PureAl_') >= 0:
                material = 'ALUMINUM'
                matname  = 'Al'
            elif filename.find('AlAlloy6061_') >= 0:
                material = 'ALUMINUM'
                matname  = 'AlAlloy6061'
            elif filename.find('al_alloy6061_') >= 0:
                material = 'ALUMINUM'
                matname  = 'al_alloy6061'
            elif filename.find('AlAlloy5083_') >= 0:
                material = 'ALUMINUM'
                matname  = 'AlAlloy5083'
            elif filename.find('al_alloy5083_') >= 0:
                material = 'ALUMINUM'
                matname  = 'al_alloy5083'
            elif filename.find('al_alloy2017_') >= 0:
                material = 'ALUMINUM'
                matname  = 'al_alloy2017'
            elif filename.find('AlAlloy6082_') >= 0:
                material = 'ALUMINUM'
                matname  = 'AlAlloy6082'
            elif filename.find('AlAlloy5251_') >= 0:
                material = 'ALUMINUM'
                matname  = 'AlAlloy5251'
            elif filename.find('StainSt304_') >= 0:
                material = 'STEEL'
                matname  = 'StSt304'
            elif filename.find('StainSt316_') >= 0:
                material = 'STEEL'
                matname  = 'StSt316'
            elif filename.find('StainSt304L_') >= 0:
                material = 'STEEL'
                matname  = 'StSt304L'
            elif filename.find('G10') >= 0:
                material = 'G10'
                matname  = 'G10'
            elif filename.find('G10_glass_phenolic') >= 0:
                material = 'G10_glass_phenolic'
                matname  = 'G10_glass_phenolic'
            elif filename.find('tufnol') >= 0:
                material = 'TUFNOL'
                matname  = 'Tufnol'
            elif filename.find('tufset') >= 0:
                material = 'POLYURETHANE'
                matname  = 'PU'
            elif filename.find('brass') >= 0:
                material = 'BRASS'
                matname  = 'CuZn'
            elif filename.find('polycarbonate') >= 0:
                material = 'POLYCARBONATE'
                matname  = 'PC'

            if filename.find(self.sourceDir) < 0:
                filename = os.path.join(self.sourceDir, filename)
            
            # parse the file and change the material references
            matfile = libxml2.parseFile(filename)
            
            positions = matfile.xpathEval("gdml/define/position")
            for pos in positions:
                if pos.prop('name').find(matname) < 0:
                    name = pos.prop('name')
                    pos.setProp('name', matname + '_' + name)
                    
            solids  = matfile.xpathEval("gdml/solids/*")
            for solid in solids:
                if solid.prop('name').find(matname) < 0:
                    name = solid.prop('name')
                    solid.setProp('name', matname + '_' +  name)
                    
            facets = matfile.xpathEval("gdml/solids/tessellated/triangular")
            for facet in facets:
                if facet.prop("vertex1").find(matname) < 0:
                    vertex = facet.prop('vertex1')
                    facet.setProp('vertex1', matname + '_' + vertex)
                if facet.prop("vertex2").find(matname) < 0:
                    vertex = facet.prop('vertex2')
                    facet.setProp('vertex2', matname + '_' + vertex)
                if facet.prop("vertex3").find(matname) < 0:
                    vertex = facet.prop('vertex3')
                    facet.setProp('vertex3', matname + '_' + vertex)
                    
            matvols = matfile.xpathEval("gdml/structure/volume")
            for example in matvols:
                name = example.prop("name")
                if name.find(matname) < 0:
                    example.setProp('name', matname+'_'+ name)
                matref = example.xpathEval("materialref")[0]
                if matref.prop('ref').find('ALUMINUM')>=0:
                    matref.setProp('ref', material)
                solref = example.xpathEval('solidref')[0]
                if solref.prop('ref').find(matname) < 0:
                    ref = solref.prop('ref')
                    solref.setProp('ref', matname + '_' +  ref)
                for physvol in example.xpathEval("physvol"):
                    physname = physvol.prop('name')
                    if type(physname) is str:
                        if physname.find(matname) < 0:
                            physname = matname + "_" + physname 
                            physvol.setProp("name", physname)
                    volref = physvol.xpathEval('volumeref')
                    if len(volref) > 0:
                        if volref[0].prop("ref").find(matname) < 0:
                            volref[0].setProp('ref', matname +'_'+ \
                                              volref[0].prop('ref'))
                    posname = physvol.xpathEval('position')[0].prop('name')
                    if posname.find(matname) < 0:
                        posname = matname + '_' +  posname
                        position = physvol.xpathEval('position')[0]
                        position.setProp('name', posname)
                    if len(physvol.xpathEval('rotation')) == 1:
                        rotname = physvol.xpathEval('rotation')[0].prop('name')
                        if rotname.find(matname) < 0:
                            rotname = matname + '_' + rotname
                            physvol.xpathEval(\
                                'rotation')[0].setProp('name', rotname)
            # write file under a new name
            filedir = filename.split('/')
            specname = filedir[-1]
            edfilename = specname[:-5] + "_ed.gdml"
            edfilename = os.path.join(self.downloadDir, edfilename)
            edfile = open(edfilename, 'w')
            matfile.saveTo(edfile)
            edfile.close()
            matfile.freeDoc()
            #if material.find('NMF')<0:
            edlist.append(edfilename)
            # clean up the old files.
            # os.remove(filename)

        #update gdmllist
        self.gdmllist = edlist

        # copy all physvols indicated in the source directory
        # to the destination directory.
        self.copy_referenced_files(self.physlist)
        self.add_dets_to_mice_info()
        shutil.copy(os.path.join(self.sourceDir, "iris1_closed.gdml"),
                    os.path.join(self.downloadDir, "iris1_closed.gdml"))
        shutil.copy(os.path.join(self.sourceDir, "iris2_closed.gdml"),
                    os.path.join(self.downloadDir, "iris2_closed.gdml"))
        shutil.copy(os.path.join(self.sourceDir, "iris3_closed.gdml"),
                    os.path.join(self.downloadDir, "iris3_closed.gdml"))
        shutil.copy(os.path.join(self.sourceDir, "iris4_closed.gdml"),
                    os.path.join(self.downloadDir, "iris4_closed.gdml"))
        shutil.copy(os.path.join(self.sourceDir, "iris1_open.gdml"),
                    os.path.join(self.downloadDir, "iris1_open.gdml"))
        shutil.copy(os.path.join(self.sourceDir, "iris2_open.gdml"),
                    os.path.join(self.downloadDir, "iris2_open.gdml"))
        shutil.copy(os.path.join(self.sourceDir, "iris3_open.gdml"),
                    os.path.join(self.downloadDir, "iris3_open.gdml"))
        shutil.copy(os.path.join(self.sourceDir, "iris4_open.gdml"),
                    os.path.join(self.downloadDir, "iris4_open.gdml"))

    def copy_referenced_files(self, physlist):
        """
        @Module copy_referenced_files

        Accesses the files referenced in the list of physical volumes
        and copy them to the destination directory.
        """
        
        for reffile in physlist:
            filename = os.path.join(self.sourceDir, reffile)
            # print "Copying "+filename
            # check if the file exists
            if not os.path.isfile(filename):
                print "copy_referenced_files::Error -- File missing: "+filename
                continue
            destination = os.path.join(self.downloadDir, reffile)
            # print filename+" copied to "+destination
            if not os.path.isfile(destination): 
                shutil.copy(filename, destination)
                
            # look for further files 
            refgdml = libxml2.parseFile(filename)
            physvol = refgdml.xpathEval("gdml/structure/volume/physvol")
            newphyslist = []
            for entry in physvol:
                if len(entry.xpathEval("file")) > 0:
                    newphyslist.append(entry.xpathEval("file")[0].prop("name"))
            self.copy_referenced_files(newphyslist)
                                       
                
    def select_module_by_position(self):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        """
        @Module select_module_by_position

        Extract subsets of physical volumes from the input files and
        sorts them into physical modules. The positions of each of the
        objects in the file is adjusted to be relative to the center
        of the assigned module. The defining ranges for each module
        are extracted from the Maus_Information file.

        """
        mod_def = self.extract_module_ranges()
        print mod_def
        #print self.gdmllist
        selphysname = []
        for key in mod_def:
            print key
            # create a new gdml file using the key name
            self.initialize_gdml_doc(key, mod_def[key])
            keyxml = libxml2.parseFile(os.path.join(self.downloadDir,
                                                    key + '.gdml'))
            # loop over all of the material gdml files
            for sourcefile in self.gdmllist:
                # parse the gdml files named
                matxml = libxml2.parseFile(sourcefile)
                # check the positions of all of the physical volumes.
                structvols = matxml.xpathEval("gdml/structure/volume")
                keyxmlvol = next(x for x in \
                                 keyxml.xpathEval("gdml/structure/volume") \
                                 if x.prop("name").find(key) >= 0)
                vol = next(x for x in structvols \
                      if x.prop("name").find("Structure") >= 0)
                # print vol.prop("name")
                physvols = vol.xpathEval("physvol")
                selectphys = []
                for physvol in physvols:
                    zpos = float(physvol.xpathEval("position")[0].prop("z"))
                    if zpos >= mod_def[key][0] and \
                           zpos < mod_def[key][1] and zpos != 0.0:
                        # correct the position for the module location
                        corrzpos = zpos - (mod_def[key][0] + mod_def[key][1])/2.
                        physvol.xpathEval("position")[0].setProp("z", \
                                                                 str(corrzpos))
                        # copy referenced objects to the new gdml file
                        selectphys.append(physvol)
                        self.collect_references_from_physvol(keyxml, \
                                                             matxml, physvol)
                        
                # End the loop over physvols
                for physvol in selectphys:
                    name = ""
                    if len(physvol.xpathEval("file"))>0:
                        name = physvol.xpathEval("file")[0].prop("name")
                    else:
                        name = physvol.xpathEval("volumeref")[0].prop("ref")
                    selphysname.append(name)
                    keyxmlvol.addChildList(physvol.copyNode(1))
                    
                # free the material specific gdml document
                matxml.freeDoc()
                
            # No more input files.
            # Create and save the new key gdml file
            out = open(os.path.join(self.downloadDir, key + ".gdml"), "w+")
            # outname = os.path.join(self.downloadDir, key + ".gdml")
            keyxml.saveTo(out)
            # keyxml.saveFormatFile(outname, 1)
            # Now I want to make the resulting file look presentable
            data = out.read()
            changed = data.replace('><', '>\n<')
            
            out.write(changed)
            out.close()
            keyxml.freeDoc()
        
        # That is all of the module keys.
        # Reopen the top level gdml file for reference.
        src = libxml2.parseFile(self.gdmlin1)
        # Now a new top level file mus be written containing the above
        # files.
        self.initialize_gdml_doc(self.outname, [0., 70000.]) 
        # Copy files that are referenced from the old top level file
        # (mostly detectors).
        new = libxml2.parseFile(os.path.join(self.downloadDir, \
                                             self.outname+".gdml"))
        newvol = next(x for x in new.xpathEval("gdml/structure/volume") \
                      if x.prop("name").find(self.outname) >= 0)
        vols = src.xpathEval("gdml/structure/volume")
        vol = next(x for x in vols \
                   if x.prop("name").find("Structure") >= 0)
        
        physvols = [x for x in vol.xpathEval("physvol") \
                   if x.xpathEval("file")[0].prop("name") \
                   in self.physlist]                       
        
        for physvol in physvols:
            name = physvol.xpathEval("file")[0].prop("name")
            if name in selphysname:
                continue
            newvol.addChildList(physvol.copyNode(1))
        for key in mod_def:
            modpos = (mod_def[key][0] + mod_def[key][1])/2.
            keynode = libxml2.newNode("physvol")
            keynode.setProp('name', key+'_phys')
            keyfile = libxml2.newNode("file")
            keyfile.setProp('name', key+'.gdml')
            keynode.addChild(keyfile)
            keypos = libxml2.newNode("position")
            keypos.setProp('name', key+'_pos')
            keypos.setProp('x', '0.0')
            keypos.setProp('y', '0.0')
            keypos.setProp('z', str(modpos))
            keypos.setProp('unit', 'mm')
            keynode.addChild(keypos)
            keyrot = libxml2.newNode("rotation")
            keyrot.setProp('name', key+'_rot')
            keyrot.setProp('x', '0.0')
            keyrot.setProp('y', '0.0')
            keyrot.setProp('z', '0.0')
            keyrot.setProp('unit', 'degree')
            keynode.addChild(keyrot)
            newvol.addChild(keynode)

        fname = os.path.join(self.downloadDir, self.outname + ".gdml")
        new.saveFormatFile(fname, 1)
        new.freeDoc()
        
    def initialize_gdml_doc(self, key, limits):
        #pylint: disable = R0912, R0913, R0914, R0915, C0301
        """
        Module @initialize_gdml_doc

        A fast way to define a non-specific gdml file prior to parsing.
        """
        newfile = open(os.path.join(self.downloadDir, key + '.gdml'),'w')
        keymat = "AIR"
        if key.find("Solenoid")>=0 or \
               key.find("FC")>=0:
            keymat = "Galactic"
        length = float(limits[1]) - float(limits[0])
        keydata = \
                """<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>
<gdml xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd\">
    
   <define>
    <constant name=\"HALFPI\" value=\"pi/2.\"/>
    <constant name=\"PI\" value=\"1.*pi\"/>
    <constant name=\"TWOPI\" value=\"2.*pi\"/>
    <position name=\"center\"/>
    <rotation name=\"identity\"/>
    <rotation name=\"aligneSurX\" y=\"HALFPI\"/>
    <rotation name=\"aligneSurY\" x=\"HALFPI\"/>
   </define>

   <materials>
      <element name=\"videRef\" formula=\"VACUUM\" Z=\"1\">
          <atom value=\"1\"/>
      </element>
      <material name=\"Galactic\" formula=\"VACUUM\">
          <D value =\"1E-25\" unit=\"g/cm3\"/>
          <fraction n=\"1.0\" ref=\"videRef\"/>
      </material>
      <isotope N=\"12\" Z=\"6\" name=\"C120xa126a70\">
          <atom unit=\"g/mole\" value=\"12\"/>
      </isotope>
      <isotope N=\"13\" Z=\"6\" name=\"C130xa126b20\">
          <atom unit=\"g/mole\" value=\"13.0034\"/>
      </isotope>
      <element name=\"C0xa126810\">
          <fraction n=\"0.9893\" ref=\"C120xa126a70\"/>
          <fraction n=\"0.0107\" ref=\"C130xa126b20\"/>
      </element>
      <isotope N=\"14\" Z=\"7\" name=\"N140xa126de0\">
          <atom unit=\"g/mole\" value=\"14.0031\"/>
      </isotope>
      <isotope N=\"15\" Z=\"7\" name=\"N150xa126e80\">
          <atom unit=\"g/mole\" value=\"15.0001\"/>
      </isotope>
      <element name=\"N0xa126b80\">
          <fraction n=\"0.99632\" ref=\"N140xa126de0\"/>
          <fraction n=\"0.00368\" ref=\"N150xa126e80\"/>
      </element>
      <isotope N=\"16\" Z=\"8\" name=\"O160xa1270f0\">
          <atom unit=\"g/mole\" value=\"15.9949\"/>
      </isotope>
      <isotope N=\"17\" Z=\"8\" name=\"O170xa127180\">
          <atom unit=\"g/mole\" value=\"16.9991\"/>
      </isotope>
      <isotope N=\"18\" Z=\"8\" name=\"O180xa1271f0\">
          <atom unit=\"g/mole\" value=\"17.9992\"/>
      </isotope>
      <element name=\"O0xa126ec0\">
          <fraction n=\"0.99757\" ref=\"O160xa1270f0\"/>
          <fraction n=\"0.00038\" ref=\"O170xa127180\"/>
          <fraction n=\"0.00205\" ref=\"O180xa1271f0\"/>
      </element>
      <isotope N=\"36\" Z=\"18\" name=\"Ar360xa1270a0\">
          <atom unit=\"g/mole\" value=\"35.9675\"/>
      </isotope>
      <isotope N=\"38\" Z=\"18\" name=\"Ar380xa1274b0\">
          <atom unit=\"g/mole\" value=\"37.9627\"/>
      </isotope>
      <isotope N=\"40\" Z=\"18\" name=\"Ar400xa1275a0\">
          <atom unit=\"g/mole\" value=\"39.9624\"/>
      </isotope>
      <element name=\"Ar0xa127230\">
          <fraction n=\"0.003365\" ref=\"Ar360xa1270a0\"/>
          <fraction n=\"0.000632\" ref=\"Ar380xa1274b0\"/>
          <fraction n=\"0.996003\" ref=\"Ar400xa1275a0\"/>
      </element>
      <material name=\"AIR\" state=\"gas\">
          <MEE unit=\"eV\" value=\"85.7\"/>
          <D unit=\"g/cm3\" value=\"0.00120479\"/>
          <fraction n=\"0.000124000124000124\" ref=\"C0xa126810\"/>
          <fraction n=\"0.755267755267755\" ref=\"N0xa126b80\"/>
          <fraction n=\"0.231781231781232\" ref=\"O0xa126ec0\"/>
          <fraction n=\"0.0128270128270128\" ref=\"Ar0xa127230\"/>
      </material>
      <element name=\"chromium\" formula=\"CR\" Z=\"24\">
          <atom unit=\"g/mole\" value=\"51.9961\"/>
      </element>
      <element name=\"nickel\" formula=\"Ni\" Z=\"28\">
          <atom unit=\"g/mole\" value=\"58.6934\"/>
      </element>
      <element name=\"iron\" formula=\"FE\" Z=\"26\">
          <atom value=\"55.845\"/>
      </element>
      <material name=\"IRON\" formula=\"IRON\">
          <D value=\"7.874\" unit=\"g/cm3\"/>
          <fraction n=\"1.0000\" ref=\"iron\"/>
      </material>
      <material name=\"STEEL\" formula=\"STEEL\">
          <D value=\"7.85\" unit=\"g/cm3\"/>
          <fraction n=\"0.71\" ref=\"iron\"/>
          <fraction n=\"0.17\" ref=\"chromium\"/>
          <fraction n=\"0.12\" ref=\"nickel\"/>
      </material>
      <element name=\"aluminum\" formula=\"Al\" Z=\"13\">
          <atom value=\"26.9815\"/>
      </element>
      <material name=\"ALUMINUM\" formula=\"ALUMINUM\">
          <D value =\"2.7000\" unit=\"g/cm3\"/>
          <fraction n=\"1.0000\" ref=\"aluminum\"/>
      </material>
                
      <element name=\"copper\" formula=\"Cu\" Z=\"29\">
          <atom value=\"63.546\"/>
      </element>
      <material name=\"COPPER\" formula=\"COPPER\">
          <D value =\"8.96\" unit=\"g/cm3\"/>
          <fraction n=\"1.0000\" ref=\"copper\"/>
      </material>
      <element name=\"lead\" formula=\"Pb\" Z=\"82\">
          <atom value=\"207.2\"/>
       </element>
       <material name=\"LEAD\" formula=\"LEAD\">
          <D value =\"11.34\" unit=\"g/cm3\"/>
          <fraction n=\"1.0000\" ref=\"lead\"/>
       </material>
       <!-- G10 -->
       <element name=\"silicon\" formula=\"Si\" Z=\"14\">
          <atom value=\"28.0855\"/>
       </element>
       <element name=\"oxygen\" formula=\"O\" Z=\"8\">
          <atom value=\"15.999\"/>
       </element>
       <element name=\"sodium\" formula=\"Na\" Z=\"11\">
          <atom value=\"22.9898\"/>
       </element>
       <element name=\"calcium\" formula=\"Ca\" Z=\"20\">
          <atom value=\"40.078\"/>
       </element>
       <element name=\"magnesium\" formula=\"Mg\" Z=\"12\">
          <atom value=\"24.3050\"/>
       </element>
       <element name=\"sulfur\" formula=\"S\" Z=\"16\">
          <atom value=\"32.06\"/>
       </element>
       <material name=\"SILICON\" formula=\"SILICON\">
          <D value =\"2.3300\" unit=\"g/cm3\"/>
          <fraction n=\"1.0000\" ref=\"silicon\"/>
       </material>
       <material name=\"SiO2\" formula=\"SiO2\">
          <D value =\"2.65\" unit=\"g/cm3\"/>
          <composite n=\"1\" ref=\"silicon\"/>
          <composite n=\"2\" ref=\"oxygen\"/>
       </material>
       <material name=\"CaO\" formula=\"CaO\">
          <D value =\"2.49\" unit=\"g/cm3\"/>
          <composite n=\"1\" ref=\"calcium\"/>
          <composite n=\"1\" ref=\"oxygen\"/>
       </material>
       <material name=\"Na2O\" formula=\"Na2O\">
          <D value =\"2.49\" unit=\"g/cm3\"/>
          <composite n=\"2\" ref=\"sodium\"/>
          <composite n=\"1\" ref=\"oxygen\"/>
       </material>
       <material name=\"MgO\" formula=\"MgO\">
          <D value =\"2.49\" unit=\"g/cm3\"/>
          <composite n=\"1\" ref=\"magnesium\"/>
          <composite n=\"1\" ref=\"oxygen\"/>
       </material>
       <material name=\"SO3\" formula=\"SO3\">
          <D value =\"2.49\" unit=\"g/cm3\"/>
          <composite n=\"1\" ref=\"sulfur\"/>
          <composite n=\"3\" ref=\"oxygen\"/>
       </material>
       <material name=\"Al2O3\" formula=\"Al2O3\">
          <D value =\"2.49\" unit=\"g/cm3\"/>
          <composite n=\"2\" ref=\"aluminum\"/>
          <composite n=\"3\" ref=\"oxygen\"/>
       </material>
       <material name=\"G10\" formula=\"G10\">
          <D value =\"2.49\" unit=\"g/cm3\"/>
          <fraction n=\"0.72\" ref=\"SiO2\"/>
          <fraction n=\"0.10\" ref=\"CaO\"/>
          <fraction n=\"0.142\" ref=\"Na2O\"/>
          <fraction n=\"0.025\" ref=\"MgO\"/>
          <fraction n=\"0.007\" ref=\"SO3\"/>
          <fraction n=\"0.006\" ref=\"Al2O3\"/>
       </material>
       <element name=\"nitrogen\" formula=\"N\" Z=\"7\">
          <atom value=\"14\"/>
       </element>
       <element name=\"carbon\" formula=\"C\" Z=\"6\">
          <atom value=\"12\"/>
       </element>
       <element name=\"hydrogen\" formula=\"C\" Z=\"1\">
          <atom value=\"1\"/>
       </element>
       <material name=\"POLYURETHANE\" formula=\"POLYURETHANE\">
          <D value =\"1.2200\" unit=\"g/cm3\"/>
          <fraction n=\"0.6438\" ref=\"carbon\"/>
          <fraction n=\"0.0901\" ref=\"hydrogen\"/>
          <fraction n=\"0.0601\" ref=\"nitrogen\"/>
          <fraction n=\"0.2060\" ref=\"oxygen\"/>
       </material>
       <material name=\"TUFNOL\" formula=\"TUFNOL\">
          <D value =\"1.36\" unit=\"g/cm3\"/>
          <fraction n=\"0.7368\" ref=\"carbon\"/>
          <fraction n=\"0.0526\" ref=\"hydrogen\"/>
          <fraction n=\"0.2105\" ref=\"oxygen\"/>
       </material>
       <element name=\"zinc\" formula=\"Zn\" Z=\"30\">
          <atom value=\"65.38\"/>
       </element>
       <material name=\"ZINC\" formula=\"ZINC\">
          <D value =\"7.14\" unit=\"g/cm3\"/>
          <fraction n=\"1.0000\" ref=\"lead\"/>
       </material>
       <material name=\"BRASS\" formula=\"BRASS\">
          <D value =\"8.73\" unit=\"g/cm3\"/>
          <fraction n=\"0.58\" ref=\"copper\"/>
          <fraction n=\"0.39\" ref=\"zinc\"/>
          <fraction n=\"0.03\" ref=\"lead\"/>
       </material>
       <material name=\"POLYCARBONATE\" formula=\"POLYCARBONATE\">
          <D value =\"1.2200\" unit=\"g/cm3\"/>
          <fraction n=\"0.7559\" ref=\"carbon\"/>
          <fraction n=\"0.0551\" ref=\"hydrogen\"/>
          <fraction n=\"0.18898\" ref=\"oxygen\"/>
       </material>
   </materials>
                
   <solids>
       <box name=\""""+str(key)\
        +"""_solid\" lunit=\"mm\" x=\"2000.0\" y=\"2000.0\" z=\""""+\
                     str(length)+"""\"/>
   </solids>
                
   <structure>
       <volume name=\""""+str(key)+"""_structvol\">
            <materialref ref=\""""+str(keymat)+"""\"/>
            <solidref ref=\""""+str(key)+"""_solid\"/>
       </volume>
   </structure>
                
   <setup name=\"Default\" version=\"1.0\">
       <world ref=\""""+str(key)+"""_structvol\"/>
   </setup>
</gdml>
"""
        newfile.write(keydata)
        newfile.close()

    def extract_module_ranges(self):
        """
        @Module extract_module_ranges

        Get the definition of the subset volumes from the Maus_Information file.
        """
        mod_def = {}
        infoxml = libxml2.parseFile(self.mausInfo)
        mods = infoxml.xpathEval("MICE_Information/Other_Information/Module")
        for entry in mods:
            key =  entry.prop("name")
            zmin = float(entry.prop("zmin"))
            zmax = float(entry.prop("zmax"))
            mod_def[key] = [zmin, zmax]
        infoxml.freeDoc()
        return mod_def
        
    def collect_references_from_physvol(self, keyxml, matxml, physvol):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        """
        @Module collect_references_from_physvol

        find and copy objects referenced from a particular physvol
        """
        # find the volumes referred to in the physical volume
        volrefs = physvol.xpathEval("volumeref")
        volname = ''
        matvols = matxml.xpathEval("gdml/structure/volume")
        keyvols = keyxml.xpathEval("gdml/structure/volume")
        keyvol = next(x for x in keyvols \
                      if x.prop("name").find("structvol") >= 0)
        matsolids = matxml.xpathEval("gdml/solids/*")
        keysolids = keyxml.xpathEval("gdml/solids")
        matdefs   = matxml.xpathEval("gdml/define")
        keydefs   = keyxml.xpathEval("gdml/define")
        
        if len(volrefs) > 0:
            volname = volrefs[0].prop("ref")
            entry = next(x for x in matvols \
                         if x.prop("name") == volname)
            keyvol.addPrevSibling(entry.copyNode(1))
            # copy the matching solids to the key doc
            solidrefs = entry.xpathEval("solidref")
            solidname = solidrefs[0].prop("ref")
            solid = next(x for x in matsolids \
                if solidname == x.prop("name"))
            keysolids[0].addChildList(solid.copyNode(1))
            # examine solid volume for vertex positions in
            # the case of a tesselated solid
            if solid.name == "tessellated":
                matpos = matdefs[0].xpathEval("position")
                for facet in solid.xpathEval("triangular"):
                    defpos = keydefs[0].xpathEval("position")
                    vert1 = facet.prop("vertex1")
                    pos = next(x for x in matpos if x.prop("name") == vert1)
                    poscheck = [x for x in defpos if x.prop("name") == vert1]
                    if len(poscheck) == 0:
                        keydefs[0].addChildList(pos.copyNode(1))
                    vert2 = facet.prop("vertex2")
                    poscheck = []
                    pos = next(x for x in matpos if x.prop("name") == vert2)
                    poscheck = [x for x in defpos if x.prop("name") == vert2]
                    if len(poscheck) == 0:
                        keydefs[0].addChildList(pos.copyNode(1))
                    vert3 = facet.prop("vertex3")
                    poscheck = []
                    pos = next(x for x in matpos if x.prop("name") == vert3)
                    poscheck = [x for x in defpos if x.prop("name") == vert3]
                    if len(poscheck) == 0:
                        keydefs[0].addChildList(pos.copyNode(1))
                    
            # examine physical volumes for references.
            for physnew in entry.xpathEval("physvol"):
                self.collect_references_from_physvol(keyxml, matxml, physnew)
                    

    def add_dets_to_mice_info(self):
        """
        @Module add_to_mice_info

        Add information to the mice information file.
        """
        info = libxml2.parseFile(self.mausInfo)
        dets  = info.xpathEval("MICE_Information/Detector_Information")

        base = libxml2.parseFile(self.gdmlin1)

        for det in dets[0].xpathEval("*"):
            if det.hasProp("gdml_posref"):
                target = det.prop("gdml_posref")
                # find the position reference in the base file.
                node = next(x for x in \
                            base.xpathEval("gdml/structure/volume/physvol") \
                            if x.xpathEval("position")[0].prop("name").\
                            find(target)>=0)
                # add the information to the info file
                det.addChildList(node.copyNode(1))
            # otherwise we need to go one step deeper
        for det in dets[0].xpathEval("*/*"):
            if det.hasProp("gdml_posref"):
                target = det.prop("gdml_posref")
                # find the position reference in the base file.
                node = next(x for x in \
                            base.xpathEval("gdml/structure/volume/physvol") \
                            if x.xpathEval("position")[0].prop("name").\
                            find(target)>=0)
                # add the information to the info file
                det.addChildList(node.copyNode(1))

        # Since the structure of the file is being changed dramatically
        # it will be written to the ultimate destination directory rather
        # than the source
        out = open(
            os.path.join( self.downloadDir, "Maus_Information.gdml"), "w+")
        info.saveTo(out)
        out.close()
    
    def add_fields_to_mice_info(self, source_file):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        """
        @Module add_to_mice_info

        Add information to the mice information file.
        """
        info = libxml2.parseFile(self.mausInfo)
        fields = info.xpathEval("MICE_Information/G4Field_Information")

        base = libxml2.parseFile(source_file)

        for field in fields[0].xpathEval("*"):
            elems = field.xpathEval("GDMLElem")
            ncoils = len(elems)
            if ncoils == 4 or ncoils == 2 or ncoils == 1:
                # This should work equally well for a quad, dipole, or solenoid
                # need to extract an average position for the field
                # (To Do: fit to establish a rotation)
                pos = []
                meanpos = [0., 0., 0.]
                for elem in elems:
                    # find the element in the source file
                    target = elem.prop("name")
                    # print "Locating physvol", target
                    physvol = base.xpathEval("gdml/structure/volume/physvol")
                    match = next(x for x in physvol \
                                 if x.prop("name").find(target)>=0)
                    elempos = match.xpathEval("position")[0]
                    pos.append([float(elempos.prop("x")), \
                                float(elempos.prop("y")), \
                                float(elempos.prop("z"))])
                    meanpos[0] += float(elempos.prop("x"))
                    meanpos[1] += float(elempos.prop("y"))
                    meanpos[2] += float(elempos.prop("z"))
                meanpos[0] = meanpos[0]/float(ncoils)
                meanpos[1] = meanpos[1]/float(ncoils)
                meanpos[2] = meanpos[2]/float(ncoils)
                
                # Now that an average position is defined it needs to
                # be written to the information file

                position = field.xpathEval("Position")[0]
                if ncoils == 4: # special treatment for quads
                    centrelength = \
                       float(field.xpathEval("CentreLength")[0].prop('value'))
                    if field.xpathEval("CentreLength")[0].prop("units")=='m':
                        centrelength *= 1000
                    elif field.xpathEval("CentreLength")[0].prop("units")=='cm':
                        centrelength *= 10
                    # subtract the centre length from the z position
                    meanpos[2] -= centrelength
                    
                position.setProp('x', str(meanpos[0]))
                position.setProp('y', str(meanpos[1]))
                position.setProp('z', str(meanpos[2]))


        out = open(self.mausInfo, "w+")
        info.saveTo(out)
        out.close()
        

