#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using std::getline;
using std::ifstream;
using std::istringstream;
using std::stof;
using std::stoi;
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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  int memory, total_memory, free_memory, buffer_memory;

  ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream linestream(line);
      linestream >> key >> memory;

      if (key == "MemTotal:") {
        total_memory = memory;
      } else if (key == "MemFree:") {
        free_memory = memory;
      } else if (key == "Buffers:") {
        buffer_memory = memory;
      }
    }
  }
  // Utilizing memory including buffers
  return ((float)(total_memory - free_memory + buffer_memory)) /
         ((float)total_memory);
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime;

  ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    getline(stream, line);

    // Pop off tokens
    istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// Read and return the total time for a PID
long LinuxParser::TotalTime(int pid) {
  int a_time, b_time, c_time, d_time;
  string line;

  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    string n;

    for (int i = 0; i < 18; i++) {
      linestream >> n;
      switch (i) {
        case 14:
          a_time = stoi(n);
          break;
        case 15:
          b_time = stoi(n);
          break;
        case 16:
          c_time = stoi(n);
          break;
        case 17:
          d_time = stoi(n);
          break;
      }
    }
  }
  return a_time + b_time + c_time + d_time;
}

// Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() {
  vector<long> cpu_utilization{};
  string line;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);

    // Pop off tokens
    istringstream linestream(line);
    string utilization;
    linestream >> utilization;
    assert(utilization == "cpu");
    for (int i = 0; i < 10; i++) {
      int time;
      linestream >> time;
      cpu_utilization.push_back(time);
    }
  }
  return cpu_utilization;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int total_processes;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      // Pop off tokens
      istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {  // DEBUG
        linestream >> total_processes;
        return total_processes;
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int running_processes;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      // Pop off tokens
      istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {  // DEBUG
        linestream >> running_processes;
        return running_processes;
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command;
  ifstream stream{kProcDirectory + to_string(pid) + kCmdlineFilename};
  if (stream.is_open() && getline(stream, command)) {
    return command;
  }
  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  int ram;
  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> ram;

        // Truncated division
        return to_string(ram / 1000);
      }
    }
  }
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string uid;
  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
        return uid;
      }
    }
  }
  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid{LinuxParser::Uid(pid)};
  if (uid.size() == 0) {
    return string();
  }
  string line;
  string name;
  string name_1;
  string name_uid;
  ifstream stream{kPasswordPath};
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':',
                   ' ');  // Make sure to replace ':' with 'Space'
      istringstream linestream(line);
      if (linestream >> name >> name_1 >> name_uid && name_uid == uid) {
        return name;
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string n;
  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open() && getline(stream, line)) {
    istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> n;
    }
    return stoi(n);
  }
  return 0;
}
