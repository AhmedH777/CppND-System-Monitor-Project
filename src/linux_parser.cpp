#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::ReturnKeyVal(string filePath , string queryKey)
{
	string line,key,val;

	std::ifstream stream(filePath);

	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
		  std::istringstream linestream(line);

		  while (linestream >> key >> val)
		  {
			if (key == queryKey)
			{
			  return val;
			}
		  }
		}
	}

	return "0";
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
/*https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290*/
float LinuxParser::MemoryUtilization()
{

	  string totalMem,freeMem;

	  string filePath = kProcDirectory + kMeminfoFilename;
	  totalMem = ReturnKeyVal(filePath , "MemTotal:");
	  freeMem  = ReturnKeyVal(filePath , "MemFree:" );

	  return (stof(totalMem)-stof(freeMem)) / stof(totalMem);
}

// Read and return the system uptime
long LinuxParser::UpTime()
{
	string line,uptime;

	std::ifstream stream(kProcDirectory + kUptimeFilename);

	if (stream.is_open())
	{
		std::getline(stream, line);
		std::istringstream linestream(line);
		linestream >> uptime;
	}

	return stol(uptime);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
	vector<string> vals = CpuUtilization();
	long total = 0;

	for (int i = CPUStates::kUser_; i < CPUStates::kGuest_; i++)
	{
		total += stol(vals[i]);
		//std::cout<<vals[i]<<std::endl;
	}

	return total;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid)
{
	string line, val;
	vector<string> vals;

	std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);

	if (stream.is_open())
	{
		std::getline(stream, line);
		std::istringstream linestream(line);

		while (linestream >> val)
		{
			vals.push_back(val);
		}
	}

	/* utime + stime + cuTime + csTime*/
	return (stol(vals[13]) + stol(vals[14]) + stol(vals[15]) + stol(vals[16]));
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
	  vector<string> jiffies = CpuUtilization();

	  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
	         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
	         stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
	  vector<string> jiffies = CpuUtilization();
	  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
	  string line, key, val;
	  vector<string> vals;

	  std::ifstream stream(kProcDirectory + kStatFilename);

	  if (stream.is_open())
	  {
	    std::getline(stream, line);
	    std::istringstream linestream(line);
	    linestream >> key;

	    while (linestream >> val)
	    {
	      vals.push_back(val);
	    }
	  }
	  return vals;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
	string filePath = (kProcDirectory + kStatFilename);

	return stoi(ReturnKeyVal(filePath , "processes"));
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
	string filePath = (kProcDirectory + kStatFilename);

	return stoi(ReturnKeyVal(filePath , "procs_running"));
}

// Read and return the command associated with a process
/*https://classroom.udacity.com/nanodegrees/nd213/parts/cd0424/modules/8a2c831e-371a-42ba-b081-5719719f0d46/lessons/80d13f3a-97d0-48d0-a662-d4e28150257f/concepts/2fb8269b-af0b-42a0-940c-340599b1d239*/
/*Linux stores the command used to launch the function in the /proc/[pid]/cmdline file.*/
string LinuxParser::Command(int pid)
{
	string cmd;

	std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);

	if (stream.is_open())
	{
		std::getline(stream, cmd);
	}

	return cmd;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{

	string filePath = (kProcDirectory + to_string(pid) + kStatusFilename);

	return to_string(stol(ReturnKeyVal(filePath , "VmSize:")) / 1024);

}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{
	string filePath = (kProcDirectory + to_string(pid) + kStatusFilename);

	return ReturnKeyVal(filePath , "Uid:");
}

// Read and return the user associated with a process
string LinuxParser::User(int pid)
{
	string line,name,uid,a;

	std::ifstream stream(kPasswordPath);

	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
		  std::replace(line.begin(), line.end(), ':', ' ');
		  std::istringstream linestream(line);

		  while (linestream >> name >> a >> uid)
		  {
			if (uid == Uid(pid))
			{
			  return name;
			}
		  }
		}
	}

	return "unknown";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{
	string line, val;
	vector<string> vals;

	std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);

	if (stream.is_open())
	{
		std::getline(stream, line);
		std::istringstream linestream(line);

		while (linestream >> val)
		{
		  vals.push_back(val);
		}

	}
	/*
	 https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599

	 Next we get the total elapsed time in seconds since the process started:

	 seconds = uptime - (starttime / Hertz)

	 */
	return ( LinuxParser::UpTime() - ( std::stol(vals[21])/sysconf(_SC_CLK_TCK) ) );
}
