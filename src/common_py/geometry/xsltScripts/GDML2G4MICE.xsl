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
    <xsl:output method="text"/>
    <xsl:template match="gdml">
        <html>
            <head>
                <title>Configuration <xsl:value-of select="structure/volume/@name"/> 
                       { 
                           Dimensions <xsl:if test="solids/sphere/@name = 'WorldSphereRef'">15000.0 10000.0 50000.0 mm</xsl:if>
                           PropertyString Material <xsl:if test="structure/volume/materialref/@ref = 'Vacuum'">AIR</xsl:if>  
                           PropertyDouble G4StepMax <xsl:value-of select="MICE_Information/Other_Information/G4StepMax/@Value"/> mm 
                </title>
            </head>
            <body>
                <xsl:for-each select="structure/volume/physvol"> 
                    Module <xsl:choose><xsl:when test="contains(file/@name, '9334')">TOF/TOF0.dat</xsl:when><xsl:when test="contains(file/@name, '9336')">Ckov/Cherenkov.dat</xsl:when><xsl:when test="contains(file/@name, 'SingleStation')">Tracker/Tracker1Station1.dat</xsl:when><xsl:otherwise><xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="substring-before(file/@name, '.')"/>.dat</xsl:otherwise></xsl:choose>
                            { 
                                Position <xsl:value-of select="position/@x"/><xsl:text> </xsl:text><xsl:value-of select="position/@y"/><xsl:text> </xsl:text><xsl:value-of select="position/@z"/> mm 
                                Rotation <xsl:choose><xsl:when test="rotationref/@ref = 'RotateY90'"> 0.0 90.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX90'"> 90.0 0.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX270'"> 270.0 0.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX180'"> 180.0 0.0 0.0 deg</xsl:when><xsl:otherwise> 0.0 0.0 0.0 deg</xsl:otherwise></xsl:choose> 
                            }
                </xsl:for-each>
                            // Fields
                <xsl:for-each select="MICE_Information/G4Field_Information/Dipole">
                            Module <xsl:value-of select="FieldName/@name"/>
                            {
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/> mm
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/> mm
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
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/> mm
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/> mm                            
                            Volume <xsl:value-of select="Volume/@name"/>
                            PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                            PropertyDouble Height <xsl:value-of select="Dimensions/@height"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble Width <xsl:value-of select="Dimensions/@width"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble Length <xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                            PropertyDouble FieldStrength <xsl:value-of select="FieldStrength/@Value"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
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
                            Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/> mm
                            Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/> mm                            
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