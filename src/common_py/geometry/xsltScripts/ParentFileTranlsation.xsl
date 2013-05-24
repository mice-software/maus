<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xd="http://www.oxygenxml.com/ns/doc/xsl" exclude-result-prefixes="xd" version="1.0">
    <xd:doc scope="stylesheet">
        <xd:desc>
            <xd:p><xd:b>Created on:</xd:b> Jan 10, 2011</xd:p>
            <xd:p><xd:b>Author:</xd:b> Matt</xd:p>
            <xd:p/>
        </xd:desc>
    </xd:doc>
    <!-- This is the translation style sheet for the FastradModel.gdml file produced by Fastrad.
         It produces the top parent file for the .dat files and icnorporates rotating the CAD
         model by 90 degrees so the beam line axis is along the z axis. It also places the 
         fields in the correct places and any detectors as well. It searches through the GDML 
         and selects certain node and writes the .dat mice module in the correcnt format.
    -->
    <xsl:output method="text"/>
    <xsl:template match="gdml">
        <html>
            <head>
                <title>Configuration <xsl:value-of select="structure/volume/@name"/> 
                       { 
                           Dimensions <xsl:if test="solids/sphere/@name = 'WorldSphereRef'">15000.0 10000.0 50000.0 mm</xsl:if>
                           PropertyString Material AIR <!--<xsl:if test="structure/volume/materialref/@ref = 'Vacuum'">AIR</xsl:if>-->  
                           PropertyDouble G4StepMax <xsl:value-of select="MICE_Information/Other_Information/G4StepMax/@Value"/> mm 
                </title>
            </head>
            <body>  
                            //Substitutions
                            <xsl:choose><xsl:when test="contains(MICE_Information/Configuration_Information/run/@beamStop, 'false') or contains(MICE_Information/Configuration_Information/run/@beamStop, 'true')">
                            Substitution $beamStop <xsl:value-of select="MICE_Information/Configuration_Information/run/@beamStop"/>
                            
                            Substitution $diffuserThickness <xsl:value-of select="MICE_Information/Configuration_Information/run/@protonAbsorberThickness"/>
                            Substitution $protonAbsroberThickness <xsl:value-of select="MICE_Information/Configuration_Information/run/@beamStop"/><xsl:text>
                                
                            </xsl:text>
                            <xsl:for-each select="MICE_Information/Configuration_Information/run/magnet"><xsl:choose>
                            <xsl:when test="contains(@name, 'D1')">Substitution $D1Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'D2')">Substitution $D2Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'DS')">Substitution $DSCurrent <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q1')">Substitution $Q1Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q2')">Substitution $Q2Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q3')">Substitution $Q3Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q4')">Substitution $Q4Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q5')">Substitution $Q5Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q6')">Substitution $Q6Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q7')">Substitution $Q7Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q8')">Substitution $Q8Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q9')">Substitution $Q9Current <xsl:value-of select="@setCurrent"/><xsl:text>
                                
                            </xsl:text></xsl:when></xsl:choose>
                            </xsl:for-each></xsl:when><xsl:otherwise> </xsl:otherwise></xsl:choose>
                            
                            //Module <xsl:value-of select="MICE_Information/Other_Information/GDML_Files/@location"/>RotatedGeometryFile.dat
                            //{
                            //    Position 0.0 0.0 0.0 mm
                            //    Rotation 0.0 90.0 0.0 deg
                            //}

                           
                            <xsl:variable name="tof_1_file_number" select="MICE_Information/Other_Information/FileNumbers/Tof1FileNumber/@number"/>
                            
                             <xsl:for-each select="structure/volume/physvol"> 
                                   Module <xsl:choose><xsl:when test="contains(file/@name, 'Iges_0')">TOF/TOF0.dat</xsl:when><xsl:when test="contains(file/@name, 'Iges_1')">TOF/TOF1.dat</xsl:when><xsl:when test="contains(file/@name, 'Iges_3')">TOF/TOF2.dat</xsl:when><xsl:when test="contains(file/@name, 'Iges_4')">KL/KL.dat</xsl:when><xsl:when test="contains(file/@name, 'Iges_6')">Ckov/Cherenkov.dat</xsl:when><xsl:when test="contains(file/@name, 'Iges_7')">Ckov/Acc1.dat</xsl:when><xsl:when test="contains(file/@name, 'Iges_8')">Ckov/Acc2.dat</xsl:when><xsl:when test="contains(file/@name, 'SingleStation')">Tracker/Tracker1Station1.dat</xsl:when><xsl:otherwise><xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="substring-before(file/@name, '.')"/>.dat</xsl:otherwise></xsl:choose>
                                    { 
                                        Position <xsl:value-of select="position/@x"/><xsl:text> </xsl:text><xsl:value-of select="position/@y"/><xsl:text> </xsl:text><xsl:value-of select="position/@z"/> mm 
                                        Rotation <xsl:choose><xsl:when test="rotationref/@ref = 'RotateY90'"> 0.0 90.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX90'"> 90.0 0.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX270'"> 270.0 0.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX180'"> 180.0 0.0 0.0 deg</xsl:when><xsl:otherwise> 0.0 0.0 0.0 deg</xsl:otherwise></xsl:choose> 
                                    }
                            </xsl:for-each> 
                	    
                            //Module Tracker/TrackerSolenoid0.dat
                            //{
                            //    Position 0. 0. 15016.24 mm
                            //    Rotation 0. 0. 0. degree
                            //}
                
                            //Module AFC/AbsorberFocusCoil.dat
                            //{
                            //    Position 0. 0. 16955.74 mm
                            //    Rotation 0. 0. 0. degree
                            //}
                
                            //Module RFCC/RFCouplingCoilUp.dat
                            //{
                            //    Position 0. 0. 18895.24 mm
                            //    Rotation 0. 0. 0. degree
                            //    ScaleFactor -1
                            //}
                                            //Other Modules
                            <xsl:choose><xsl:when test="contains(MICE_Information/Configuration_Information/run, @diffuserThickness)">
                            //Diffuser thickness is <xsl:value-of select="MICE_Information/Configuration_Information/run/@diffuserThickness"/>
                            //Go and select the diffuser model
                            </xsl:when></xsl:choose>
                
                            <xsl:choose><xsl:when test="contains(MICE_Information/Configuration_Information/run, @protonAbsorberThickness)">
                            //Diffuser thickness is <xsl:value-of select="MICE_Information/Configuration_Information/run/@protonAbsorberThickness"/>
                            //Go and select the diffuser model
                            </xsl:when></xsl:choose>

                            // Fields
                <xsl:for-each select="MICE_Information/G4Field_Information/Dipole">
                            Module <xsl:value-of select="FieldName/@name"/>
                            {
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/> 
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                            ScaleFactor <xsl:value-of select="ScaleFactor/@value"/>
                            Volume <xsl:value-of select="Volume/@name"/>
                            PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                            PropertyString FieldMapMode <xsl:value-of select="FieldMapMode/@name"/>
                            PropertyString FileType <xsl:value-of select="FileType/@name"/>
                            PropertyString FileName <xsl:value-of select="FileName/@name"/>
                            PropertyString Symmetry <xsl:value-of select="Symmetry/@name"/>
                            }
                </xsl:for-each>
                <xsl:for-each select="MICE_Information/G4Field_Information/Quadrupole">
                            Module <xsl:value-of select="FieldName/@name"/>
                            {
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/> 
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                            ScaleFactor <xsl:value-of select="ScaleFactor/@value"/>
                            Volume <xsl:value-of select="Volume/@name"/>
                            PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                            PropertyDouble Height <xsl:value-of select="Dimensions/@height"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble Width <xsl:value-of select="Dimensions/@width"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble Length <xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble FieldStrength <xsl:value-of select="FieldStrength/@Value"/><xsl:text> </xsl:text><xsl:value-of select="FieldStrength/@units"/>
                            PropertyInt Pole <xsl:value-of select="Pole/@Value"/>
                            PropertyInt MaxEndPole <xsl:value-of select="MaxEndPole/@Value"/>
                            PropertyString EndFieldType <xsl:value-of select="EndFieldType/@name"/>
                            PropertyDouble EndLength <xsl:value-of select="EndLength/@Value"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble CentreLength <xsl:value-of select="CentreLength/@Value"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            }
                </xsl:for-each>
                <xsl:for-each select="MICE_Information/G4Field_Information/Solenoid">
                            Module <xsl:value-of select="FieldName/@name"/>
                            {
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                            Volume <xsl:value-of select="Volume/@name"/>
                            PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                            PropertyString FileName <xsl:value-of select="FileName/@name"/>
                            PropertyDouble CurrentDensity <xsl:value-of select="CurrentDensity/@Value"/>
                            PropertyDouble Length <xsl:value-of select="Length/@Value"/>
                            PropertyDouble Thickness <xsl:value-of select="Thickness/@Value"/>
                            PropertyDouble InnerRadius <xsl:value-of select="InnerRadius/@Value"/>
                            ScaleFactor <xsl:value-of select="ScaleFactor/@name"/>
                            }
                </xsl:for-each>
                       } 
                       
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>