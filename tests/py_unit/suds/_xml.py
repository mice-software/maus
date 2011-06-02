"XML used to mimic soap responses."

from base64 import b64encode

STATUS_OK = "<ok>feeling good</ok>"
STATUS_WARN = "<warning>a bit poorly</warning>"
STATUS_ERROR = "<error>dead</error>"
G_STATUS_OK = b64encode("<ok>feeling good</ok>")
G_STATUS_WARN = b64encode("<warning>a bit poorly</warning>")
G_STATUS_ERROR = b64encode("<error>dead</error>")

ALH_TAGGED_ALH = """<alarmhandler creationtime='2011-02-09 08:56:18.616'
tag='tagged_alh'><alarm><name>A1</name><hihi>20.0</hihi><hi>15.0</hi>
<lo>5.0</lo><lolo>0.0</lolo></alarm><alarm><name>A2</name><hihi>105.0</hihi>
<hi>104.0</hi><lo>102.0</lo><lolo>101.0</lolo></alarm></alarmhandler>"""

ALH_USED_ALH = """<alarmhandler creationtime='2011-02-09 08:56:18.616'
tag='used_alh'><alarm><name>A1</name><hihi>20.0</hihi><hi>15.0</hi><lo>5.0</lo>
<lolo>0.0</lolo></alarm><alarm><name>A2</name><hihi>105.0</hihi><hi>104.0</hi>
<lo>102.0</lo><lolo>101.0</lolo></alarm></alarmhandler>"""

ALH_USED_TAGS = """<tags><tag starttime='2011-02-09 08:56:18.477' name='Harry'/>
<tag starttime='2011-02-09 08:56:18.502' name='Harry'/>
<tag starttime='2011-02-09 08:56:18.522' name='Fred'/>
<tag starttime='2011-02-09 08:56:18.544' name='Harry'/>
<tag starttime='2011-02-09 08:56:18.568' name='Harry'/>
<tag starttime='2011-02-09 08:56:18.588' name='Harry'/>
<tag starttime='2011-02-09 08:56:18.61' name='Harry'/>
<tag starttime='2011-02-09 08:56:18.644' name='Harry'/></tags>"""

ALH_LIST_TAGS = """<tags><tag name='tag1'/><tag name='tag2'/></tags>"""

BL_DATES = """<runs><run runNumber='1' startTime='2001-01-22 15:01:40.339916'
endTime='2001-01-22 15:01:45.339916' notes='null' optics='null'
protonAbsorberThickness='55' pulseStart='1110' pulseEnd='1120' step='12.3'
status='false' runType='10100000000000000000000000000000' daqTrigger='TOF1'
daqGateWidth='110.0' daqVersion='DATES' isisTargetFrequency='199'
isisRepRate='16.3' isisNominalBeamCurrent='110.8' isisNominalBeamLoss='11.9'
isisBeamPositionX='1.2' isisBeamPositionY='3.4' targetDepth='13.1'
targetDelay='10.0' targetDriveVoltage='14.2'><magnet name='q11'
setCurrent='19.4' polarity='positive'/><magnet name='q12' setCurrent='19.7'
polarity='negative'/></run><run runNumber='2' startTime='2002-02-22
15:01:40.339916' endTime='2002-02-22 15:01:45.339916' notes='null' optics='null'
protonAbsorberThickness='55' pulseStart='2110' pulseEnd='2120' step='22.3'
status='true' runType='01010000000000000000000000000000' daqTrigger='TOF2'
daqGateWidth='210.0' daqVersion='no' isisTargetFrequency='299'
isisRepRate='26.3' isisNominalBeamCurrent='210.8' isisNominalBeamLoss='21.9'
isisBeamPositionX='1.2' isisBeamPositionY='3.4' targetDepth='23.1'
targetDelay='20.0' targetDriveVoltage='24.2'><magnet name='q21'
setCurrent='29.4' polarity='positive'/><magnet name='q22' setCurrent='29.7'
polarity='negative'/></run></runs>
"""

BL_PULSES = """<runs><run runNumber='1' startTime='2001-01-22 15:01:40.339916'
endTime='2001-01-22 15:01:45.339916' notes='null' optics='null'
protonAbsorberThickness='55' pulseStart='1110' pulseEnd='1120' step='12.3'
status='false' runType='10100000000000000000000000000000' daqTrigger='TOF1'
daqGateWidth='110.0' daqVersion='PULSES' isisTargetFrequency='199'
isisRepRate='16.3' isisNominalBeamCurrent='110.8' isisNominalBeamLoss='11.9'
isisBeamPositionX='1.2' isisBeamPositionY='3.4' targetDepth='13.1'
targetDelay='10.0' targetDriveVoltage='14.2'><magnet name='q11'
setCurrent='19.4' polarity='positive'/><magnet name='q12' setCurrent='19.7'
polarity='negative'/></run><run runNumber='2' startTime='2002-02-22
15:01:40.339916' endTime='2002-02-22 15:01:45.339916' notes='null' optics='null'
protonAbsorberThickness='55' pulseStart='2110' pulseEnd='2120' step='22.3'
status='true' runType='01010000000000000000000000000000' daqTrigger='TOF2'
daqGateWidth='210.0' daqVersion='no' isisTargetFrequency='299'
isisRepRate='26.3' isisNominalBeamCurrent='210.8' isisNominalBeamLoss='21.9'
isisBeamPositionX='1.2' isisBeamPositionY='3.4' targetDepth='23.1'
targetDelay='20.0' targetDriveVoltage='24.2'><magnet name='q21'
setCurrent='29.4' polarity='positive'/><magnet name='q22' setCurrent='29.7'
polarity='negative'/></run></runs>
"""

