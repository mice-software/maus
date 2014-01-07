<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    exclude-result-prefixes="xs"
    version="2.0">
    <xsl:output method="text"/>
    <xsl:template match="/Tracker_Information">
        
    <xsl:for-each select="TrackerSolenoid">
    <xsl:variable name="TrackerFileName" select="concat('TrackerSolenoid', @number, '.dat')"/>
        <xsl:value-of select="$TrackerFileName"/>
        <xsl:result-document href="{$TrackerFileName}" method="text">
            <html>
                <head>
                    <title>Module TrackerSolenoid<xsl:value-of select="@number"/>
                           {
                                Volume <xsl:value-of select="Volume/@name"/>
                                Dimensions <xsl:value-of select="Dimensions/@radius"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                                PropertyHep3Vector MagneticField <xsl:value-of select="MagneticField/@x"/><xsl:text> </xsl:text><xsl:value-of select="MagneticField/@y"/><xsl:text> </xsl:text><xsl:value-of select="MagneticField/@z"/><xsl:text> </xsl:text><xsl:value-of select="MagneticField/@units"/>
                                PropertyString Material <xsl:value-of select="Material/@name"/>
                                PropertyBool Invisible <xsl:value-of select="Invisible/@x"/>
                                PropertyInt KalamanSolenoid <xsl:value-of select="KalmanSolenoid/@x"/>
                                <xsl:text> 
                                </xsl:text>
                    </title>
                </head>
                <body>
                                Module Tracker/Crysotat<xsl:value-of select="Cryostat/@number"/>.dat
                                {
                                  Position <xsl:value-of select="Cryostat/Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Cryostat/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Cryostat/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Cryostat/Position/@units"/>
                                  Rotation <xsl:value-of select="Cryostat/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Cryostat/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Cryostat/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Cryostat/Rotation/@units"/>
                                  <xsl:text> </xsl:text>
                                }
                                
                                Module Tracker/Tracker<xsl:value-of select="Tracker/@number"/>.dat
                                {
                                  Position <xsl:value-of select="Tracker/Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Tracker/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Tracker/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Tracker/Position/@units"/>
                                  Rotation <xsl:value-of select="Tracker/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Tracker/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Tracker/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Tracker/Rotation/@units"/>
                                  <xsl:text> </xsl:text>
                                }
                                
                                Module Tracker/TrackerCoil.dat
                                {
                                  Position <xsl:value-of select="TrackerCoil/Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="TrackerCoil/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="TrackerCoil/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="TrackerCoil/Position/@units"/>
                                  Rotation <xsl:value-of select="TrackerCoil/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="TrackerCoil/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="TrackerCoil/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="TrackerCoil/Rotation/@units"/>
                                  <xsl:text> </xsl:text>
                                }
                           }
                </body>
            </html>
        </xsl:result-document>
    </xsl:for-each>
        
    <xsl:for-each select="TrackerSolenoid/Cryostat">
        <xsl:variable name="CrysotatFileName" select="concat('Cryostat',@number,'.dat')"/>
        <xsl:value-of select="$CrysotatFileName"/>
        <xsl:result-document href="{$CrysotatFileName}" method="text">
            <html>
                <head>
                    <title>Module Cryostat<xsl:value-of select="@number"/>
                           {
                                Volume <xsl:value-of select="Volume/@name"/>
                                Dimensions <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                                PropertyDouble BlueColour <xsl:value-of select="RGB_Values/@b"/>
                                PropertyDouble GreenColour <xsl:value-of select="RGB_Values/@g"/>
                                PropertyDouble RedColour <xsl:value-of select="RGB_Values/@r"/>
                                PropertyString Material <xsl:value-of select="Material/@name"/>
                                <xsl:text> 
                                </xsl:text>
                    </title>
                </head>
                <body>
                                Module Tracker/CryostatOuterVessel.dat
                                {
                                  Position <xsl:value-of select="CryostatOuterVessel/Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Position/@units"/>
                                  Rotation <xsl:value-of select="CryostatOuterVessel/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Rotation/@units"/>
                                }
                                
                                Module Tracker/CryostatInnerTube.dat
                                {
                                  Position <xsl:value-of select="CryostatInnerTube/Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Position/@units"/>
                                  Rotation <xsl:value-of select="CryostatInnerTube/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Rotation/@units"/>
                                }
                                
                                Module Tracker/CryostatFrontEndPlate.dat
                                {
                                  Position <xsl:value-of select="CryostatFrontEndPlate/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Position/@units"/>
                                  Rotation <xsl:value-of select="CryostatFrontEndPlate/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Rotation/@units"/>
                                }
                                
                                Module Tracker/CryostatBackEndPlate.dat
                                {
                                  Position <xsl:value-of select="CryostatBackEndPlate/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Position/@units"/>
                                  Rotation <xsl:value-of select="CryostatBackEndPlate/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Rotation/@units"/>
                                }
                                
                           }   
                </body>
            </html>
        </xsl:result-document>
    </xsl:for-each>
    
    <xsl:for-each select="TrackerSolenoid/Cryostat">
    <xsl:variable name="CrystatOuterVesselFile" select="concat('CryostatOuterVessel','.dat')"/>
    <xsl:value-of select="$CrystatOuterVesselFile"/>
    <xsl:if test="@to_print = 'Yes'">
    <xsl:result-document href="{$CrystatOuterVesselFile}" method="text">
        <html>
          <head>
          Module TrackerCryostatOuterVessel
          {
            Volume <xsl:value-of select="CryostatOuterVessel/Volume/@name"/>
            Dimensions <xsl:value-of select="CryostatOuterVessel/Dimensions/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Dimensions/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Dimensions/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatOuterVessel/Dimensions/@units"/>
            PropertyString Material <xsl:value-of select="CryostatOuterVessel/Material/@name"/>
            PropertyDouble BlueColour <xsl:value-of select="CryostatOuterVessel/RGB_Values/@b"/>
            PropertyDouble GreenColour <xsl:value-of select="CryostatOuterVessel/RGB_Values/@g"/>
            PropertyDouble RedColour <xsl:value-of select="CryostatOuterVessel/RGB_Values/@r"/>
           }
          </head>
        </html>
        
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>
    
    <xsl:for-each select="TrackerSolenoid/Cryostat">
    <xsl:variable name="CryostatInnerTubeFile" select="concat('CryostatInnerTube','.dat')"/>
    <xsl:value-of select="$CryostatInnerTubeFile"/>
    <xsl:if test="@to_print = 'Yes'">
    <xsl:result-document href="{$CryostatInnerTubeFile}" method="text">
        <html>
            <head>
            Module TrackerCryostatInnerTube
            {
              Volume <xsl:value-of select="CryostatInnerTube/Volume/@name"/>
              Dimensions <xsl:value-of select="CryostatInnerTube/Dimensions/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Dimensions/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Dimensions/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatInnerTube/Dimensions/@units"/>
              PropertyString Material <xsl:value-of select="CryostatInnerTube/Material/@name"/>
              PropertyDouble BlueColour <xsl:value-of select="CryostatInnerTube/RGB_Values/@b"/>
              PropertyDouble GreenColour <xsl:value-of select="CryostatInnerTube/RGB_Values/@g"/>
              PropertyDouble RedColour <xsl:value-of select="CryostatInnerTube/RGB_Values/@r"/>
            }
            </head>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>

    <xsl:for-each select="TrackerSolenoid/Cryostat">
    <xsl:variable name="CryostatFrontEndPlateFile" select="concat('CryostatFrontEndPlate','.dat')"/>
    <xsl:value-of select="$CryostatFrontEndPlateFile"/>
    <xsl:if test="@to_print = 'Yes'">
    <xsl:result-document href="{$CryostatFrontEndPlateFile}" method="text">
        <html>
            <head>
            Module TrackerCryostatFrontEndPlate
            {
              Volume <xsl:value-of select="CryostatFrontEndPlate/Volume/@name"/>
              Dimensions <xsl:value-of select="CryostatFrontEndPlate/Dimensions/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Dimensions/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Dimensions/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatFrontEndPlate/Dimensions/@units"/>
              PropertyString Material <xsl:value-of select="CryostatFrontEndPlate/Material/@name"/>
              PropertyDouble BlueColour <xsl:value-of select="CryostatFrontEndPlate/RGB_Values/@b"/>
              PropertyDouble GreenColour <xsl:value-of select="CryostatFrontEndPlate/RGB_Values/@g"/>
              PropertyDouble RedColour <xsl:value-of select="CryostatFrontEndPlate/RGB_Values/@r"/>
            }
            </head>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>

    <xsl:for-each select="TrackerSolenoid/Cryostat">
    <xsl:variable name="CryostatBackEndPlateFile" select="concat('CryostatBackEndPlate','.dat')"/>
    <xsl:value-of select="$CryostatBackEndPlateFile"/>
    <xsl:if test="@to_print = 'Yes'">
    <xsl:result-document href="{$CryostatBackEndPlateFile}" method="text">
        <html>
            <head>
            Module TrackerCryostatFrontEndPlate
            {
              Volume <xsl:value-of select="CryostatBackEndPlate/Volume/@name"/>
              Dimensions <xsl:value-of select="CryostatBackEndPlate/Dimensions/@x"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Dimensions/@y"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Dimensions/@z"/><xsl:text> </xsl:text><xsl:value-of select="CryostatBackEndPlate/Dimensions/@units"/>
              PropertyString Material <xsl:value-of select="CryostatBackEndPlate/Material/@name"/>
              PropertyDouble BlueColour <xsl:value-of select="CryostatBackEndPlate/RGB_Values/@b"/>
              PropertyDouble GreenColour <xsl:value-of select="CryostatBackEndPlate/RGB_Values/@g"/>
              PropertyDouble RedColour <xsl:value-of select="CryostatBackEndPlate/RGB_Values/@r"/>
            }
            </head>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>
        
    <xsl:for-each select="TrackerSolenoid/Tracker">
    <xsl:variable name="TrackerFileName" select="concat('Tracker',@number,'.dat')"/>
    <xsl:value-of select="$TrackerFileName"/>
    <xsl:result-document href="{$TrackerFileName}" method="text">
        <html>
            <head>Module Tracker<xsl:value-of select="@number"/>
                  {
                    Volume <xsl:value-of select="Volume/@name"/>
                    Dimensions <xsl:value-of select="Dimensions/@radius"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                    PropertyInt Tracker <xsl:value-of select="Tracker/@x"/>
                    PropertyBool Invisible <xsl:value-of select="Invisible/@x"/>
                    PropertyString Material <xsl:value-of select="Material/@name"/>
                    <xsl:text> 
                    </xsl:text>
            </head>
            <body>
                    Module Tracker/TrackerRef<xsl:value-of select="TrackerRef/@number"/>.dat
                    {
                      Position <xsl:value-of select="TrackerRef/Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="TrackerRef/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="TrackerRef/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="TrackerRef/Position/@units"/>
                      Rotation <xsl:value-of select="TrackerRef/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="TrackerRef/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="TrackerRef/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="TrackerRef/Rotation/@units"/>
                    }
                    <xsl:for-each select="TrackerStation">
                    Module Tracker/TrackerStation<xsl:value-of select="@number"/>.dat
                    {
                      Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                      Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                    }
                    </xsl:for-each>
                    
                    <xsl:if test="@number = '1'">
                    <xsl:for-each select="Tracker1Station">
                    Module Tracker/Tracker1Station<xsl:value-of select="@number"/>.dat
                    {
                      Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                      Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                    }
                    </xsl:for-each>                        
                    </xsl:if>
                  } 
            </body>
        </html>
    </xsl:result-document>
    </xsl:for-each>

    <xsl:for-each select="TrackerSolenoid/Tracker/TrackerRef">
    <xsl:variable name="TrackerRefFile" select="concat('TrackerRef',@number,'.dat')"/>
    <xsl:value-of select="$TrackerRefFile"/>
    <xsl:result-document href="{$TrackerRefFile}" method="text">
        <html>
            <head>
                Module TrackerRef <xsl:value-of select="@number"/>
                {
                  Volume <xsl:value-of select="Volume/@name"/>
                  Dimensions <xsl:value-of select="Dimensions/@radius"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                  PropertyInt RedColour <xsl:value-of select="RBG_Values/@r"/>
                  PropertyInt BlueColour <xsl:value-of select="RBG_Values/@b"/>
                  PropertyInt GreenColour <xsl:value-of select="RBG_Values/@g"/>
                  PropertyInt Station <xsl:value-of select="Station/@x"/>
                  PropertyInt Plane <xsl:value-of select="Plane/@x"/>
                  PropertyString SensitiveDetector <xsl:value-of select="SensitiveDetector/@name"/>
                  PropertyString Material <xsl:value-of select="Material/@name"/>
                }
            </head>
        </html>
    </xsl:result-document>
    </xsl:for-each>
        
    <xsl:for-each select="TrackerSolenoid/Tracker/TrackerStation">
    <xsl:variable name="TrackerStationFile" select="concat('TrackerStation',@number,'.dat')"/>
    <xsl:value-of select="$TrackerStationFile"/>
    <xsl:result-document href="{$TrackerStationFile}" method="text">
        <html>
            <head>
                Module TrackerStation<xsl:value-of select="@number"/>
                {
                  Volume <xsl:value-of select="Volume/@name"/>
                  Dimensions <xsl:value-of select="Dimensions/@radius"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                  PropertyDouble GreenColour <xsl:value-of select="RBG_Values/@g"/>
                  PropertyDouble RedColour <xsl:value-of select="RBG_Values/@r"/>
                  PropertyInt Station <xsl:value-of select="Station/@x"/>
                  PropertyString Material <xsl:value-of select="Material/@name"/>
                  PropertyDouble OpticsMaterialLength <xsl:value-of select="OpticsMaterialLength/@x"/><xsl:text> </xsl:text><xsl:value-of select="OpticsMaterialLength/@units"/>
                  <xsl:text>
                  </xsl:text>
            </head>
            <body>
                  <xsl:for-each select="TrackerView">
                  Module Tracker/TrackerView<xsl:value-of select="@letter"/>.dat
                  {
                    Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                    Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                  }    
                  </xsl:for-each>
                
                  <xsl:for-each select="TrackerMylar">
                  Module Tracker/TrackerMylar.dat
                  {
                    Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                    Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                  } 
                  </xsl:for-each>
                }   
            </body>
        </html>
    </xsl:result-document>
    </xsl:for-each>    

    <xsl:for-each select="TrackerSolenoid/Tracker/TrackerStation/TrackerView">
    <xsl:if test="@to_print = 'Yes'">
    <xsl:variable name="TrackerViewFile" select="concat('TrackerView',@letter,'.dat')"/>
    <xsl:value-of select="$TrackerViewFile"/>
    <xsl:result-document href="{$TrackerViewFile}" method="text">
        <html>
            <head>
                Module TrackerView<xsl:value-of select="@letter"/>
                {
                  Volume <xsl:value-of select="Volume/@name"/>
                  Dimensions <xsl:value-of select="Dimensions/@radius"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                  PropertyString Material <xsl:value-of select="Material/@name"/>
                  PropertyString SensitiveDetector <xsl:value-of select="SensitiveDectector/@name"/>
                  PropertyInt Plane <xsl:value-of select="Plane/@x"/>
                  PropertyDouble ActiveRadius <xsl:value-of select="ActiveRadius/@x"/><xsl:text> </xsl:text><xsl:value-of select="ActiveRadius/@units"/>
                  PropertyDouble Pitch <xsl:value-of select="Pitch/@x"/><xsl:text> </xsl:text><xsl:value-of select="Pitch/@units"/>
                  PropertyDouble FibreDiameter <xsl:value-of select="FibreDiameter/@x"/><xsl:text> </xsl:text><xsl:value-of select="FibreDiameter/@units"/>
                  PropertyDouble CoreDiameter <xsl:value-of select="CoreDiameter/@x"/><xsl:text> </xsl:text><xsl:value-of select="CoreDiameter/@units"/>
                  PropertyDouble CentralFibre <xsl:value-of select="CentralFibre/@x"/>
                  PropertyBool Invisible <xsl:value-of select="Invisible/@x"/>
                  PropertyString G4Detector <xsl:value-of select="G4Detector/@name"/>
                  PropertyDouble G4StepMax <xsl:value-of select="G4StepMax/@x"/><xsl:text> </xsl:text><xsl:value-of select="G4StepMax/@units"/>
                }
            </head>
            <body></body>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>
    
    <xsl:for-each select="TrackerSolenoid/Tracker/TrackerStation/TrackerMylar">
    <xsl:if test="@to_print = 'Yes'">
    <xsl:variable name="TrackerMylarFile" select="concat('TrackerMylar','.dat')"/>
    <xsl:value-of select="$TrackerMylarFile"/>
    <xsl:result-document href="{$TrackerMylarFile}" method="text">
        <html>
            <head>
                Module TrackerMylar
                {
                  Volume <xsl:value-of select="Volume/@name"/>
                  Dimensions <xsl:value-of select="Dimensions/@radius"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@length"/><xsl:text> </xsl:text><xsl:value-of select="Dimensions/@units"/>
                  PropertySting Material <xsl:value-of select="Material/@name"/>
                  PropertyDouble RedColour <xsl:value-of select="RBG_Values/@r"/>
                }
            </head>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>
        
    <xsl:for-each select="TrackerSolenoid/TrackerCoil">
    <xsl:if test="@to_print = 'Yes'">
    <xsl:variable name="TrackerCoilFile" select="concat('TrackerCoil','.dat')"/>
    <xsl:value-of select="$TrackerCoilFile"/>
    <xsl:result-document href="{$TrackerCoilFile}" method="text">
        <html>
            <head>
                Module TrackerCoil
                {
                  Volume <xsl:value-of select="Volume/@name"/>
                  Dimensions <xsl:value-of select="Dimension/@x"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@y"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@z"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@units"/>
                  <xsl:text>
                  </xsl:text>
            </head>
            <body>
                  <xsl:for-each select="MatchCoil">
                  Module Tracker/MatchCoil<xsl:value-of select="@number"/>.dat
                  {
                    Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                    Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                    ScaleFactor
                  }
                  </xsl:for-each>
               
                  <xsl:for-each select="EndCoil">
                  Module Tracker/EndCoil<xsl:value-of select="@number"/>.dat
                  {
                    Position <xsl:value-of select="Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="Position/@units"/>
                    Rotation <xsl:value-of select="Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="Rotation/@units"/>
                    ScaleFactor
                  }
                  </xsl:for-each>
                 Module Tracker/CentreCoil.dat
                 {
                   Position <xsl:value-of select="CenterCoil/Position/@x"/><xsl:text> </xsl:text><xsl:value-of select="CenterCoil/Position/@y"/><xsl:text> </xsl:text><xsl:value-of select="CenterCoil/Position/@z"/><xsl:text> </xsl:text><xsl:value-of select="CenterCoil/Position/@units"/>
                   Rotation <xsl:value-of select="CenterCoil/Rotation/@x"/><xsl:text> </xsl:text><xsl:value-of select="CenterCoil/Rotation/@y"/><xsl:text> </xsl:text><xsl:value-of select="CenterCoil/Rotation/@z"/><xsl:text> </xsl:text><xsl:value-of select="CenterCoil/Rotation/@units"/>
                   ScaleFactor
                 }
            </body>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>
    
    <xsl:for-each select="TrackerSolenoid/TrackerCoil/MatchCoil">
    <xsl:if test="@to_print = 'Yes'">
    <xsl:variable name="MatchCoilFile" select="concat('MatchCoil',@number,'.dat')"/>
    <xsl:value-of select="$MatchCoilFile"/>
    <xsl:result-document href="{$MatchCoilFile}" method="text">
        <html>
            <head>
                Module MatchCoil<xsl:value-of select="@number"/>
                {
                  Volume <xsl:value-of select="Volume/@name"/>
                  Dimensions <xsl:value-of select="Dimension/@x"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@y"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@z"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@units"/>
                  PropertyDouble RedColour <xsl:value-of select="RGBColor/@r"/>
                  PropertyString Material <xsl:value-of select="Material/@name"/>
                  
                  //Field
                  PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                  PropertyString FileName <xsl:value-of select="FileName/@name"/>
                  PropertyDouble CurrentDensity <xsl:value-of select="CurrentDensity/@x"/>
                  PropertyDouble Length <xsl:value-of select="Length/@x"/>
                  PropertyDouble Thickness <xsl:value-of select="Thickness/@x"/>
                  PropertyDouble InnerRadius <xsl:value-of select="InnerRadius/@x"/>
                  PropertyDouble FieldTolerance <xsl:value-of select="FieldTolerance/@x"/><xsl:text> </xsl:text><xsl:value-of select="FieldTolerance/@units"/>
                }
            </head>
            <body></body>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>
        
    <xsl:for-each select="TrackerSolenoid/TrackerCoil/EndCoil">
    <xsl:if test="@to_print = 'Yes'">
    <xsl:variable name="EndCoilFile" select="concat('EndCoil',@number,'.dat')"/>
    <xsl:value-of select="$EndCoilFile"/>
    <xsl:result-document href="{$EndCoilFile}" method="text">
        <html>
            <head>
                Module EndCoil<xsl:value-of select="@number"/>
                {
                  Volume <xsl:value-of select="Volume/@name"/>
                  Dimensions <xsl:value-of select="Dimension/@x"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@y"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@z"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@units"/>
                  PropertyDouble RedColour <xsl:value-of select="RGBColor/@r"/>
                  PropertyString Material <xsl:value-of select="Material/@name"/>
                  
                  //Field
                  PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                  PropertyString FileName <xsl:value-of select="FileName/@name"/>
                  PropertyDouble CurrentDensity <xsl:value-of select="CurrentDensity/@x"/>
                  PropertyDouble Length <xsl:value-of select="Length/@x"/>
                  PropertyDouble Thickness <xsl:value-of select="Thickness/@x"/>
                  PropertyDouble InnerRadius <xsl:value-of select="InnerRadius/@x"/>
                  PropertyDouble FieldTolerance <xsl:value-of select="FieldTolerance/@x"/><xsl:text> </xsl:text><xsl:value-of select="FieldTolerance/@units"/>
                }
            </head>
            <body></body>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>    

    <xsl:for-each select="TrackerSolenoid/TrackerCoil/CenterCoil">
    <xsl:if test="@to_print = 'Yes'">
    <xsl:variable name="CenterCoilFile" select="concat('CenterCoil',@number,'.dat')"/>
    <xsl:value-of select="$CenterCoilFile"/>
    <xsl:result-document href="{$CenterCoilFile}" method="text">
        <html>
            <head>
                Module CenterCoil<xsl:value-of select="@number"/>
                {
                  Volume <xsl:value-of select="Volume/@name"/>
                  Dimensions <xsl:value-of select="Dimension/@x"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@y"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@z"/><xsl:text> </xsl:text><xsl:value-of select="Dimension/@units"/>
                  PropertyDouble RedColour <xsl:value-of select="RGBColor/@r"/>
                  PropertyString Material <xsl:value-of select="Material/@name"/>
                  
                  //Field
                  PropertyString FieldType <xsl:value-of select="FieldType/@name"/>
                  PropertyString FileName <xsl:value-of select="FileName/@name"/>
                  PropertyDouble CurrentDensity <xsl:value-of select="CurrentDensity/@x"/>
                  PropertyDouble Length <xsl:value-of select="Length/@x"/>
                  PropertyDouble Thickness <xsl:value-of select="Thickness/@x"/>
                  PropertyDouble InnerRadius <xsl:value-of select="InnerRadius/@x"/>
                  PropertyDouble FieldTolerance <xsl:value-of select="FieldTolerance/@x"/><xsl:text> </xsl:text><xsl:value-of select="FieldTolerance/@units"/>
                }
            </head>
            <body></body>
        </html>
    </xsl:result-document>
    </xsl:if>
    </xsl:for-each>
    
  </xsl:template>
</xsl:stylesheet>