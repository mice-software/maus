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
         and selects certain node and writes the .dat mice module in the correct format.
    -->
    <xsl:output method="text"/>
    <xsl:template match="gdml">


        <html>
            <head>
                <title>Configuration Structure
                       { 
                           Dimensions 15000.0 10000.0 50000.0 mm
                           PropertyString Material AIR 
                           PropertyDouble G4StepMax <xsl:value-of select="MICE_Information/Other_Information/G4StepMax/@Value"/> mm
                           PropertyString GDMLFile <xsl:value-of select="MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="MICE_Information/Other_Information/GDML_Files/@basefile"/>
                </title>
            </head>
            <body>  
	

                            //Substitutions
                            <xsl:variable name="run_number" select="MICE_Information/Configuration_Information/run/@runNumber"/>
		            <xsl:variable name="diffuserThickness" select="MICE_Information/Configuration_Information/run/@diffuserThickness"/>
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
                            <xsl:when test="contains(@name, 'DS')">Substitution $DSCurrent <xsl:value-of select="@setCurrent"/>
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
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'SSU')">Substitution $SSUCurrent <xsl:value-of select="@setCurrent"/>
			    Substitution $SSUPolarity <xsl:value-of select="@polarity"/><xsl:text>

                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'DDS')">Substitution $DDSCurrent <xsl:value-of select="@setCurrent"/>
			    Substitution $DDSPolarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'FCU')">Substitution $FCUCurrent <xsl:value-of select="@setCurrent"/>
			    Substitution $FCUPolarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'CCU')">Substitution $CCUCurrent <xsl:value-of select="@setCurrent"/>
			    Substitution $CCUPolarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'FCM')">Substitution $FCMCurrent <xsl:value-of select="@setCurrent"/>
			    Substitution $FCMPolarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'CCD')">Substitution $CCDCurrent <xsl:value-of select="@setCurrent"/>
			    Substitution $CCDPolarity <xsl:value-of select="@polarity"/><xsl:text>
                                
                            </xsl:text></xsl:when>
                            <xsl:when test="contains(@name, 'FCD')">Substitution $FCDCurrent <xsl:value-of select="@setCurrent"/>
			    Substitution $FCDPolarity <xsl:value-of select="@polarity"/><xsl:text>
                                
			    </xsl:text></xsl:when></xsl:choose>
                            </xsl:for-each></xsl:when><xsl:otherwise> </xsl:otherwise></xsl:choose>
                            
                            
  	         	            Module Virtuals
                            {
                               Volume None
                               Position 0.0 0.0 0.0+0.1*@RepeatNumber m
                               PropertyString SensitiveDetector Virtual
                               PropertyBool RepeatModule2 True
                               PropertyInt NumberOfRepeats 240
                            }
                            
                            // Detectors
                            <xsl:for-each select="MICE_Information/Detector_Information/*/*/physvol">
                            Module <xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="substring-before(file/@name, '.')"/>.dat    
                                    { 
                                    Position <xsl:value-of select="position/@x"/><xsl:text> </xsl:text><xsl:value-of select="position/@y"/><xsl:text> </xsl:text><xsl:value-of select="position/@z"/> mm 
	                                Rotation <xsl:choose><xsl:when test="rotationref/@ref = 'RotateY90'"> 0.0 90.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX90'"> 90.0 0.0 0.0 deg</xsl:when><xsl:when test="physvol/rotationref/@ref = 'RotateX270'"> 270.0 0.0 0.0 deg</xsl:when><xsl:when test="physvol/rotationref/@ref = 'RotateX180'"> 180.0 0.0 0.0 deg</xsl:when><xsl:when test="contains(rotation/@name, 'rotRef')"><xsl:value-of select="rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="rotation/@z"/> deg </xsl:when><xsl:otherwise> 0.0 0.0 0.0 deg</xsl:otherwise></xsl:choose> 
                                    }
                            </xsl:for-each>
                            <xsl:for-each select="MICE_Information/Detector_Information/*/physvol">
                            Module <xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="substring-before(file/@name, '.')"/>.dat    
                                    { 
                                    Position <xsl:value-of select="position/@x"/><xsl:text> </xsl:text><xsl:value-of select="position/@y"/><xsl:text> </xsl:text><xsl:value-of select="position/@z"/> mm 
                                    Rotation <xsl:choose><xsl:when test="rotationref/@ref = 'RotateY90'"> 0.0 90.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX90'"> 90.0 0.0 0.0 deg</xsl:when><xsl:when test="physvol/rotationref/@ref = 'RotateX270'"> 270.0 0.0 0.0 deg</xsl:when><xsl:when test="physvol/rotationref/@ref = 'RotateX180'"> 180.0 0.0 0.0 deg</xsl:when><xsl:when test="contains(rotation/@name, 'rotRef')"><xsl:value-of select="rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="rotation/@z"/> deg </xsl:when><xsl:otherwise> 0.0 0.0 0.0 deg</xsl:otherwise></xsl:choose> 
                            }
                            </xsl:for-each>
                
                            
                
                             
                            // Fields
                <xsl:for-each select="MICE_Information/G4Field_Information/Dipole">
                            Module <xsl:value-of select="FieldName/@name"/>
                            {
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/> 
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                            ScaleFactor <xsl:choose>
                                <xsl:when test="contains(FieldName/@name, 'D1') and boolean($run_number)">0.71415/0.391*(3.58/1000.0+$D1Polarity*$D1Current*3.88/1000.0+$D1Current*$D1Current*2.619/1000.0/1000.0-$D1Polarity*$D1Current*$D1Current*$D1Current*1.55/1000.0/1000.0/1000.0)</xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'D2') and boolean($run_number)">0.71415/0.391*(3.58/1000.0+$D1Polarity*$D1Current*3.88/1000.0+$D1Current*$D1Current*2.619/1000.0/1000.0-$D1Polarity*$D1Current*$D1Current*$D1Current*1.55/1000.0/1000.0/1000.0)</xsl:when>
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
                                <xsl:otherwise><xsl:value-of select="FieldStrength/@value"/></xsl:otherwise>
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
                                <xsl:when test="contains(FieldName/@name, 'Match') and contains(FieldName/@name, '_0') and boolean($run_number)">$SSUPolarity * $SSUCurrent/281 *<xsl:value-of select="translate(ScaleFactor/@name,'-','')"/></xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'Center') and contains(FieldName/@name, '_0') and boolean($run_number)">$SSUPolarity * $SSUCurrent/281 *<xsl:value-of select="translate(ScaleFactor/@name,'-','')"/></xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'EndCoil') and contains(FieldName/@name, '_0') and boolean($run_number)">$SSUPolarity * $SSUCurrent/281 *<xsl:value-of select="translate(ScaleFactor/@name,'-','')"/></xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'FCoil') and contains(FieldName/@name, '_0') and boolean($run_number)">$SSUPolarity * $FCMCurrent/180 *<xsl:value-of select="translate(ScaleFactor/@name,'-','')"/></xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'Match') and contains(FieldName/@name, '_1') and boolean($run_number)">$DDSPolarity * $DDSCurrent/281 *<xsl:value-of select="translate(ScaleFactor/@name,'-','')"/></xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'Center') and contains(FieldName/@name, '_1') and boolean($run_number)">$DDSPolarity * $DDSCurrent/281 *<xsl:value-of select="translate(ScaleFactor/@name,'-','')"/></xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'EndCoil') and contains(FieldName/@name, '_1') and boolean($run_number)">$DDSPolarity * $DDSCurrent/281 *<xsl:value-of select="translate(ScaleFactor/@name,'-','')"/></xsl:when>
                                <xsl:when test="contains(FieldName/@name, 'FCoil') and contains(FieldName/@name, '_1') and boolean($run_number)">$DDSPolarity * $FCMCurrent/180 *<xsl:value-of select="translate(ScaleFactor/@name,'-','')"/></xsl:when>
                                <xsl:otherwise><xsl:value-of select="ScaleFactor/@name"/></xsl:otherwise>
                            </xsl:choose>
                            }
                </xsl:for-each>
		}        
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>