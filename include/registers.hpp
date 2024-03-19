#ifndef DBG_REGISTERS_HPP
#define DBG_REGISTERS_HPP
#include <array>
#include <type_traits>
#include <sys/user.h>
#include <iostream>
#include <string_view>
#include <utility>
#include <variant>
#include <algorithm>
#include <cstring>
#include <sys/ptrace.h>
#include "map_macro.h"

namespace dbg {

#define GET_FIELD(STRUCT, FIELD)    ((STRUCT *)0)->FIELD

	MK_ENUM(regs, r15,r14,r13,r12,rbp,rbx,r11,r10,r9,r8,rax,rcx,rdx,rsi,
					rdi,orig_rax,rip,cs,eflags,rsp,ss,fs_base,gs_base,ds,es,fs,gs);

	constexpr std::array dwarf_regs {15,14,13,12,6,3,11,10,9,8,0,2,1,4,5,-1,-1,51,49,7,52,58,59,53,50,54,55};

	template<typename E>
	constexpr auto toUtype(E enumerator) noexcept -> decltype(std::underlying_type_t<E>{})
	{
		return static_cast<std::underlying_type_t<E>> (enumerator);
	}

	using type = decltype(GET_FIELD(user_regs_struct,rax));

	constexpr auto reg_num = sizeof(user_regs_struct)/sizeof(type);

	struct arch_regs{
		union{
			user_regs_struct ptrace_regs;
			std::array<type, reg_num>reg_array;
		};
	};

	using pair_type = std::pair<std::string_view, int>;
	using arr_type  = std::array<pair_type , reg_num>;

	template<typename P>
	constexpr P fillOutHelper(unsigned I)
	{
		return {regs_to_strings[I], dwarf_regs[I]};
	}

	template<typename Pair, std::size_t ... Is>
	constexpr arr_type fillOut (std::index_sequence<Is...>)
	{
		return {fillOutHelper<Pair>(Is)... };
	}

	constexpr static arr_type g_reg_descriptors = fillOut<pair_type>(std::make_index_sequence<reg_num>{});

	auto get_register_value(pid_t pid, regs r)
	{
		arch_regs regs;
		auto err = ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
		if (err){
			std::cout<<"Ptrace:: Error Getting Regs!:"<<std::strerror(errno)<<std::endl;
			return 0ULL;
		}
		return (regs.reg_array[toUtype(r)]);
	}

	auto get_register_value(pid_t pid, const std::string& r)
	{
		auto it = std::find(std::begin(regs_to_strings),std::end(regs_to_strings),r);
		if (it != std::end(regs_to_strings))
		{
			return get_register_value(pid, regs_enum_arr[std::distance(std::begin(regs_to_strings),it)]);
		}
		else {
			std::cerr<<"Unkown Reg"<<std::endl;
			return 0ULL;
		}
	}

	auto get_register_value(pid_t pid, decltype(dwarf_regs[0]) r)
	{
		auto it = std::find(std::begin(dwarf_regs),std::end(dwarf_regs),r);
		if (it != std::end(dwarf_regs))
		{
			return get_register_value(pid, regs_enum_arr[std::distance(std::begin(dwarf_regs),it)]);
		}
		else {
			std::cerr << "Unkown Reg" << std::endl;
			return 0ULL;
		}
	}

	void set_register_value(pid_t pid, regs r, uint64_t value)
	{
		arch_regs regs;
		ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
		regs.reg_array[toUtype(r)] = value;
		ptrace(PTRACE_SETREGS, pid, nullptr, &regs);
	}

	void set_register_value(pid_t pid, const std::string & r, uint64_t value)
	{

	}

	void set_register_value(pid_t pid, decltype(dwarf_regs[0]) r, uint64_t value)
	{

	}


}

#endif
