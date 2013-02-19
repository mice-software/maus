import sys
import math

NumberOfLayers = 48
BarsPerLayer = 59
ExportFolder = "EMR/"
WithBars = 1

class G4MiceGeoBase:
    def __init__( self, Name ):#pylint:disable=C0103
        self.Name = Name
        self.Height = 0.0
        self.Length = 0.0
        self.Width = 0.0
        self.X = 0.0
        self.Y = 0.0
        self.Z = 0.0
        self.Phi = 0.0
        self.Theta = 0.0
        self.Psi = 0.0

    def SetDimensions( self, Length, Width, Height ):
        self.Length = Length
        self.Width = Width
        self.Height = Height

    def SetPosition( self, X, Y, Z ):
        self.X = X
        self.Y = Y
        self.Z = Z

    def SetRotation( self, Phi, Theta, Psi ):
        self.Phi = Phi
        self.Theta = Theta
        self.Psi = Psi

    def __str__( self  ):
        message = """Name: %s
          Dimensions: %d %d %d
          Position: %d %d %d
          Rotation:
        """% ( self.Name, self.Width, self.Height, self.Length, self.X, self.Y, self.Z )
        return message

class EMRBar( G4MiceGeoBase ):
    def __init__( self, LayerId, BarId, BarLength, BarHeight, BarWidth ):
        self.Name = "EMRLayer%dBar%d" % ( LayerId, BarId )
        self.BarId = BarId
        self.LayerId = LayerId
        self.Length = BarLength
        self.Width = BarWidth
        self.Height = BarHeight
        self.X = 0.0
        self.Y = 0.0
        self.Z = 0.0
        self.Phi = 0.0
        self.Theta = 0.0
        self.Psi = 0.0


    def Export( self ):

    #  if( self.BarId % 2 ):
    #    Y1HalfLength = 0
    #    Y2HalfLength = ( self.Width / 2 )
    #  else:
    #    Y1HalfLength = ( self.Width / 2 )
    #    Y2HalfLength = 0

    #  XHalfLength = ( self.Length / 2 )
    #  ZHalfLength = ( self.Height / 2 )

        Content = """
        // FILES/Models/Modules/%s%s.dat
        //
        Module %s
        {
          Volume Trapezoid
          Dimensions %.2f %.2f %.2f %.2f %.2f cm
          PropertyInt Layer %d
          PropertyInt Cell %d
          PropertyInt numPMTs 2
          PropertyDouble BlueColour 1.0
          Module EMR/%sSci.dat
          {
            Position 0.0 0.0 0.0 mm
            Rotation 0.0 0.0 0.0 degree
          }
        }
        """ % ( ExportFolder,
                self.Name,
                self.Name,
                self.Length / 2,
                self.Length / 2,
                0.,
                self.Width / 2,
                self.Height / 2,
                self.LayerId,
                self.BarId,
                self.Name )

        #% ( ExportFolder, self.Name, self.Name, XHalfLength, XHalfLength, Y1HalfLength,
        #Y2HalfLength, ZHalfLength, self.LayerId, self.BarId, self.Name )

        BarFile = open( ExportFolder + self.Name + ".dat", 'w' )
        BarFile.write( Content )
        BarFile.close()
        Content = """
        // FILES/Models/Modules/%s%sSci.dat
        //

        Module %sSci
        {
          Volume Trapezoid
          Dimensions %.2f %.2f %.2f %.2f %.2f cm
          PropertyInt Layer %d
          PropertyInt Cell %d
          PropertyString SensitiveDetector EMR
          PropertyString Material POLYSTYRENE
          PropertyDouble GreenColour 1.0
          PropertyDouble RedColour 1.0
        }
        """ % ( ExportFolder,
                self.Name,
                self.Name,
                self.Length / 2,
                self.Length / 2,
                0.,
                self.Width / 2,
                self.Height / 2,
                self.LayerId,
                self.BarId )
        #% ( ExportFolder, self.Name, self.Name, XHalfLength, XHalfLength,
        #Y1HalfLength, Y2HalfLength, ZHalfLength, self.LayerId, self.BarId )
        SciFile = open( ExportFolder + self.Name + "Sci.dat", 'w' )
        SciFile.write( Content )
        SciFile.close()

