#include <vector>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/personality.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <unordered_map>

#include "shared_data.h"
#include "dbg.h"
#include "breakpoint.hpp"
#include "linenoise.h"

struct dbg::debugger::dbg_data{
	std::unordered_map<std::intptr_t, dbg::breakpoint> mp;
};
dbg::debugger::debugger(dbg::prg_data&& init):p_prg_data(std::make_shared<prg_data>(std::move(init))), p_dbg_data(std::make_unique<dbg_data>()){}
dbg::debugger::~debugger() = default;
dbg::debugger::debugger(dbg::debugger &&rhs) = default;
dbg::debugger& dbg::debugger::operator=(dbg::debugger &&rhs) = default;
dbg::debugger::debugger(const dbg::debugger& rhs):p_prg_data(nullptr), p_dbg_data(nullptr)
{
	if (rhs.p_dbg_data) p_dbg_data = std::make_unique<dbg_data>(*rhs.p_dbg_data);
	p_prg_data = rhs.p_prg_data;
}
dbg::debugger& dbg::debugger::operator=(const dbg::debugger &rhs)
{
	if (!rhs.p_dbg_data) p_dbg_data.reset();
	else if (!p_dbg_data) p_dbg_data = std::make_unique<dbg_data>(*rhs.p_dbg_data);
	else *p_dbg_data = *rhs.p_dbg_data;
	p_prg_data = rhs.p_prg_data;
}

void dbg::debugger::continue_execution() {
	ptrace(PTRACE_CONT, p_prg_data->pid, nullptr, nullptr);
	int wait_status;
	auto options = 0;
	waitpid(p_prg_data->pid, &wait_status, options);
}

auto split(const std::string &s, char delimiter) {
	std::vector<std::string> out{};
	std::stringstream ss {s};
	std::string item;

	while (std::getline(ss,item,delimiter)) {
		if(!item.empty())
			out.push_back(item);
	}
	return out;
}

auto is_prefix(const std::string& s, const std::string& of) {
	if (s.size() > of.size()) return false;
	return std::equal(s.begin(), s.end(), of.begin());
}

void dbg::debugger::dispatcher(const std::string & line) {
	auto args = split(line, ' ');
	if (args.empty())
		return;
	auto command = args[0];

	if (is_prefix(command, "continue")){
		continue_execution();
	}
	else if (is_prefix(command, "break"))
	{
		std::string addr (args[1], 2);
		set_brk(std::stol(addr,0,16));
	}
	else
	{
		std::cerr<<"Unknown command\n";
	}
}

void dbg::debugger::event_loop() {
	auto wait_status{0}, options{0};
	waitpid(p_prg_data->pid, &wait_status, options);

	char* line = nullptr;
	while((line= linenoise("dbg> ")) != nullptr){
		dispatcher(line);
		linenoiseHistoryAdd(line);
		linenoiseFree(line);
	}
}

void execute_tracee (const std::string& prog_name) {
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        std::cerr << "Error in ptrace\n";
        return;
    }
    execl(prog_name.c_str(), prog_name.c_str(), nullptr);
}

void dbg::debugger::set_brk(std::intptr_t addr) {
	std::cout<<"Set break point at address 0x"<<std::hex<<addr<<std::endl;
	breakpoint bpObj {p_prg_data, addr};
	bpObj.set_brk();
	p_dbg_data->mp[addr]=bpObj;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Program name not specified";
        return -1;
    }

    auto prog = argv[1];

    auto pid = fork();
    if (pid == 0) {
        //child
        personality(ADDR_NO_RANDOMIZE);
        execute_tracee(prog);
    }
    else if (pid >= 1)  {
        //parent
        std::cout << "Started debugging process " << pid << '\n';
		dbg::debugger dbg{{prog,pid}};
		dbg.event_loop();
    }
}
