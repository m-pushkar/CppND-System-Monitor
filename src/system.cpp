#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::reverse;
using std::set;
using std::size_t;
using std::sort;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() {
  vector<long> x = LinuxParser::CpuUtilization();
  cpu_ = Processor(x);
  return cpu_;
}

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  long uptime{LinuxParser::UpTime()};
  for (int pid : LinuxParser::Pids()) {
    processes_.push_back(Process{pid, (int)uptime});
  }
  sort(processes_.begin(), processes_.end());
  reverse(processes_.begin(), processes_.end());
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
