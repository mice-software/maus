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
                            <xsl:variable name="run_number" select="MICE_Information/Configuration_Information/run/@runNumber"/>
		                    <xsl:variable name="diffuserThickness" select="MICE_Information/Configuration_Information/run/@diffuserThickness"/>
                            <xsl:variable name="mcmode" select="MICE_Information/Configuration_Information/coolingchannel/magnets/magnet/@mode"/>
                            <xsl:choose><xsl:when test="contains(MICE_Information/Configuration_Information/run/@beamStop, 'false') or contains(MICE_Information/Configuration_Information/run/@beamStop, 'true')">
                            Substitution $beamStop <xsl:value-of select="MICE_Information/Configuration_Information/run/@beamStop"/>
                            
                            Substitution $diffuserThickness <xsl:value-of select="MICE_Information/Configuration_Information/run/@diffuserThickness"/>
                            Substitution $protonAbsorberThickness <xsl:value-of select="MICE_Information/Configuration_Information/run/@beamStop"/><xsl:text>
                            
                            </xsl:text>

                            <xsl:for-each select="MICE_Information/Configuration_Information/run/magnet"><xsl:choose>
		            
                            <xsl:when test="contains(@name, 'D1')">Substitution $D1Current <xsl:value-of select="@setCurrent"/>
			    Substitution $D1Polarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'D2')">Substitution $D2Current <xsl:value-of select="@setCurrent"/>
			    Substitution $D2Polarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'DS') and contains(string-length(@name),2)">Substitution $DSCurrent <xsl:value-of select="@setCurrent"/>
			    Substitution $DSPolarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q1')">Substitution $Q1Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q1Polarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q2')">Substitution $Q2Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q2Polarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q3')">Substitution $Q3Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q3Polarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q4')">Substitution $Q4Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q4Polarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q5')">Substitution $Q5Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q5Polarity <xsl:value-of select="@polarity"/><xsl:text>

                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q6')">Substitution $Q6Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q6Polarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q7')">Substitution $Q7Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q7Polarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q8')">Substitution $Q8Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q8Polarity <xsl:value-of select="@polarity"/><xsl:text>
                              
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'Q9')">Substitution $Q9Current <xsl:value-of select="@setCurrent"/>
			    Substitution $Q9Polarity <xsl:value-of select="@polarity"/><xsl:text>

			    </xsl:text></xsl:when></xsl:choose>
                            </xsl:for-each></xsl:when><xsl:otherwise> </xsl:otherwise></xsl:choose>
                <xsl:for-each select="MICE_Information/Configuration_Information/coolingchannel/magnets/magnet">
                    <xsl:choose>
                        <xsl:when test="contains(@name, 'SSU')">
                            <xsl:for-each select="coil">
                                <xsl:choose>
                                    <xsl:when test="contains(@name, 'SSU-T2')">Substitution $SSUT2 <xsl:value-of select="@iset"/>
                                        Substitution $SSUT2Polarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'SSU-T1')">Substitution $SSUT1 <xsl:value-of select="@iset"/>
                                        Substitution $SSUT2Polarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'SSU-C')">Substitution $SSUC <xsl:value-of select="@iset"/>
                                        Substitution $SSUCPolarity <xsl:value-of select="../@polarity"/><xsl:text> 
                        
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'SSU-M1')">Substitution $SSUM1 <xsl:value-of select="@iset"/>
                                        Substitution $SSUM1Polarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'SSU-M2')">Substitution $SSUM2 <xsl:value-of select="@iset"/>
                                        Substitution $SSUM2Polarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>            
                                </xsl:choose>
                            </xsl:for-each>
                        </xsl:when>
                        <xsl:when test="contains(@name, 'FCM') and contains(@mode, 'Solenoid')">
                            Substitution $FCMMode +1<xsl:text>
                                        
                        </xsl:text>
                            <xsl:for-each select="coil">
                                <xsl:choose>
                                    <xsl:when test="contains(@name, 'FCM-U')">Substitution $FCMU <xsl:value-of select="@iset"/>
                                        Substitution $FCMUPolarity <xsl:value-of select="../@polarity"/><xsl:text>
                                        
                        </xsl:text>
                                    </xsl:when>
                                    <xsl:when test="contains(@name, 'FCM-D')">Substitution $FCMD <xsl:value-of select="@iset"/>
                                        Substitution $FCMDPolarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>                   
                                </xsl:choose>
                            </xsl:for-each>
                        </xsl:when>
                        <xsl:when test="contains(@name, 'FCM') and contains(@mode, 'Flip')">
                            Substitution $FCMMode +1<xsl:text>
                                        
                        </xsl:text>
                            <xsl:for-each select="coil">
                                <xsl:choose>
                                    <xsl:when test="contains(@name, 'FCM-U')">Substitution $FCMU <xsl:value-of select="@iset"/>
                                        Substitution $FCMUPolarity <xsl:value-of select="../@polarity"/><xsl:text>
                                        
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'FCM-D')">Substitution $FCMD <xsl:value-of select="@iset"/>
                                        Substitution $FCMDPolarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>    
                                </xsl:choose>
                            </xsl:for-each>
                        </xsl:when>
                        <xsl:when test="contains(@name, 'SSD')">
                            <xsl:for-each select="coil">
                                <xsl:choose>
                                    <xsl:when test="contains(@name, 'SSD-T2')">Substitution $SSDT2 <xsl:value-of select="@iset"/>
                                        Substitution $SSDT2Polarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'SSD-T1')">Substitution $SSDT1 <xsl:value-of select="@iset"/>
                                        Substitution $SSDT2Polarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'SSD-C')">Substitution $SSDC <xsl:value-of select="@iset"/>
                                        Substitution $SSDCPolarity <xsl:value-of select="../@polarity"/><xsl:text> 
                        
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'SSD-M1')">Substitution $SSDM1 <xsl:value-of select="@iset"/>
                                        Substitution $SSDM1Polarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>
                                    <xsl:when test="contains(@name, 'SSD-M2')">Substitution $SSDM2 <xsl:value-of select="@iset"/>
                                        Substitution $SSDM2Polarity <xsl:value-of select="../@polarity"/><xsl:text>
                            
                        </xsl:text></xsl:when>    
                                </xsl:choose>
                            </xsl:for-each>
                        </xsl:when>
                    </xsl:choose>
                </xsl:for-each>
                
                            
                            
	            Module Virtuals
                        {
                               Volume None
                               Position 0.0 0.0 0.0+0.1*@RepeatNumber m
                               PropertyString SensitiveDetector Virtual
                               PropertyBool RepeatModule2 True
                               PropertyInt NumberOfRepeats 130
                        }


                            <xsl:variable name="tof_0_file_number" select="MICE_Information/Other_Information/FileNumbers/Tof0FileNumber/@number"/>
                            <xsl:variable name="tof_1_file_number" select="MICE_Information/Other_Information/FileNumbers/Tof1FileNumber/@number"/>
                            <xsl:variable name="tof_2_file_number" select="MICE_Information/Other_Information/FileNumbers/Tof2FileNumber/@number"/>
                            <xsl:variable name="kl_file_number" select="MICE_Information/Other_Information/FileNumbers/KLFileNumber/@number"/>
                            <xsl:variable name="ckov1_file_number" select="MICE_Information/Other_Information/FileNumbers/Ckov1FileNumber/@number"/>
                            <xsl:variable name="ckov2_file_number" select="MICE_Information/Other_Information/FileNumbers/Ckov2FileNumber/@number"/>
                            <xsl:variable name="emr_file_number" select="MICE_Information/Other_Information/FileNumbers/EMRFileNumber/@number"/>
                            <xsl:variable name="tracker0_file_number" select="MICE_Information/Other_Information/FileNumbers/Tracker0FileNumber/@number"/>
                            <xsl:variable name="tracker1_file_number" select="MICE_Information/Other_Information/FileNumbers/Tracker1FileNumber/@number"/>
                            <xsl:variable name="absorber0_file_number" select="MICE_Information/Other_Information/FileNumbers/Absorber0FileNumber/@number"/>                            
                            <xsl:variable name="absorber1_file_number" select="MICE_Information/Other_Information/FileNumbers/Absorber1FileNumber/@number"/>                            
                            <xsl:variable name="absorber2_file_number" select="MICE_Information/Other_Information/FileNumbers/Absorber2FileNumber/@number"/>                            
                     
                            <xsl:for-each select="structure/volume/physvol"> 
                                Module <xsl:choose><xsl:when test="contains(file/@name, $tof_0_file_number)">TOF/TOF0.dat</xsl:when>
                                                   <xsl:when test="contains(file/@name, $tof_1_file_number)">TOF/TOF1.dat</xsl:when>
                                                   <xsl:when test="contains(file/@name, $tof_2_file_number)">TOF/TOF2.dat</xsl:when>
                                                   <xsl:when test="contains(file/@name, $kl_file_number)">KL/KL.dat</xsl:when>
                                                   <xsl:when test="contains(file/@name, $ckov1_file_number)">Ckov/Vessel1.dat</xsl:when>
                                                   <xsl:when test="contains(file/@name, $ckov2_file_number)">Ckov/Vessel2.dat</xsl:when>
                                                   <xsl:when test="contains(file/@name, $emr_file_number)">EMR/Calorimeter.dat</xsl:when>
                                                   <xsl:when test="contains(file/@name, $tracker0_file_number)">Tracker/</xsl:when>
                                                   <xsl:when test="contains(file/@name, $tracker1_file_number)"><xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="$tracker1_file_replacement"/></xsl:when>
                                                   <xsl:when test="contains(file/@name, $absorber1_file_number)"><xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="$absorber_file_replacement"/></xsl:when>
                                                   <xsl:otherwise><xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="substring-before(file/@name, '.')"/>.dat</xsl:otherwise></xsl:choose>
                                    { 
                                        Position <xsl:value-of select="position/@x"/><xsl:text> </xsl:text><xsl:value-of select="position/@y"/><xsl:text> </xsl:text><xsl:value-of select="position/@z"/> mm 
	                                Rotation <xsl:choose><xsl:when test="rotationref/@ref = 'RotateY90'"> 0.0 90.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX90'"> 90.0 0.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX270'"> 270.0 0.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX180'"> 180.0 0.0 0.0 deg</xsl:when><xsl:when test="contains(rotation/@name, 'rotRef')"><xsl:value-of select="rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="rotation/@z"/> deg </xsl:when><xsl:otherwise> 0.0 0.0 0.0 deg</xsl:otherwise></xsl:choose> 
                                    }
                            </xsl:for-each> 
                            
                            <!--<xsl:choose><xsl:when test="not contains(structure/volume/physvol/file/@name"></xsl:when></xsl:choose>-->    
                                            //Other Modules
