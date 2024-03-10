//
// Created by umar on 10.03.24.
//

#include<cstdint>
#include<sys/ptrace.h>
#include<memory>
#include "dbg.h"
#include "breakpoint.hpp"

struct dbg::breakpoint::brkpt_data{
	std::intptr_t addr;
	bool enabled;
	uint8_t saved_data;
	pid_t pid;
};

dbg::breakpoint::breakpoint() = default;
dbg::breakpoint::~breakpoint()=default;
dbg::breakpoint::breakpoint(breakpoint&&) = default; //move constructor
dbg::breakpoint& dbg::breakpoint::operator=(dbg::breakpoint &&) =default; //move aperator
dbg::breakpoint::breakpoint(const dbg::breakpoint & rhs): p_brkpt_data(nullptr) {
	if (rhs.p_brkpt_data)
	{
		p_brkpt_data = std::make_unique<brkpt_data>(*rhs.p_brkpt_data);
	}

}

dbg::breakpoint& dbg::breakpoint::operator=(const dbg::breakpoint & rhs) {
	if (!rhs.p_brkpt_data)
	{
		p_brkpt_data.reset();
	}else if (!p_brkpt_data) p_brkpt_data = std::make_unique<brkpt_data>(*rhs.p_brkpt_data);
	else *p_brkpt_data = *rhs.p_brkpt_data;
}

dbg::breakpoint::breakpoint(pid_t program_pid, std::intptr_t addr):p_brkpt_data(std::make_unique<brkpt_data>()) {
	p_brkpt_data->pid = program_pid;
	p_brkpt_data->addr = addr;
	p_brkpt_data->enabled = false;
	p_brkpt_data->saved_data = 0;
}

void dbg::breakpoint::set_brk()
{
	auto data = ptrace(PTRACE_PEEKDATA, p_brkpt_data->pid, p_brkpt_data->addr, nullptr);
	p_brkpt_data->saved_data = static_cast<uint8_t> (data & 0xFF);
	ptrace(PTRACE_POKEDATA, p_brkpt_data->pid, ((data & ~0xFF) | 0xCC)); //set lower byte of data at the address to opcode of 'int 3'
	p_brkpt_data->enabled = true;
}

void dbg::breakpoint::unset_brk() {
	auto data = ptrace(PTRACE_PEEKDATA, p_brkpt_data->pid, p_brkpt_data->addr, nullptr);
	ptrace(PTRACE_POKEDATA, p_brkpt_data->pid, ((data & ~0xFF) | p_brkpt_data->saved_data)); //set lower byte of data at the address to opcode of 'int 3'
	p_brkpt_data->enabled = false;
}
