#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { pid_ = pid; }

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const
{
	/*
	 https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599

	 Calculation

		First we determine the total time spent for the process:

		total_time = utime + stime

		We also have to decide whether we want to include the time from children processes. If we do, then we add those values to total_time:

		total_time = total_time + cutime + cstime

		Next we get the total elapsed time in seconds since the process started:

		seconds = uptime - (starttime / Hertz)

		Finally we calculate the CPU usage percentage:

		cpu_usage = 100 * ((total_time / Hertz) / seconds)

	 */
	float totalTime = float(LinuxParser::ActiveJiffies(pid_));
	float seconds   = float(LinuxParser::UpTime(pid_));

	float cpu_usage = ((totalTime / sysconf(_SC_CLK_TCK)) / seconds);

	return float(cpu_usage);
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// TReturn the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const
{
	return stol(Ram()) < stol(a.Ram());
}
