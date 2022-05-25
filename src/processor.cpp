#include "processor.h"
#include "linux_parser.h"
#include <iostream>

// Return the aggregate CPU utilization
float Processor::Utilization()
{
	/*

	https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

	PrevIdle = previdle + previowait
	Idle = idle + iowait

	PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
	NonIdle = user + nice + system + irq + softirq + steal

	PrevTotal = PrevIdle + PrevNonIdle
	Total = Idle + NonIdle

	# differentiate: actual value minus the previous one
	totald = Total - PrevTotal
	idled = Idle - PrevIdle

	CPU_Percentage = (totald - idled)/totald
	*/

	double idle   = LinuxParser::IdleJiffies();
	//double active = LinuxParser::ActiveJiffies();
	double total  = LinuxParser::Jiffies();

	double totalDiff  = abs(total  - this->prevTotal_);
	double idleDiff   = abs(idle   - this->prevIdle_);
	//double activeDiff = abs(active - this->prevActive_);

	double cpuPercent = abs( totalDiff - idleDiff )/ totalDiff;
/*
	std::cout<<"Total : "<<total<<" Idle : "<<idle<<std::endl;
	std::cout<<"PTotal : "<<this->prevTotal_<<" PIdle : "<<this->prevIdle_<<std::endl;
	std::cout<<"DTotal : "<<totalDiff<<" DIdle : "<<idleDiff<<std::endl;
	std::cout<<"Diff   : "<<abs( totalDiff - idleDiff )<<std::endl;
	std::cout<<"Perc   : "<<cpuPercent<<std::endl;
*/
	this->prevTotal_  = total;
	this->prevIdle_   = idle;
	//this->prevActive_ = active;

	return cpuPercent;
}
