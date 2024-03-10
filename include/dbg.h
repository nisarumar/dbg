//
// Created by umar on 03.03.24.
//

#ifndef DBG_DBG_H
#define DBG_DBG_H

namespace dbg {
	struct prg_data;
	class debugger {
		std::shared_ptr<prg_data>p_prg_data;
		struct dbg_data;
		std::unique_ptr<dbg_data>p_dbg_data;
	public:
		debugger(prg_data&&);
		~debugger();
		debugger(const debugger&);
		debugger& operator=(const debugger&);
		debugger(debugger&&);
		debugger& operator=(debugger&&);
		void event_loop();
		void dispatcher(const std::string &);
		void continue_execution();
		void set_brk(std::intptr_t);
	};
}
#endif //DBG_DBG_H
