#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
float LinuxParser::MemoryUtilization()
{
	  string val,line,key,totalMem,freeMem;

	  std::ifstream stream(kProcDirectory + kMeminfoFilename);

	  if (stream.is_open())
	  {
	    while (std::getline(stream, line))
	    {
	      std::istringstream linestream(line);
	      while (linestream >> key >> val)
	      {
	        if (key == "MemTotal:")
	        {
	          totalMem = val;
	        }
	        if (key == "MemFree:")
	        {
	          freeMem = val;
	        }
	      }
	    }
	  }

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
	vector<CPUStates> allStats = {kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_};
	vector<string> vals = LinuxParser::CpuUtilization();
	vector<long> valslong(10, 0);
	long total = 0;

	for (int i : allStats)
	{
		valslong[i] = stol(vals[i]);
		total += valslong[i];
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

	return stol(vals[13] + vals[14]);
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
	string line,key,val;

	std::ifstream stream(kProcDirectory + kStatFilename);

	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
		  std::istringstream linestream(line);

		  while (linestream >> key >> val)
		  {
			if (key == "processes")
			{
			  return stoi(val);
			}
		  }
		}
	}
	return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
	string line,key,val;

	std::ifstream stream(kProcDirectory + kStatFilename);

	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
		  std::istringstream linestream(line);

		  while (linestream >> key >> val)
		  {
			if (key == "procs_running")
			{
			  return stoi(val);
			}
		  }
		}
	}

	return 0;
}

// Read and return the command associated with a process
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
string LinuxParser::Ram(int pid[[maybe_unused]])
{
	string line,key,val;

	std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);

	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
		  std::istringstream linestream(line);

		  while (linestream >> key)
		  {
			if (key == "VmSize:")
			{
			  linestream >> val;

			  return to_string(stol(val) / 1024);
			}
		  }
		}
	}

	return "0";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{
	string line,key,val;

	std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);

	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
		  std::istringstream linestream(line);

		  while (linestream >> key >> val)
		  {
			if (key == "Uid:")
			{
			  return val;
			}
		  }
		}
	}

	return "0";
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
			if (uid == LinuxParser::Uid(pid))
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
	return LinuxParser::UpTime() - (stol(vals[21]) / 100);
}
