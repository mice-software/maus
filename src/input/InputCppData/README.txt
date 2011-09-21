This is a description of the structure of the Json string after the unpacking of the raw data :

{
	"daq_data" :  This is a string containing all information collected by the DAQ system.

	"daq_event_type" : This is a string. The possible types are "start_of_run", "end_of_run", "start_of_burst", "end_of_burst", "physics_event" and "calibration_event". In case of "start_of_run", "end_of_run", "start_of_burst" or "end_of_burst" the "daq_data" is empty.

	"spill_num" : This is an integer number.
}

The daq_data string contains a list of detectors :
{
	"tof0" :
	"tof1" :
	"tof2" :
	"kl" :
	"ckov" :
	........
	........
}

Each detector contains array of particle events and each particle events contains a list of equipment. For example in "tof0" detector the equipments are "V1290" and "V1714"

"tof0" :
	[
		{
			"V1290" :
			"V1724" :
		}
		{
			"V1290" :
			"V1724" :
		}
		{
			"V1290" :
			"V1724" :
		}
		........
		........
	]

Each equipments contains array of measurements.

V1290" :
	[

		{
			"bunch_id" : 384,
			"channel" : 21,
			"channel_key" : "DAQChannelKey 1 1 21 102 tof0",
			"detector" : "tof0",
			"equip_type" : 102,
			"geo" : 1,
			"ldc_id" : 1,
			"leading_time" : 416572,
			"part_event_number" : 0,
			"phys_event_number" : 0,
			"time_stamp" : 1281804484,
			"trailing_time" : -99,
			"trigger_time_tag" : 93666646
		},

		{
			"bunch_id" : 1664,
			"channel" : 21,
			"channel_key" : "DAQChannelKey 1 2 21 102 tof0",
			"detector" : "tof0",
			"equip_type" : 102,
			"geo" : 2,
			"ldc_id" : 1,
			"leading_time" : 1727593,
			"part_event_number" : 0,
			"phys_event_number" : 0,
			"time_stamp" : 1281804484,
			"trailing_time" : -99,
			"trigger_time_tag" : 84183169
		}

		........

		........

	]

The scaler V830 is an unique equipment because it doesn't belong to any detector. Also the scaler's data is not grouped in particle events because we have only one measurement per spill. Here is an example:

		"V830" :
		[

			{
				"channels" :
				{
					"ch0" : 36,
					"ch1" : 38,
					"ch10" : 315,
					"ch11" : 0,
					"ch12" : 3237,
					"ch13" : 0,
					"ch14" : 0,
					"ch15" : 0,
					"ch16" : 0,
					"ch17" : 1,
					"ch18" : 3,
					"ch19" : 11,
					"ch2" : 2227,
					"ch20" : 6,
					"ch21" : 13,
					"ch22" : 6,
					"ch23" : 2,
					"ch24" : 1,
					"ch25" : 1,
					"ch26" : 2,
					"ch27" : 3,
					"ch28" : 12,
					"ch29" : 14,
					"ch3" : 89,
					"ch30" : 7,
					"ch31" : 2,
					"ch4" : 38,
					"ch5" : 0,
					"ch6" : 98,
					"ch7" : 7,
					"ch8" : 1151,
					"ch9" : 1132
				},
				"equip_type" : 111,
				"geo" : 0,
				"ldc_id" : 1,
				"phys_event_number" : 0,
				"time_stamp" : 1281804484
			}
		],
