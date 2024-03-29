//
// Created by umar on 10.03.24.
//

#include<cstdint>
#include<sys/ptrace.h>
#include<memory>
#include <string>
#include <iostream>
#include <cerrno>
#include <cstring>

#include "shared_data.h"
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
	p_prg_data = rhs.p_prg_data;
}

dbg::breakpoint& dbg::breakpoint::operator=(const dbg::breakpoint & rhs) {
	if (!rhs.p_brkpt_data)
	{
		p_brkpt_data.reset();
	}else if (!p_brkpt_data) p_brkpt_data = std::make_unique<brkpt_data>(*rhs.p_brkpt_data);
	else *p_brkpt_data = *rhs.p_brkpt_data;

	p_prg_data = rhs.p_prg_data;
}

dbg::breakpoint::breakpoint(std::shared_ptr<prg_data> ptrPrgData, std::intptr_t addr):p_prg_data{ptrPrgData}, p_brkpt_data(std::make_unique<brkpt_data>()) {
	p_brkpt_data->addr = addr;
	p_brkpt_data->enabled = false;
	p_brkpt_data->saved_data = 0;
}

void dbg::breakpoint::set_brk()
{
	auto data = ptrace(PTRACE_PEEKDATA, p_prg_data->pid, p_brkpt_data->addr, nullptr);
	p_brkpt_data->saved_data = static_cast<uint8_t> (data & 0xFF);
	uint64_t data_with_int3 = ((data & ~0xFF ) | 0xCC);
	auto err = ptrace(PTRACE_POKEDATA, p_prg_data->pid, p_brkpt_data->addr, ((data & ~0xFF) | 0xCC)); //set lower byte of data at the address to opcode of 'int 3'
	if (err)
	{
		std::cout<<"Error Setting brkpt!:"<<std::strerror(errno)<<std::endl;
		p_brkpt_data->saved_data = 0;
		return;

	}
	p_brkpt_data->enabled = true;
}

void dbg::breakpoint::unset_brk() {
	if (p_brkpt_data->enabled)
	{
		auto data = ptrace(PTRACE_PEEKDATA, p_prg_data->pid, p_brkpt_data->addr, nullptr);
		auto err = ptrace(PTRACE_POKEDATA, p_prg_data->pid, ((data & ~0xFF) | p_brkpt_data->saved_data)); //restore the lower byte to the saved value
		if (err)
		{
			std::cout<<"Error unsetting brkpt!:"<<std::strerror(errno)<<std::endl;
			return;
		}
		p_brkpt_data->saved_data = 0;
		p_brkpt_data->enabled = false;
	}
}
