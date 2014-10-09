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
                           Dimensions <xsl:if test="solids/box/@name = 'ChannelBoxRef'"><xsl:value-of select="solids/box/@x"/><xsl:text> </xsl:text><xsl:value-of select="solids/box/@y"/><xsl:text> </xsl:text><xsl:value-of select="solids/box/@z"/><xsl:text> mm</xsl:text></xsl:if>
                           PropertyString Material Galactic <!--<xsl:if test="structure/volume/materialref/@ref = 'Vacuum'">Galactic</xsl:if>-->  
                           PropertyDouble G4StepMax <xsl:value-of select="MICE_Information/Other_Information/G4StepMax/@Value"/> mm 
                </title>
            </head>
            <body>  
                <xsl:variable name="run_number" select="MICE_Information/Configuration_Information/run/@runNumber"/>
                <xsl:variable name="diffuserThickness" select="MICE_Information/Configuration_Information/run/@diffuserThickness"/>
                Substitution $beamStop <xsl:value-of select="MICE_Information/Configuration_Information/run/@beamStop"/>
                
                Substitution $diffuserThickness <xsl:value-of select="MICE_Information/Configuration_Information/run/@diffuserThickness"/>
                Substitution $protonAbsorberThickness <xsl:value-of select="MICE_Information/Configuration_Information/run/@beamStop"/><xsl:text>

                Module Virtuals
                                {
                                        Volume None
                                        Position 0.0 0.0 0.0+0.1*@RepeatNumber m
                                        PropertyString SensitiveDetector Virtual
                                        PropertyBool RepeatModule2 True
                                        PropertyInt NumberOfRepeats 70
                                }



                            </xsl:text>
                <xsl:for-each select="structure/volume/physvol"> 
                          Module <xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/><xsl:value-of select="substring-before(file/@name, '.')"/>.dat
                                  { 
                                    Position <xsl:value-of select="position/@x"/><xsl:text> </xsl:text><xsl:value-of select="position/@y"/><xsl:text> </xsl:text><xsl:value-of select="position/@z"/> mm 
	                            Rotation <xsl:choose><xsl:when test="rotationref/@ref = 'RotateY90'"> 0.0 90.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX90'"> 90.0 0.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX270'"> 270.0 0.0 0.0 deg</xsl:when><xsl:when test="rotationref/@ref = 'RotateX180'"> 180.0 0.0 0.0 deg</xsl:when><xsl:when test="contains(rotation/@name, 'rotRef')"><xsl:value-of select="rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="rotation/@z"/> deg </xsl:when><xsl:otherwise> 0.0 0.0 0.0 deg</xsl:otherwise></xsl:choose> 
                                   }
                            </xsl:for-each> 
			<xsl:for-each select="MICE_Information/Detector_Information/Diffuser">
		                  Module <xsl:value-of select="ancestor::gdml/MICE_Information/Other_Information/GDML_Files/@location"/>iris<xsl:value-of select="Iris/@name"/><xsl:choose>
			                     <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'0000')">_open.dat</xsl:when> 
			                     <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'0010')">_open.dat</xsl:when> 
			                     <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'0100')">_open.dat</xsl:when> 
			                     <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'0110')">_open.dat</xsl:when> 
			                     <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'1000')">_open.dat</xsl:when> 
			                     <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'1010')">_open.dat</xsl:when> 
			                     <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'1100')">_open.dat</xsl:when> 
			                     <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'1110')">_open.dat</xsl:when>
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'0001')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'0011')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'0101')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'0111')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'1001')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'1011')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'1101')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'1') and contains($diffuserThickness,'1111')">_closed.dat</xsl:when>
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'0000')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'0100')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'1000')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'1100')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'0001')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'0101')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'1001')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'1101')">_open.dat</xsl:when>
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'0010')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'0110')">_closed.dat</xsl:when> 
 			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'1010')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'1110')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'0011')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'0111')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'1011')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'2') and contains($diffuserThickness,'1111')">_closed.dat</xsl:when>
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'0000')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'0001')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'0010')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'0011')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'1000')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'1001')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'1010')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'1011')">_open.dat</xsl:when>
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'0100')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'0101')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'0110')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'0111')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'1100')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'1101')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'1111')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'3') and contains($diffuserThickness,'1111')">_closed.dat</xsl:when>
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'0000')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'0001')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'0010')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'0011')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'0100')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'0101')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'0110')">_open.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'0111')">_open.dat</xsl:when>
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'1000')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'1001')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'1010')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'1011')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'1100')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'1101')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'1110')">_closed.dat</xsl:when> 
			        <xsl:when test="boolean($run_number) and contains(Iris/@name,'4') and contains($diffuserThickness,'1111')">_closed.dat</xsl:when>
			        <xsl:otherwise>_open.dat</xsl:otherwise>
			     </xsl:choose>
        			                {
		          	                Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/> 
			                        Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
			                        }
               		</xsl:for-each>
                         


		}        
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>