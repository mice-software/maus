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
    <xsl:param name="file_path">__file_path__</xsl:param>
    <xsl:param name="step_max">__step_max__</xsl:param>
    <xsl:output method="text"/>
    <xsl:template match="gdml">
        <html>
            <head>
                <title>Configuration <xsl:value-of select="structure/volume/@name"/> 
                       { 
                           Dimensions <xsl:if test="solids/sphere/@name = 'WorldSphereRef'">15000.0 10000.0 50000.0 mm</xsl:if>
                           PropertyString Material <xsl:if test="structure/volume/materialref/@ref = 'Vacuum'">AIR</xsl:if>  
                           PropertyDouble G4StepMax <xsl:value-of select="$step_max"/> mm 
                </title>
            </head>
            <body>
                <xsl:for-each select="structure/volume/physvol"> 
                        Module <xsl:value-of select="$file_path"/>/<xsl:value-of select="substring-before(file/@name, '.')"/>.dat 
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
