Create a slice:
	$ ./create -d dir_name -s slice_name -k key

Check a slice:
	$ ./check dir_name

------------------------------------------------------------
binding.txt:
	List all bindings.
		1. Directory Name
		2. Slice Name
		3. Group Key

------------------------------------------------------------
Log Filename Format

.locallog
	Filename	Username	Time	Seq#
	file01.txt	yifei		10001	001
	file02.txt	minhan		10020	002

.repolog
	Filename	Username	Seq#
	file01.txt	yifei		001
	file02.txt	minhan		002