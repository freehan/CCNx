Create a slice and a local folder:
	$ ./init -d dir_name -s slice_name -k key

Check a slice and do synchronization:
	$ ./main dir_name

------------------------------------------------------------
binding.txt:
	List all bindings.
		1. Directory Name
		2. Slice Name
		3. Group Key

------------------------------------------------------------
Log Filename Format

.locallog
	Filename	Username	Time	Seq#	Del
	file01.txt	yifei		10001	001		0
	file02.txt	minhan		10020	002		0
	file03.txt	minhan		10030	002		1 		<- The file is deleted

.repolog
	Filename	Username	Seq# 	Del
	file01.txt	yifei		001		0
	file02.txt	minhan		002		0	
	file03.txt	minhan		002		1 		<- The file is deleted

------------------------------------------------------------
Handling local log and repo log

if (delete flag ==1)
{	
	delete file
}	
if (local_new) 
{
 // Local Seq # > Repo Seq #
 put_file();
 } 
 else if (repo_new)
 {
 // Repo Seq # > Local Seq #
 get_file();
 } 
 else 
 {
 // Repo Seq # = Local Seq #
 // Check user
 	if (user is the same as me) {
 // Do nothing
 	} 
 	else 
 	{
 		//Conflict
 	}
 }