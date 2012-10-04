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
                <title>Configuration RotatedGeometry 
                       { 
                           Volume Box
                           Dimensions 50000.0 50000.0 50000.0 mm
                           PropertyString Material <xsl:if test="structure/volume/materialref/@ref = 'Vacuum'">AIR</xsl:if>  
                           PropertyDouble G4StepMax <xsl:value-of select="MICE_Information/Other_Information/G4StepMax/@Value"/> mm
                           PropertyBool Invisible 1
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
                       
                            Module Tracker/TrackerSolenoid0.dat
                            {
                                Position 0. 0. 15016.24 mm
                                Rotation 0. 0. 0. degree
                            }
                       
                            Module AFC/AbsorberFocusCoil.dat
                            {
                                Position 0. 0. 16955.74 mm
                                Rotation 0. 0. 0. degree
                            }
                       
                            Module RFCC/RFCouplingCoilUp.dat
                            {
                                Position 0. 0. 18895.24 mm
                                Rotation 0. 0. 0. degree
                                ScaleFactor -1
                            }
                       
                       }
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>
