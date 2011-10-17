"Constants for use with tests."
import datetime

CLASS_LIST = ['AlarmHandler', 'Beamline', 'CdbError', 'CdbPermanentError',
              'CdbTemporaryError', 'Control', 'Geometry', 'self']

ALH_STR = """AlarmHandler         
\tset_url(string url)         
\tget_status()         
\tget_tagged_alh(string tag)         
\tget_used_alh(string timestamp)         
\tget_used_tags(string start_time, string stop_time)         
\tlist_tags()"""

ALH_TAGGED_ALH = ({'alarms': {'A1': {'lolo': 0.0, 'lo': 5.0, 'hi': 15.0, 'hihi':
20.0}, 'A2': {'lolo': 101.0, 'lo': 102.0, 'hi': 104.0, 'hihi': 105.0}}, 'tag':
'tagged_alh', 'creationtime': datetime.datetime(2011, 2, 9, 8, 56, 18, 616000)})

ALH_USED_ALH = ({'alarms': {'A1': {'lolo': 0.0, 'lo': 5.0, 'hi': 15.0, 'hihi':
20.0}, 'A2': {'lolo': 101.0, 'lo': 102.0, 'hi': 104.0, 'hihi': 105.0}}, 'tag':
'used_alh', 'creationtime': datetime.datetime(2011, 2, 9, 8, 56, 18, 616000)})

ALH_USED_TAGS = ({datetime.datetime(2011, 2, 9, 8, 56, 18, 644000): 'Harry',
datetime.datetime(2011, 2, 9, 8, 56, 18, 588000): 'Harry',
datetime.datetime(2011, 2, 9, 8, 56, 18, 568000): 'Harry',
datetime.datetime(2011, 2, 9, 8, 56, 18, 610000): 'Harry',
datetime.datetime(2011, 2, 9, 8, 56, 18, 522000): 'Fred',
datetime.datetime(2011, 2, 9, 8, 56, 18, 502000): 'Harry',
datetime.datetime(2011, 2, 9, 8, 56, 18, 544000): 'Harry',
datetime.datetime(2011, 2, 9, 8, 56, 18, 477000): 'Harry'})

ALH_LIST_TAGS = ['tag1', 'tag2']

BL_STR = """Beamline         
\tset_url(string url)         
\tget_status()         
\tget_all_beamlines()         
\tget_beamline_for_run(string run_number)         
\tget_beamlines_for_dates(string start_time, string stop_time)         
\tget_beamlines_for_pulses(string start_pulse, string end_pulse)"""

BL_DATES = {1L: {'daqVersion': 'DATES', 'targetDriveVoltage': 14.2,
'isisBeamPositionY': 3.4, 'isisBeamPositionX': 1.2, 'targetDepth': 13.1,
'pulseStart': 1110L, 'isisRepRate': 16.3, 'magnets': {'q11': {'polarity':
'positive', 'setCurrent': 19.4}, 'q12': {'polarity': 'negative', 'setCurrent':
19.7}}, 'isisNominalBeamLoss': 11.9, 'isisNominalBeamCurrent': 110.8,
'isisTargetFrequency': 199L, 'runNumber': 1L, 'runType': ['spam', 'chips'],
'daqTrigger': 'TOF1', 'status': False, 'daqGateWidth': 110.0, 'targetDelay':
10.0, 'step': 12.3, 'startTime': datetime.datetime(2001, 1, 22, 15, 1, 40,
339916), ' protonAbsorberThickness': 55L, 'notes': None, 'endTime':
datetime.datetime(2001, 1, 22, 15, 1, 45, 339916), 'optics': None, 'pulseEnd':
1120L}, 2L: {'daqVersion': 'no', 'targetDriveVoltage': 24.2,
'isisBeamPositionY': 3.4, 'isisBeamPositionX': 1.2, 'targetDepth': 23.1,
'pulseStart': 2110L, 'isisRepRate': 26.3, 'magnets': {'q21': {'polarity':
'positive', 'setCurrent': 29.4}, 'q22': {'polarity': 'negative', 'setCurrent':
29.7}}, 'isisNominalBeamLoss': 21.9, 'isisNominalBeamCurrent': 210.8,
'isisTargetFrequency': 299L, 'runNumber': 2L, 'runType': ['egg', 'beans'],
'daqTrigger': 'TOF2', 'status': True, 'daqGateWidth': 210.0, 'targetDelay':
20.0, 'step': 22.3, 'startTime': datetime.datetime(2002, 2, 22, 15, 1, 40,
339916), ' protonAbsorberThickness': 55L, 'notes': None, 'endTime':
datetime.datetime(2002, 2, 22, 15, 1, 45, 339916), 'optics': None, 'pulseEnd':
2120L}}

