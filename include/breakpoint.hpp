#ifndef MINIDBG_BREAKPOINT_HPP
#define MINIDBG_BREAKPOINT_HPP

namespace dbg {
    class breakpoint {
    public:
        breakpoint();
		breakpoint(pid_t , std::intptr_t );
		~breakpoint();
		breakpoint(const breakpoint&);
		breakpoint& operator=(const breakpoint&);
		breakpoint(breakpoint&& );
		breakpoint& operator=(breakpoint&&);
		void set_brk();
		void unset_brk();

    private:
		struct brkpt_data;
		std::unique_ptr<brkpt_data> p_brkpt_data;
    };
}

#endif