BL_RUNS = """<runs><run runNumber='1' startTime='2001-01-22 15:01:40.339916'
endTime='2001-01-22 15:01:45.339916' notes='null' optics='null'
protonAbsorberThickness='55' pulseStart='1110' pulseEnd='1120' step='12.3'
status='false' runType='10100000000000000000000000000000' daqTrigger='TOF1'
daqGateWidth='110.0' daqVersion='RUNS' isisTargetFrequency='199'
isisRepRate='16.3' isisNominalBeamCurrent='110.8' isisNominalBeamLoss='11.9'
isisBeamPositionX='1.2' isisBeamPositionY='3.4' targetDepth='13.1'
targetDelay='10.0' targetDriveVoltage='14.2'><magnet name='q11'
setCurrent='19.4' polarity='positive'/><magnet name='q12' setCurrent='19.7'
polarity='negative'/></run><run runNumber='2' startTime='2002-02-22
15:01:40.339916' endTime='2002-02-22 15:01:45.339916' notes='null' optics='null'
protonAbsorberThickness='55' pulseStart='2110' pulseEnd='2120' step='22.3'
status='true' runType='01010000000000000000000000000000' daqTrigger='TOF2'
daqGateWidth='210.0' daqVersion='no' isisTargetFrequency='299'
isisRepRate='26.3' isisNominalBeamCurrent='210.8' isisNominalBeamLoss='21.9'
isisBeamPositionX='1.2' isisBeamPositionY='3.4' targetDepth='23.1'
targetDelay='20.0' targetDriveVoltage='24.2'><magnet name='q21'
setCurrent='29.4' polarity='positive'/><magnet name='q22' setCurrent='29.7'
polarity='negative'/></run></runs>
"""

BL_RUN = """<runs><run runNumber='1' startTime='2001-01-22 15:01:40.339916'
endTime='2001-01-22 15:01:45.339916' notes='null' optics='null'
protonAbsorberThickness='55' pulseStart='1110' pulseEnd='1120' step='12.3'
status='false' runType='10100000000000000000000000000000' daqTrigger='TOF1'
daqGateWidth='110.0' daqVersion='RUN' isisTargetFrequency='199'
isisRepRate='16.3' isisNominalBeamCurrent='110.8' isisNominalBeamLoss='11.9'
isisBeamPositionX='1.2' isisBeamPositionY='3.4' targetDepth='13.1'
targetDelay='10.0' targetDriveVoltage='14.2'><magnet name='q11'
setCurrent='19.4' polarity='positive'/><magnet name='q12' setCurrent='19.7'
polarity='negative'/></run></runs>
"""

C_CONTROLS = """
<controls>
<crate name='1' rack='HV Rack 1'>
<channel name='0' module='0' remoteChannelName='n_1'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1700.0'/>
</channel>
<channel name='1' module='0' remoteChannelName='n_2'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1700.0'/>
</channel>
<channel name='2' module='0' remoteChannelName='n_3'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1750.0'/>
</channel>
</crate>
<crate name='2' rack='HV Rack 1'>
<channel name='3' module='1' remoteChannelName='n_4'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1750.0'/>
</channel>
<channel name='4' module='1' remoteChannelName='n_5'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1750.0'/>
</channel>
</crate>
</controls>
"""

C_CONTROLS_FOR_CRATE = """
<controls>
<crate name='3' rack='HV Rack 1'>
<channel name='0' module='0' remoteChannelName='n_1'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1700.0'/>
</channel>
<channel name='1' module='0' remoteChannelName='n_2'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1700.0'/>
</channel>
<channel name='2' module='0' remoteChannelName='n_3'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1750.0'/>
</channel>
</crate>
</controls>"""

C_CONTROLS_FOR_PREVIOUS = """
<controls>
<crate name='4' rack='HV Rack 1'>
<channel name='0' module='0' remoteChannelName='n_1'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1700.0'/>
</channel>
<channel name='1' module='0' remoteChannelName='n_2'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1700.0'/>
</channel>
<channel name='2' module='0' remoteChannelName='n_3'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1750.0'/>
</channel>
</crate>
<crate name='5' rack='HV Rack 1'>
<channel name='3' module='1' remoteChannelName='n_4'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1750.0'/>
</channel>
<channel name='4' module='1' remoteChannelName='n_5'>
<parameter name='ILim' value='600.0'/>
<parameter name='On/Off' value='0'/>
<parameter name='RmpDn' value='100'/>
<parameter name='RmpUp' value='100'/>
<parameter name='Trip' value='10'/>
<parameter name='VSet' value='1750.0'/>
</channel>
</crate>
</controls>
"""

G_GDML_CURRENT = b64encode("GDML_CURRENT")
G_GDML_FOR_ID = b64encode("GDML_FOR_ID")
G_GDML_FOR_RUN = b64encode("GDML_FOR_RUN")

G_IDS = """<ids><id name='9' validFrom='2010-10-15 12:32:59.80'
created='2011-01-21 15:48:01.099'/><id name='13' validFrom='2010-10-15
12:32:59.80' created='2011-01-21 15:48:52.615'/><id name='17'
validFrom='2010-10-15 12:32:59.80' created='2011-01-21 15:49:08.027'/><id
name='21' validFrom='2010-10-15 12:32:59.80' created='2011-01-21
15:57:31.68'/><id name='25' validFrom='2010-10-15 12:32:59.80'
created='2011-01-21 16:06:45.675'/></ids>"""