BL_PULSES = {1L: {'daqVersion': 'PULSES', 'targetDriveVoltage': 14.2,
'isisBeamPositionY': 3.4, 'isisBeamPositionX': 1.2, 'targetDepth': 13.1,
'pulseStart': 1110L, 'isisRepRate': 16.3, 'magnets': {'q11': {'polarity':
'positive', 'setCurrent': 19.4}, 'q12': {'polarity': 'negative', 'setCurrent':
19.7}}, 'isisNominalBeamLoss': 11.9, 'isisNominalBeamCurrent': 110.8,
'isisTargetFrequency': 199L, 'runNumber': 1L, 'runType': ['spam', 'chips'],
'daqTrigger': 'TOF1', 'status': False, 'daqGateWidth': 110.0, 'targetDelay':
10.0, 'step': 12.3, 'startTime': datetime.datetime(2001, 1, 22, 15, 1, 40,
339916), ' protonAbsorberThickness': 55L, 'notes': None, 'endTime':
datetime.datetime(2001, 1, 22, 15, 1, 45, 339916), 'optics': None, 'pulseEnd':
1120L}, 2L: {'daqVersion': 'no', 'targetDriveVoltage': 24.2,
'isisBeamPositionY': 3.4, 'isisBeamPositionX': 1.2, 'targetDepth': 23.1,
'pulseStart': 2110L, 'isisRepRate': 26.3, 'magnets': {'q21': {'polarity':
'positive', 'setCurrent': 29.4}, 'q22': {'polarity': 'negative', 'setCurrent':
29.7}}, 'isisNominalBeamLoss': 21.9, 'isisNominalBeamCurrent': 210.8,
'isisTargetFrequency': 299L, 'runNumber': 2L, 'runType': ['egg', 'beans'],
'daqTrigger': 'TOF2', 'status': True, 'daqGateWidth': 210.0, 'targetDelay':
20.0, 'step': 22.3, 'startTime': datetime.datetime(2002, 2, 22, 15, 1, 40,
339916), ' protonAbsorberThickness': 55L, 'notes': None, 'endTime':
datetime.datetime(2002, 2, 22, 15, 1, 45, 339916), 'optics': None, 'pulseEnd':
2120L}}

BL_RUNS = {1L: {'daqVersion': 'RUNS', 'targetDriveVoltage': 14.2,
'isisBeamPositionY': 3.4, 'isisBeamPositionX': 1.2, 'targetDepth': 13.1,
'pulseStart': 1110L, 'isisRepRate': 16.3, 'magnets': {'q11': {'polarity':
'positive', 'setCurrent': 19.4}, 'q12': {'polarity': 'negative', 'setCurrent':
19.7}}, 'isisNominalBeamLoss': 11.9, 'isisNominalBeamCurrent': 110.8,
'isisTargetFrequency': 199L, 'runNumber': 1L, 'runType': ['spam', 'chips'],
'daqTrigger': 'TOF1', 'status': False, 'daqGateWidth': 110.0, 'targetDelay':
10.0, 'step': 12.3, 'startTime': datetime.datetime(2001, 1, 22, 15, 1, 40,
339916), ' protonAbsorberThickness': 55L, 'notes': None, 'endTime':
datetime.datetime(2001, 1, 22, 15, 1, 45, 339916), 'optics': None, 'pulseEnd':
1120L}, 2L: {'daqVersion': 'no', 'targetDriveVoltage': 24.2,
'isisBeamPositionY': 3.4, 'isisBeamPositionX': 1.2, 'targetDepth': 23.1,
'pulseStart': 2110L, 'isisRepRate': 26.3, 'magnets': {'q21': {'polarity':
'positive', 'setCurrent': 29.4}, 'q22': {'polarity': 'negative', 'setCurrent':
29.7}}, 'isisNominalBeamLoss': 21.9, 'isisNominalBeamCurrent': 210.8,
'isisTargetFrequency': 299L, 'runNumber': 2L, 'runType': ['egg', 'beans'],
'daqTrigger': 'TOF2', 'status': True, 'daqGateWidth': 210.0, 'targetDelay':
20.0, 'step': 22.3, 'startTime': datetime.datetime(2002, 2, 22, 15, 1, 40,
339916), ' protonAbsorberThickness': 55L, 'notes': None, 'endTime':
datetime.datetime(2002, 2, 22, 15, 1, 45, 339916), 'optics': None, 'pulseEnd':
2120L}}

BL_RUN = {1L: {'daqVersion': 'RUN', 'targetDriveVoltage': 14.2,
'isisBeamPositionY': 3.4, 'isisBeamPositionX': 1.2, 'targetDepth': 13.1,
'pulseStart': 1110L, 'isisRepRate': 16.3, 'magnets': {'q11': {'polarity':
'positive', 'setCurrent': 19.4}, 'q12': {'polarity': 'negative', 'setCurrent':
19.7}}, 'isisNominalBeamLoss': 11.9, 'isisNominalBeamCurrent': 110.8,
'isisTargetFrequency': 199L, 'runNumber': 1L, 'runType': ['spam', 'chips'],
'daqTrigger': 'TOF1', 'status': False, 'daqGateWidth': 110.0, 'targetDelay':
10.0, 'step': 12.3, 'startTime': datetime.datetime(2001, 1, 22, 15, 1, 40,
339916), ' protonAbsorberThickness': 55L, 'notes': None, 'endTime':
datetime.datetime(2001, 1, 22, 15, 1, 45, 339916), 'optics': None, 'pulseEnd':
1120L}}

