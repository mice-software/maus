<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xd="http://www.oxygenxml.com/ns/doc/xsl" exclude-result-prefixes="xd" version="1.0">
    <xd:doc scope="stylesheet">
        <xd:desc>
            <xd:p><xd:b>Created on:</xd:b> Jan 20, 2011</xd:p>
            <xd:p><xd:b>Author:</xd:b> Matt</xd:p>
            <xd:p/>
        </xd:desc>
    </xd:doc>

    <xsl:output method="text"/>
    <xsl:template match="gdml">
        <html>
            <head>Module <xsl:value-of select="structure/volume/@name"/> 
            </head>
            <body>
                <text>
                {</text>
                Volume TessellatedSolid
                PropertyString Material Al <!--<xsl:if test="structure/volume/materialref/@ref = 'ALUMINUM'">Al</xsl:if>-->
                PropertyDouble BlueColour 0.75
                PropertyDouble GreenColour 0.3
                PropertyDouble RedColour 0.75
                PropertyInt noOfVertices <xsl:value-of select="count(define/position)"/>
                PropertyInt noOfTFacets <xsl:value-of select="count(solids/tessellated/triangular)"/>
                PropertyInt noOfQFacets <xsl:value-of select="count(solids/tessellated/quadrangular)"/><xsl:text>
                    
                </xsl:text>                                
                <xsl:for-each select="define/position">
                PropertyHep3Vector Vector<xsl:number/><xsl:text> </xsl:text><xsl:value-of select="@x"/><xsl:text> </xsl:text><xsl:value-of select="@y"/><xsl:text> </xsl:text><xsl:value-of select="@z"/>
                </xsl:for-each><xsl:text>
                    
                </xsl:text>              
                <xsl:for-each select="solids/tessellated/triangular">
                PropertyString TFacet<xsl:number/><xsl:text> </xsl:text><xsl:value-of select="translate(@vertex1, 'v', '')"/><xsl:text> </xsl:text><xsl:value-of select="translate(@vertex2, 'v', '')"/><xsl:text> </xsl:text><xsl:value-of select="translate(@vertex3, 'v', '')"/>
                </xsl:for-each><xsl:text>
                    
                </xsl:text>
                <xsl:for-each select="solids/tessellated/quadrangular">
                PropertyString QFacet<xsl:number/><xsl:text> </xsl:text><xsl:value-of select="translate(@vertex1, 'v', '')"/><xsl:text> </xsl:text><xsl:value-of select="translate(@vertex2, 'v', '')"/><xsl:text> </xsl:text><xsl:value-of select="translate(@vertex3, 'v', '')"/><xsl:text> </xsl:text><xsl:value-of select="translate(@vertex4, 'v', '')"/>
                </xsl:for-each>
		        }
            </body>            
        </html>
    </xsl:template>
</xsl:stylesheet>
