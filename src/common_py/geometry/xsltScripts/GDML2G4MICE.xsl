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
                Dimensions 15000.0 10000.0 50000.0 mm <!-- <xsl:if test="solids/sphere/@name, WorldSphereRef">15000.0 10000.0 50000.0 mm</xsl:if> --> 
                PropertyString Material AIR
                PropertyDouble G4StepMax 5.0 mm
                </title>
            </head>
            <body>
                <xsl:for-each select="structure/volume/physvol">
                Module /home/matt/maus-littlefield/src/common_py/geometry/Download/<xsl:value-of select="substring-before(file/@name, '.')"/>.dat
                {
                Position <xsl:value-of select="position/@x"/><xsl:text> </xsl:text><xsl:value-of select="position/@y"/><xsl:text> </xsl:text><xsl:value-of select="position/@z"/> mm
                Rotation <xsl:if test="rotationref/@ref = 'identity'"> 0.0 0.0 0.0 deg</xsl:if>    
                }
                </xsl:for-each>
                }
                
                //<xsl:value-of select="MICE_Information"/>
                
            </body>
        </html>
    </xsl:template>
    
</xsl:stylesheet>