C_STR = """Control         
\tset_url(string url)         
\tget_status()         
\tget_controls()         
\tget_controls_for_crate(crate)         
\tget_previous_settings(string timestamp)"""

C_CONTROLS = ([{'CH': '0', 'Name': 'n_1', 'RmpUp': '100', 'Crate': '1', 'ILim':
'600.0', 'VSet': '1700.0', 'On/Off': '0', 'SL': '0', 'Trip': '10', 'RmpDn':
'100'}, {'CH': '1', 'Name': 'n_2', 'RmpUp': '100', 'Crate': '1', 'ILim':
'600.0', 'VSet': '1700.0', 'On/Off': '0', 'SL': '0', 'Trip': '10', 'RmpDn':
'100'}, {'CH': '2', 'Name': 'n_3', 'RmpUp': '100', 'Crate': '1', 'ILim':
'600.0', 'VSet': '1750.0', 'On/Off': '0', 'SL': '0', 'Trip': '10', 'RmpDn':
'100'}, {'CH': '3', 'Name': 'n_4', 'RmpUp': '100', 'Crate': '2', 'ILim':
'600.0', 'VSet': '1750.0', 'On/Off': '0', 'SL': '1', 'Trip': '10', 'RmpDn':
'100'}, {'CH': '4', 'Name': 'n_5', 'RmpUp': '100', 'Crate': '2', 'ILim':
'600.0', 'VSet': '1750.0', 'On/Off': '0', 'SL': '1', 'Trip': '10', 'RmpDn':
'100'}])

C_CONTROLS_FOR_CRATE = ([{'CH': '0', 'Name': 'n_1', 'RmpUp': '100', 'Crate':
'3', 'ILim': '600.0', 'VSet': '1700.0', 'On/Off': '0', 'SL': '0', 'Trip': '10',
'RmpDn': '100'}, {'CH': '1', 'Name': 'n_2', 'RmpUp': '100', 'Crate': '3',
'ILim': '600.0', 'VSet': '1700.0', 'On/Off': '0', 'SL': '0', 'Trip': '10',
'RmpDn': '100'}, {'CH': '2', 'Name': 'n_3', 'RmpUp': '100', 'Crate': '3',
'ILim': '600.0', 'VSet': '1750.0', 'On/Off': '0', 'SL': '0', 'Trip': '10',
'RmpDn': '100'}])

C_CONTROLS_FOR_PREVIOUS = ([{'CH': '0', 'Name': 'n_1', 'RmpUp': '100', 'Crate':
'4', 'ILim': '600.0', 'VSet': '1700.0', 'On/Off': '0', 'SL': '0', 'Trip': '10',
'RmpDn': '100'}, {'CH': '1', 'Name': 'n_2', 'RmpUp': '100', 'Crate': '4',
'ILim': '600.0', 'VSet': '1700.0', 'On/Off': '0', 'SL': '0', 'Trip': '10',
'RmpDn': '100'}, {'CH': '2', 'Name': 'n_3', 'RmpUp': '100', 'Crate': '4',
'ILim': '600.0', 'VSet': '1750.0', 'On/Off': '0', 'SL': '0', 'Trip': '10',
'RmpDn': '100'}, {'CH': '3', 'Name': 'n_4', 'RmpUp': '100', 'Crate': '5',
'ILim': '600.0', 'VSet': '1750.0', 'On/Off': '0', 'SL': '1', 'Trip': '10',
'RmpDn': '100'}, {'CH': '4', 'Name': 'n_5', 'RmpUp': '100', 'Crate': '5',
'ILim': '600.0', 'VSet': '1750.0', 'On/Off': '0', 'SL': '1', 'Trip': '10',
'RmpDn': '100'}])

G_STR = """Geometry         
\tset_url(string url)         
\tget_status()         
\tget_current_gdml()         
\tget_gdml_for_id(string id)         
\tget_gdml_for_run(string run_number)         
\tget_ids(string start_time, string stop_time)"""

G_GDML_CURRENT = "GDML_CURRENT"
G_GDML_FOR_ID = "GDML_FOR_ID"
G_GDML_FOR_RUN = "GDML_FOR_RUN"

G_IDS = ({9: {'validFrom': datetime.datetime(2010, 10, 15, 12, 32, 59, 800000),
'created': datetime.datetime(2011, 1, 21, 15, 48, 1, 99000)}, 25: {'validFrom':
datetime.datetime(2010, 10, 15, 12, 32, 59, 800000), 'created':
datetime.datetime(2011, 1, 21, 16, 6, 45, 675000)}, 21: {'validFrom':
datetime.datetime(2010, 10, 15, 12, 32, 59, 800000), 'created':
datetime.datetime(2011, 1, 21, 15, 57, 31, 680000)}, 13: {'validFrom':
datetime.datetime(2010, 10, 15, 12, 32, 59, 800000), 'created':
datetime.datetime(2011, 1, 21, 15, 48, 52, 615000)}, 17: {'validFrom':
datetime.datetime(2010, 10, 15, 12, 32, 59, 800000), 'created':
datetime.datetime(2011, 1, 21, 15, 49, 8, 27000)}})