class EMRLayer( G4MiceGeoBase ):
    def __init__(self, LayerId, BarCount, BarLength, BarHeight, BarWidth ):
        self.Bars = []
        self.LayerId = LayerId
        self.BarCount = BarCount
        self.Name = "EMRLayer%d" % ( LayerId )
        self.BarHeight = BarHeight
        self.BarLength = BarLength
        self.BarWidth = BarWidth
        self.Length = self.BarLength
        self.Width = self.BarWidth * BarCount
        self.Height = self.BarHeight
        self.X = 0.0
        self.Y = 0.0
        self.Z = 0.0
        self.Phi = 0.0
        self.Theta = 0.0
        self.Psi = 0.0
        HalfLayerLength = ( ( self.BarWidth / 2 ) * (self.BarCount - 1) ) / 2
        if WithBars == 1 :
            for i in range( self.BarCount ):
                b = EMRBar( self.LayerId, i, self.BarLength, self.BarHeight, self.BarWidth )
                #if not( self.LayerId % 2 ):
                #  XPos = 0.0
                #  YPos = ( -HalfLayerLength + ( i * ( b.Width / 2 ) ) )
                #else:
                #  XPos = ( -HalfLayerLength + ( i * ( b.Width / 2 ) ) )
                #  YPos = 0.0
                #b.SetPosition( XPos, YPos, 0.0 )
                b.SetPosition( 0.0, ( -HalfLayerLength + ( i * ( b.Width / 2 ) ) ), 0.0 )
                if i % 2 == 0:
                    b.SetRotation( 0.0, 0.0, 0.0 )
                else:
                    b.SetRotation( 0.0, 180.0, 0.0 )
                self.Bars.insert( i, b )

    def IsVertical( self ):
        return self.LayerId % 2

    def Export( self ):
        #Calculating trapezoid halflengths
        #ZHL = ( self.BarHeight / 2 )
        #if not self.IsVertical():
        #  X1HL = self.BarLength / 2
        #  X2HL = self.BarLength / 2
        #  Y1HL = ( self.BarWidth * ( self.BarCount / 2 ) ) / 2
        #  Y2HL = ( self.BarWidth * ( ( self.BarCount / 2 ) + ( self.BarCount % 2 ) ) ) / 2
        #else:
        #  X1HL = ( self.BarWidth * ( self.BarCount / 2 ) ) / 2;
        #  X2HL = ( self.BarWidth * ( ( self.BarCount / 2 ) + ( self.BarCount % 2 ) ) ) / 2
        #  Y1HL = self.BarLength / 2
        #  Y2HL = self.BarLength / 2

        Content = """// FILES/Models/Modules/%s%s.dat
        //

          Module %s
          {
            Volume Trapezoid
            Dimensions  %.2f %.2f %.2f %.2f %.2f cm

            PropertyString Material Galactic

            PropertyInt Layer %d
            //  PropertyInt numSlabs %d
            PropertyInt numBars %d
            PropertyBool Invisible 1
          """% ( ExportFolder,
                 self.Name,
                 self.Name,
                 self.Length / 2,
                 self.Length / 2,
                 ( self.BarWidth * ( self.BarCount / 2 ) ) / 2,
                 ( self.BarWidth * ( ( self.BarCount / 2 ) + ( self.BarCount % 2 ) ) ) / 2,
                 self.Height / 2,
                 self.LayerId,
                 self.BarCount,
                 self.BarCount )
            #% ( ExportFolder, self.Name, self.Name, X1HL, X2HL, Y1HL, Y2HL, ZHL,
            #self.LayerId, self.BarCount, self.BarCount )
        if WithBars == 1:
            for bar in self.Bars:
                Content += """Module %s%s.dat
                {
                  Position %.2f %.2f %.2f cm
                  Rotation %.2f %.2f %.2f degree
                }
                """ % ( ExportFolder, bar.Name, bar.X, bar.Y, bar.Z, bar.Phi, bar.Theta, bar.Psi )
                bar.Export()
        Content += "}"
        LayerFile = open( ExportFolder + self.Name + ".dat", 'w' )
        LayerFile.write( Content )
        LayerFile.close()

class EMRGeometry( G4MiceGeoBase ):
    def __init__(self, NumLayers, NumBars, BarLength, BarHeight, BarWidth ):
        self.Layers = []
        self.Name = "Calorimeter"

        self.LayersCount = NumLayers
        self.BarsCount = NumBars

        self.BarHeight = BarHeight
        self.BarLength = BarLength
        self.BarWidth = BarWidth

        self.Width = self.BarHeight * self.LayersCount
        self.Height = self.BarWidth * ( self.BarsCount / 2 )
        self.Length = self.BarLength

        self.X = 0.0
        self.Y = 0.0
        self.Z = 0.0

        self.Phi = 0.0
        self.Theta = 0.0
        self.Psi = 0.0

        HalfEMRLength = ( self.BarHeight * (self.LayersCount - 1 ) ) / 2
        for i in range( self.LayersCount ):
            l = EMRLayer( i, self.BarsCount, self.BarLength, self.BarHeight, self.BarWidth )
            l.SetPosition( 0.0, 0.0, - HalfEMRLength + ( i * self.BarHeight ) )
            if i % 2 == 0:
                l.SetRotation( 0.0, 0.0, 0.0 )
            else:
                l.SetRotation( 0.0, 0.0, 90.0 )
            self.Layers.insert( i, l )

    def Export( self ):
        Content = """// FILES/Models/Modules/%s%s.dat
        //
        //

        Module %s
        {
          Volume Box
          Dimensions %.2f %.2f %.2f cm
          PropertyString Material Galactic

          PropertyBool Invisible 1
          // PropertyInt numPlanes %d
          PropertyInt numLayers %d
          PropertyDouble G4StepMax 1.0 mm
        """ % ( ExportFolder,
                self.Name,
                self.Name,
                self.Length,
                self.Width,
                self.Height,
                self.LayersCount,
                self.LayersCount )
        for layer in self.Layers:
            Content += """
              Module %s%s.dat
              {
                Position %.2f %.2f %.2f  cm
                Rotation %.2f %.2f %.2f degree
              }
            """ % ( ExportFolder,
                    layer.Name,
                    layer.X,
                    layer.Y,
                    layer.Z,
                    layer.Phi,
                    layer.Theta,
                    layer.Psi )
            layer.Export()
        Content += "}"
        EMRFile = open( ExportFolder + self.Name + ".dat", 'w' )
        EMRFile.write( Content )
        EMRFile.close()

#Geo = G4MiceGeoBase( "Test" )
#print Geo

#Bar = EMRBar( 10, 10, 110.0, 1.7, 3.3 )
#print Bar
#Bar.Export()

#Layer = EMRLayer( 10, 20, 110.0, 1.7, 3.3 );
#print Layer
#Layer.Export()

EMR = EMRGeometry( NumberOfLayers, BarsPerLayer, 110.0, 1.7, 3.3 );
print EMR
EMR.Export()