<!--
                    <xsl:variable name="isCool" select="'Cooling_Channel'"/>
                                    <xsl:choose><xsl:when test="not boolean(structure/volume/physvol/"></xsl:when></xsl:choose>
                           
                    
                            //<xsl:choose><xsl:when test="contains(MICE_Information/Configuration_Information/run, @diffuserThickness)">
                            //Diffuser thickness is <xsl:value-of select="MICE_Information/Configuration_Information/run/@diffuserThickness"/>
                            //Go and select the diffuser model
                            //</xsl:when></xsl:choose>
                
                            //<xsl:choose><xsl:when test="contains(MICE_Information/Configuration_Information/run, @protonAbsorberThickness)">
                            //Diffuser thickness is <xsl:value-of select="MICE_Information/Configuration_Information/run/@protonAbsorberThickness"/>
                            //Go and select the diffuser model
                            //</xsl:when></xsl:choose>
-->
                            // Fields
                <xsl:for-each select="MICE_Information/G4Field_Information/Dipole">
                            Module <xsl:value-of select="FieldName/@name"/>
                            {
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/> 
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                            ScaleFactor <xsl:choose>
                                <xsl:when test="contains(FieldName/@name, 'D1') and boolean($run_number)">0.71415/0.391*(3.58/1000.0+$D1Polarity*$D1Current*3.88/1000.0+$D1Current*$D1Current*2.619/1000.0/1000.0-$D1Polarity*$D1Current*$D1Current*$D1Current*1.55/1000.0/1000.0/1000.0)</xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'D2') and boolean($run_number)">0.71415/0.391*(3.58/1000.0+$D2Polarity*$D2Current*3.88/1000.0+$D2Current*$D2Current*2.619/1000.0/1000.0-$D2Polarity*$D2Current*$D2Current*$D2Current*1.55/1000.0/1000.0/1000.0)</xsl:when>
                                <xsl:otherwise><xsl:value-of select="ScaleFactor/@value"/></xsl:otherwise>
                            </xsl:choose>
                            Volume <xsl:value-of select="Volume/@name"/>
                            PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                            PropertyString FieldMapMode <xsl:value-of select="FieldMapMode/@name"/>
                            PropertyString FileType <xsl:value-of select="FileType/@name"/>
                            PropertyString FileName <xsl:value-of select="FileName/@name"/>
                            PropertyString Symmetry <xsl:value-of select="Symmetry/@name"/>
                            }
                </xsl:for-each>
                <xsl:for-each select="MICE_Information/G4Field_Information/Quadrupole">
			    <xsl:variable name="QuadName" select="FieldName/@name"/>
                            Module <xsl:value-of select="$QuadName"/>
                            {
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/> 
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                            ScaleFactor <xsl:value-of select="ScaleFactor/@value"/>
                            Volume <xsl:value-of select="Volume/@name"/>
                            PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                            PropertyDouble Height <xsl:value-of select="Dimensions/@height"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble Width <xsl:value-of select="Dimensions/@width"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble Length <xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>                     
                            PropertyDouble FieldStrength <xsl:choose>
                                <xsl:when test="contains($QuadName, 'Q1') and boolean($run_number)">$Q1Polarity*$Q1Current/96.0</xsl:when>
                                <xsl:when test="contains($QuadName, 'Q2') and boolean($run_number)">$Q2Polarity*$Q2Current/96.0</xsl:when>
                                <xsl:when test="contains($QuadName, 'Q3') and boolean($run_number)">$Q3Polarity*$Q3Current/96.0</xsl:when>
                                <xsl:when test="contains($QuadName, 'Q4') and boolean($run_number)">$Q4Polarity*$Q4Current/174.0</xsl:when>
                                <xsl:when test="contains($QuadName, 'Q5') and boolean($run_number)">$Q5Polarity*$Q5Current/174.0</xsl:when>
                                <xsl:when test="contains($QuadName, 'Q6') and boolean($run_number)">$Q6Polarity*$Q6Current/174.0</xsl:when>
                                <xsl:when test="contains($QuadName, 'Q7') and boolean($run_number)">$Q7Polarity*$Q7Current/174.0</xsl:when>
                                <xsl:when test="contains($QuadName, 'Q8') and boolean($run_number)">$Q8Polarity*$Q8Current/174.0</xsl:when>
                                <xsl:when test="contains($QuadName, 'Q9') and boolean($run_number)">$Q9Polarity*$Q9Current/174.0</xsl:when>
                                <xsl:otherwise><xsl:value-of select="FieldStrength/@name"/></xsl:otherwise>
                            </xsl:choose><xsl:text> </xsl:text><xsl:value-of select="FieldStrength/@units"/>
                            PropertyInt Pole <xsl:value-of select="Pole/@value"/>
                            PropertyInt MaxEndPole <xsl:value-of select="MaxEndPole/@value"/>
                            PropertyString EndFieldType <xsl:value-of select="EndFieldType/@name"/>
                            PropertyDouble EndLength <xsl:value-of select="EndLength/@value"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble CentreLength <xsl:value-of select="CentreLength/@value"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            }
                </xsl:for-each>
                <xsl:for-each select="MICE_Information/G4Field_Information/Solenoid">
                            Module <xsl:value-of select="FieldName/@name"/>
                            {
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
	                    Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                            Volume <xsl:value-of select="Volume/@name"/>
                            PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                            PropertyString FileName <xsl:value-of select="FileName/@name"/>
                            PropertyDouble CurrentDensity <xsl:value-of select="CurrentDensity/@value"/>
                            PropertyDouble Length <xsl:value-of select="Length/@value"/>
                            PropertyDouble Thickness <xsl:value-of select="Thickness/@value"/>
                            PropertyDouble InnerRadius <xsl:value-of select="InnerRadius/@value"/>
                            ScaleFactor <xsl:choose>
                            <xsl:when test="contains(FieldName/@name, 'MatchCoil1_0') and boolean($mcmode)">$SSUM1Polarity * $SSUM1</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'MatchCoil2_0') and boolean($mcmode)">$SSUM2Polarity * $SSUM2</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'CenterCoil_0') and boolean($mcmode)">$SSUCPolarity * $SSUC</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'EndCoil1_0') and boolean($mcmode)">$SSUT1Polarity * $SSUT1</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'EndCoil2_0') and boolean($mcmode)">$SSUT2Polarity * $SSUT2</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'FCoil_0') and boolean($mcmode)">$FCMUPolarity * $FCMU</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'FCoil_1') and boolean($mcmode)">$FCMUPolarity * $FCMMode * $FCMD</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'MatchCoil1_1') and boolean($mcmode)">$SSDM1Polarity * $FCMMode * $SSDM1</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'MatchCoil2_1') and boolean($mcmode)">$SSDM2Polarity * $FCMMode * $SSDM2</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'CenterCoil_1') and boolean($mcmode)">$SSDCPolarity * $FCMMode * $SSDC</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'EndCoil1_1') and boolean($mcmode)">$SSDT1Polarity * $FCMMode * $SSDT1</xsl:when>
                            <xsl:when test="contains(FieldName/@name, 'EndCoil2_1') and boolean($mcmode)">$SSDT2Polarity * $FCMMode * $SSDT2</xsl:when>
                            <xsl:otherwise><xsl:value-of select="ScaleFactor/@value"/></xsl:otherwise>
                            </xsl:choose>
                            }
                </xsl:for-each>
		}        
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>