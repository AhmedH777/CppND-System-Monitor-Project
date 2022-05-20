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

	long total = LinuxParser::Jiffies();
	long idle  = LinuxParser::IdleJiffies();

	long totalDiff = total - this->prevTotal_;
	long idleDiff  = idle  - this->prevIdle_;

	long cpuPercent = ( totalDiff - idleDiff )/ totalDiff;
/*
	std::cout<<"Total : "<<total<<" Idle : "<<idle<<std::endl;
	std::cout<<"PTotal : "<<this->prevTotal_<<" PIdle : "<<this->prevIdle_<<std::endl;
	std::cout<<"DTotal : "<<totalDiff<<" DIdle : "<<idleDiff<<std::endl;
*/
	this->prevTotal_ = total;
	this->prevIdle_  = idle;

	return cpuPercent;
}
